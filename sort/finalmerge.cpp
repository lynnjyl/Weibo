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
//const int nfile = 10;
const int nfile = 33;
const int lines = 10000;        //read every time per file
const int round = 16500;
const int times = 500;
const string Max = "299999999 ";

double wallclock(void)
{     struct timeval tv;
     struct timezone tz;
     double t;

     gettimeofday(&tv, &tz);

     t = (double)tv.tv_sec*1000;
     t += ((double)tv.tv_usec)/1000.0;

     return t;
}
void GetDataStrings(string num, int i, long &offset, string *datastring, int &ReadTimes)
{
	ReadTimes++;
	stringstream i_str;
	i_str << i;
	string inputfilename;
	if(i <10)
		inputfilename = "../../Data/p0" + i_str.str() + ".txt";
	else
		inputfilename = "../../Data/p" + i_str.str() + ".txt";
	cout << inputfilename << ": " << ReadTimes << endl;
//	cout << inputfilename << endl;
	// string inputfilename = num + "-" + i_str.str() + ".txt";
	// cout << inputfilename << endl;
	ifstream ifs(inputfilename.c_str());
	ifs.seekg(offset, ifs.beg);
	long offset_temp = offset;
	string str_temp;
	
	for(int  j = 0; j < lines; j++)
	{
		getline(ifs, str_temp, '\n');
		offset_temp += str_temp.length();
		datastring[j] = str_temp;
		// cout << str_temp << endl;
	}
	//cout << str_temp << endl;
	offset = offset_temp + lines;
	// cout << offset << endl;
}

void GetID(string str, int &id)
{
	int pos;
	string id_str;
	pos = str.find_first_of(" ");
	//cout << "pos is " << pos << endl;	
    	id_str = str.substr(0, pos);
//	cout << str << endl;
//	cout << id_str << endl;
//	int a;
//	cin >> a;
	id = atoi(id_str.c_str());
	//cout << "integer is " << id << endl;
}


int FindMin(string *cmp_array)
{
	int ids[nfile];
	int Min_ID, Min_Index;
	for(int i = 0; i < nfile; i++)
	{
		GetID(cmp_array[i], ids[i]);
		if(i == 0)
		{
			Min_ID = ids[i];
			Min_Index = i;
		}
		else
		{
			if(ids[i] < Min_ID)
			{
				Min_ID = ids[i];
				Min_Index = i;
			}
		}
	}
    
	//cout << Min_ID << endl;
	return Min_Index;
}

int main(int argc, char * argv[])
{
	string num = argv[1];		/*No. of part*/
	string DataStrings[nfile][lines];	/*data from files*/
	string ResultStrings[lines];
	int position[nfile];
	long offset[nfile];
	int ReadTimes[nfile];
	
//	int offset_temp;
	stringstream i_str;
	string inputfilename;
	string outputfilename = argv[2];
	string str_temp;
	string cmp_array[nfile];
	int Min_Index;
	int NumberOfLines;
	
	memset(offset, 0, sizeof(offset));
	memset(position, 0, sizeof(position));
	memset(ReadTimes, 0, sizeof(ReadTimes));
	
	double t1 = wallclock();
	/*read 10000 lines from each file and store in DataStrings*/
	for(int i = 0; i < nfile; i++)
	{
		GetDataStrings(num, i, offset[i], DataStrings[i], ReadTimes[i]);
		cmp_array[i] = DataStrings[i][0];
	}	
	
	for(int i = 0; i < round; i++)
	{
		NumberOfLines = 0;
		while(NumberOfLines < lines)
		{
			Min_Index = FindMin(cmp_array);
			ResultStrings[NumberOfLines] = DataStrings[Min_Index][position[Min_Index]];
			NumberOfLines++;
			position[Min_Index]++;
			if(position[Min_Index] >= lines)
			{
				if(ReadTimes[Min_Index] < times)
					GetDataStrings(num, Min_Index, offset[Min_Index], DataStrings[Min_Index], ReadTimes[Min_Index]);
				else
					DataStrings[Min_Index][0] = Max;
				position[Min_Index] = 0;
			}
			cmp_array[Min_Index] = DataStrings[Min_Index][position[Min_Index]];
		}
		ofstream ofs(outputfilename.c_str(), ios::app);
		while(NumberOfLines > 0)
		{
			ofs << ResultStrings[lines - NumberOfLines] << endl;
			NumberOfLines--;
		}
		ofs.close();
	}
	double t2 = wallclock();
	cout << "time is " << t2 - t1 << endl;
}
