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
	int i = 0;
	int nlines = 0;
	string output;
	stringstream i_str;
	string str;
	// int filesize = atoi(argv[2]);
	string num = argv[2];
	
	while(i < 10)
	{
		nlines = 0;
		i_str << i;
		string output = num +"-"+ i_str.str() + ".txt";
		cout << output << endl;
		ofstream ofs(output.c_str(),ios::out);
		
		while(nlines < 500000 && (!ifs.eof()))
		{
			getline(ifs, str, '\n');
			nlines++;
			ofs << str << endl;
		}
		cout << "file " << i << " has finished" << endl;
		ofs.close();
		i++;
		i_str.str("");
	}


	return 0;
}