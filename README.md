# CPS-3236-Assignment 1

zcau0001
---

## How to build

0. Ensure `gcc`, `mpich`, and `openmpi` are installed. Alternatively, use the provided dev container to install the project.
    * Open Visual Studio Code.
    * Ensure the Remote Containers extension is installed.
    * Open the Show all Commands prompt and select `Reopen in Container`.
    * Wait for the dev container to be built.
    * Once file menu is visible, you should be able to build and run the project.
1. Run `make` from the root project to build the project.
    * Run `make use_omp=false` to disable OpenMP in the compiled program.
    * Run `make debug=true` to enable the debug flag during compilation.
    * Run `make debug=mpi` to enable MPI-specific debugging functionality.
    * Run `make clean` to remove any compiled objects and the currently-compiled program.
2. The compiled program will be named `nbody.out`. Run `./nbody.out -h` to see all available parameters.

## How to use the project

* `./nbody.out -f <path-to-file>` Runs the application using a given file as input. Results are output in the directors `res/out/` relative to the project.
* `mpirun --map-by ppr:1:node ./nbody.out -f <path-to-file>` Run the application in a distributed memory environment. Runs one instance of the program per-node/host. Changing the number in `ppr:1:node` changes how many instances are run per-node/host.

## How to debug the project.

For debugging a single instance of application:
* Clean the project using `make clean` then build using `make debug=true` to enable debugging.
* Open VSCode and set breakpoints as necessary throughout the code.
* Run either the `Nbody with file` or the `Nbody without file` debug preset.
* VSCode will pause execution on the first breakpoint encountered.

For debugging an MPI instance of the project:
* `mpirun -np 2 ./nbody.out`
* For each process:
  * Debug in 2 VSCode windows each using the `Just attach` preset.
  * Select the nbody.out process.
* Pause execution.
* Step up until you can see the `z` variable in main.c.
* Set the value to 1 so that the while loop terminates.
* Continue through debugging the code as normal.
