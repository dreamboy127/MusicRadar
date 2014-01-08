#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"some_function.h"
#include"fa_fft.h"



/*return the length of DATAF, eg. max(go.n,go.m)*/
int length(DATAF go)
{
	return go.n>go.m?go.n:go.m;
}


void fputi(DATAI go)
{
	int i,j;
	FILE *fp=fopen("L_output.txt","w+");
	for (i=0;i<go.n;i++)
	{
		fprintf(fp,"%d  ",i+1);
		for (j=0;j<go.m;j++)
			fprintf(fp,"%d   ",go.data[i*go.m+j]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}

/*output menbers of DATAF into txt file*/
void fput(DATAF go)
{
	int i,j;
	FILE *fp=fopen("cur_out.txt","w+");
	for (i=0;i<go.n;i++)
	{
		for (j=0;j<go.m;j++)
		{
			fprintf(fp,"%f   ",go.data[i*go.m+j]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}


/*output data in matrix form*/
void put(DATAF go)
{
	int i,j;
	for (i=0;i<go.n;i++)
	{
		for (j=0;j<go.m;j++)
		{
			printf("%d %d %.5f  ",i+1,j+1,go.data[i*go.m+j]);
		}
		printf("\n");
	}
}

/*output menbers of DATAI*/
void puti(DATAI go)
{
	int i,j;
	printf("\n%d %d\n",go.n,go.m);
	for (i=0;i<go.n;i++)
	{
		for (j=0;j<go.m;j++)
		{
			printf("%d %d %d   ",i+1,j+1,go.data[i*go.m+j]);
		}
		printf("\n");
	}
}

/*doesn't used so far
return go(:) ; turn n*m matrix to (n*m)*1 matrix*/
DATAF vedic(DATAF go)
{
	int i,j,k;
	DATAF ans;
	ans.n=go.n*go.m;
	ans.m=1;
	ans.malloc_len=ans.n*ans.m;
	ans.data=(float *)malloc(sizeof(float)*ans.malloc_len);
	
	k=0;
	for (i=0;i<go.m;i++)
	{
		for (j=0;j<go.n;j++)
		{
			ans.data[k++]=go.data[j*go.m+i];
		}
	}
	return ans;
}

/*malloc space and init length for DATAF menber*/
void init(DATAF *go,int n,int m)
{
	go->n=n;
	go->m=m;
	go->malloc_len=n*m;
	go->data=(float *)malloc(sizeof(float)*go->malloc_len);
}

void locmax(DATAF *go)
{
	int i;
	int b[300]={0};
	for (i=length(*go)-2;i;--i)
		b[i] = (go->data[i]>go->data[i-1] && go->data[i]>=go->data[i+1]);
		
	b[0] = (go->data[0]>go->data[1]);
	for (i=0;i<length(*go);++i)
		go->data[i]=b[i]?go->data[i]:0.0;
}

/*remain the locmax and set other menbers to zeros and the lost one must be a zero*/
/*
void locmax2(DATAF *go)
{
	int ii,jj;
    int i,j,k;
    float temp;
    DATAF cur;
    
	init(&cur,go->n,go->m);
    k=0;

   
	for (i=0;i<cur.malloc_len;++i)
		cur.data[i]=go->data[i];
	
    temp=cur.data[cur.n-1];
    cur.data[cur.n-1]=0;
    if (cur.n>2)
    for (i=cur.n-2;i;)
    {
        if (cur.data[i]>temp && cur.data[i]>cur.data[i-1])
        {
            temp=cur.data[i--];
        }
        else if (cur.data[i]>temp && cur.data[i]==cur.data[i-1])
        {
            j=i;
            while (--j>1 && cur.data[j]==cur.data[j-1])
                cur.data[j]=0;
            temp=cur.data[i];
            if (cur.data[j]>cur.data[j+1])
                cur.data[i]=0;
            i=j;
        }
        else
        {
            temp=cur.data[i];
            cur.data[i--]=0;
        }
    }

    if (cur.n==1)
        cur.data[0]=0;
    else if (cur.n<1)
    ;
    else if (cur.data[0]<=temp)
        cur.data[0]=0;
    //change the value of 'cur.m' and 'cur.n'
	//free(go->data);
//	go->n=cur.m;
//	go->m=cur.n;
//	forij(go->n,go->m) go->data[ii*go->m+jj]=cur.data[jj*cur.m+ii];
	for (i=0;i<cur.malloc_len;++i)
		go->data[i]=cur.data[i];
	free(cur.data);
}
*/


/***********************************************************
  Each point (maxima) in X is "spread" (convolved) with theprofile E; 
  Y is the pointwise max of all of these.
  If E is a scalar, it's the SD of a gaussian used as the spreading function (default 4).
  E is dault to f_sd and its value is 30
*************************************************************/
void spread(DATAF *Y,DATAF X)
{
	int i,j;
	int lenx,maxi,spos;
	float *E,*EE;
	int f_sd=30,W=120,lenE=241;

	E=(float *)malloc(sizeof(float)*lenE);
	
	for (i=-W;i<=W;i++)
	{
		E[i+W]=exp(-0.5*((float)i*i/900.0));  //remenber to translate variable to float type
	}
	

	
	locmax(&X);

	for (i=0;i<Y->malloc_len;i++)
		Y->data[i]=0.0;
		
	lenx=length(X);
	maxi=lenx+lenE;

	spos=(lenE)/2;
	EE=(float *)malloc(sizeof(float)*(maxi));
	
	for (i=0;i<maxi;i++)
		EE[i]=0;

	for (i=0;i<length(X);i++) if (X.data[i]>0.0)
	{
		for (j=0;j<i;j++)
			EE[j]=0;

		
		for (j=i;j<i+lenE;j++)
			EE[j]=E[j-i];
		EE[maxi-1]=0;

		for (j=0;j<lenx;j++)
			Y->data[j]=max(Y->data[j],X.data[i]*EE[spos+j]);
	}
	free(EE);
	free(E);
}

/*change the valule of two float menber*/
void swap(float *a,float *b)
{
	float c;
	c=*a;
	*a=*b;
	*b=c;
}

/*reverse maxtrix of DATAF
note: using this function in the form of go=reverse(go);*/
void reverse(DATAF *go)
{
	int ii,jj;
	DATAF temp;
	init(&temp,go->m,go->n);
	
	forij(temp.n,temp.m) temp.data[ii*temp.m+jj]=go->data[jj*go->m+ii];
	free(go->data);
	
	*go=temp;
}

/*reverse an maxtrix from right to left, left to right*/
void fliplr(DATAF *go)
{
	int i,j;
	for (i=0;i<(go->m)/2;i++)
	{
		for (j=0;j<go->n;j++)
		{
			swap(&(go->data[j*go->m+i]),&(go->data[j*go->m+go->m-1-i]));
		}
	}
}

/*********************************************************
function: compute the stft of DATAF
input: go--data input (array) 
	   nfft--length of every fft
	   s--overlap window between fft
**********************************************************/
void specgram(DATAF *ans,DATAF go,int nfft,int s)
{
	int i,j,t;
	float *hanning=malloc(sizeof(float)*nfft);
	int len=length(go);
	int nans=257;
	float *temp=(float*)malloc(sizeof(float)*nfft*3);
	uintptr_t handle;
	handle=fa_fft_init(nfft);
	if (go.n*go.m<=0)
	{
		puts("The input data is empty");
		return;
	}
	
	/*for (i=0;i<nfft;i++)
		hanning[i]=(1.0-cos(2.0*pi*i/(nfft-1)))/2.0;*/

	for (i=0;i<nfft/2;i++)
		hanning[i]=0.5*(1-cos(2*pi*(i+1)/(nfft+1)));

	for (j=0,i=nfft-1;i>=nfft/2;i--,j++)
		hanning[i]=hanning[j];


	if (go.n>go.m)
		reverse(&go);
	ans->n=nans;
	ans->m=go.m/s-1;   

	if (ans->m<1)
	{

		init(ans,nans,1);

		for (i=0;i<go.m;i++)
			temp[i*2]=hanning[i]*go.data[i],temp[i*2+1]=0;
		for (i=go.m;i<nfft;i++)
			temp[i*2]=temp[i*2+1]=0;
		fa_fft(handle,temp);
		for (i=0;i<nans;i++)
			ans->data[i]=sqrt(temp[2*i]*temp[2*i]+temp[2*i+1]*temp[2*i+1]);
		return;
	}
	ans->malloc_len=ans->n*ans->m;
//	printf("%d %d\n",ans.n,ans.m);
	ans->data=(float *)malloc(sizeof(float)*ans->malloc_len);
	for (t=0;t<ans->m;t++)
	{
		for (j=0;j<nfft;j++)
		{
			temp[j*2]=hanning[j]*go.data[t*s+j];
			temp[j*2+1]=0;
			
		}
		fa_fft(handle,temp);
	
		for (i=0;i<nans;i++)
			ans->data[t+i*ans->m]=sqrt(temp[i*2]*temp[i*2]+temp[i*2+1]*temp[2*i+1]);
	}
	free(hanning);
	free(temp);
	fa_fft_uninit(handle);
}


/*filter([1 -1],[1,-0.98],x);*/
void filter(DATAF *go)
{
	int i,j;
	
	DATAF cur;
	init(&cur,go->n,go->m);
	
	
	//length is too short 
	if (cur.malloc_len<1)
		return;
		
	//when data is only one row
	if (cur.m==1)
	{
		cur.data[0]=go->data[0];
		for (i=1;i<cur.n;i++)
		{
			cur.data[i]=cur.data[i-1]*0.98+go->data[i]-go->data[i-1];	
		}
		return;
	}
	
	//there is more than one row
	for (i=0;i<cur.n;i++)
		cur.data[i*cur.m]=go->data[i*cur.m];
	
	for (i=0;i<cur.n;i++)
	{
		for (j=1;j<cur.m;j++)
		{
			cur.data[i*cur.m+j]=cur.data[i*cur.m+j-1]*0.98+go->data[i*go->m+j]-go->data[i*go->m+j-1];
		}
	}
	free(go->data);
	*go=cur;
}

/*find the max menber of an maxtrix*/
float find_max(DATAF go)
{
	int i;
	float big=0;
	float *t=go.data;
	for (i=0;i<go.malloc_len;i++,t++)
		big=max(big,*t);
	return big;
}

int cmp(const void *a,const void *b)
{
	return *((float*)a) < *((float*)b) ? 1 : -1;
}
sort2(float go[],int n)
{
	qsort(go,n,sizeof(float),cmp);
}



int quick_sort(float l[],int hi[],int k,int low,int high,int isup)
{
    float x;
    int y;
    if (isup)
    {
        y=hi[k+low];
        x=l[k+low];
        while (low<high)
        {
            while (low<high && l[k+high]>=x)
                --high;
            l[k+low]=l[k+high];
            hi[k+low]=hi[k+high];
            while (low<high && l[k+low]<=x)
                ++low;
            l[k+high]=l[k+low];
            hi[k+high]=hi[k+low];
        }
        l[k+low]=x;
        hi[k+low]=y;
        return low;
    }
    else
    {
        x=l[k+low];
        y=hi[k+low];
        while (low<high)
        {
            while (low<high && l[k+high]<=x)
                --high;
            l[k+low]=l[k+high];
            hi[k+low]=hi[k+high];
            while (low<high && l[k+low]>=x)
                ++low;
            l[k+high]=l[k+low];
            hi[k+high]=hi[k+low];
        }
        l[low+k]=x;
        hi[k+low]=y;
        return low;
    }
}


int quick_sort2(float l[],int hi[],int k,int n,int low,int high,int isup)
{
    float x;
    int y;
   // printf("%d %d %d %d %d %d\n",k,n,low,high,k+low*n,k+high*n);
    if (isup)
    {
        y=hi[k+low*n];
        x=l[k+low*n];
        while (low<high)
        {
            while (low<high && l[k+high*n]>=x)
                --high;
            l[k+low*n]=l[k+high*n];
            hi[k+low*n]=hi[k+high*n];
            while (low<high && l[k+low*n]<=x)
                ++low;
            l[k+high*n]=l[k+low*n];
            hi[k+high*n]=hi[k+low*n];
        }
        l[k+low*n]=x;
        hi[k+low*n]=y;
        return low;
    }
    else
    {
        x=l[k+low*n];
        y=hi[k+low*n];
        while (low<high)
        {
            while (low<high && l[k+high*n]<=x)
                --high;
            l[k+low*n]=l[k+high*n];
            hi[k+low*n]=hi[k+high*n];
            while (low<high && l[k+low*n]>=x)
                ++low;
            l[k+high*n]=l[k+low*n];
            hi[k+high*n]=hi[k+low*n];
        }
        l[low*n+k]=x;
        hi[k+low*n]=y;
        return low;
    }
}


void sort_hang(float s[],int hi[],int k,int low,int high,int isup)
{
    int mid;
    if (low<high)
    {
        mid=quick_sort(s,hi,k,low,high,isup);
        sort_hang(s,hi,k,low,mid-1,isup);
        sort_hang(s,hi,k,mid+1,high,isup);
    }
}
void sort_lie(float s[],int hi[],int k,int n,int low,int high,int isup)
{
    int mid;
    if (low<high)
    {
        mid=quick_sort2(s,hi,k,n,low,high,isup);
        sort_lie(s,hi,k,n,low,mid-1,isup);
        sort_lie(s,hi,k,n,mid+1,high,isup);
    }
}


/**hi : the index number of sort go;
hang_lie : 1 sort in the form of row; 2 sort in the form of column  ; if there is only one row then sort in the form of row;
isup: 1 sort in ascending ; 0 sort in descending*/
void sort(DATAF *ans,DATAF go,DATAI *hi,int hang_lie,int isup)
{
    int i,j;
	int ii,jj;
 //   hi->malloc_len=go.n*go.m;
 //   hi->n=go.n;
  //  hi->m=go.m;


    ///hi->data=(int *)malloc(sizeof(int)*hi->malloc_len);
    if (hang_lie==2 || go.n==1)
    {

        for (i=0;i<go.n;i++)
        {

            for (j=0;j<go.m;j++)
                hi->data[i*go.m+j]=j;

            sort_hang(go.data,hi->data,i*go.m,0,go.m-1,isup);
        }

    }
    else
    {
        for (i=0;i<go.m;i++)
        {
            for (j=0;j<go.n;j++)
                hi->data[i+j*go.m]=j;

            sort_lie(go.data,hi->data,i,go.m,0,go.n-1,isup);
        }
    }

	forij(go.n,go.m) ans->data[ii*ans->m+jj]=go.data[ii*go.m+jj];


}



