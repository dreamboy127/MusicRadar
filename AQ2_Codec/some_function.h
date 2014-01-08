
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define pi 3.1415926535898 
#define forij(n,m) for(ii=0;ii<n;ii++) for (jj=0;jj<m;jj++)


#ifndef only_once   //prevent redefence           
#define only_once 
typedef struct data_float   
{
    int n,m;
    float *data;
    int malloc_len;
}DATAF;

typedef struct data_int
{
    int n,m;
    int *data;
    int malloc_len;
}DATAI;
#endif


/*return the length of DATAF, eg. max(go.n,go.m)*/
int length(DATAF go);

/*output menbers of DATAF into txt file*/
void fput(DATAF go);

/*output data in matrix form*/
void put(DATAF go);

/*output menbers of DATAI*/
void puti(DATAI go);

/*return go(:) ; turn n*m matrix to (n*m)*1 matrix*/
DATAF vedic(DATAF go);

/*malloc space and init length for DATAF menber*/
void init(DATAF *go,int n,int m);

/*remain the locmax and set other menbers to zeros and the lost one must be a zero*/
void locmax(DATAF *go);

/***********************************************************
  Each point (maxima) in X is "spread" (convolved) with theprofile E; 
  Y is the pointwise max of all of these.
  If E is a scalar, it's the SD of a gaussian used as the spreading function (default 4).
  E is dault to f_sd and its value is 30
*************************************************************/
void spread(DATAF *F,DATAF X);

/*change the valule of two float menber*/
void swap(float *a,float *b);

/*reverse maxtrix of DATAF
note: using this function in the form of go=reverse(go);*/
void reverse(DATAF *go);

/*reverse an maxtrix from right to left, left to right*/
void fliplr(DATAF *go);

/*********************************************************
function: compute the stft of DATAF
input: go--data input (array) 
	   nfft--length of every fft
	   s--overlap window between fft
output: ans--the stft of go
**********************************************************/
void specgram(DATAF *ans,DATAF go,int nfft,int s);

/*filter([1 -1],[1,-0.98],x);*/
void filter(DATAF *go);

/*find the max menber of an maxtrix*/
float find_max(DATAF go);

sort2(float go[],int n);

void sort(DATAF *ans,DATAF go,DATAI *hi,int hang_lie,int isup);


