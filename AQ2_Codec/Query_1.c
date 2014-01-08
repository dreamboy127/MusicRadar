#include "Query.h"
#include "../WAV/wavheader.h"
#include <stdlib.h>
#include "FingerPrint_Hash.h"
#include "Find_LandMarks.h"
#include <stdio.h>
#include <assert.h>

int compare_HashValue(const void *a, const void *b){
	if(((HASHVALUE*)a)->timeoff > ((HASHVALUE*)b)->timeoff)
		return 1;
	else if(((HASHVALUE*)a)->timeoff < ((HASHVALUE*)b)->timeoff)
		return -1;
	else
		return (((HASHVALUE*)a)->value > ((HASHVALUE*)b)->value)? 1:-1;
}

AUDIOQUERYDLL2_API int AQ2_FPQuery_File(HANDLE pAQ2_HANDLE,char* QueryWav,QUERYRESULT* queryResult)
{
	FILE* Query_FP;
	head_pama WavFileHeader;
	short *WavData;

	int ret=0;
	int ResultNum;
	
	assert(pAQ2_HANDLE!=NULL);
	assert(QueryWav!=NULL);
	assert(queryResult!=NULL);
	if((Query_FP = fopen(QueryWav, "rb")) == NULL) {
		printf("input file can not be opened;\n");
		return -1;
	}
	ret = wav_header_read(Query_FP,&WavFileHeader);
	if (ret!=0)
	{
		return -2;
	}
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
	if(WavFileHeader.datasize >= 8000*2*60*20)//>=20分
	{
		printf("数据太大");
		return -6;
	}

	WavData=(short*)malloc(sizeof(short)*WavFileHeader.datasize/2);

	fseek(Query_FP,44,SEEK_SET);

	fread(WavData,2,WavFileHeader.datasize/2,Query_FP);

	fclose(Query_FP);

	ResultNum=AQ2_FPQuery_Buffer(pAQ2_HANDLE,WavData,WavFileHeader.datasize/2,queryResult);
	
	free(WavData);

	return ResultNum;
}

AUDIOQUERYDLL2_API int AQ2_FPQuery_Buffer(HANDLE pAQ2_HANDLE,short* inbuf,int inbufsize,QUERYRESULT* queryResult)
{
	int i,j;
	int **Lt;

	DATAF wav_data;
    DATAF S,maxes;
	HASHVALUE *hashvalue;
	Query_Result TmpqueryResult[QUERYRESULTNUM];
	
	int **H;
	int tmpnum;
	DATAI L1,L2,L3,L4;
	HASH_RESULT *hashresult;
	int m=0;
	int LtLength;
	double landmarks_hopt = 0.032;
	int *ResultNum=&((*queryResult).foundnum);
	songinfo* songs=((*queryResult).songs);
	
	assert(pAQ2_HANDLE!=NULL);
	assert(inbuf!=NULL);
	assert(inbufsize!=0);
	assert(queryResult!=NULL);
	if(inbufsize >= 8000*60*20)//>=20分
	{
		printf("数据太大");
		return -1;
	}
	m=inbufsize;
	wav_data.m=m;
	wav_data.n=1;
	wav_data.malloc_len=wav_data.n*wav_data.m;
	wav_data.data=(float*)malloc(sizeof(float)*wav_data.malloc_len);
	for (i=0;i<wav_data.malloc_len;i++)
	{
		wav_data.data[i]=(float)(inbuf[i]/pow(2,15));
	}
	
	find_landmarks(&L1,&S,&maxes,wav_data,8000,20);
	
	free(wav_data.data);
	free(S.data);
	free(maxes.data);
//下面是查询的三次截取
	m=inbufsize-(round(landmarks_hopt/4*8000)-1);
	wav_data.m=m;
	wav_data.n=1;
	wav_data.malloc_len=wav_data.n*wav_data.m;
	wav_data.data=(float*)malloc(sizeof(float)*wav_data.malloc_len);
	for (i=0;i<wav_data.malloc_len;i++)
	{
		wav_data.data[i]=(float)(inbuf[i+round(landmarks_hopt/4*8000)-1]/pow(2,15));
	}
	
	find_landmarks(&L2,&S,&maxes,wav_data,8000,20);
	
	free(wav_data.data);
	free(S.data);
	free(maxes.data);

	m=inbufsize-(round(landmarks_hopt/2*8000)-1);
	wav_data.m=m;
	wav_data.n=1;
	wav_data.malloc_len=wav_data.n*wav_data.m;
	wav_data.data=(float*)malloc(sizeof(float)*wav_data.malloc_len);
	for (i=0;i<wav_data.malloc_len;i++)
	{
		wav_data.data[i]=(float)(inbuf[i+round(landmarks_hopt/2*8000)-1]/pow(2,15));
	}
	find_landmarks(&L3,&S,&maxes,wav_data,8000,20);
	
	free(wav_data.data);
	free(S.data);
	free(maxes.data);
	
	m=inbufsize-(round(landmarks_hopt*3/4*8000)-1);
	wav_data.m=m;
	wav_data.n=1;
	wav_data.malloc_len=wav_data.n*wav_data.m;
	wav_data.data=(float*)malloc(sizeof(float)*wav_data.malloc_len);
	for (i=0;i<wav_data.malloc_len;i++)
	{
		wav_data.data[i]=(float)(inbuf[i+round(landmarks_hopt*3/4*8000)-1]/pow(2,15));
	}
	find_landmarks(&L4,&S,&maxes,wav_data,8000,20);
	free(wav_data.data);
	free(S.data);
	free(maxes.data);
//
	LtLength=L1.n+L2.n+L3.n+L4.n;
	Lt=(int**)malloc(sizeof(int*)*LtLength);
	for(i=0;i<L1.n;i++)
	{
		Lt[i]=(int*)malloc(sizeof(int)*L1.m);
	}
	for(i=L1.n;i<L1.n+L2.n;i++)
	{
		Lt[i]=(int*)malloc(sizeof(int)*L2.m);
	}
	for(i=L1.n+L2.n;i<L1.n+L2.n+L3.n;i++)
	{
		Lt[i]=(int*)malloc(sizeof(int)*L3.m);
	}
	for(i=L1.n+L2.n+L3.n;i<L1.n+L2.n+L3.n+L4.n;i++)
	{
		Lt[i]=(int*)malloc(sizeof(int)*L4.m);
	}
	
	for(i=0;i<L1.n;i++)
	{
		for(j=0;j<L1.m;j++)
			Lt[i][j]=L1.data[i*L1.m+j];
    }
	for(i=0;i<L2.n;i++)
	{
		for(j=0;j<L2.m;j++)
			Lt[i+L1.n][j]=L2.data[i*L2.m+j];
    }
	for(i=0;i<L3.n;i++)
	{
		for(j=0;j<L3.m;j++)
			Lt[i+L1.n+L2.n][j]=L3.data[i*L3.m+j];
    }
	for(i=0;i<L4.n;i++)
	{
		for(j=0;j<L4.m;j++)
			Lt[i+L1.n+L2.n+L3.n][j]=L4.data[i*L4.m+j];
    }

	hashvalue=(HASHVALUE*)malloc(sizeof(HASHVALUE)*LtLength);
	LandMark2Hash(Lt,LtLength,0,hashvalue);
	//
	H=(int**)malloc(sizeof(int*)*LtLength);
    for(i=0;i<LtLength;i++)
        H[i]=(int*)malloc(sizeof(int)*2);
	tmpnum=0;

	qsort(hashvalue,LtLength,sizeof(HASHVALUE),compare_HashValue);
//unique 
    for(i=0;i<LtLength;i++)
    {
		for (j=tmpnum-1;j>=0;j--)
		{
			if (hashvalue[i].timeoff==H[j][0]&&hashvalue[i].value==H[j][1])
			{
				break;
			}
		}
		if (j<0)
		{
			H[tmpnum][0]=hashvalue[i].timeoff;
			H[tmpnum][1]=hashvalue[i].value;
			tmpnum++;
		}
    }
//
	free(hashvalue);
	
    Get_Hash_Hits(((AQ2_Handle*)pAQ2_HANDLE)->HashTable,H,tmpnum,&hashresult,ResultNum);

	tmpnum=*ResultNum;
    MergeHashResult(hashresult,tmpnum,TmpqueryResult,ResultNum);//这部分跟matlab不同
	for(i=0;i<LtLength;i++)
        free(Lt[i]);
    free(Lt);
	for(i=0;i<LtLength;i++)
        free(H[i]);
    free(H);
	free(hashresult);
	free(L1.data);
	free(L2.data);
	free(L3.data);
	free(L4.data);

	for(i=0;i<min(SMAX,*ResultNum);i++)
	{
		songs[i].id=TmpqueryResult[i].songID;
		if (((AQ2_Handle*)pAQ2_HANDLE)->SongNameList!=NULL)
		{
			strcpy(songs[i].name,((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[TmpqueryResult[i].songID-1].songName);
		}
		songs[i].hits=TmpqueryResult[i].RealHits;
		songs[i].spos[0]=(float)(TmpqueryResult[i].timeoff*0.032);
		songs[i].sposnum=1;
	}
	return *ResultNum;
}