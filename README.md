# PIM TEST

The directory structure:

- download_sdk_and_setup_env.sh: the UPMEM SDK setup script for Ubuntu 22.04 LTS - x86_64
- demos: simple example of UPMEM code
- prim-benchmarks: the folder contains benchmark code for some demo UPMEM code. It is a good material to learn how to write UPMEM code and how to evaluate the performance


## 1 Setup 

```sh
# clone this repo
git clone https://github.com/LangInteger/pim_test.git

# pull submodule - the benchmarks repo
# originally at: https://github.com/CMU-SAFARI/prim-benchmarks for paper "Benchmarking Memory-centric Computing Systems: Analysis of Real Processing-in-Memory Hardware"
# forked at https://github.com/LangInteger/prim-benchmarks
git submodule update --init

# for future submodule update
# git submodule update --recursive --remote

# download UPMEM SDK and setup env
# use source instead of bash to make env virables valid
source ./download_sdk_and_setup_env.sh

# after this, UPMEM SDK commandline tools, e.g., dpu-upmem-dpurte-clang and dpu-lldb should be available.
```

## 2 Demo code

Go to the `demos` directory at first.

```sh
cd ./demos 
```

### 2.1 Hello world

Compile `helloworld.c` and run it solely (just as run it on a DPU).

```sh 
# compile 
dpu-upmem-dpurte-clang -DNR_TASKLETS=12 -o helloworld helloworld.c 
# enter dpu-lldb
dpu-lldb
# run the helloworld program inside dpu-lldb 
file helloworld
process launch
# exit dpu-lldb
exit
```

You will see "Hello World!" is printed. It is printed for 12 times because we compile with parameter `-DNR_TASKLETS=12`, so there will be 12 tasklets executing the same code in the same DPU.

`dpu-lldb` is for debug purpose. Real world DPU program is executed by host program. In `helloworld_host.c`, we specified how many DPU we want to use, and explictly specify `helloworld` is the program for DPU to execute. We can compile the host code and run it.

```sh
# compile 
gcc --std=c99 helloworld_host.c -o helloworld_host `dpu-pkg-config --cflags --libs dpu`
# execute host program 
./helloworld_host
```

We specified two DPU in the host program, so you will see "Hello World!" is printed for 24 times in total.

### 2.2 Global variable update

In DPU program, the global variable is stored on memory shared by all the tasklets. Update operation on global variables leads to race condition and give unexpected result.

The `global_var.c` is a DPU program that will increase the global variable `i` (initially zero) by one and then print the updated value. The corresponding host program starts one DPU to execute `global_var`.

```sh 
# compile 
dpu-upmem-dpurte-clang -DNR_TASKLETS=12 -o global_var global_var.c 
gcc --std=c99 global_var_host.c -o global_var_host `dpu-pkg-config --cflags --libs dpu`
# execute 
./global_var_host
```

With the above commands, the ideal output is that each tasklet print the value of `i` from 1 to 12, as there are exactly 12 tasklet. However, due to the race condition mentioned above, the output is more random. In my test, it is:

```text
=== DPU#0x0 ===
tasklet 0: i = 2
tasklet 1: i = 3
tasklet 2: i = 4
tasklet 3: i = 5
tasklet 4: i = 6
tasklet 5: i = 7
tasklet 6: i = 8
tasklet 7: i = 9
tasklet 8: i = 10
tasklet 9: i = 11
tasklet 10: i = 12
tasklet 11: i = 12
```

### 2.3 Global variable updatge synchronized 

To solve the above problem, we use mutex provided by UPMEM SDK to synchronize the code block updateing `i` and print its value.

```sh 
# compile 
dpu-upmem-dpurte-clang -DNR_TASKLETS=12 -o global_var_ordered global_var_ordered.c 
gcc --std=c99 global_var_ordered_host.c -o global_var_ordered_host `dpu-pkg-config --cflags --libs dpu`
# execute 
./global_var_ordered_host
```

With the above updated code, now the output is as expected:

```text
=== DPU#0x0 ===
tasklet 0: i = 1
tasklet 1: i = 2
tasklet 2: i = 3
tasklet 3: i = 4
tasklet 4: i = 5
tasklet 5: i = 6
tasklet 6: i = 7
tasklet 7: i = 8
tasklet 8: i = 9
tasklet 9: i = 10
tasklet 10: i = 11
tasklet 11: i = 12
```

## 3 Benchmark dive-in

### 3.1 BFS 

- directory: `./prim-benchmarks/BFS/`
- DPU code: `./prim-benchmarks/BFS/dpu/task.c`
- Host code: `ls ./prim-benchmarks/BFS/host/app.c`

It turns out that BFS is one of the two benchmark examples that pay huge overhead of inter-DPU synchronization via the host CPU.

There are two level of synchronizations:

- At the host CPU level, it broadcasts the complete current frontier to all the DPUs and distributes vertices for future visit evenly. Then CPU retrives the next frontier produced by each DPU and compute the complete next frontier by union. So each level of vertice visiting is **synchronized** at the host CPU.
- At the DPU level, when executing one level of vertice visiting, vertices in the current frontier belonged to current DPU are assigned to different tasklets evenly and they do the next step visiting concurrently. After every tasklet has finished, then the DPU merge the node they visited and the next frontier. So each level of vertice visiting inside DPU is **synchronized** at the DPU.

To run BFS via host program

```shell
cd prim-benchmarks/BFS
NR_DPUS=32 NR_TASKLETS=16 make all 
./bin/host_code -v 0 -f ./data/loc-gowalla_edges.txt 
```

To run BFS using the python script, you need to modify the `rootdir` variable in the script to be the absolute path to the `prim-benchmarks` directory on your machine. The python script is making experienments with different number of tasklets and DPUs settings.

```shell
cd prim-benchmarks

# update rootdir in run_strong_rank.py

# run_strong can directly run, as it tests a fixed workload with different level of DPU resources. The fixed workload has been provided
python run_strong_rank.py

# you can see the result at the `./prim-benchmarks/BFS/profile/` directory

# you need to generate matGraph at first and then properly configure the file path
# https://github.com/cmuparlay/pbbsbench/blob/master/testData/graphData/rMatGraph.C
# run weak sims to increase workload with more resource given, so different level of input should be provided - which is not given by the suite
# python3 run_weak.py BFS
```

## 4 Something else 

### 4.1 UPMEM 2025 SDK failed to work with the benchmark code

After setup with UPMEM 2025 SDK, compile `BFS` host program fail with following output. UPMEM 2024 SDK does not have this problem.

```text
Running = NR_DPUS=16 NR_TASKLETS=4 make all
rm -f   bin/.NR_DPUS_*_NR_TASKLETS_*.conf
touch   bin/.NR_DPUS_16_NR_TASKLETS_4.conf
cc -o bin/host_code host/app.c -Wall -Wextra -g -Isupport -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=4 -DNR_DPUS=16 
/bin/sh: 1: dpu-pkg-config: not found
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:50: undefined reference to `dpu_alloc'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:51: undefined reference to `dpu_load'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:52: undefined reference to `dpu_get_nr_dpus'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:76: undefined reference to `dpu_set_dpu_iterator_from'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyToDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /tmp/cccqoYoT.o:/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: more undefined references to `dpu_copy_to' follow
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:76: undefined reference to `dpu_set_dpu_iterator_next'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:166: undefined reference to `dpu_launch'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:182: undefined reference to `dpu_set_dpu_iterator_from'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyFromDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:33: undefined reference to `dpu_copy_from'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:182: undefined reference to `dpu_set_dpu_iterator_next'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyFromDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:33: undefined reference to `dpu_copy_from'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:209: undefined reference to `dpu_set_dpu_iterator_from'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:209: undefined reference to `dpu_set_dpu_iterator_next'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyToDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_copy_to'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:236: undefined reference to `dpu_set_dpu_iterator_from'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:236: undefined reference to `dpu_set_dpu_iterator_next'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyFromDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:33: undefined reference to `dpu_copy_from'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:33: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:308: undefined reference to `dpu_set_dpu_iterator_from'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:310: undefined reference to `dpu_log_read'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:308: undefined reference to `dpu_set_dpu_iterator_next'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `copyToDPU':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/mram-management.h:29: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /tmp/cccqoYoT.o: in function `main':
/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:166: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:310: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:52: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:51: undefined reference to `dpu_error_to_string'
/usr/bin/ld: /tmp/cccqoYoT.o:/home/ubuntu/gitrepo/pim_test/prim-benchmarks/BFS/host/app.c:50: more undefined references to `dpu_error_to_string' follow
```

