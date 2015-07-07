
#! /bin/bash

for (( i=25; i<=31; i++))
do
file="${i}"
cd ../../WeiboData
ls
split -d -l 1000000 $file s
cd ../Weibo.project/matrix
echo "s00"
mpirun -np 8 ./map ../../WeiboData/s00 ../../WeiboData/s00.txt
echo "s01"
mpirun -np 8 ./map ../../WeiboData/s01 ../../WeiboData/s01.txt
echo "s02"
mpirun -np 8 ./map ../../WeiboData/s02 ../../WeiboData/s02.txt
echo "s03"
mpirun -np 8 ./map ../../WeiboData/s03 ../../WeiboData/s03.txt
echo "s04"
mpirun -np 8 ./map ../../WeiboData/s04 ../../WeiboData/s04.txt
cd ../../WeiboData
out="part${file}.txt"
echo $out
cat s00.txt s01.txt s02.txt s03.txt s04.txt > $out
rm s*
#rm $file

done
echo "done"
