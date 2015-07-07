#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include "mpi.h"
#include <math.h>
#include<boost/unordered_map.hpp> 
#include <sys/time.h>
// #include <unordered_map>
// #include <unordered_map>
// #include <boost.h>

using namespace std;
int RankOfRem[41] = {1,1,1,1,2,2,2,2,2,2,
					 2,3,3,3,3,4,4,4,4,4,
					 4,4,5,5,5,5,5,6,6,6,
					 6,6,6,7,7,7,7,7,7,7,7};
// int offset[8] = {0, 0, 37947166, 75087138, 113154746, 151900473, 189320290, 227076120};
int offset[8] = {0, 0, 37962895, 75118342, 113202098, 151962104, 189398233, 227169647};
					 
typedef boost::unordered_map<string,int> umap;
umap m;
// typedef unordered_map<string, int> mymap;
size_t hash_value(const string& str)  
{  
    size_t seed = 0;  
    boost::hash_combine(seed, boost::hash_value(str));  
    return seed;  
} 
double wallclock(void)
{	struct timeval tv;
	struct timezone tz;
	double t;

	gettimeofday(&tv, &tz);

	t = (double)tv.tv_sec*1000;
	t += ((double)tv.tv_usec)/1000.0;

	return t;
}
void readidlist(int rank, vector <string>&idlist, int ReadIndex, int ReadNum)
{
	stringstream index;
	string filename;
	string str;
	ifstream input;
	for(int i = ReadIndex; i < ReadIndex+ReadNum; i++)
	{
		index << i;
		filename = "rank"+index.str()+".txt";
		input.open(filename.c_str());
		while(!input.eof())
		{
			getline(input, str, '\n');
			idlist.push_back(str);
		}
		input.close();
		idlist.erase(idlist.end());
		index.str("");
	}
}
int GetRankId(string str)
{
	int bitnum = str.length();
	int fnum = bitnum/2;
	int bnum = bitnum - bitnum/2;
	string f, b;
	f = str.substr(0, fnum);
	b = str.substr(fnum, bnum);	
	
	long long front = atoll(f.c_str());
	long long back = atoll(b.c_str());
	
	long long rem1, rem2, rem3, rem;
	rem1 = front%40;
	rem2 = static_cast<long long>(pow(10, bnum));
	rem2 = rem2%40;
	rem3 = back%40;
	rem = (int)((rem1*rem2 +rem3)%40);
	return RankOfRem[rem];
}

void PrintPairs(vector <int> NewIDList, vector <int> FirstIndex)
{
	// cout << "PrintPairs" << endl;
	// cout << "NewIDList is " << NewIDList.size() << endl;
	// cout << "FirstIndex is " << FirstIndex.size() << endl;
	int NowIndex, NextIndex;
	int Row, Column;
	int i, j;
	for(i = 0; i < FirstIndex.size(); i++)
	{
		NowIndex = FirstIndex[i];
		Row = NewIDList[NowIndex];
		if(i == FirstIndex.size()-1)
			NextIndex = NewIDList.size();
		else	
			NextIndex = FirstIndex[i+1];
		// cout << "i = " << i << ", NowIndex is " << NowIndex << ", NextIndex is " << NextIndex << endl;
		for(j =  NowIndex+1; j < NextIndex; j++)
		{
			Column = NewIDList[j];
			cout << Row << ", " << Column << endl;
		}
	}

}

void WriteFile(vector <int> NewIDList, vector <int> FirstIndex, string outputfilename)
{
	ofstream ofs(outputfilename.c_str(), ios::app);
	if (!ofs)
	{
		cerr<<"open error"<<endl;
		exit(1);
	}
	
	int NowIndex, NextIndex;
	int Row, Column;
	int i, j;
	for(i = 0; i < FirstIndex.size(); i++)
	{
		NowIndex = FirstIndex[i];
		Row = NewIDList[NowIndex];
		if(i == FirstIndex.size()-1)
			NextIndex = NewIDList.size();
		else	
			NextIndex = FirstIndex[i+1];
		// cout << "i = " << i << ", NowIndex is " << NowIndex << ", NextIndex is " << NextIndex << endl;
		ofs << Row;
		for(j =  NowIndex+1; j < NextIndex; j++)
		{
			// Column = NewIDList[j];
			ofs << " " << NewIDList[j];
			// cout << Row << ", " << Column << endl;
		}
		ofs << endl;
	}
}

void getids(vector <string> datastring, vector <int> &NewIDList, vector <int> &FirstIndex)
{
	int NumOfLines = datastring.size(); //cout << "NumOfLines is " << NumOfLines << endl;
	string data, IDString;
	bool flag = 1;
	int pos1, pos2, pos3;
	int rankID, NumOfIDs, FirstIDLength, FirstID;
	int *NewID;
	int i, j, k;
	MPI_Request req[7];
	
	vector <vector <int> > IDLength;
	IDLength.resize(8);
	vector <int> SumOfIDLength(8,0);
	
	string IDs[8];
	MPI_Status status;
	
	for(k = 0; k < NumOfLines; k++)
	{
		//cout << "k = " << k << endl;
		data = datastring[k];
		/*get the first id of one line*/
		pos1 = pos2 = pos3 = 0;
		pos3 = data.find_first_of(']', pos3); if(pos3 < 0) break;
		pos2 = data.find_first_of(':', pos1); 
		pos2 += 2;
		pos1 = data.find_first_of(',', pos2);
		if(data.at(pos2) == 'N')
		{
			pos2 = data.find_first_of('(', pos2);
			pos2++;
			pos1--;
		}
		IDString = data.substr(pos2, pos1-pos2); //cout << "First ID is " << IDString << endl;
		rankID = GetRankId(IDString);			//cout << "rankID is " << rankID << endl;
		FirstIDLength = IDString.length();		//cout << "FirstIDLength is " << FirstIDLength << endl;
		NumOfIDs = 1;
		
		
		MPI_Send(&NumOfIDs, 1, MPI_INT, rankID, 0, MPI_COMM_WORLD);
		MPI_Send(&FirstIDLength, 1, MPI_INT, rankID, 1, MPI_COMM_WORLD);	
		MPI_Send(IDString.c_str(), FirstIDLength, MPI_CHAR, rankID, 2, MPI_COMM_WORLD);	
		MPI_Recv(&FirstID, 1, MPI_INT, rankID, 3, MPI_COMM_WORLD, &status);
		FirstID += offset[rankID];
		
		FirstIndex.push_back(NewIDList.size());
		NewIDList.push_back(FirstID);			//cout << "First New ID is " << FirstID << endl;

		for(i = 0; i < 8; i++)
			IDs[i] = "";
			
		/*get the rest of the ids of the line*/
		flag = 1;
		pos2 = data.find_first_of('[', pos1);
		if(pos2 + 1 == pos3)
			flag = 0;
		pos2 +=2;
		NumOfIDs = 0;
		while(flag)
		{
			pos1 = data.find_first_of(',', pos2);
			if(pos1 > pos3)
			{
				pos1 -= 2;
				flag = 0;
			}
			if(data.at(pos2) == 'N')
			{
				pos2 = data.find_first_of('(', pos2);
				pos2++;
				pos1--;
				IDString = data.substr(pos2, pos1 - pos2);
				pos2 = pos1+3;
			}
			else
			{
				IDString = data.substr(pos2, pos1 - pos2);
				pos2 = pos1 + 2;
			}
			rankID = GetRankId(IDString);
			//cout << "rankID is " << rankID << endl;
			IDs[rankID] += IDString.c_str();
			IDLength[rankID].push_back(IDString.length());
			SumOfIDLength[rankID] += IDString.length();
			//NumOfIDs++;
		}
		/**/
		for(i = 1; i < 8; i++)
		{
			/*send the number of ids */
			NumOfIDs = IDLength[i].size();
			MPI_Isend(&NumOfIDs, 1, MPI_INT, i, 0, MPI_COMM_WORLD, req[i-1]);
		}
		MPI_Waitall(7, req, MPI_STATUS_IGNORE);
		
		for(i = 1; i < 8; i++)
		{
			if(IDLength[i].size() != 0)
			{
				NewID = new int[NumOfIDs];
			
			
			}
		
		
		}
		// for(i = 1; i < 8; i++)
		// {
			// NumOfIDs = IDLength[i].size();
			// //cout << "i = " << i << " NumOfIDs is " << NumOfIDs << endl;
			// MPI_Send(&NumOfIDs, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			// if(NumOfIDs != 0)
			// {
				// NewID = new int[NumOfIDs];
				// MPI_Send(&IDLength[i][0], NumOfIDs, MPI_INT, i, 1, MPI_COMM_WORLD);
				// MPI_Send(IDs[i].c_str(), SumOfIDLength[i], MPI_CHAR, i, 2, MPI_COMM_WORLD);
				// MPI_Recv(&NewID[0], NumOfIDs, MPI_INT, i, 3, MPI_COMM_WORLD, &status);
				// for(j = 0; j < NumOfIDs; j++)
				// {
					// //cout <<"new id " <<  NewID[j] << endl;
					// if(NewID[j] == -1)
					// {
						// int temp = 1;
						// MPI_Send(&temp, 1, MPI_INT, 7, 0, MPI_COMM_WORLD);
						// MPI_Send(&IDLength[i][j],1, MPI_INT, 7, 1, MPI_COMM_WORLD);
						// MPI_Send(IDs[i].c_str(), SumOfIDLength[i], MPI_CHAR, i, 2, MPI_COMM_WORLD);
					
					// }
					// NewID[j] += offset[i];
					// //cout << "the exact id is " << NewID[j] << endl;
					// NewIDList.push_back(NewID[j]);	
				// }
				// delete [] NewID;
			// }
			// IDLength[i].clear();
			// IDs[i] = "";
			// SumOfIDLength[i] = 0;
		// }
	}
	// NumOfIDs = -1;
	// for(i = 1; i < 8; i++)
		// MPI_Send(&NumOfIDs, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
}

void MapIDs(int rank)
{
	// cout << "Rank " << rank << " is ok" << endl;
	int NumOfIDs;
	char *IDs;
	int *IDLength;
	int i, j;
	int SumOfIDLength;
	int startindex;
	int *NewID;
	string str;
	MPI_Status status;
	
	while(true)
	{
		
		MPI_Recv(&NumOfIDs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); //cout <<"Rank " << rank << " Recv NumOfIDs is " << NumOfIDs << endl;
		if(NumOfIDs == -1)
			break;
		if(NumOfIDs > 0)
		{
			IDLength = new int[NumOfIDs];
			NewID = new int[NumOfIDs];
			SumOfIDLength = 0;
			MPI_Recv(&IDLength[0], NumOfIDs, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			//cout << "Rank " << rank << " Recv IDLength is " << IDLength[0] << endl;
			// cout << "Recv IDLength is " << IDLength[1] << endl;
			
			for(i = 0; i < NumOfIDs; i++)
				SumOfIDLength += IDLength[i];
			//cout << "SumOfIDLength is " << SumOfIDLength << endl;
			IDs = new char[SumOfIDLength];
			MPI_Recv(&IDs[0], SumOfIDLength, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
			//cout << "IDs is " << IDs << endl;
			startindex = 0;
			for(i = 0 ; i < NumOfIDs; i++)
			{
				str = "";
				//cout <<"IDLength is " << IDLength[i] << endl;
				//cout << "startindex is " << startindex << endl;
				for(j = startindex ; j < startindex + IDLength[i]; j++)
				{
					str += IDs[j];
					// cout << IDs[j] << endl;
				}
				// cout << str << endl;
				umap::const_iterator got = m.find (str);
				if(got == m.end())
				{
					cout << "not found";
					cout << "IDString is " << str << endl;
					NewID[i] = -1;
				}
				else
				{
					NewID[i] = got->second;
				}
				// cout << got->second << endl;
				startindex = j;
			}
			MPI_Send(&NewID[0], NumOfIDs, MPI_INT, 0, 3, MPI_COMM_WORLD);
			delete [] IDLength;
			delete [] NewID;
			delete [] IDs;
		}
	}
	return;
}

int main(int argc, char * argv[])
{
	double t1,t2;
	t1 = wallclock();
	int rank, size;
	// string idlistfilename = "sidlist.txt";
	string datafilename = argv[1];
	string outputfilename = argv[2];
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	/*index and number of ids for each rank should read from idlist.txt*/
	// int ReadIndex[7] = {0, 37947166, 75087138, 113154746, 151900473, 189320290, 227076120};
	// int ReadNum[7] = {37947165, 37139972, 38067608, 38745727, 37419817, 37755830, 38511782};
	int ReadIndex[7] = {0,4,11,15,22,27,33};
	int ReadNum[7] = {4,7,4,7,5,6,8};
	/*idlist used to store the ids*/
	vector <string> idlist;
	
	// umap m;
	// mymap m;
	if(rank != 0)
	{
		readidlist(rank, idlist, ReadIndex[rank-1], ReadNum[rank-1]);
		int N = idlist.size();
		for(int i = 0; i < N; i++)
		{
			// if(idlist[i] == "2410758570")
				// cout << i << endl;
			m.insert(umap::value_type(idlist[i],i));
			// m.insert({inlist[i],i});
		}
		cout <<rank << " done create map" << endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	/*master read the file*/
	if(rank == 0)
	{
		ifstream ifs(datafilename.c_str());
		string str;
		vector <string> datastring;
		vector <int> NewIDList;
		vector <int> FirstIndex;
		int lines;
		int nline = 10000;
		for(int i = 0; i < 500; i++)
		{
			lines = 0;
			while(lines < nline)
			{
				getline(ifs, str, '\n');
				datastring.push_back(str);
				lines++;
			}
			cout << i << ": there are " << datastring.size() << " lines" << endl;
			getids(datastring, NewIDList, FirstIndex);
			WriteFile(NewIDList, FirstIndex, outputfilename);
			datastring.clear();
			FirstIndex.clear();
			NewIDList.clear();
		}

		getids(datastring, NewIDList, FirstIndex);
		WriteFile(NewIDList, FirstIndex, outputfilename);
		datastring.clear();
		FirstIndex.clear();
		NewIDList.clear();
		
		int NumOfIDs = -1;
		for(int i = 1; i < 8; i++)
			MPI_Send(&NumOfIDs, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		// PrintPairs(NewIDList, FirstIndex);
	}
	else
	{
		// cout << "rank " << rank << " begins to map" << endl;
		MapIDs(rank);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	// if(rank == 0)
	// {
		
	// }
	// MPI_Barrier(MPI_COMM_WORLD);
	t2 = wallclock();
	cout << "time is " << t2-t1 << endl;
	
	
	MPI_Finalize();
	return 0;
}