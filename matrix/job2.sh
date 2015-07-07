
#! /bin/bash

for (( i=9; i<=9; i++))
do
file="0${i}"
input0="$file-00"
input1="$file-01"
input2="$file-02"
input3="$file-03"
input4="$file-04"
output0="$file-00.txt"
output1="$file-01.txt"
output2="$file-02.txt"
output3="$file-03.txt"
output4="$file-04.txt"
echo "s00"
mpirun -np 8 ./map ../../WeiboData/$input0 ../../WeiboData/$output0
echo "s01"
mpirun -np 8 ./map ../../WeiboData/$input1 ../../WeiboData/$output1
echo "s02"
mpirun -np 8 ./map ../../WeiboData/$input2 ../../WeiboData/$output2
echo "s03"
mpirun -np 8 ./map ../../WeiboData/$input3 ../../WeiboData/$output3
echo "s04"
mpirun -np 8 ./map ../../WeiboData/$input4 ../../WeiboData/$output4
cd ../../WeiboData
out="part0${i}.txt"
cat $output0 $output1 $output2 $output3 $output4 > $out

done
