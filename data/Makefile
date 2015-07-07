CXX=mpicxx
CXXFLAGS=-Wall

round=1;

all: merge

test1:merge
	mpirun -np 20 ./merge 27 1 $(round)
	
test2:merge
	mpirun -np 20 ./merge 27 27 $(round)
	
merge: merge.o 
	$(CXX) -std=c++11 -o merge *.o 
	
	
clean:
	rm merge merge.o