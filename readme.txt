* How to build: make in root directory.
    barneshut-s - Serial
    barneshut-m - MIC native version (Built using -mmic flag)
    barneshut-p - Parallel version (-offload flag)
        barneshut-p - Parallel Host-only version (Use -no-offload flag)
    gen - Dataset generator

* How to run:
    bin/barneshut-* <dataset> <#years>
        Example: bin/barneshut-s datasets/128_1 0.1
        This will simulate the given dataset for a lifetime of 0.1 * year in seconds.
    bin/gen <#particles>
        This will generate a uniformly distributed dataset with the given #particles.

==============================

* Data:
    First line of dataset must contain the particle count.
    Rest lines are of the following form:
        mass posX posY posZ velX velY velZ

* Visualization:
    Generate an output by running a simulation (bin/barneshut-s datasets/128_1 0.1).
    A corresponding out_* directory will be created.
    Execute ./animate.sh out* to generate an animated gif of the simulation.
    Please note that this might take a lot of time for large output file count.

==============================

* Misc:
    Use ./cleanup.sh to quickly delete old simulations.

    There is a commanding Makefile in the root directory, which uses all four Makefiles in the src.
    To change them, navigate to the corresponding implementation.

    Each implementation contains a constants.h header file. You can change the parameters in that file to modify the behavior of the simulator, and adjust values for specific datasets (such as the smoothening factor or theta, etc.)
    You can also remove the "_NO" form "#define TIME_NO" to enable timing measurement of code sections, as described in the report.

    This project was implemented by me from scratch, without given baseline serial code.

==============================

* Env for compilation:
    module purge
    module load intel
    . ${INTEL_INCLUDE}/../../bin/compilervars_arch.sh
    export LC_ALL=C

* Env for Offload and Host-only modes:
    export MIC_ENV_PREFIX=PHI
    export PHI_KMP_AFFINITY=compact
    export PHI_KMP_PLACE_THREADS=56c,4t
    export PHI_OMP_NUM_THREADS=224
    export PHI_USE_2MB_BUFFERS=64K

    export KMP_AFFINITY=compact
    export OMP_NUM_THREADS=8

* Env for Native mode (Required libraries were copied to /tmp):
    export LD_LIBRARY_PATH=/tmp
    export KMP_AFFINITY=compact
    export KMP_PLACE_THREADS=57c,4t
    export OMP_NUM_THREADS=228
    export USE_2MB_BUFFERS=64K