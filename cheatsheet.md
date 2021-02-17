Debug in mpi mode
* compile using `make debug=true`.
* `mpirun --print-all-exitcodes -np 2 xterm -e bash -c "gdb -x commands.gdb --args ./nbody.out -f res/data/input_64.txt -i 500 -o false; sleep 60"`
* For each process:
  * Debug in VSCode using using the 'Just attach' preset.
  * Select the nbody.out process
* Pause execution.
* Step up until you can see the `z` variable in main.c.
* Set the value to 1 so that the while loop terminates.
* Continue through code as normal.
