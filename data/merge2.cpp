#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
// #include "mpi.h"
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <sys/time.h>

using namespace std;


int FindMin(string s1, string s2)
{
	if(strcmp(s1.c_str(), s2.c_str()) > 0)
		return 0;
	else
		return 1;
}

int main(int argc, char * argv[])
{
	int rank, size;
	
	// MPI_Init(&argc, &argv);
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int index = atoi(argv[1]);
	// int num = atoi(argv[2]);
	
	string cmp_array[2];
	
	vector <string> data1;
	vector <string> data2;
	
	string temp;
	
	stringstream rank_str;
	rank_str << index;
	
	string name1 = "../matrix/rank" + rank_str.str() + ".txt";
	string name2 = "rank" + rank_str.str() + "_merge17.txt";
	string name3 = "rank" + rank_str.str() + ".txt";
	cout << name1 << endl;
	cout << name2 << endl;
	ifstream ifs1(name1.c_str());
	ifstream ifs2(name2.c_str());
	
	/*read file1*/
	while(!ifs1.eof())
	{
		getline(ifs1, temp, '\n');
		// cout << temp << endl;
		data1.push_back(temp);
	}
	ifs1.close();
	/*read file2*/
	while(!ifs2.eof())
	{
		getline(ifs2, temp, '\n');
		// cout << temp << endl;
		data2.push_back(temp);
	}
	ifs2.close();

	vector <string> output;
	string LastWrite = "a";
	string MinStr;
	int pos1, pos2;
	pos1 = pos2 = 0;
	int size1 = data1.size();
	int size2 = data2.size();
	int min;
	// cout << "ok" << endl;
	while(pos1 < size1 && pos2 < size2)
	{
		min = FindMin(data1[pos1], data2[pos2]);
		switch(min)
		{
			case 1:
				MinStr = data1[pos1];
				pos1++;
				break;
			case 0:
				MinStr = data2[pos2];
				pos2++;
				break;
		}
		if(MinStr != LastWrite)
		{
			output.push_back(MinStr);
			LastWrite = MinStr;
		}
	}
	if(pos1 < size1)
	{
		cout << "file 1 finished" <<endl;
		int k = pos1;
		while(pos1 < size1)
		{
			output.push_back(data1[pos1]);
			pos1++;
		}
	}
	else
	{
		cout << "file 1 finished" <<endl;
		int k = pos2;
		while(pos2 < size2)
		{
			output.push_back(data2[pos2]);
			pos2++;
		}
	}
	
	ofstream ofs(name3.c_str());
	int result_size = output.size();
	for( int i = 0; i <result_size; i++)
		ofs << output[i] << endl;
	ofs.close();
	
	
	
	// MPI_Finalize();
	return 0;
}