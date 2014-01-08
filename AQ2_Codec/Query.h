#ifndef QUERY_H
#define QUERY_H
#include <Windows.h>
#ifdef AUDIOQUERYDLL2_EXPORTS  
#define AUDIOQUERYDLL2_API __declspec(dllexport)  
#else  
#define AUDIOQUERYDLL2_API __declspec(dllimport)  
#endif 
#define SMAX 5
#define MAX  5
typedef struct{
	int id;
	int hits;
	char name[80];
	float spos[MAX];
	int sposnum;
}songinfo;
typedef struct{
	songinfo songs[SMAX];
	int      foundnum;
}QUERYRESULT;
#ifdef __cplusplus
extern "C" AUDIOQUERYDLL2_API int AQ2_FPQuery_File(HANDLE HashTable,char* QueryWav,QUERYRESULT* queryResult);
#else
AUDIOQUERYDLL2_API int AQ2_FPQuery_File(HANDLE HashTable,char* QueryWav,QUERYRESULT* queryResult);
#endif

#ifdef __cplusplus
extern "C" AUDIOQUERYDLL2_API int AQ2_FPQuery_Buffer(HANDLE HashTable,short* inbuf,int inbufsize,QUERYRESULT* queryResult);
#else
AUDIOQUERYDLL2_API int AQ2_FPQuery_Buffer(HANDLE HashTable,short* inbuf,int inbufsize,QUERYRESULT* queryResult);
#endif


#endif