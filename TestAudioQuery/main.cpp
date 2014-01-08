#include <stdio.h>
#include <Windows.h>
#include "AudioConvert.h"
#include "Creat_Database.h"
#include "Query.h"

//#define _CRTDBG_MAP_ALLOC
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef  _DEBUG
//#define new DEBUG_NEW
#define new new(_CLIENT_BLOCK,__FILE__,__LINE__)       //注意，这里是__  不是_
#endif

#define HASHTABLEFILE_0_30000 "0-30000_HashTableFile.DataBase"//"HashTable.dataset"
#define SONGLISTFILE_0_30000 "0-30000_SongsList.txt"//"SongList.txt"
#define HASHTABLEFILE "HashTable.dataset"
#define SONGLISTFILE "SongList.txt"

char QueryWav_0_10000[10][80]={
	"test/0-10000/000dc2a9b46b422e98cabddffef5bc15.mp3",
	"test/0-10000/0a354b04-04e3-4506-a0e4-3ade910b9cfb.mp3",
	"test/0-10000/0c3891f8-8bed-4ad5-a1a8-ce0570528f3e.mp3",
	"test/0-10000/07a8be9e-c989-427b-bdbd-81ba7f41e4f0.mp3",
	"test/0-10000/027d4e76-8b49-45ed-827b-6c646cdbe129.mp3",
	"test/0-10000/038eede9-208e-448f-9e0d-0863a135e51c.mp3",
	"test/0-10000/000069d4049a4edca6f8690d82c251f4.mp3",
	"test/0-10000/03404c3c-565d-435a-b28d-c4fd47769fbf.mp3",
	"test/0-10000/053981a3-1bb0-49b1-bba1-3366c5d28fbf.mp3",
	"test/0-10000/088951db-8521-454b-851f-3da054c51c9d.mp3"
};
char QueryWav_10000_20000[10][80]={
	"test/10000-20000/1a38e540-4b06-4deb-a4f5-22d7f8f84e08.mp3",
	"test/10000-20000/1afe66ba-4ed4-42a5-8fe4-92619fe6f62b.mp3",
	"test/10000-20000/1c267518-ad3b-44fd-b407-b55ce7fe9e87.mp3",
	"test/10000-20000/1f95a9fe-e4d5-4d99-8d4b-3ceb29dd7ef2.mp3",
	"test/10000-20000/16b9b06e-9ce3-47d1-89e0-4856ca4326bb.mp3",
	"test/10000-20000/17b3cf5f-7f5b-4c86-811b-de8e66522061.mp3",
	"test/10000-20000/20ae0cd5-e26c-4aea-ad67-30a31c916ef4.mp3",
	"test/10000-20000/119c3f14-3059-4d6d-973a-7b9b440aec09.mp3",
	"test/10000-20000/197cfec7-9b57-49c4-a649-27a8d1a8422b.mp3",
	"test/10000-20000/1080b683-9b9b-4050-8366-bf20d42a3fe5.mp3"
};
char QueryWav_20000_30000[10][80]={
	"test/20000-30000/2b7bb9d4-def9-40c5-8101-cc5182eef65a.mp3",
	"test/20000-30000/2f24d231-df0c-4573-b881-4a397b8b1529.mp3",
	"test/20000-30000/28b7d1da-1ab4-4252-b996-ee4e23c03ccb.mp3",
	"test/20000-30000/29c6bc6c-31ad-42a7-8051-717bd7984b75.mp3",
	"test/20000-30000/219c6ef1-5d6d-4a69-8767-43fc0d143358.mp3",
	"test/20000-30000/228f9a18-a8d1-4833-845a-f3296db733d0.mp3",
	"test/20000-30000/2165a943-537c-4805-b4c0-54912f3dc8cf.mp3",
	"test/20000-30000/2538d621-f19d-4cd2-a019-fc67e1bdf32c.mp3",
	"test/20000-30000/23854ece-90dd-481b-a37c-61d219f57fe5.mp3",
	"test/20000-30000/2274324f-dd04-43aa-bf51-70211def1b0b.mp3",
};
char QueryWav_30000_40000[10][80]={
	"test/30000-40000/3d5950d9-72fa-48ba-a6fc-57e4bdaa921c.mp3",
	"test/30000-40000/31fde6c2-b438-4b64-9eec-eb3469cf754b.mp3",
	"test/30000-40000/39c4d757-e5ab-4326-ab32-be9387a2e256.mp3",
	"test/30000-40000/320ded35-6818-4664-9a0f-ff52e5050199.mp3",
	"test/30000-40000/33716b56-1c8a-469a-b1c3-d08d3f4cfc5e.mp3",
	"test/30000-40000/37884c2a-cea6-4bd7-a39d-4cd575f4986d.mp3",
	"test/30000-40000/334841f8-6943-4b93-80b0-b595fe8de9d3.mp3",
	"test/30000-40000/340126fc-a587-4a43-a5b3-100accb57926.mp3",
	"test/30000-40000/356491b6-b196-41f0-8db1-b79cc2088e5c.mp3",
	"test/30000-40000/404569e5-a97e-4a27-b526-eaa20f914c79.mp3",
};
int main(int argc,char* argv[])
{
	_CrtDumpMemoryLeaks();
	//_CrtSetBreakAlloc(63); 
	int j;
	int i;
	HANDLE AQHandle;
	QUERYRESULT queryResult;
	//short PCMBuffer[160000];
	//int* leak = new int[10];
	
	AQHandle=AQ2_LoadFPBase(HASHTABLEFILE_0_30000,SONGLISTFILE_0_30000);
	//memset(PCMBuffer,0,sizeof(PCMBuffer));
	//AQ2_FPQuery_Buffer(AQHandle_0_70000,PCMBuffer,160000,&queryResult);
	for (j=0;j<10;j++)
	{
		printf("%d. Search Wav:%s\n",j+1,QueryWav_0_10000[j]);
		AudioConvert(QueryWav_0_10000[j],8000,1,20,"tmp.wav");
		AQ2_FPQuery_File(AQHandle,"tmp.wav",&queryResult);
		
		for(i=0;i<min(5,queryResult.foundnum);i++)
		{
			printf("%d %d:%s %3d %-7.3f\n",i+1,queryResult.songs[i].id,queryResult.songs[i].name,queryResult.songs[i].hits,queryResult.songs[i].spos[0]);
		}

	}
	AQ2_ReleaseFPBase(AQHandle);

	return 0;
}