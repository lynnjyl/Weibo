#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <sstream>
#include<sys/sysinfo.h>


using namespace std;




// const string round("1");
struct userid
{
	long long front;
	long long back;
	long long bnum;
};
vector <userid> uid;
int mod(userid uid, int n)
{
	long long rem1, rem2, rem3;
	rem1 = uid.front%n;
	rem2 = static_cast<long long>(pow(10, uid.bnum));
	rem2 = rem2%n;
	rem3 = uid.back%n;
	return (int)((rem1*rem2 +rem3)%n);
}

void check(int rank)
{
	struct sysinfo s_info;
	int error;
	error = sysinfo(&s_info);
	cout << "rank " << rank << "ram：total " << s_info.totalram << " free " << s_info.freeram << endl;
}

void uidset(userid &uid, string str)
{
	//cout << "set uid" << endl;
	int bitnum = str.length();
	int fnum = bitnum/2;
	uid.bnum = bitnum - bitnum/2;
	string f, b;
	f = str.substr(0, fnum);
	b = str.substr(fnum, uid.bnum);
		
	uid.front = atoll(f.c_str());
	uid.back = atoll(b.c_str());
	cout << uid.front  << uid.back << endl;
	// while(1);
}



void getids(string datastring, int nline, vector <userid> &uid, int rank)
{
	userid temp;
	string str;
	
	int i;
	bool flag = 1;
	int pos1, pos2, pos3;
	pos1 = pos2 = pos3 = 0;
	//cout << "ok" << endl;
	
	for(i = 0; i < nline; i++) 	
	{
		//cout << "for loop" << endl;
		flag = 1;
		pos3 = datastring.find_first_of(']', pos3);
		pos2 = datastring.find_first_of(':', pos1);
		pos2 += 2;
		pos1 = datastring.find_first_of(',', pos2);
		//cout << pos1 << " " << pos2 << " " << pos3 << endl;
		if(datastring.at(pos2) == 'N')
		{
			//cout << "Number long" << endl;
			pos2 = datastring.find_first_of('(', pos2);
			pos2++;
			pos1--;
		}
		
		uidset(temp, datastring.substr(pos2, pos1 - pos2));
		uid.push_back(temp);
		pos2 = datastring.find_first_of('[', pos1);
		if(pos2 + 1 == pos3)
			flag = 0;
		pos2 +=2;
		while(flag)
		{
			pos1 = datastring.find_first_of(',', pos2);
			if(pos1 > pos3)
			{
				pos1 -= 2;
				flag = 0;
			}
			if(datastring.at(pos2) == 'N')
			{
				pos2 = datastring.find_first_of('(', pos2);
				pos2++;
				pos1--;
				str = datastring.substr(pos2, pos1 - pos2);
				uidset(temp, str);
				uid.push_back(temp);
				pos2 = pos1 + 3;
			}
			else
			{
				str = datastring.substr(pos2, pos1 - pos2);
				uidset(temp, str);
				uid.push_back(temp);
				
				pos2 = pos1 + 2;
			}
		}
		pos1 = pos2 = pos3 = datastring.find_first_of('{', pos3);
		
	}
	//cout << rank << "finishi get id" << endl;
	
}


int main(int argc, char * argv[])
{
	

	ifstream in("special.json", ios::in);
	istreambuf_iterator<char> beg(in), end;
	string strdata(beg, end);
	in.close();
	vector <userid> uid;
		
	getids(strdata, 2, uid, 0);
	cout << uid.size() << endl;
		
	
	
    return 0;	
}
