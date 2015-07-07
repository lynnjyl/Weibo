# !/bash/bin
mpirun -np 20 ./merge 17 1 1 13 0 1
mpirun -np 20 ./merge 17 1 1 13 20 1
mpirun -np 20 ./merge 17 2 14 13 0 1
mpirun -np 20 ./merge 17 2 14 13 20 1
mpirun -np 20 ./merge 1 1 1 2 0 0
mpirun -np 20 ./merge 1 1 1 2 20 0



# mpirun -np 20 ./merge 1 1 1 17 0 2
# mpirun -np 20 ./merge 1 1 1 17 20 2
