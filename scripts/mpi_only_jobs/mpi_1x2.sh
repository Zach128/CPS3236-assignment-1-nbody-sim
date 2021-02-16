#!/bin/sh
#
# Your job name
#$ -N NBODY_1x2
#
# Use current working directory
#$ -cwd
#
# pe (Parallel environment) request. Set your number of processors here.
#$ -pe openmpi 2
#
# Run job through bash shell
#$ -S /bin/bash
# Tell SGE that this is an array job, with "tasks" to be numbered 1 to 3
#$ -t 1-3

# If modules are needed, source modules environment:
. /etc/profile.d/modules.sh

# Add any modules you might require:
module add shared openmpi/gcc/64/1.8.8

echo $PE_HOSTFILE

# Run your application
mpirun -np 2 ./nbody.out -f res/data/input_1024.txt -i 1000 -o false
