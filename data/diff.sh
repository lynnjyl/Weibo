# !/bash/bin

for ((i=0;i<40;i++))
do
	filename="rank${i}_merge1.txt"
	echo -e $filename
	diff ./done/$filename $filename
	
done

done