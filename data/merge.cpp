#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "mpi.h"
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <sys/time.h>

using namespace std;


int FindMin(vector <string> &cmp_array, vector <int> &cmp_index)
{
	int index = 0;
	int array_index = cmp_index[0];
	string min = cmp_array[0];
	int size = cmp_array.size();
	for(int i = 0; i < size; i++)
	{
		if(strcmp(min.c_str(), cmp_array[i].c_str()) > 0)
		{
			min = cmp_array[i];
			array_index = cmp_index[i];
			index = i;
		}
	}
	//cout << "min is " << min << endl;
	//cout << "array_index is " << array_index << endl;
	cmp_array.erase(cmp_array.begin() + index);
	cmp_index.erase(cmp_index.begin() + index);
	return array_index;
}
int main(int argc, char * argv[])
{
	int rank, size;
	int task = atoi(argv[1]);
	int round = atoi(argv[2]);
	int file_index = atoi(argv[3]);
	int nfile = atoi(argv[4]);
	int rank_index = atoi(argv[5]);
	int flag = atoi(argv[6]);
	
	vector <string> cmp_array;				/*store the ids to be compared*/
	vector <vector <string> > ids;
	vector <int> cmp_index;
	ids.resize(nfile);
	int *position = new int[nfile];			/*record the position*/
	int *NumOfIds = new int[nfile];
	string in_filename, out_filename;
	ifstream ifs;
	int i;
	int NumOfFile = nfile;
	int index;
	int num = 0;
	
	//int first = 20;
	
	struct  timeval start_t;
	struct  timeval end_t;
	//struct 	timeval t1, t2, t3, t4;
	gettimeofday(&start_t,NULL);
	unsigned long diff = 0;
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	
	stringstream rank_temp, i_temp;
	rank_temp << (rank+rank_index);
	stringstream task_str, round_str;
	task_str << task;
	round_str << round;
	// rank_temp << rank;
	string id_temp;
	int *names = new int[nfile];
	// int names[17] ={3,5,6,7,9,12,13,14,15,17,18,19,20,21,22,23,24};
	//int a;
	for(i = 0; i < nfile; i++)
	{
		names[i] = i+file_index;
	}
	
	gettimeofday(&start_t,NULL);
	/*create the file names*/
	for(i = 0; i < nfile; i++)
	{
		position[i] = 0;
		i_temp << names[i];
		if(flag == 1)
			in_filename = "./t"+ task_str.str() + "/rank" + rank_temp.str() + "_" + i_temp.str() + ".txt";
		if(flag == 0)
			in_filename = "./t" + task_str.str() + "/rank" + rank_temp.str() + "_merge" + task_str.str() + "_" + i_temp.str() + ".txt";
		if(flag == 2)
			in_filename = "./done/rank" + rank_temp.str() + "_merge" + i_temp.str() + ".txt";
	
		if(rank == 0)
			cout << in_filename << endl;
		ifs.open(in_filename.c_str());
		
		while(!ifs.eof())
		{
			ifs >> id_temp;
			ids[i].push_back(id_temp);	
		}
		ifs.close();
		i_temp.str("");
		NumOfIds[i] = ids[i].size();
		//cout << ids[i][0] <<endl;
	}
	// gettimeofday(&end_t,NULL);
	// diff = 1000000 * (end_t.tv_sec-start_t.tv_sec)+ end_t.tv_usec-start_t.tv_usec;
	// cout << "read file " << diff << endl;
	/*result to store the sorted ids*/
	vector <string> result;
	/*output file*/
	
	
	for(i = 0; i <nfile; i++)
	{
		cmp_array.push_back(ids[i][0]);
		cmp_index.push_back(i);
	}
	
	
	index = FindMin(cmp_array, cmp_index);
	string LastWrite = ids[index][position[index]];
	result.push_back(LastWrite);
	num++;
	position[index]++;
	
	
	if(position[index] < NumOfIds[index])
	{
		cmp_array.push_back(ids[index][position[index]]);	
		cmp_index.push_back(index);
	}
	else
	{
		NumOfFile--;
	}
	while(NumOfFile)
	{
		index = FindMin(cmp_array, cmp_index);
		if(ids[index][position[index]] != LastWrite)
		{
			LastWrite = ids[index][position[index]];
			result.push_back(LastWrite);
			num++;
			
			// if(num%10000 == 0)
				// cout << num << endl;
		}
		/*erase the smallest id and update the cmp_array and cmp_index*/
		position[index]++;
		if(position[index] < NumOfIds[index])
		{
			cmp_array.push_back(ids[index][position[index]]);	
			cmp_index.push_back(index);
		}
		else
		{
			NumOfFile--;
			// cout << NumOfFile << endl;
		}
		
		// cout << "array index is " << index << endl;
		// cout << "LastWrite is " << LastWrite << endl;
		// cout << "position of each array is " << endl;
		// for(i = 0 ; i < nfile; i++)
			// cout << "array " << i << ": " << position[i] << endl;
	
		//cin>>a;
	
	}
	if(flag == 1)
		out_filename = "./t" + task_str.str()  + "/rank" + rank_temp.str() + "_merge" + task_str.str() + "_" + round_str.str() + ".txt";
	if(flag == 0)
		out_filename = "rank" + rank_temp.str() + "_merge"+task_str.str() + ".txt";
	if(flag == 2)
		out_filename = "rank" + rank_temp.str() + ".txt";
	ofstream ofs(out_filename.c_str(),ios::out);

	int result_size = result.size();
	for(i = 0; i <result_size; i++)
		ofs << result[i] << endl;
	
	ofs.close();
	MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday(&end_t,NULL);
	diff = 1000000 * (end_t.tv_sec-start_t.tv_sec)+ end_t.tv_usec-start_t.tv_usec;
	cout << "end " << diff << endl;
	cout << rank  << " done" << endl;
	MPI_Finalize();
	return 0;
}