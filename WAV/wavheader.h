#ifndef _WAVHEADER_H
#define _WAVHEADER_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//����wav�ļ�ͷ����Ϣ
typedef struct head_pama{
    short channels;//����
    short bits;//����
    int rate;//������
	int datasize;//���ݴ�С�����ֽ�Ϊ��λ
}head_pama;

//wav��ʽ��Ƶ�ļ�ͷ������
int wav_header_read(FILE* fp,head_pama* head);

#endif
