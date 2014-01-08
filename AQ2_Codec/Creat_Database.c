#include <stdio.h>
#include <conio.h>    
#include <io.h>
#include <windows.h>
#include "Creat_Database.h"
#include "../WAV/wavheader.h"
#include "FingerPrint_Hash.h"
#include "Find_LandMarks.h"
#include "AudioConvert.h"
#include <assert.h>

#define gap 30000

#define CMD_Decode		1
#define CMD_Decode_ERR	2
#define CMD_Add			3
#define CMD_Add_ERR		4

#define NORMALTYPE		0
#define ERRORTYPE		1

char AudioConvertERROR[10][30]={
	"打开输入文件失败",
	"查找流信息失败",
	"未找到音频流格式",
	"编码上下文初始化失败",
	"查找解码器失败",
	"重采样初始化失败",
	"重采样缓存分配失败",
	"打开解码器失败",
	"打开输出文件失败",
	"解码过程失败"
};
char AddWavFile2DATASETERROR[7][30]={
	"打开输入文件失败",
	"WAV头信息有误",
	"音频格式非单通道",
	"音频格式非8K采样率",
	"音频格式非16bit",
	"音频数据少于10秒",
	"音频数据超过20分"
};
void LogWrite(char* logText,int Type,int CMD_Type)
{
	SYSTEMTIME now;
	FILE* m_logFP;
	char  m_LogFileName[1000];
	GetLocalTime(&now);
	if (Type==NORMALTYPE)
	{
		sprintf(m_LogFileName,"log/%4d-%02d-%02d_creatdatabase_NORMAL.log",now.wYear,now.wMonth,now.wDay);
	}
	else if (Type==ERRORTYPE)
	{
		sprintf(m_LogFileName,"log/%4d-%02d-%02d_creatdatabase_ERROR.log",now.wYear,now.wMonth,now.wDay);
	}

	m_logFP=fopen(m_LogFileName,"at");
	
	switch(CMD_Type)
	{
	case CMD_Add:
		fprintf(m_logFP,"[%4d/%02d/%02d %02d:%02d:%02d]Adding %s\r\n",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,logText);
		break;
	case CMD_Add_ERR:
		fprintf(m_logFP,"[%4d/%02d/%02d %02d:%02d:%02d]Adding %s\r\n",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,logText);
		break;
	case CMD_Decode:
		fprintf(m_logFP,"[%4d/%02d/%02d %02d:%02d:%02d]Decoding %s\r\n",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,logText);
		break;
	case CMD_Decode_ERR:
		fprintf(m_logFP,"[%4d/%02d/%02d %02d:%02d:%02d]Decoding %s\r\n",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,logText);
		break;
	default:break;
	}
	fclose(m_logFP);
}
int GenerateWavlist(HashBucketCreate *HashTable,const char* folderPath,FILE* songTxt,int* songNum,long * parentwinhand)
{
	struct _finddata_t filesx;   
	int fhandle;   
	int IsHave=0;
	char strfind[1000];
	static int songid=0;
	int count=0;
	int first=0;

	strcpy(strfind,folderPath);      
	strcat(strfind,"\\*.mp3");
	fhandle = _findfirst(strfind,&filesx);   
	if(fhandle == -1)   
	{   
		//printf("fhandle error\n");   
		_getch();   
		return -1;   
	}   
	do{   
		if(filesx.attrib & _A_SUBDIR)//目录  
		{
			if (!strcmp(filesx.name,".") || !strcmp(filesx.name,".."))   
			{
				//Do nothing for "." and ".." folders    
			}
			else
			{
				char subfolder[1000];
				int ret;
				strcpy(subfolder,folderPath);
				sprintf(subfolder,"%s\\%s",subfolder,filesx.name);
				ret=GenerateWavlist(HashTable,subfolder,songTxt,songNum,parentwinhand);
				if (ret==0)
				{
					IsHave=1;
				}
				//songid+=(*songNum);
			}
		}
		else  
		{
			char tmp[1000];
			char *pname=filesx.name;
			int ret;
			
			LogWrite(filesx.name,NORMALTYPE,CMD_Add);
			//TEST
			/*if (first==0)
			{
			if (strcmp("7e4f77e290994fd98eb0fd6f7b0e39b6.mp3",filesx.name)==0)
			{
			sprintf(message,"%d",songid);
			PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Add,(LPARAM)message);
			first = 1;
			}
			else
			{
			songid++;
			continue;
			}
			}*/
			//
			/*extname=strrchr(filesx.name,'.');
			sprintf(message,"%s",filesx.name);
			PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Add,(LPARAM)message);
			if (extname==NULL||strlen(extname)!=4)
			{
				continue;
			}
			extname++;*/
			
			//if(strncmp(extname,"wav",3)==0||strncmp(extname,"mp2",3)==0||strncmp(extname,"mp3",3)==0||strncmp(extname,"wma",3)==0)
			{
				char logmessage[1000];
				
				sprintf(tmp,"%s\\%s",folderPath,filesx.name);
				sprintf(logmessage,"#%d %s",songid+1,filesx.name);
				LogWrite(logmessage,NORMALTYPE,CMD_Decode);
				//PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Decode,(LPARAM)logmessage);
				ret=AudioConvert(tmp,8000,1,0,"tmp.wav");//mp3文件转换为8000采样率，单通道，16bit的wav文件
				if (ret!=0)
				{
					//char newfile[1000];
					char errormessage[1000];
					//sprintf(newfile,"log/%s",filesx.name);
					sprintf(errormessage,"#%d %s error:%d %s",songid+1,filesx.name,ret,AudioConvertERROR[ret-1]);
					//CopyFile(tmp,newfile,FALSE);
					LogWrite(errormessage,ERRORTYPE,CMD_Decode_ERR);
					//PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Decode_ERR,(LPARAM)errormessage);
					continue;
				}
				while (*pname==' ')pname++; 
				while(*pname!='\0')
				{
					if (*pname==' ')
					{
						*pname='_';
					}
					pname++;
				}

				
				sprintf(logmessage,"#%d %s",songid+1,filesx.name);
				LogWrite(logmessage,NORMALTYPE,CMD_Add);
				//PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Add,(LPARAM)logmessage);
				ret=AddWavFile2DATASET(HashTable,"tmp.wav",songid+1);
				if (ret!=0)
				{
					//char newfile[1000];
					char errormessage[1000];
					//sprintf(newfile,"log/%s",tmp);
					sprintf(errormessage,"#%d %s error:%d %s",songid+1,filesx.name,ret,AddWavFile2DATASETERROR[(-1*ret)-1]);
					//CopyFile(tmp,newfile,FALSE);
					LogWrite(errormessage,ERRORTYPE,CMD_Add_ERR);
					//PostMessage((HWND)parentwinhand,WM_PROCESS_MESSAGE,CMD_Add_ERR,(LPARAM)errormessage);
					continue;
				}
				songid++;
				IsHave=1;
				fprintf(songTxt,"%d %s\n",songid,filesx.name);
				LogWrite("Success!",NORMALTYPE,CMD_Add);

				if (songid%gap==0)
				{
					FILE* HashTable_FP;
					char Temp[1000];
					int i,j;
					sprintf(Temp,"%d-%d_HashTableFile.DataBase",songid-gap+count*gap,songid+count*gap);
					HashTable_FP=fopen(Temp,"wb");
					for (i=0;i<HashNum;i++)
					{
						ENTRY* CurrentEntry=HashTable[i].Entries;
						ENTRY* TmpCurrent;//
						fwrite(&HashTable[i].nEntries,sizeof(int),1,HashTable_FP);
						for (j=0;j<HashTable[i].nEntries&&CurrentEntry!=NULL;j++)
						{
							fwrite(&CurrentEntry->Entries,sizeof(int),1,HashTable_FP);
							TmpCurrent=CurrentEntry;//
							CurrentEntry=CurrentEntry->next;
							free(TmpCurrent);//
						}
					}
					//free(HashTable);
					fclose(HashTable_FP);
					ClearHashTableCreate(HashTable,HashNum);
					fclose(songTxt);
					sprintf(Temp,"%d-%d_SongsList.txt",songid-gap+count*gap,songid+count*gap);
					CopyFile("SongTxtTmp.txt",Temp,FALSE);
					sprintf(Temp,"Save %d-%d",songid-gap+count*gap,songid+count*gap);
					LogWrite(Temp,NORMALTYPE,CMD_Add);
					songTxt=fopen("SongTxtTmp.txt","wt+");
					songid=0;
					count++;
				}

			}
		}
	}while(!_findnext(fhandle,&filesx));   

	_findclose(fhandle);   
	*songNum=songid;
	if (IsHave==0)
	{
		return -1;
	}
	return 0;   
}

AUDIOQUERYDLL2_API int AQ2_CreatFPBase(char* folderPath,char* HashTableFile,char* SongTxtfile,long * parentwinhand)
{
	FILE* HashTable_FP;
	FILE* SongTxtTmp_FP;
	FILE* SongTxt_FP;
	int ret;
	int songNum;
	int i,j;
	char line[1000];
	HashBucketCreate *HashTableCreate;
	
	assert(folderPath!=NULL);
	assert(HashTableFile!=NULL);
	assert(SongTxtfile!=NULL);
	HashTableCreate=(HashBucketCreate*)malloc(sizeof(HashBucketCreate)*HashNum);
	if (HashTableCreate==NULL)
	{
		return -1;
	}
	ClearHashTableCreate(HashTableCreate,HashNum);//初始化哈希表

	SongTxtTmp_FP=fopen("SongTxtTmp.txt","wt+");
	
	ret=GenerateWavlist(HashTableCreate,folderPath,SongTxtTmp_FP,&songNum,parentwinhand);
	fclose(SongTxtTmp_FP);
	if (ret!=0)
	{
		free(HashTableCreate);
		//remove("SongTxtTmp.txt");
		return ret;
	}

	HashTable_FP=fopen(HashTableFile,"wb");
	for (i=0;i<HashNum;i++)
	{
		ENTRY* CurrentEntry=HashTableCreate[i].Entries;
		ENTRY* TmpCurrent;//
		fwrite(&HashTableCreate[i].nEntries,sizeof(int),1,HashTable_FP);
		for (j=0;j<HashTableCreate[i].nEntries&&CurrentEntry!=NULL;j++)
		{
			fwrite(&CurrentEntry->Entries,sizeof(int),1,HashTable_FP);
			TmpCurrent=CurrentEntry;//
			CurrentEntry=CurrentEntry->next;
			free(TmpCurrent);//
		}
	}
	free(HashTableCreate);
	fclose(HashTable_FP);
	SongTxtTmp_FP=fopen("SongTxtTmp.txt","rt");
	SongTxt_FP=fopen(SongTxtfile,"wt");
	fprintf(SongTxt_FP,"%d\n",songNum);
	fseek(SongTxtTmp_FP,0,0);
	for(i=0;i<songNum;i++)
	{
		fgets(line,128,SongTxtTmp_FP);
		fprintf(SongTxt_FP,"%s",line);
	}

	fclose(SongTxtTmp_FP);
	fclose(SongTxt_FP);

	remove("SongTxtTmp.txt");

	return ret;
}

AUDIOQUERYDLL2_API HANDLE AQ2_LoadFPBase(char* HashTableFile,char* SongTxtFile)
{
	FILE* HashTable_FP;
	FILE* SongTxt_FP;
	FILE* testfile;
	int SongNum;
	int i,j;
	int Songid;
	char SongName[100];
	int ret;
	AQ2_Handle* pAQ2_HANDLE;
	int TotolSongNum;
	SongInformation* SongNameList;
	HashBucketLoad* HashTable;
	assert(HashTableFile!=NULL);
	assert(SongTxtFile!=NULL);
	HashTable=(HashBucketLoad*)malloc(sizeof(HashBucketLoad)*HashNum);
	if (HashTable==NULL)
	{
		return NULL;
	}
	ClearHashTableLoad((HashBucketLoad*)HashTable,HashNum);//初始化哈希表
	
	HashTable_FP=fopen(HashTableFile,"rb");
	if (HashTable_FP==NULL)
	{
		free(HashTable);
		return NULL;
	}
	SongTxt_FP=fopen(SongTxtFile,"rt");
	if (SongTxt_FP==NULL)
	{
		free(HashTable);
		fclose(HashTable_FP);
		return NULL;
	}
	
	for (i=0;i<HashNum;i++)
	{
		fread(&HashTable[i].nEntries,sizeof(int),1,HashTable_FP);
		if (HashTable[i].nEntries>MaxEntries)
		{
			testfile=fopen("error_i_entries.txt","at");
			fprintf(testfile,"i:%d entries:%d",i,HashTable[i].nEntries);
			fclose(testfile);
			HashTable[i].nEntries=MaxEntries;
		}
		HashTable[i].Entries=(unsigned int*)malloc(sizeof(int)*HashTable[i].nEntries);
		for (j=0;j<HashTable[i].nEntries;j++)
		{
			fread(&HashTable[i].Entries[j],sizeof(int),1,HashTable_FP);
		}
	}
	
	TotolSongNum=0;
	fscanf(SongTxt_FP,"%d",&SongNum);
	TotolSongNum=SongNum;
	SongNameList=NULL;
	SongNameList=(SongInformation*)malloc(sizeof(SongInformation)*(TotolSongNum));
	if (SongNameList==NULL)
	{
		free(HashTable);
		fclose(HashTable_FP);
		fclose(SongTxt_FP);
		return NULL;
	}
	for (i=0;i<TotolSongNum;i++)
	{
		SongNameList[i].songID=0;
		SongNameList[i].songName=NULL;
	}
	for (i=0;i<TotolSongNum;i++)
	{
		fscanf(SongTxt_FP,"%d",&Songid);
		fscanf(SongTxt_FP,"%s",SongName);

		if (SongNameList[Songid-1].songName==NULL&&SongNameList[Songid-1].songID==0)
		{
			SongNameList[Songid-1].songID=Songid;
			SongNameList[Songid-1].songName=(char*)malloc(sizeof(char)*(strlen(SongName)+1));
			strcpy(SongNameList[Songid-1].songName,SongName);
		}	
	}

	fclose(HashTable_FP);
	fclose(SongTxt_FP);
	pAQ2_HANDLE=(AQ2_Handle*)malloc(sizeof(AQ2_Handle));
	pAQ2_HANDLE->TotolSongNum=TotolSongNum;
	pAQ2_HANDLE->SongNameList=SongNameList;
	pAQ2_HANDLE->HashTable=HashTable;
	return pAQ2_HANDLE;
}
AUDIOQUERYDLL2_API int AQ2_ReleaseFPBase(HANDLE pAQ2_HANDLE)
{
	int i;
	if (((AQ2_Handle*)pAQ2_HANDLE)->HashTable==NULL||((AQ2_Handle*)pAQ2_HANDLE)->SongNameList==NULL||((AQ2_Handle*)pAQ2_HANDLE)->TotolSongNum==0)
	{
		return -1;
	}
	for (i=0;i<((AQ2_Handle*)pAQ2_HANDLE)->TotolSongNum;i++)
	{
		if (((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[i].songID==0||((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[i].songName==NULL)
		{
			continue;
		}
		if (((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[i].songName!=NULL)
		{
			free(((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[i].songName);
			((AQ2_Handle*)pAQ2_HANDLE)->SongNameList[i].songName=NULL;
		}
	}
	for (i=0;i<HashNum;i++)
	{
		HashBucketLoad TmpHashTable=((AQ2_Handle*)pAQ2_HANDLE)->HashTable[i];
		if (TmpHashTable.nEntries>0&&TmpHashTable.Entries!=NULL)
		{
			free(TmpHashTable.Entries);
			TmpHashTable.Entries=NULL;
		}
	}
	free(((AQ2_Handle*)pAQ2_HANDLE)->SongNameList);
	((AQ2_Handle*)pAQ2_HANDLE)->SongNameList=NULL;
	free(((AQ2_Handle*)pAQ2_HANDLE)->HashTable);
	((AQ2_Handle*)pAQ2_HANDLE)->HashTable=NULL;
	free(pAQ2_HANDLE);
	return 0;
}