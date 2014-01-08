#ifndef _CREAT_DATASET_H
#define _CREAT_DATASET_H
#include <Windows.h>
#ifdef AUDIOQUERYDLL2_EXPORTS  
#define AUDIOQUERYDLL2_API __declspec(dllexport)  
#else  
#define AUDIOQUERYDLL2_API __declspec(dllimport)  
#endif 
#ifdef __cplusplus
extern "C" AUDIOQUERYDLL2_API int AQ2_CreatFPBase(char* folderPath,char* HashTableFile,char* SongTxtfile,long * parentwinhand);
#else
AUDIOQUERYDLL2_API int AQ2_CreatFPBase(char* folderPath,char* HashTableFile,char* SongTxtfile,long * parentwinhand);
#endif 

#ifdef __cplusplus
extern "C" AUDIOQUERYDLL2_API HANDLE AQ2_LoadFPBase(char* HashTableFile,char* SongTxtFile);
#else
AUDIOQUERYDLL2_API HANDLE AQ2_LoadFPBase(char* HashTableFile,char* SongTxtFile);
#endif 

#ifdef __cplusplus
extern "C" AUDIOQUERYDLL2_API int AQ2_ReleaseFPBase(HANDLE hashtable);
#else
AUDIOQUERYDLL2_API int AQ2_ReleaseFPBase(HANDLE hashtable);
#endif 

#endif