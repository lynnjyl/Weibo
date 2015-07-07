#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
// #include "mpi.h"
#include <string>
#include <string.h>
#include <sstream>
#include <vector>

using namespace std;
int main(int argc, char * argv[])
{
	ifstream ifs("nohup.out");
	ofstream ofs("addids.txt");
	string str, id;
	int pos;
	int start;
	// int times= 250;
	while(!ifs.eof())
	{
		getline(ifs, str, '\n');
		pos = str.find_first_of("S");
		// cout <<"pos is " << pos << endl;
		if(pos!=string::npos)
		{
			// cout <<str << endl;
			start = str.find_first_of("s", pos);
			start += 2;
			id = str.substr(start);
			ofs << id << endl;
			// cout <<id << endl;
		}
		// times--;
	}
	return 0;


}