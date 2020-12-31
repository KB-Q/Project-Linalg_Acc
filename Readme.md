# A RISC-V based Linear Algebra accelerator for SoCs

This the master repository for the source code of the project "A RISC-V based Linear Algebra accelerator for SoC designs", as part of the EE2003 course in IIT Madras. 

**Project member: Karthik Balaji, EP18B033.**

The source code is organized into the following:

- __linalg-benchmarks__: contains the API framework, the HPCC benchmarks, x86, RISC-V and CUDA

- __gem5__: The gem5 repository snapshot with the additional code for the accelerator

- __riscv-tools__: The complete RISC-V toolchain modified for the accelerator. Only a handful of the sub-repositories have been modified

- __sched-framework__: Task scheduling framework for multicore accelerator architectures. This is work-in-progress at the moment.


## Installing riscv-tools
I roughly followed the installation guide for the [riscv-tools meta-repository](https://github.com/riscv/riscv-tools), although I have edited a few submodules for the project.

First, put the following in the `.bashrc` file:

    export RISCV=/path/to/install/riscv/toolchain
    export PATH=$PATH:$RISCV/bin

Then, install the ubuntu packages from the [riscv-tools](https://github.com/riscv/riscv-tools) guide:

    sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev

Additionally, I ran into some issues and had to install the following packages:

    sudo apt-get install expat python babeltrace gettext

Then, `cd` into the riscv-tools directory and run the following build scripts:

    ./build.sh

This will build the newlib cross-compiler, which is what we want. We __don't__ want to build the glibc cross-compiler. Also, `spike` and `pk` are built along with the compiler, which will be needed for functional simulation.

## Testing the riscv toolchain

To make sure the RISC-V compiler with the accelerator extension is working correctly, we will first cross-compile the test scripts in the linalg-benchmarks repository, and then we will run them on the `spike` functional simulator

First, change directories into `linalg-benchmarks/api`. Then run the following command:

    mkdir out
    make test_api \
         test_data_movement_dp \
         test_data_execution_dp \
         test_data_movement_sp \
         test_data_execution_sp

This will build all the functional test scripts to make sure the API can be compiled successfully. The purpose of `test_api` is to be inspected by objdump manually to make sure everything looks correct. Next, we want to test the other four API test files using the spike simulator. Run the following 4 commands in shell, and make sure all tests are passing for each of them:

    SCRATCH_SIZE=16 spike --extension=la_core pk out/test_data_movement_dp
    SCRATCH_SIZE=16 spike --extension=la_core pk out/test_data_execution_dp
    SCRATCH_SIZE=16 spike --extension=la_core pk out/test_data_movement_sp
    SCRATCH_SIZE=16 spike --extension=la_core pk out/test_data_execution_sp

The `SCRATCH_SIZE=16` is a bad hack to pass parameters to the extension within the spike simulator by using environment variables instead of command line arguments. It means the scratchpad to simulate should be 2^16 bytes, or 64 kB, but the scratchpad size can be any power of 2.

## cross-compiling GSL for RISC-V

The next step is cross-compiling GNU Scientific Library for RISC-V. We need GSL in order to run the HPCC benchmarks on the RISC-V platform, since we use GSL to verify the correctness of the results. First, download [GSL sources](https://www.gnu.org/software/gsl/).

    cd ~
    wget http://mirrors.syringanetworks.net/gnu/gsl/gsl-latest.tar.gz
    tar xzf gsl-latest.tar.gz

Then `cd` into the gsl directory and run the following:

    ./configure --host=riscv64-unknown-elf --prefix=$RISCV
    make
    make install



## Building the HPCC benchmarks

Now we will cross-compile the modified HPCC benchmarks for the accelerator to be run on the spike simulator. First change directories into `linalg-benchmarks/benchmarks`. Then run the following:

    mkdir out
    make dgemm_la_core_sweep \
     dstream_la_core_sweep \
     dfft_la_core_sweep \
     drandom_access_la_core_sweep \
     dlu_solve_la_core_sweep \
       ptrans_la_core_sweep \
     dtrsm_la_core_sweep 

This will build the 6 modified HPCC benchmarks (and DTRSM, a BLAS-3 routine) and put the output binaries in the `out` folder. Each of the 7 benchmarks takes slightly different parameters which can be seen by taking a look at the `main()` function for each of them. For starters, here are simple command lines to run each of the benchmarks on the spike functional simulator using a 64 kB scratchpad and arbitrary workload sizes. __NOTE__: all matrix and vector data is randomly generated floating point numbers. The random number generator can use a different seed via `--seed=X`, where `X` is a positive integer.

The following will run DGEMM with 64x64 sized matrices, and a block size of 64x64

    SCRATCH_SIZE=16 spike --extension=la_core pk out/dgemm_la_core_sweep --size=64 --bs=64 --scratch_size=16

The following will run the STREAM benchmark with vector sizes of 2^12
    
    SCRATCH_SIZE=16 spike --extension=la_core pk out/dstream_la_core_sweep --size=12 --scratch_size=16

The following will run the 1-D FFT benchmark with a vector size of 2^12. 
    
    SCRATCH_SIZE=16 spike --extension=la_core pk out/dfft_la_core_sweep --log_size=12 --scratch_size=16

The following will run the Random Access benchmark with a table size of 2^16. Note that the application doesn't require `--scratch_size` but spike still requires `SCRATCH_SIZE=16`.
    
    SCRATCH_SIZE=16 spike --extension=la_core pk out/drandom_access_la_core_sweep --log_size=16

The following will run the HPL benchmark with a matrix size of 64x64
    
    SCRATCH_SIZE=16 spike --extension=la_core pk out/dlu_solve_la_core_sweep --log_size=6 --scratch_size=16

The following will run the PTRANS benchmark with a matrix size of 64x64
    
    SCRATCH_SIZE=16 spike --extension=la_core pk out/ptrans_la_core_sweep --log_size=6 --scratch_size=16

The following will run the DTRSM kernel with a matrix size of 64x64

    SCRATCH_SIZE=16 spike --extension=la_core pk out/dtrsm_la_core_sweep --size=64 --scratch_size=16

## Building gem5 models

Now I built the gem5 models and ran the HPCC benchmarks on gem5 to get cycle-accurate results. We can run the HPCC benchmarks using any of the following CPU models:

- __AtomicSimpleCPU__: everything instruction takes 1 cycle
- __TimingSimpleCPU__: timing accurate, but only 1-deep pipeline
- __MinorCPU__: pipelined, timing accurate CPU model

The configuration scripts for running these models can be found in the `gem5/configs/la_core` folder. The following examples will use the `MinorCPU` model. The config scripts are respectively:

- configs/la_core/atomic_simple_la_core.py
- configs/la_core/full_timing_la_core.py
- configs/la_core/minor_la_core.py

To run the HPCC benchmarks and DTRSM on the pipelined model in gem5, with the same workload arguments as above, use the following command lines.

    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/dgemm_la_core_sweep --size=64 --bs=64 --scratch_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/dstream_la_core_sweep --size=12 --scratch_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/dfft_la_core_sweep --log_size=12 --scratch_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/drandom_access_la_core_sweep --log_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/dlu_solve_la_core_sweep --log_size=6 --scratch_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/ptrans_la_core_sweep --log_size=6 --scratch_size=16"
    
    ./build/RISCV_LA_CORE/gem5.opt ./configs/la_core/minor_la_core.py --cmd="../linalg-benchmarks/benchmarks/out/dtrsm_la_core_sweep --size=64 --scratch_size=16"



## Building x86 HPCC benchmarks

The HPCC benchmarks have also been written for the x86 superscalar platform. to build these, I installed GSL and FFTW3 using `apt-get`:

    sudo apt-get install libfftw3-dev libgsl-dev

Then, change directories into `linalg-benchmarks/benchmarks` and run the following command:

    make dgemm_x86_sweep \
         dstream_x86_sweep \
         dfft_x86_sweep \
         drandom_access_x86_sweep \
         dlu_solve_x86_sweep \
         ptrans_x86_sweep

To verify they work, I ran them directly on my local machine such as:

    ./out/dgemm_x86_sweep --size=64 --bs=64
    ./out/dstream_x86_sweep --size=4096
    ./out/dfft_x86_sweep --log_size=12
    ./out/drandom_access_x86_sweep --log_size=12
    ./out/dlu_solve_x86_sweep --log_size=6
    ./out/ptrans_x86_sweep --log_size=6



## Building x86 Superscalar gem5 model

Now I built the gem5 x86 superscalar model and ran the x86 HPCC benchmarks using that configuration, which gives us greater control over the x86 system parameters. To do this, change directories into `gem5` and run the following command:

    scons -j10 build/X86/gem5.opt

This will build the x86 models for us. After this finishes, I ran the HPCC benchmarks using the X86 configuration file located in `gem5/configs/la_core/x86_O3`. For example, here are the gem5 command lines for the above x86 HPCC benchmarks that were run on my machine:

    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/dgemm_x86_sweep --size=64 --bs=64"
    
    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/dstream_x86_sweep --size=4096"
       
    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/dfft_x86_sweep --log_size=12"
    
    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/drandom_access_x86_sweep --log_size=12"
    
    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/dlu_solve_x86_sweep --log_size=6"
    
    ./build/X86/gem5.opt ./configs/la_core/x86_O3.py --cmd="../linalg-benchmarks/benchmarks/out/ptrans_x86_sweep --log_size=6"

All of these worked, which means I have successfully built and ran the x86 benchmarks in gem5.

## Building CUDA HPCC Benchmarks

This was a painful setup and debugging process, and one should beware that running the CUDA HPCC in gem5-gpu may not be worth the time they spend setting this up, debugging and waiting for the very slow gpgpu-sim simulation results.

For the undeterred, first read the [gem5-gpu getting started wiki](https://gem5-gpu.cs.wisc.edu/wiki/start) to understand the general gist of how gem5 and gpgpu-sim are working together here. As we see, we need CUDA Toolkit 3.2 and gcc 4.7 to be installed. I used [Vagrant](https://www.vagrantup.com/) to setup an isolated environment since so many build tools and environment variables are getting messed around with here. I would recommend the same.

After we get CUDA 3.2 and gcc 4.7 installed correctly in the Vagrant Box, we must copy the linalg-benchmarks directory into the `gem5-gpu/benchmarks` folder, since we need to link against a bunch of libraries in there. This is not an ideal workflow and needs to be fixed. 

Also, I installed gem5-gpu inside the Vagrant Box by following the [installation guide wiki](https://gem5-gpu.cs.wisc.edu/wiki/start). Now I built the CUDA binaries inside the Vagrant Box using the following commands:

    make dgemm_cuda_sweep \
         dstream_cuda_sweep \
         dfft_cuda_sweep \
         drandom_access_cuda_sweep \
         dlu_solve_cuda_sweep \
         ptrans_cuda_sweep

We now have binaries that can be run on gem5-gpu. To run the binaries on the gem5-gpu simulator, change directories into `gem5` (within Vagrant Box), and run something similar to the following command lines:

    ./build/X86_VI_hammer_GPU/gem5.opt ../gem5-gpu/configs/se_fusion.py --clusters=1 --cores_per_cluster=2 --gpu-core-clock=1GHz --cpu-type=detailed --mem-size=8GB --cpu-clock=3GHz --caches --l2cache --access-host-pagetable -c /home/vagrant/gem5-gpu/benchmarks/linalg-benchmarks/benchmarks/out/ptrans_cuda_sweep -o "--log_size=6"

**NOTE:** Absolute paths to the workload is a MUST (bug in gem5-gpu), and all the arguments to the workload MUST be passed in using the `-o` flag. Command lines for DGEMM, FFT, STREAM, Random Access and HPL are all the same as for PTRANS above, except the `-o` workload arguments differ. Refer to the source files for exact usage.

## Future developments:

#### Developing gem5

The accelerator extension touches a lot of c++ source files in the gem5 respository, but mainly, you should be looking at `src/arch/RISC-V` for ISA changes and `src/cpu/la_core` for micro-architectural changes. Sometimes, both will have to be changed if new instructions with new functionality are added to the ISA.

After you make those changes, just build using `scons -j10 build/RISCV_LA_CORE/gem5.gpu` as described in the installation section.

The configuration scripts are written in python. These scripts just hook together the hardware pieces right before simulation, and are useful for parameter sweeps. You will only need to edit these to add a new parameter to any of the SimObject Python files in the src/ directory.
