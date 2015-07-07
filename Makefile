CXX=mpicxx
CXXFLAGS=-Wall

all: read

test1: read
	mpirun -np 8 ./read
	
test2: read
	mpirun -np 40 -hostfile hosts ./read

read: read.o 
	$(CXX) -std=c++11 -o read *.o 
	

	
#read.o: read.cpp
#	mpic++ -c read.cpp

# modid.o: modid.cpp userid.h
	# mpic++ -c modid.cpp

clean:
	rm read read.o


