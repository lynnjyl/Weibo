#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "mpi.h"
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <sstream>
#include<sys/sysinfo.h>


using namespace std;

struct sysinfo s_info;
//const char *filename = "r1.json";
//string filename;
string inputfilename;
string r;
string fileindex;
// string round;
//stringstream r_str;
long long *idarray;
long long *newid;


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
	int bitnum = str.length();
	int fnum = bitnum/2;
	uid.bnum = bitnum - bitnum/2;
	string f, b;
	f = str.substr(0, fnum);
	b = str.substr(fnum, uid.bnum);
		
	uid.front = atoll(f.c_str());
	uid.back = atoll(b.c_str());
	// if(uid.front == 0 && uid.back == 0)
	// {
		// cout << str << endl;
		// while(1);
	// }
}

int distributelines(int n, int size, int &nline, vector <int> offset)
{
	// int temp;			//the number of lines the last processor should read.
	// int i = 1;				
	// if(n%size == 0)
		// nline = n/size;
	// else
		// nline = n/size + 1;
	// temp = n - (size - 1)*nline;
	// /*send the start and end offset to the processors*/
	// while(i != size - 1)
	// {	
		// MPI_Send(&offset[i*nline], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		// MPI_Send(&offset[(i + 1)*nline], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		// MPI_Send(&nline, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
		// i++;
	// }
	// /*send the start and end offset to the last processor*/
	// MPI_Send(&offset[i*nline], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	// MPI_Send(&offset.back(), 1, MPI_INT, i, 1, MPI_COMM_WORLD);
	// MPI_Send(&temp, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

	// return offset[nline];			//return the end offset of root.
	
	int nline2;
	if(n%size == 0)
		nline = nline2 = n/size;
	else
	{
		nline = n/size + 1;
		nline2 = n/size;
	}
	int temp = n%size;
	for(int i= 1; i < size; i++)
	{
		if(i < temp)
		{
			MPI_Send(&offset[i*nline], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&offset[(i + 1)*nline], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&nline, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Send(&offset[temp*nline + nline2*(i-temp)], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&offset[temp*nline + nline2*(1+i-temp)], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&nline2, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
		}
	
	}
	return offset[nline];
}

/*processors except the root receive the message from the root.*/
void recvoffset(int &startoffset, int &endoffset, int &nline, MPI_Status *status)
{	
	
	MPI_Recv(&startoffset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, status);
	MPI_Recv(&endoffset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, status);
	MPI_Recv(&nline, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, status);
}

void readfile(int startoffset, int endoffset, string &datastring, MPI_Status *status, int rank)
{
	MPI_File file;
	char *temp;
	temp = new char[endoffset - startoffset];
	//cout << rank << ": " << startoffset << " " << endoffset << endl;
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_File_open(MPI_COMM_WORLD, inputfilename.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_at(file, startoffset*sizeof(char), temp, (endoffset - startoffset), MPI_CHAR, status);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
		MPI_File_close(&file);
	datastring = temp;
	
}

void getids(string datastring, int nline, vector <userid> &uid, int rank)
{
	userid temp;
	string str;
	
	int i;
	bool flag = 1;
	int pos1, pos2, pos3;
	pos1 = pos2 = pos3 = 0;
	
	for(i = 0; i < nline; i++) 	
	{
		flag = 1;
		pos3 = datastring.find_first_of(']', pos3); if(pos3 < 0) break;
		pos2 = datastring.find_first_of(':', pos1); 
		pos2 += 2;
		pos1 = datastring.find_first_of(',', pos2);
		if(datastring.at(pos2) == 'N')
		{
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
	MPI_Barrier(MPI_COMM_WORLD);
}

// bool id_compare(string s1, string s2)
// {
	// int l1 = s1.length();
	// int l2 = s2.length();
	// int i;
	
	// if(s1.compare(s2) == 0)
	// {
		// // cout << "s1: " << s1 << endl;
		// // cout << "s2: " << s2 << endl;
		// // cout << "they are the same" << endl;
		// return true;
	// }
	// if(l1 == l2)
	// {
		// for(i = 0; i < l1; i++)	
		// {
			// if(s1[i] != s2[i])
			// {
				// if(s1[i] < s2[i])
					// return true;
				// else 
					// return false;
			// }
		// }
		// //return true;
	// }
	// else
	// {
		// if(l1 < l2)
			// return true;
		// else
			// return false;
	// }

// }

int cstring_cmp(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
	/* strcmp functions works exactly as expected from
	comparison function */ 
} 

// void sort_write(long newid[], int loc, int rank)
void sort_write(int loc, int rank)
{
	
	int bnum, zero_num;
	//vector <string> user_id;
	string _user_id[loc];
	int i;
	stringstream fs, bs;
	size_t strings_len = sizeof(_user_id) / sizeof(char *);
	/*store ids into string array*/
	for(i = 0; i < loc; i++)
	{
		bs << newid[i*3+1];
		fs << newid[i*3];
		
		// if(fs.str() == "0")
		// {
			// user_id.push_back("");
		// }
		// else
		// {
			// user_id.push_back(fs.str());
			// _user_id[i] = fs.str();
		// }
		if(fs.str() != "0")
			_user_id[i] = fs.str();
		
		
		bnum = newid[i*3+2];
		zero_num = bnum - bs.str().length();
		
		while(zero_num > 0)
		{
			//user_id[i] += '0';
			_user_id[i] += '0';
			zero_num--;
		}
		//user_id[i] += bs.str();
		_user_id[i] += bs.str();
		
		// if(_user_id[i] == "0000000000000000")
		// {
			// cout << "fs: " << newid[i*3] << ", bs : " <<newid[i*3+1] << endl;
			// while(1);
		// }
		
		fs.str("");
		bs.str("");	
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
		cout << "rank " << rank << " sort the ids." << endl;
	
	//delete [] newid;
	/*sort the id array*/
	//sort(user_id.begin(), user_id.end(), id_compare);
	qsort(_user_id, strings_len, sizeof(char *), cstring_cmp);
	MPI_Barrier(MPI_COMM_WORLD);
	
	//cout << "rank " << rank << "start to write" <<endl;
	/*write the ids to files*/
	stringstream rank_str;
	rank_str << rank;
	//round_str << round;
	string outputfilename = "/home/yiling/Weibo.project/data/t"+fileindex +"/rank"+rank_str.str()+"_" + r + ".txt";
	ofstream outfile(outputfilename.c_str(),ios::out);
	if (!outfile)
	{
		cerr<<"open error"<<endl;
		exit(1);
	}
	if(rank == 0)
		cout << "rank " << rank << " write " << loc << " elements." << endl;
	// if(rank == 4){
	string last_write = _user_id[0];
	outfile << _user_id[0];
	for(i = 1; i < loc; i++)
	{
		if(last_write.compare(_user_id[i]) != 0)
		{
			outfile << endl << _user_id[i];
			last_write = _user_id[i];
		}
	}
	// cout <<"ok" <<endl;
	// }
	// while(1);
	MPI_Barrier(MPI_COMM_WORLD);
	//cout << "end writing" << endl;
	outfile.close();
}

#define num_of_rem(r,c) (_num_of_rem[r*size+c])
// void messagetrans(int *maxarray, int *_num_of_rem, int rank, int size, long idarray[])
// void messagetrans(int *_num_of_rem, int rank, int size, long idarray[])
// void messagetrans(int *_num_of_rem, int rank, int size)
int messagetrans(int *_num_of_rem, int rank, int size)
{	
	// int rem;
	// while(1);
	MPI_Request req[(size-1)*2];
	int loc = 0;
	int i;
	int startoffset = 0;
	int rstartoffset = 0;
	int startoffsettmp = 0;
	// for(i = 0; i < size; i++)
	// {
		// if(maxarray[i] == rank)
		// {
			// rem = i;
			// break;
		// }
	// }
	for(i = 0; i < size; i++)
	{
		loc += num_of_rem(i, rank);
	}
	// cout << rank << ": " << loc << endl; 
	//MPI_Barrier(MPI_COMM_WORLD);

	newid = new long long[loc*3];
	int num_req = 0;
	
	for(i = 0; i < size; i++)
	{
		if(i != rank)
		{
			// MPI_Isend(&idarray[startoffset], num_of_rem(rank, i)*3, MPI_LONG, maxarray[i], rank, MPI_COMM_WORLD, &req[num_req++]);
			MPI_Isend(&idarray[startoffset], num_of_rem(rank, i)*3, MPI_LONG_LONG_INT, i, rank, MPI_COMM_WORLD, &req[num_req++]);
		}
		else
		{
			startoffsettmp = startoffset; 
		}
		startoffset += num_of_rem(rank, i)*3;
	}
	for(i = 0; i < size; i++)
	{
		if(i != rank)
		{
			// MPI_Irecv(&newid[rstartoffset], num_of_rem(i, rem)*3, MPI_LONG, i, i, MPI_COMM_WORLD, &req[num_req++]);
			MPI_Irecv(&newid[rstartoffset], num_of_rem(i, rank)*3, MPI_LONG_LONG_INT, i, i, MPI_COMM_WORLD, &req[num_req++]);
			rstartoffset += num_of_rem(i, rank)*3;
		}
		else
		{
			memcpy(&newid[rstartoffset], &idarray[startoffsettmp], num_of_rem(i, rank)*3*sizeof(long long));
			// cout << idarray[startoffsettmp] << " " << idarray[startoffsettmp+1] << " " << idarray[startoffsettmp+2]<<endl;
			// cout << newid[rstartoffset] << " " << newid[rstartoffset+1] << " " << newid[rstartoffset+2]<<endl; 
			rstartoffset += num_of_rem(i, rank)*3;
		}
	}
	MPI_Waitall(num_req, req, MPI_STATUSES_IGNORE);
	MPI_Barrier(MPI_COMM_WORLD);
	//cout << "rank " << rank << " begins to sort the ids and write them to file." << endl;
	

	//delete []idarray;
	
	// sort_write(newid, loc, rank);
	//sort_write(loc, rank);
	return loc;

}

#define num_of_rem(r,c) (_num_of_rem[r*size+c])
// void modid(vector <userid> &uid, int size, int rank)
// void modid(int size, int rank)
int modid(int size, int rank)
{
	if(rank == 0)
		cout << "rank " << rank << " begins to do mod" << endl;
	MPI_Request req[size-1];
	vector <vector <userid> > idvec;			// vector stores the uid of different remainders
	idvec.resize(size);
	int num = uid.size();
	int k, j, i;
	int rem;
	int *_num_of_rem = new int[size*size];  	// for the root to store the number of uid of different remainders
	int *pnum_of_rem = new int[size];			// for the processor to store the number of uid of different remainders
	//int *maxarray = new int[size];				// remainder i should be stored in maxarray[i];
	//int *myints = new int[size]; 				// temp array;
	int t;
	/*store id information in array*/
	// long *idarray = new long[num*3];
	idarray = new long long[num*3];
	//int maxsum = 0, tempsum = 0;
	
	
	/*distribute the ids into different vectors according to the remainder*/
	// if(rank == 4){
	for(i = 0; i < num; i++)
	{
		rem = mod(uid[i], size);
		idvec[rem].push_back(uid[i]);
	}
	// cout << "ok" << endl;
	// }
	// while(1);
	/*pnum_of_rem stores the number of uid of different remainders*/
	for(i = 0; i < size; i++)
	{
		t = idvec[i].size();
		// pnum_of_rem[i] = idvec[i].size();
		pnum_of_rem[i] = t;
	}
	/*11.11 store id information in array idarray and bnum*/
	/* k is the index of long array*/
	k = 0;										
	for(i = 0 ; i < size; i++)
	{									
		/*the number of data whose remainder is i is pnum_of_rem[i]*/
		for(j = 0; j < pnum_of_rem[i]; j++)
		{
			idarray[k++] = idvec[i][j].front;
			idarray[k++] = idvec[i][j].back;
			idarray[k++] = idvec[i][j].bnum;
		}
		/*release idvec memory*/
		// idvec[i].clear();
		// vector<userid>().swap(idvec[i]);
	}

	//cout <<rank << "begin to clear the memory" << endl;
	/*release uid memory*/
	// uid.clear();
	// vector <userid> ().swap(uid);

	// if(rank == 3)
		// cout << idvec[1][0].front << " " << idvec[1][0].back << " " << idvec[1][0].bnum << endl;
	//cout <<rank << " begin to send the number " << endl;
	/*the root create a size*size matrix to store the number*/
	int num_req = 0;
	if(rank == 0)
	{
		for(i = 0; i < size; i++)
			num_of_rem(0, i) = pnum_of_rem[i];
		
		for(i = 1; i < size; i++)
		{
			MPI_Irecv(&num_of_rem(i, 0), size, MPI_INT, i, i, MPI_COMM_WORLD, &req[num_req++]);
		}
	}
	else
		MPI_Isend(pnum_of_rem, size, MPI_INT, 0, rank, MPI_COMM_WORLD, &req[num_req++]);
	
	MPI_Waitall(num_req, req, MPI_STATUSES_IGNORE);
	MPI_Barrier(MPI_COMM_WORLD);
	// while(1);
	//cout <<"after barrier" << endl;
	/*the root schedule the message passing*/
	// if(rank == 0)
	// {		
		// for(i = 0; i < size; i++)
			// myints[i] = i;
		// /*find the best schedule*/	
		// do{
			// /*the sum of not transferred elements*/
			// for(i = 0; i < size; i++)
				// tempsum += num_of_rem(myints[i], i);
			// /*find the max tempsum*/
			// if(tempsum > maxsum)
			// {
				// maxsum = tempsum;
				// memcpy(&maxarray[0], &myints[0], sizeof(int)*size);
			// }
			// tempsum = 0;
		// }while( next_permutation(myints, myints + size));
		
		// cout << "the best schedule is " << endl;
		// for(i = 0; i < size; i++)
			// cout << maxarray[i] << " " ;
		// cout << maxsum << endl;		
	// }
	//while(1);
	//MPI_Bcast(maxarray, size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&num_of_rem(0, 0), size*size, MPI_INT, 0, MPI_COMM_WORLD);
	if(rank == 0)
	cout << "rank " << rank << " begins to transfer ids." << endl;
	// messagetrans(maxarray, &num_of_rem(0, 0), rank, size, idarray);
	// messagetrans(&num_of_rem(0, 0), rank, size, idarray);
	return messagetrans(&num_of_rem(0, 0), rank, size);
}


int main(int argc, char * argv[])
{
	
	int rank, size;
	string str;
	
	
	int n, nline;
	MPI_Status status;
	int startoffset, endoffset;
	int loc;
	
	// int a = atoi(argv[1]);

	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	// for(int i=0;i<15;i++)
	// {
		// r_str.str("");
		// cout << "i is " << i << endl;
		inputfilename = argv[1];
		r = argv[2];
		fileindex =argv[3];
		if(rank == 0)
			cout << inputfilename << endl;
		//while(1);
		ifstream ifs(inputfilename.c_str());
		//cout << "this is rank " << rank << endl;
		string datastring;					//read the file into datastring
		vector <int> offset;
		offset.push_back(0);
		
		/*the root scan the file and find the offset of each line*/
		if(rank == 0)
		{
			cout << "rank 0 begin to scan the file." << endl;
			n = 0;
			/*get the number of lines*/
			while(!ifs.eof())
			{
				// int a;
				getline(ifs, str, '\n');
				// cout << str << endl;
				// cin>>a;
				offset.push_back(offset[n] + 1 + str.length());
				n++;
				//cout << offset[n] << endl;
			}
			n--;
			cout << "There are " << n << " lines in the file." << endl;
			startoffset = 0;
			/*distribute the lines to the processors*/
			endoffset = distributelines(n, size, nline, offset);
		}
		else
		{
			/*other processors receive the line information*/
			recvoffset(startoffset, endoffset, nline, &status);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		
		/*release the memory of offset*/
		offset.clear();
		vector<int>().swap(offset);
		MPI_Barrier(MPI_COMM_WORLD);
		
		/*processors read the file at different offset*/
		if(rank == 0)
			cout << "rank " << rank << " begin to read the file." << endl;  
		readfile(startoffset, endoffset, datastring, &status, rank);
		MPI_Barrier(MPI_COMM_WORLD);
		
		/*get user ids*/
		if(rank == 0)
			cout << "rank " << rank << " begin to get the ids." << endl;
		getids(datastring, nline, uid, rank);
		// cout << "it's here" << endl;
		MPI_Barrier(MPI_COMM_WORLD);
		
		
		/*release the memory of datastring*/
		datastring.~string();
		
		/*mod ids to classify them*/
		// modid(uid, size, rank);
		loc = modid(size, rank);
		MPI_Barrier(MPI_COMM_WORLD);
		//delete []idarray;
		//delete []newid;
		sort_write(loc, rank);
	
		MPI_Barrier(MPI_COMM_WORLD);
	// }
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	cout << "Finished!" <<endl;
	MPI_Finalize();
	
    return 0;	
}
