#ifndef FINGERPRINT_HASH_H
#define FINGERPRINT_HASH_H

#include <stdio.h>

#define HashNum 1048576
#define MaxEntries 200 //20
#define QUERYRESULTNUM 20

#define TIMELIMIT (3600*24)

typedef struct HASHVALUE{
	int songID;
	int timeoff;
	int value;
}HASHVALUE,HASH_RESULT;

typedef struct ENTRY{
	unsigned int Entries;
	struct ENTRY* next;
}ENTRY;

typedef struct HashBucketCreate{
	int nEntries;
	ENTRY* Entries;
}HashBucketCreate;

typedef struct HashBucketLoad{
	int nEntries;
	unsigned int* Entries;
}HashBucketLoad;

typedef struct {
	int songID;
	union
	{
		int RealHits;
		int endindex;
	};
	union
	{
		int timeoff;
		int startindex;
	};
	int Hits;
}Query_Result;
typedef struct{
	int songID;
	char* songName;
}SongInformation;

typedef struct{
	int TotolSongNum;
	SongInformation* SongNameList;
	HashBucketLoad* HashTable;
}AQ2_Handle;


int round(double in);
int LandMark2Hash(int **L,int m,int SongID,HASHVALUE* hashvalue);
int AddPoint2HashTable(HashBucketCreate *HashTable,HASHVALUE hashvalue[],int m);
int AddWavFile2DATASET(HashBucketCreate *HashTable,char* wavfile,int SongID);
int Get_Hash_Hits(HashBucketLoad *HashTable,int **H,int m,HASH_RESULT **HashResult,int *num);
int MergeHashResult(HASH_RESULT *HashResult,int num,Query_Result* queryResult,int* ResultNum);
int ClearHashTableCreate(HashBucketCreate *hashTable,int hashNum);
int ClearHashTableLoad(HashBucketLoad *hashTable,int hashNum);
#endif // FINGERPRINT_HASH_H
