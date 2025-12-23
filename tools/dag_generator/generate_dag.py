import json
import sys
import os

def generate_c_code(config_file, output_file):
    with open(config_file, 'r') as f:
        data = json.load(f)

    nodes = data['nodes']
    config = data.get('config', {})
    worker_func = config.get('worker_func', 'vCuNodeWork')
    default_cu_id = config.get('default_cu_id', 'CU_ANY_ID')

    # Pre-process to find all data objects and relationships
    # node_map: name -> node_dict
    node_map = {n['name']: n for n in nodes}
    
    # data_objects: list of {name, size, producer, consumers: []}
    # But in this simple model, edges are defined in outputs.
    # Let's normalize.
    
    # We need to track inputs for each node to generate input arrays
    node_inputs = {n['name']: [] for n in nodes} # list of data names
    node_outputs = {n['name']: [] for n in nodes} # list of data names
    
    all_data = []

    for node in nodes:
        producer_name = node['name']
        for out in node.get('outputs', []):
            # Support both old format {"to": "B"} and new format {"consumers": ["B", "C"]}
            consumers = out.get('consumers', [])
            if 'to' in out:
                consumers.append(out['to'])
            
            # Deduplicate
            consumers = list(set(consumers))
            
            data_name = out['name']
            size = out['size']
            
            # Create a unique variable name for the data object
            # If multiple consumers, we can just use the data name sanitized
            safe_data_name = data_name.replace("->", "_to_").replace(" ", "_")
            var_name = f"data_{safe_data_name}"
            
            data_obj = {
                'name': data_name,
                'size': size,
                'producer': producer_name,
                'consumers': consumers,
                'var_name': var_name
            }
            all_data.append(data_obj)
            
            node_outputs[producer_name].append(data_obj)
            
            for consumer_name in consumers:
                if consumer_name in node_inputs:
                    node_inputs[consumer_name].append(data_obj)
                else:
                    print(f"Warning: Consumer {consumer_name} not found in nodes list")

    # Start generating code
    lines = []
    
    # 1. File Header & Includes
    lines.append("/*")
    lines.append(" * freertos_dag_example.c")
    lines.append(" *")
    lines.append(" * Auto-generated DAG example from config.json")
    lines.append(" */")
    lines.append("")
    lines.append("#include <stddef.h>")
    lines.append("#include <stdint.h>")
    lines.append("#include <string.h>")
    lines.append("")
    lines.append('#include "FreeRTOS.h"')
    lines.append('#include "cu_offload.h"')
    lines.append('#include "dag_runtime.h"')
    lines.append('#include "portmacro.h"')
    lines.append('#include "task.h"')
    lines.append('#include "uart16550.h"')
    lines.append("")
    
    # 2. Defines
    lines.append("#define NUM_WORKERS (configNUMBER_OF_CORES)")
    lines.append("#define DAG_QUEUE_LEN 8")
    lines.append("#define DAG_ASYNC_DONE_QUEUE_LEN 8")
    lines.append("")

    # 3. Context Definition
    lines.append("/* Node Context Structure */")
    lines.append("typedef struct {")
    lines.append("  uint32_t cu_id;")
    lines.append("  uint32_t job_id;")
    lines.append("  DagNode* node;")
    lines.append("} CuNodeCtx_t;")
    lines.append("")

    # 4. Worker Function (vCuNodeWork)
    lines.append("/* Worker function running on CU or CPU */")
    lines.append("static void vCuNodeWork(void* arg) {")
    lines.append("  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;")
    lines.append("")
    lines.append('  LOGF("Node %s start on core %d\\n", ctx->node->name,')
    lines.append("       (uint32_t)portGET_CORE_ID());")
    lines.append("")
    lines.append("  ctx->node->async_pending = 1U;")
    lines.append("  uint32_t actual_cu = vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);")
    lines.append('  LOGF("Node %s submitted to CU %d on core %d\\n", ctx->node->name, actual_cu,')
    lines.append("       (uint32_t)portGET_CORE_ID());")
    lines.append("}")
    lines.append("")

    # 5. Define Data Variables
    lines.append("/* Data Objects */")
    for d in all_data:
        lines.append(f"static DagData {d['var_name']};")
    lines.append("")

    # 6. Define Node Variables
    lines.append("/* Node Objects */")
    for n in nodes:
        lines.append(f"static DagNode node{n['name']};")
    lines.append("")

    # 7. Define Contexts
    lines.append("/* Node Contexts */")
    for n in nodes:
        lines.append(f"static CuNodeCtx_t ctx{n['name']};")
    lines.append("")

    # 8. Define Arrays (Successors, Inputs, Outputs)
    lines.append("/* Arrays for adjacency and data pointers */")
    for n in nodes:
        name = n['name']
        
        # Successors
        successors = []
        seen_succ = set()
        for out in node_outputs[name]:
            for cons in out['consumers']:
                if cons not in seen_succ:
                    successors.append(cons)
                    seen_succ.add(cons)
        
        if successors:
            lines.append(f"static DagNode* succOf{name}[{len(successors)}];")
        
        # Inputs
        inputs = node_inputs[name]
        if inputs:
            lines.append(f"static DagData* inputs{name}[{len(inputs)}];")
            
        # Outputs
        outputs = node_outputs[name]
        if outputs:
            lines.append(f"static DagData* outputs{name}[{len(outputs)}];")
    lines.append("")

    # 9. Initialization Function
    lines.append("static void vDagInit(void) {")
    
    # Init Data
    lines.append("  /* Initialize Data */")
    for d in all_data:
        lines.append(f"  memset(&{d['var_name']}, 0, sizeof({d['var_name']}));")
        lines.append(f"  {d['var_name']}.name = \"{d['name']}\";")
        lines.append(f"  {d['var_name']}.sizeBytes = {d['size']}U;")
        lines.append(f"  {d['var_name']}.consumers = {len(d['consumers'])}U;")
    lines.append("")

    # Init Nodes
    lines.append("  /* Initialize Nodes */")
    for n in nodes:
        name = n['name']
        inputs = node_inputs[name]
        outputs = node_outputs[name]
        
        # Calculate successors based on outputs
        successors = []
        seen_succ = set()
        for out in outputs:
            for cons in out['consumers']:
                if cons not in seen_succ:
                    successors.append(cons)
                    seen_succ.add(cons)

        lines.append(f"  memset(&node{name}, 0, sizeof(node{name}));")
        lines.append(f"  node{name}.name = \"{name}\";")
        lines.append(f"  node{name}.run = {worker_func};")
        lines.append(f"  node{name}.arg = (void*)&ctx{name};")
        
        in_size = sum(d['size'] for d in inputs)
        out_size = sum(d['size'] for d in outputs)
        
        lines.append(f"  node{name}.inputSizeBytes = {in_size}U;")
        lines.append(f"  node{name}.outputSizeBytes = {out_size}U;")
        lines.append(f"  node{name}.computeDelayMs = {n.get('compute_delay', 'UINT32_MAX')};")
        lines.append(f"  node{name}.storageSizeBytes = {n.get('storage_size', 0)}U;")
        
        lines.append(f"  node{name}.numInputs = {len(inputs)}U;")
        if inputs:
            lines.append(f"  node{name}.inputs = inputs{name};")
        else:
            lines.append(f"  node{name}.inputs = NULL;")
            
        lines.append(f"  node{name}.numOutputs = {len(outputs)}U;")
        if outputs:
            lines.append(f"  node{name}.outputs = outputs{name};")
        else:
            lines.append(f"  node{name}.outputs = NULL;")
            
        lines.append(f"  node{name}.indegree = {len(inputs)}U;")
        
        lines.append(f"  node{name}.numSuccessors = {len(successors)}U;")
        if successors:
            lines.append(f"  node{name}.successors = succOf{name};")
        else:
            lines.append(f"  node{name}.successors = NULL;")
            
        mode = n.get('mode', 'DAG_NODE_ASYNC')
        lines.append(f"  node{name}.mode = {mode};")
        lines.append("")
        
        # Init Context
        lines.append(f"  /* Context for {name} */")
        lines.append(f"  ctx{name}.cu_id = {default_cu_id};")
        lines.append(f"  ctx{name}.job_id = {n.get('job_id', 0)}U;")
        lines.append(f"  ctx{name}.node = &node{name};")
        lines.append("")

    # Link Arrays
    lines.append("  /* Link Arrays */")
    for n in nodes:
        name = n['name']
        
        # Inputs
        inputs = node_inputs[name]
        for i, d in enumerate(inputs):
            lines.append(f"  inputs{name}[{i}] = &{d['var_name']};")
            
        # Outputs
        outputs = node_outputs[name]
        for i, d in enumerate(outputs):
            lines.append(f"  outputs{name}[{i}] = &{d['var_name']};")
            
        # Successors
        successors = []
        seen_succ = set()
        for out in outputs:
            for cons in out['consumers']:
                if cons not in seen_succ:
                    successors.append(cons)
                    seen_succ.add(cons)
        
        for i, succ_name in enumerate(successors):
            lines.append(f"  succOf{name}[{i}] = &node{succ_name};")
            
    lines.append("}")
    lines.append("")

    # 10. Start Function
    lines.append("/* ===================== 对外启动接口 ===================== */")
    lines.append("void vStartDagDemo(void) {")
    lines.append("  vDagInit();")
    lines.append("  vCuInit();")
    lines.append("")
    lines.append("  DagRuntimeConfig cfg = {.workerCount = NUM_WORKERS,")
    lines.append("                          .workerPriority = tskIDLE_PRIORITY + 2,")
    lines.append("                          .workerStackWords = 512,")
    lines.append("                          .asyncPriority = tskIDLE_PRIORITY + 3,")
    lines.append("                          .asyncStackWords = 512,")
    lines.append("                          .readyQueueLen = DAG_QUEUE_LEN,")
    lines.append("                          .asyncDoneQueueLen = DAG_ASYNC_DONE_QUEUE_LEN,")
    lines.append("                          .useCoreAffinity = 1};")
    lines.append("")
    lines.append("  vDagRuntimeStart(&cfg);")
    lines.append("")
    
    # Find root nodes (indegree 0)
    roots = [n['name'] for n in nodes if len(node_inputs[n['name']]) == 0]
    if roots:
        lines.append(f"  DagNode* root = &node{roots[0]};")
        lines.append("  vDagSubmitReadyNode(root);")
    else:
        lines.append("  /* No root node found */")
        
    lines.append("}")
    lines.append("")
    lines.append("/*-----------------------------------------------------------*/")

    with open(output_file, 'w') as f:
        f.write('\n'.join(lines))
    
    print(f"Generated {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python generate_dag.py <config.json> <output.c>")
        sys.exit(1)
    
    generate_c_code(sys.argv[1], sys.argv[2])
