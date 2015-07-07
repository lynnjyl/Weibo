#! /bin/sh

start=0

for (( i=1; i<=15; i++ ))
do
	filename="r${i}.json"
	echo -e $filename
	lines=100000
	end=$(($start+$lines))
	head -n ${end} t17.json | tail -n ${lines} > $filename
	size=$(du -sk $filename | awk '{print $1}')

	while [[ $size -lt 360000 ]]
	do
		lines=$(($lines+10000))
		end=$(($start+$lines))
		head -n ${end} t17.json | tail -n ${lines} > $filename
		size=$(du -sk $filename | awk '{print $1}')
	done

	lines=$(($lines-10000))
	end=$(($start+$lines))
	start=$end
	head -n ${end} t17.json | tail -n ${lines} > $filename

	size=$(du -sk $filename | awk '{print $1}')
	echo -e $i
	echo -e $lines
	echo -e $size
	echo -e $end
done
echo "end task"

#read PERSON
#echo "Hello, $PERSON"
