<<<<<<< HEAD

# GEM5-freertos

## Getting started

To make it easy for you to get started with GitLab, here's a list of recommended next steps.

Already a pro? Just edit this README.md and make it your own. Want to make it easy? [Use the template at the bottom](#editing-this-readme)!

## Add your files

- [ ] [Create](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#create-a-file) or [upload](https://docs.gitlab.com/ee/user/project/repository/web_editor.html#upload-a-file) files
- [ ] [Add files using the command line](https://docs.gitlab.com/ee/gitlab-basics/add-file.html#add-a-file-using-the-command-line) or push an existing Git repository with the following command:

```
cd existing_repo
git remote add origin http://10.0.0.17:6655/syshu/gem5-freertos.git
git branch -M main
git push -uf origin main
```

## Integrate with your tools

- [ ] [Set up project integrations](http://10.0.0.17:6655/syshu/gem5-freertos/-/settings/integrations)

## Collaborate with your team

- [ ] [Invite team members and collaborators](https://docs.gitlab.com/ee/user/project/members/)
- [ ] [Create a new merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html)
- [ ] [Automatically close issues from merge requests](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically)
- [ ] [Enable merge request approvals](https://docs.gitlab.com/ee/user/project/merge_requests/approvals/)
- [ ] [Set auto-merge](https://docs.gitlab.com/ee/user/project/merge_requests/merge_when_pipeline_succeeds.html)

## Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] [Get started with GitLab CI/CD](https://docs.gitlab.com/ee/ci/quick_start/index.html)
- [ ] [Analyze your code for known vulnerabilities with Static Application Security Testing (SAST)](https://docs.gitlab.com/ee/user/application_security/sast/)
- [ ] [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/ee/topics/autodevops/requirements.html)
- [ ] [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/ee/user/clusters/agent/)
- [ ] [Set up protected environments](https://docs.gitlab.com/ee/ci/environments/protected_environments.html)

---

# Editing this README

When you're ready to make this README your own, just edit this file and use the handy template below (or feel free to structure it however you want - this is just a starting point!). Thanks to [makeareadme.com](https://www.makeareadme.com/) for this template.

## Suggestions for a good README

Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

## Name

Choose a self-explaining name for your project.

## Description

Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.

## Badges

On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

## Visuals

Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

## Installation

Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

## Usage

Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

## Support

Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

## Roadmap

If you have ideas for releases in the future, it is a good idea to list them in the README.

## Contributing

State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment

Show your appreciation to those who have contributed to the project.

## License

For open source projects, say how it is licensed.

## Project status

If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
================================================================================================================================================================================================================================================================================================================================================

# The gem5 Simulator

This is the repository for the gem5 simulator. It contains the full source code
for the simulator and all tests and regressions.

The gem5 simulator is a modular platform for computer-system architecture
research, encompassing system-level architecture as well as processor
microarchitecture. It is primarily used to evaluate new hardware designs,
system software changes, and compile-time and run-time system optimizations.

The main website can be found at [http://www.gem5.org](http://www.gem5.org).

## Testing status

**Note**: These regard tests run on the develop branch of gem5:
[https://github.com/gem5/gem5/tree/develop](https://github.com/gem5/gem5/tree/develop).

[![Daily Tests](https://github.com/gem5/gem5/actions/workflows/daily-tests.yaml/badge.svg?branch=develop)](https://github.com/gem5/gem5/actions/workflows/daily-tests.yaml)
[![Weekly Tests](https://github.com/gem5/gem5/actions/workflows/weekly-tests.yaml/badge.svg?branch=develop)](https://github.com/gem5/gem5/actions/workflows/weekly-tests.yaml)
[![Compiler Tests](https://github.com/gem5/gem5/actions/workflows/compiler-tests.yaml/badge.svg?branch=develop)](https://github.com/gem5/gem5/actions/workflows/compiler-tests.yaml)

## Getting started

A good starting point is [http://www.gem5.org/about](http://www.gem5.org/about), and for
more information about building the simulator and getting started
please see [http://www.gem5.org/documentation](http://www.gem5.org/documentation) and
[http://www.gem5.org/documentation/learning_gem5/introduction](http://www.gem5.org/documentation/learning_gem5/introduction).

## Building gem5

To build gem5, you will need the following software: g++ or clang,
Python (gem5 links in the Python interpreter), SCons, zlib, m4, and lastly
protobuf if you want trace capture and playback support. Please see
[http://www.gem5.org/documentation/general_docs/building](http://www.gem5.org/documentation/general_docs/building) for more details
concerning the minimum versions of these tools.

Once you have all dependencies resolved, execute
`scons build/ALL/gem5.opt` to build an optimized version of the gem5 binary
(`gem5.opt`) containing all gem5 ISAs. If you only wish to compile gem5 to
include a single ISA, you can replace `ALL` with the name of the ISA. Valid
options include `ARM`, `NULL`, `MIPS`, `POWER`, `RISCV`, `SPARC`, and `X86`
The complete list of options can be found in the build_opts directory.

See https://www.gem5.org/documentation/general_docs/building for more
information on building gem5.

## The Source Tree

The main source tree includes these subdirectories:

* build_opts: pre-made default configurations for gem5
* build_tools: tools used internally by gem5's build process.
* configs: example simulation configuration scripts
* ext: less-common external packages needed to build gem5
* include: include files for use in other programs
* site_scons: modular components of the build system
* src: source code of the gem5 simulator. The C++ source, Python wrappers, and Python standard library are found in this directory.
* system: source for some optional system software for simulated systems
* tests: regression tests
* util: useful utility programs and files

## gem5 Resources

To run full-system simulations, you may need compiled system firmware, kernel
binaries and one or more disk images, depending on gem5's configuration and
what type of workload you're trying to run. Many of these resources can be
obtained from [https://resources.gem5.org](https://resources.gem5.org).

More information on gem5 Resources can be found at
[https://www.gem5.org/documentation/general_docs/gem5_resources/](https://www.gem5.org/documentation/general_docs/gem5_resources/).

## Getting Help, Reporting bugs, and Requesting Features

We provide a variety of channels for users and developers to get help, report
bugs, requests features, or engage in community discussions. Below
are a few of the most common we recommend using.

* **GitHub Discussions**: A GitHub Discussions page. This can be used to start
  discussions or ask questions. Available at
  [https://github.com/orgs/gem5/discussions](https://github.com/orgs/gem5/discussions).
* **GitHub Issues**: A GitHub Issues page for reporting bugs or requesting
  features. Available at [https://github.com/gem5/gem5/issues](https://github.com/gem5/gem5/issues).
* **Jira Issue Tracker**: A Jira Issue Tracker for reporting bugs or requesting
  features. Available at [https://gem5.atlassian.net/](https://gem5.atlassian.net/).
* **Slack**: A Slack server with a variety of channels for the gem5 community
  to engage in a variety of discussions. Please visit
  [https://www.gem5.org/join-slack](https://www.gem5.org/join-slack) to join.
* **gem5-users@gem5.org**: A mailing list for users of gem5 to ask questions
  or start discussions. To join the mailing list please visit
  [https://www.gem5.org/mailing_lists](https://www.gem5.org/mailing_lists).
* **gem5-dev@gem5.org**: A mailing list for developers of gem5 to ask questions
  or start discussions. To join the mailing list please visit
  [https://www.gem5.org/mailing_lists](https://www.gem5.org/mailing_lists).

## Contributing to gem5

We hope you enjoy using gem5. When appropriate we advise sharing your
contributions to the project. [https://www.gem5.org/contributing](https://www.gem5.org/contributing) can help you
get started. Additional information can be found in the CONTRIBUTING.md file.

# 实现了简单的uint8加减法计算单元

## 清理

```
scons -c build/RISCV/gem5.opt
```

## 编译

```
scons -j$(nproc) build/RISCV/gem5.opt
```

## 编译计算单元测试elf

```
cd tests/test-progs/riscv32/riscv32_compute_test
```

```
make all
```

## gem5直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf
```

## 调试--debug-flags=All

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_cu_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf > gem5_cu_test_log_all.txt
```

## 使用minor cpu运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf
```

## 使用minor cpu运行--debug-flags=All

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_cu_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf > gem5_cu_test_log_min_All.txt
```

# 另开一个终端

```
m5term localhost 3456
```
