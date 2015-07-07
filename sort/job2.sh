#! /bin/bash
cd ../../WeiboData
for ((i=2;i<=3;i++))
do
	cd ~/Data
	id="0${i}"
	filename="part${id}.txt"
	echo $filename
	out="${id}-"
	echo "start spliting"
	split -l 500000 -d $filename $out
	#rm $filename

	cd ~/Weibo.project/sort
	echo "start sorting"
	mpirun -np 10 ./sort ../../Data/${id}
	
	echo "start merging"
	./merge ${id}

	rm "${id}*"
	
done
echo  "done"
