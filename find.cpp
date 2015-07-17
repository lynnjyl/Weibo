//
//  find.cpp
//  
//
//  Created by XoX on 15/7/17.
//
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>

using namespace std;
const int lines = 5000000;


bool cmp(string str)
{
    int pos;
    string id_str;
    pos = str.find.first_of(" ");
    id_str = str.substr(0, pos);
    int id = atoi(id_str);
    
    if(id >= 265696336)
    {
        cout << id << endl;
        return true;
    }
    else
        return false;
    
}

int main(int argc, char * argv[])
{
    long offset;
    offset = 0;
    string str;
    
    string filename = argv[1];
    string output = argv[2];
    ifstream ifs(filename.c_str());
    
    
    for(int i = 0; i < lines; i++)
    {
        ifs.seekg(offset, ifs.beg);
        getline(ifs, str, '\n');
        offset += str.length() + 1;
        if(cmp(str))
            break;
    }
    if (cmp(str))
    {
        ofstream ofs(output.c_str(),ios::app);
        ofs << str << endl;
        for(i; i < lines; i++)
        {
            ifs.seekg(offset, ifs.beg);
            getline(ifs, str, '\n');
            offset += str.length() + 1;
            ofs << str << endl;
        }
    }
    
    
    return 0;
}