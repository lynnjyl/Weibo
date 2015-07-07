# !/bash/bin
count=0
for ((i=1; i<40; i++))
do
	filename="rank${i}.txt"
	size=$(wc -l $filename | awk '{print $1}')
	count=$(($count+$size))
done
echo -e $count