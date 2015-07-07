#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <algorithm>

#include "mpi.h"

using namespace std;
const int NumOfLines = 2710045;

void getid(string str, int &id)
{
	int pos;
	string id_str;
	pos = str.find_first_of(":");
	id_str = str.substr(0, pos);
	// cout << "id string is " << id_str << endl;
	id = atoi(id_str.c_str());
	// cout << "integer is " << id << endl;
}

bool userid_cmp(string a, string b) 
{ 
    int id_a, id_b;
	getid(a, id_a);
	getid(b, id_b);
	return (id_a<id_b);
}


int main(int argc, char *argv[])
{
	int rank, size;
	int i;
	//int ids[NumOfLines];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	stringstream rankstr;
	rankstr << rank;
	string num = argv[1];
	string inputfile = num + "-0" + rankstr.str() ;
	cout << "rank " << rank <<" inputfile is " << inputfile << endl;
	string outputfile = inputfile + "temp.txt";
	
	ifstream ifs(inputfile.c_str());
	string datastring[NumOfLines];
	for(i = 0; i < NumOfLines; i++)
	{
		getline(ifs, datastring[i], '\n');
	}
	cout << "rank " << rank << " finishes reading"  << endl;
	sort(datastring, datastring+NumOfLines, userid_cmp);
	cout << "rank " << rank << " finishes sorting"  << endl;

	/*test*/
	//for(i = 0; i < NumOfLines; i++)
	//{
	//	 getid(datastring[i], ids[i]);
	//	 cout << ids[i] << endl;
	//}
	/*end*/
	
	ofstream ofs(outputfile.c_str(), ios::out);
	cout << outputfile << endl;
	for(i = 0; i < NumOfLines; i++)
	{
		ofs << datastring[i] << endl;
	}
	
	
	MPI_Finalize();

}
