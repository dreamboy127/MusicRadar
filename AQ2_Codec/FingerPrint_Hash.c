#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "FingerPrint_Hash.h"
#include "Find_LandMarks.h"
#include "../WAV/wavheader.h"

#define TIMESIZE 16384

int compare_timeoff(const void *a, const void *b){
	return (((HASH_RESULT*)a)->timeoff > ((HASH_RESULT*)b)->timeoff)? 1:-1;
}

int compare_realhits(const void *a, const void *b){
	if(((Query_Result*)a)->RealHits > ((Query_Result*)b)->RealHits)
		return -1;
	else if(((Query_Result*)a)->RealHits < ((Query_Result*)b)->RealHits)
		return 1;
	else
		if(((Query_Result*)a)->Hits > ((Query_Result*)b)->Hits)
			return -1;
		else if(((Query_Result*)a)->Hits < ((Query_Result*)b)->Hits)
			return 1;
		else
			return (((Query_Result*)a)->songID > ((Query_Result*)b)->songID)? 1:-1;
}
int compare_hits(const void *a, const void *b){
	if(((Query_Result*)a)->Hits > ((Query_Result*)b)->Hits)
		return -1;
	else if(((Query_Result*)a)->Hits < ((Query_Result*)b)->Hits)
		return 1;
	else
		return (((Query_Result*)a)->songID > ((Query_Result*)b)->songID)? 1:-1;
}
int compare_songid(const void *a, const void *b){
	if(((HASHVALUE*)a)->songID > ((HASHVALUE*)b)->songID)
		return 1;
	else if(((HASHVALUE*)a)->songID < ((HASHVALUE*)b)->songID)
		return -1;
	else
		return (((HASHVALUE*)a)->timeoff > ((HASHVALUE*)b)->timeoff)? 1:-1;
}
int round(double in)
{
	return (int)floor(in + 0.5);
}
int LandMark2Hash(int** L,int m,int SongID,HASHVALUE* hashvalue)
{
    int i;
    int *F1=(int*)malloc(sizeof(int)*m);
    int *DF=(int*)malloc(sizeof(int)*m);
    int *DT=(int*)malloc(sizeof(int)*m);
    int flag;
    for(i=0;i<m;i++)
    {
        F1[i]=(int)(round(L[i][1]))%256;
        DF[i]=(int)round(L[i][2]-L[i][1]);
        if(L[i][3]>=0)
            DT[i]=L[i][3];
        else
            DT[i]=-L[i][3];
        DT[i]=DT[i]%64;
    }
    for(i=0,flag=0;i<m;i++)
    {
        if(DF[i]>=0)
        {
            flag=1;break;
        }
    }
    if(flag==0)
    {
        for(i=0;i<m;i++)
        {
            DF[i]=DF[i]+256;
            DF[i]=DF[i]%64;
        }
    }
    for(i=0;i<m;i++)
    {
        hashvalue[i].songID=SongID;
        hashvalue[i].timeoff=(L[i][0]+1)%TIMESIZE;
        hashvalue[i].value=F1[i]*4096+DF[i]*64+DT[i];
    }
	free(F1);
	free(DF);
	free(DT);
    return 0;
}


int ClearHashTableCreate(HashBucketCreate *hashTable,int hashNum)
{
	memset(hashTable,0,sizeof(HashBucketCreate)*hashNum);
	return 0;
}
int ClearHashTableLoad(HashBucketLoad *hashTable,int hashNum)
{
	memset(hashTable,0,sizeof(HashBucketLoad)*hashNum);
	return 0;
}

int AddPoint2HashTable(HashBucketCreate *HashTable,HASHVALUE hashvalue[],int m)
{
    int N=0;
    int i;
    for(i=0;i<m;i++)
     {
        unsigned int SongID=hashvalue[i].songID;
        int Timeoff=hashvalue[i].timeoff;
        int Value=hashvalue[i].value;
        int nentries=HashTable[Value].nEntries+1;
		int tmpnentries=nentries;
        if(nentries>MaxEntries)
		{
			tmpnentries=(rand()%MaxEntries)+1;//MaxEntries
			nentries=MaxEntries;
		}
        if (nentries<=MaxEntries)
        {
            unsigned int hashval = SongID*TIMESIZE + Timeoff;
			ENTRY** CurrentEntry=&HashTable[Value].Entries;
			while(*CurrentEntry!=NULL&&tmpnentries>1)
			{
				CurrentEntry=&(*CurrentEntry)->next;
				tmpnentries--;
			}
			if (*CurrentEntry!=NULL)
			{
				(*CurrentEntry)->Entries=hashval;
			}
			else
			{
				*CurrentEntry=(ENTRY*)malloc(sizeof(ENTRY));
				(*CurrentEntry)->Entries=hashval;
				(*CurrentEntry)->next=NULL;
			}
			HashTable[Value].nEntries=nentries;
			N = N+1;
        }
    }
    return N;
}

int AddWavFile2DATASET(HashBucketCreate *HashTable,char* wavfile,int SongID)
{
	FILE* WavFile;
	head_pama WavFileHeader;
	int **Lt;
	
	DATAI L;
	DATAF wav_data;
    DATAF S,maxes;
	HASHVALUE *hashvalue;
	short *WavData;
	
	int i,j;
	int ret=0;
	
	if ((WavFile = fopen(wavfile, "rb")) == NULL) {
		printf("input file can not be opened;\n");
		return -1;
	}
	ret = wav_header_read(WavFile,&WavFileHeader);
	if (ret!=0)
	{
		return -2;
	}
	// next maybe change !!!
	if(WavFileHeader.channels != 1)
	{
		printf("非单通道\n");
		return -3;
	}
	if(WavFileHeader.rate != 8000)
	{
		printf("采样率非8K");
		return -4;
	}
	if(WavFileHeader.bits != 16)
	{
		printf("编码非16bit");
		return -5;
	}
	if(WavFileHeader.datasize <= 8000*2*10)//<=10s
	{
		printf("数据太小");
		return -6;
	}
	if(WavFileHeader.datasize >= 8000*2*60*20)//>=20分
	{
		printf("数据太大");
		return -7;
	}
	//why is 2?
	wav_data.m=WavFileHeader.datasize/2;
	wav_data.n=1;
	wav_data.malloc_len=wav_data.n*wav_data.m;
	
	WavData=(short*)malloc(sizeof(short)*wav_data.malloc_len);
	
	
	wav_data.data=(float*)malloc(sizeof(float)*wav_data.malloc_len);
	
	fseek(WavFile,44,SEEK_SET);
	
	fread(WavData,2,wav_data.malloc_len,WavFile);
	
	fclose(WavFile);
	
	for (i=0;i<wav_data.malloc_len;i++)
	{
		wav_data.data[i]=(float)(WavData[i]/pow(2,15));
	}
    
	find_landmarks(&L,&S,&maxes,wav_data,8000,7);//n=7???
	
	free(WavData);
	free(wav_data.data);
	free(S.data);
	free(maxes.data);
	
	Lt=(int**)malloc(sizeof(int*)*L.n);
	for(i=0;i<L.n;i++)
	{
		Lt[i]=(int*)malloc(sizeof(int)*L.m);
	}
	hashvalue=(HASHVALUE*)malloc(sizeof(HASHVALUE)*L.n);
	
	for(i=0;i<L.n;i++)
	{
		for(j=0;j<L.m;j++)
			Lt[i][j]=L.data[i*L.m+j];
    }
	free(L.data);
	
	LandMark2Hash(Lt,L.n,SongID,hashvalue);
    AddPoint2HashTable(HashTable,hashvalue,L.n);
	for(i=0;i<L.n;i++)
    {
        free(Lt[i]);
    }
    free(Lt);
    free(hashvalue);
	
	return 0;
}

int Get_Hash_Hits(HashBucketLoad *HashTable,int **H,int m,HASH_RESULT **HashResult,int *num)
{
    int Rnum=0;
    int i,j;
    int MaxHashResultNum=1000;
    *HashResult=(HASH_RESULT*)malloc(sizeof(HASH_RESULT)*MaxHashResultNum);
    for(i=0;i<m;i++)
    {
		
        int hashval=H[i][1];
        int htime=H[i][0];
        int nentries=HashTable[hashval].nEntries>MaxEntries?MaxEntries:HashTable[hashval].nEntries;

        for(j=0;j<nentries;j++)
        {
            unsigned int htcol=HashTable[hashval].Entries[j];//
            int songs=(int)(htcol/TIMESIZE);//
            int times = round(htcol-songs*TIMESIZE);//
			int dtime;
            if(Rnum+nentries>=MaxHashResultNum)
            {
                MaxHashResultNum+=MaxHashResultNum;
                *HashResult=(HASH_RESULT*)realloc(*HashResult,sizeof(HASH_RESULT)*MaxHashResultNum);
            }
            dtime=times-htime;
            (*HashResult)[Rnum+j].songID=songs;
            (*HashResult)[Rnum+j].timeoff=dtime;
            (*HashResult)[Rnum+j].value=hashval;
        }
        Rnum = Rnum + nentries;
    }
    *num=Rnum;
    return 0;
}

int MergeHashResult(HASH_RESULT *HashResult,int num,Query_Result* queryResult,int* ResultNum)
{
    int i,j;
    int tmpSong;
    int indexoff;
    int startindex,endindex;
    int tmpMaxRealHits=0;
    int tmpMaxindex;
	Query_Result* TmpQueryResult=(Query_Result*)malloc(sizeof(Query_Result)*num);
	int TmpQueryResultNum=0;
	int Num=0;
	
	qsort(HashResult,num,sizeof(HASH_RESULT),compare_songid);
    for(i=0;i<num;i++)
    {
        tmpSong=HashResult[i].songID;
        indexoff=i+1;
        startindex=i;
        for(j=i+1;j<num;j++)
        {
            if(tmpSong==HashResult[j].songID)
            {
                i=indexoff;
                indexoff++;
                if(i>=num)
                    break;
            }
			else
				break;
        }
        endindex=i;

        TmpQueryResult[TmpQueryResultNum].songID=tmpSong;
        TmpQueryResult[TmpQueryResultNum].Hits=endindex-startindex+1;
		TmpQueryResult[TmpQueryResultNum].startindex=startindex;
		TmpQueryResult[TmpQueryResultNum].endindex=endindex;
        TmpQueryResultNum++;
    }
    qsort(TmpQueryResult,TmpQueryResultNum,sizeof(Query_Result),compare_hits);

	for(i=0;i<min(20,TmpQueryResultNum);i++)
    {
		tmpMaxRealHits=1;
        tmpMaxindex=TmpQueryResult[i].startindex;
		for (j=TmpQueryResult[i].startindex;j<=TmpQueryResult[i].endindex;)
		{	
			int k;
			int tmphit=1;
			int tmptimeoff=HashResult[j].timeoff;
			for (k=j+1;k<=TmpQueryResult[i].endindex;k++)
			{	
				if (tmptimeoff==HashResult[k].timeoff)
				{
					tmphit++;
				}
				else
					break;
			}
			if (tmphit>tmpMaxRealHits)
			{
				tmpMaxRealHits=tmphit;
				tmpMaxindex=j;
			}
			j=k;			
		}
		tmpMaxRealHits=0;
		for (j=TmpQueryResult[i].startindex;j<=TmpQueryResult[i].endindex;j++)
		{
			if (abs(HashResult[j].timeoff-HashResult[tmpMaxindex].timeoff)<=1)
			{
				tmpMaxRealHits++;
			}
		}

        queryResult[Num].songID=TmpQueryResult[i].songID;
        queryResult[Num].Hits=TmpQueryResult[i].Hits;
        queryResult[Num].RealHits=tmpMaxRealHits;
        queryResult[Num].timeoff=HashResult[tmpMaxindex].timeoff;
        Num++;
    }
	qsort(queryResult,Num,sizeof(Query_Result),compare_realhits);
	/*for (i=0;i<Num;i++)
	{
		if (queryResult[i].RealHits<4)
		{
			Num=i;
			break;
		}
	}*/

	free(TmpQueryResult);
    *ResultNum=Num;
    return Num;
}
