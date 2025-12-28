import json
import sys
import re

def sanitize(name, tid):
    # Replace non-alphanumeric with underscore and append tid for uniqueness
    clean = re.sub(r'[^a-zA-Z0-9_]', '_', name)
    return f"{clean}_{tid}"

def sanitize_data_name(name):
    # Replace non-alphanumeric with underscore for data names
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

def parse_port(port_str):
    # port_str is like "0b0000000000"
    val = int(port_str, 2)
    task_id = val >> 4
    output_idx = val & 0xF
    return task_id, output_idx

def generate_c_code(json_file, output_file):
    with open(json_file, 'r') as f:
        raw_data = json.load(f)
    
    # Filter out the last element if it's not a task
    tasks_raw = [t for t in raw_data if 'current_taskId' in t]
    
    nodes = []
    node_map = {} # taskId -> node_info
    
    for t in tasks_raw:
        tid = t['current_taskId']
        name = t.get('debug_task_name', f"Task_{tid}")
        safe_name = sanitize(name, tid)
        
        node_info = {
            'id': tid,
            'name': name,
            'safe_name': safe_name,
            'inputs': [],  # Data objects consumed (list of data_obj references)
            'input_data_list': [], # All input data info for this task
            'outputs': {}, # outputIdx -> Data object produced
            'successors': set(), # Set of safe_names
            'producers': set(), # Set of producer safe_names
            'storage_size': t.get('total_length', 256),
            'job_id': tid
        }
        nodes.append(node_info)
        node_map[tid] = node_info

    # Data objects: unique by name -> data_info
    # For external data (type 0b01): keyed by name
    # For task-to-task (type 0b00): keyed by (producer_id, output_idx)
    data_objects = {}          # For task-to-task dependencies
    external_data_objects = {} # For external/constant data (type 0b01)

    for t in tasks_raw:
        consumer_id = t['current_taskId']
        consumer_node = node_map[consumer_id]
        
        for inp in t.get('all_input', []):
            input_type = inp.get('type', '')
            input_name = inp.get('name', f"data_{consumer_id}_{len(consumer_node['input_data_list'])}")
            input_length = inp.get('length', 64)  # Default 64 if not specified
            safe_data_name = sanitize_data_name(input_name)
            
            if input_type == "0b01": # External/constant data input
                key = safe_data_name
                
                if key not in external_data_objects:
                    external_data_objects[key] = {
                        'name': input_name,
                        'safe_name': safe_data_name,
                        'var_name': f"extdata_{safe_data_name}",
                        'producer': None,  # No producer - external data
                        'consumers': [],
                        'size': input_length,
                        'is_external': True
                    }
                
                data_obj = external_data_objects[key]
                # Update size to max if different tasks have different sizes
                if input_length > data_obj['size']:
                    data_obj['size'] = input_length
                    
                if consumer_node['safe_name'] not in data_obj['consumers']:
                    data_obj['consumers'].append(consumer_node['safe_name'])
                
                # Add to this node's input list
                consumer_node['input_data_list'].append({
                    'data_obj': data_obj,
                    'name': input_name,
                    'length': input_length,
                    'type': 'external'
                })
                
            elif input_type == "0b00": # Task-to-task dependency
                producer_id, out_idx = parse_port(inp['parentTasksPort'])
                
                if producer_id not in node_map:
                    print(f"Warning: Task {consumer_id} depends on unknown Task {producer_id}")
                    continue
                
                producer_node = node_map[producer_id]
                key = (producer_id, out_idx)
                
                if key not in data_objects:
                    # Use the input name from JSON if available
                    data_name = input_name if input_name else f"{producer_node['safe_name']}_out{out_idx}"
                    safe_data_name = sanitize_data_name(data_name)
                    data_objects[key] = {
                        'name': data_name,
                        'safe_name': safe_data_name,
                        'var_name': f"data_{safe_data_name}",
                        'producer': producer_node['safe_name'],
                        'consumers': [],
                        'size': input_length,
                        'is_external': False
                    }
                
                data_obj = data_objects[key]
                # Update size to max if different consumers expect different sizes
                if input_length > data_obj['size']:
                    data_obj['size'] = input_length
                    
                if consumer_node['safe_name'] not in data_obj['consumers']:
                    data_obj['consumers'].append(consumer_node['safe_name'])
                
                # Link to nodes
                consumer_node['inputs'].append(data_obj)
                consumer_node['input_data_list'].append({
                    'data_obj': data_obj,
                    'name': input_name,
                    'length': input_length,
                    'type': 'task_dep'
                })
                consumer_node['producers'].add(producer_node['safe_name'])
                producer_node['outputs'][out_idx] = data_obj
                producer_node['successors'].add(consumer_node['safe_name'])

    # Start generating code
    lines = []
    lines.append("/* Auto-generated DAG from dag1.json */")
    lines.append("#include <stddef.h>")
    lines.append("#include <stdint.h>")
    lines.append("#include <string.h>")
    lines.append('#include "FreeRTOS.h"')
    lines.append('#include "cu_offload.h"')
    lines.append('#include "dag_runtime.h"')
    lines.append('#include "portmacro.h"')
    lines.append('#include "task.h"')
    lines.append('#include "uart16550.h"')
    lines.append("")
    lines.append("#define NUM_WORKERS (configNUMBER_OF_CORES)")
    lines.append("#define DAG_QUEUE_LEN 16")
    lines.append("#define DAG_ASYNC_DONE_QUEUE_LEN 16")
    lines.append("")
    lines.append("typedef struct { uint32_t cu_id; uint32_t job_id; DagNode* node; } CuNodeCtx_t;")
    lines.append("")

    # Forward declarations
    for n in nodes:
        lines.append(f"static DagNode node_{n['safe_name']};")
    lines.append("")

    # External Data Objects (type 0b01)
    if external_data_objects:
        lines.append("/* External/Constant Data Objects */")
        for d in external_data_objects.values():
            lines.append(f"static DagData {d['var_name']}; /* {d['name']}, size={d['size']} */")
        lines.append("")

    # Task-to-Task Data Objects (type 0b00)
    if data_objects:
        lines.append("/* Task-to-Task Data Objects */")
        for d in data_objects.values():
            lines.append(f"static DagData {d['var_name']}; /* {d['name']}, size={d['size']} */")
        lines.append("")

    # Contexts
    for n in nodes:
        lines.append(f"static CuNodeCtx_t ctx_{n['safe_name']};")
    lines.append("")

    # Arrays - now include all inputs (external + task dependencies)
    for n in nodes:
        if n['successors']:
            lines.append(f"static DagNode* succOf_{n['safe_name']}[{len(n['successors'])}];")
        total_inputs = len(n['input_data_list'])
        if total_inputs > 0:
            lines.append(f"static DagData* inputs_{n['safe_name']}[{total_inputs}];")
        if n['outputs']:
            lines.append(f"static DagData* outputs_{n['safe_name']}[{len(n['outputs'])}];")
    lines.append("")

    # Worker function
    lines.append("static void vCuNodeWork(void* arg) {")
    lines.append("  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;")
    lines.append("  ctx->node->async_pending = 1U;")
    lines.append("  vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);")
    lines.append("}")
    lines.append("")

    # Init function
    lines.append("static void vDagInit(void) {")
    
    # Init External Data Objects
    if external_data_objects:
        lines.append("  /* Initialize External/Constant Data Objects */")
        for d in external_data_objects.values():
            lines.append(f"  memset(&{d['var_name']}, 0, sizeof(DagData));")
            lines.append(f"  {d['var_name']}.name = \"{d['name']}\";")
            lines.append(f"  {d['var_name']}.sizeBytes = {d['size']}U;")
            lines.append(f"  {d['var_name']}.consumers = {len(d['consumers'])}U;")
        lines.append("")
    
    # Init Task-to-Task Data Objects
    if data_objects:
        lines.append("  /* Initialize Task-to-Task Data Objects */")
        for d in data_objects.values():
            lines.append(f"  memset(&{d['var_name']}, 0, sizeof(DagData));")
            lines.append(f"  {d['var_name']}.name = \"{d['name']}\";")
            lines.append(f"  {d['var_name']}.sizeBytes = {d['size']}U;")
            lines.append(f"  {d['var_name']}.consumers = {len(d['consumers'])}U;")
        lines.append("")

    # Init Nodes
    for n in nodes:
        sn = n['safe_name']
        lines.append(f"  /* Node {n['name']} */")
        lines.append(f"  memset(&node_{sn}, 0, sizeof(DagNode));")
        lines.append(f"  node_{sn}.name = \"{n['name']}\";")
        lines.append(f"  node_{sn}.run = vCuNodeWork;")
        lines.append(f"  node_{sn}.arg = &ctx_{sn};")
        # 设置为0或者UINT32_MAX都没问题，但是设置和storage_size一样会卡死
        # lines.append(f"  node_{sn}.computeDelayMs = {n['storage_size']}U;")
        lines.append(f"  node_{sn}.computeDelayMs = {100000 * n['storage_size']}U;")
        # lines.append(f"  node_{sn}.storageSizeBytes = {n['storage_size']}U;")
        lines.append(f"  node_{sn}.indegree = {len(n['producers'])}U;")
        lines.append(f"  node_{sn}.mode = DAG_NODE_ASYNC;")
        
        # Include all inputs (external + task dependencies)
        total_inputs = len(n['input_data_list'])
        if total_inputs > 0:
            lines.append(f"  node_{sn}.numInputs = {total_inputs}U;")
            lines.append(f"  node_{sn}.inputs = inputs_{sn};")
            for i, inp_info in enumerate(n['input_data_list']):
                d = inp_info['data_obj']
                lines.append(f"  inputs_{sn}[{i}] = &{d['var_name']}; /* {inp_info['name']}, len={inp_info['length']} */")
        
        if n['outputs']:
            lines.append(f"  node_{sn}.numOutputs = {len(n['outputs'])}U;")
            lines.append(f"  node_{sn}.outputs = outputs_{sn};")
            # Sort outputs by index to match array
            sorted_outs = [n['outputs'][i] for i in sorted(n['outputs'].keys())]
            for i, d in enumerate(sorted_outs):
                lines.append(f"  outputs_{sn}[{i}] = &{d['var_name']};")

        if n['successors']:
            lines.append(f"  node_{sn}.numSuccessors = {len(n['successors'])}U;")
            lines.append(f"  node_{sn}.successors = succOf_{sn};")
            for i, succ in enumerate(sorted(list(n['successors']))):
                lines.append(f"  succOf_{sn}[{i}] = &node_{succ};")
        
        lines.append(f"  ctx_{sn}.cu_id = CU_ANY_ID;")
        lines.append(f"  ctx_{sn}.job_id = {n['job_id']}U;")
        lines.append(f"  ctx_{sn}.node = &node_{sn};")
        lines.append("")

    lines.append("}")
    lines.append("")

    # Start function
    lines.append("void vStartDagDemo(void) {")
    lines.append("  vDagInit(); vCuInit();")
    lines.append("  DagRuntimeConfig cfg = { .workerCount = NUM_WORKERS, .workerPriority = tskIDLE_PRIORITY + 2,")
    lines.append("    .workerStackWords = 512, .asyncPriority = tskIDLE_PRIORITY + 3, .asyncStackWords = 512,")
    lines.append("    .readyQueueLen = DAG_QUEUE_LEN, .asyncDoneQueueLen = DAG_ASYNC_DONE_QUEUE_LEN, .useCoreAffinity = 1 };")
    lines.append("  vDagRuntimeStart(&cfg);")
    
    # Submit roots
    for n in nodes:
        if not n['inputs']:
            lines.append(f"  vDagSubmitReadyNode(&node_{n['safe_name']});")
    
    lines.append("}")

    with open(output_file, 'w') as f:
        f.write('\n'.join(lines))
    print(f"Generated {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python generate_dag_v2.py <dag1.json> <output.c>")
    else:
        generate_c_code(sys.argv[1], sys.argv[2])
