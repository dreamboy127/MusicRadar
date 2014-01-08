#ifndef _WAVHEADER_H
#define _WAVHEADER_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//定义wav文件头部信息
typedef struct head_pama{
    short channels;//声道
    short bits;//精度
    int rate;//采样率
	int datasize;//数据大小，以字节为单位
}head_pama;

//wav格式音频文件头部解析
int wav_header_read(FILE* fp,head_pama* head);

#endif
