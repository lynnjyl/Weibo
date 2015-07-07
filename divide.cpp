#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main(int argc, char * argv[])
{
	string filename = argv[1];
	
	ifstream ifs(filename.c_str());
	int i = 1;
	int size = 0;
	string output;
	stringstream i_str;
	string str;
	int filesize = atoi(argv[2]);
	
	while(!ifs.eof())
	{
		size = 0;
		i_str << i;
		string output = "test" + i_str.str() + ".json";
		ofstream ofs(output.c_str(),ios::out);
		
		while(size < filesize && (!ifs.eof()))
		{
			getline(ifs, str, '\n');
			size += str.length();
			ofs << str << endl;
		}
		cout << "file " << i << " has finished" << endl;
		ofs.close();
		i++;
		i_str.str("");
	}


	return 0;
}