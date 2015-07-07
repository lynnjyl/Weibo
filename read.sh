#! /bin/sh


for (( i=5; i<=26; i++))
do
	filename="test${i}.json"
	mpirun -np 40 ./read $filename ${i} 17
done 
