# PIM TEST

## 1 Setup 

```sh
# after clone this repo

# pull submodule - the benchmarks
git submodule update --init

# for future submodule update
# git submodule update --recursive --remote

# download UPMEM SDK and setup env
source ./download_sdk_and_setup_env.sh
```

## 3 Something else 

### 3.1 UPMEM 2025 SDK is not good to use with the benchmark 

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

