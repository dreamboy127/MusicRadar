#include<stdio.h>

#include"find_landmarks.h"


void find_landmarks(DATAI *L,DATAF *S,DATAF *maxes,DATAF D,float SR,float N)
{
	int i,j,k,p,len;
	float a_dec=1-0.01*(N/35.0);
	/*The scheme relies on just a few landmarks being common to both
	query and reference items.  The greater the density of landmarks,
	the more like this is to occur (meaning that shorter and noisier
	queries can be tolerated), but the greater the load on the
	database holding the hashes.*/

	int f_sd=30;
	/*The decay rate of the masking skirt behind each peak
	(proportion per frame).  A value closer to one means fewer
	peaks found.*/

	int maxpksperframe=5;
	/*The maximum number of peaks allowed for each frame.  In
    practice, this is rarely reached, since most peaks fall
    below the masking skirt*/

	float hpf_pole=0.98;
	/*The high-pass filter applied to the log-magnitude
    envelope, which is parameterized by the position of the
    single real pole.  A pole close to +1.0 results in a
    relatively flat high-pass filter that just removes very
    slowly varying parts; a pole closer to -1.0 introduces
    increasingly extreme emphasis of rapid variations, which
    leads to more peaks initially.*/

	int targetdf=31,targetdt=63;
	/*The number of pairs made with each peak.  All maxes within a
    "target region" following the seed max are made into pairs,
    so the larger this region is (in time and frequency), the
    more maxes there will be.  The target region is defined by a
    freqency half-width (in bins)
	The actual frequency and time differences are quantized and
    packed into the final hash; if they exceed the limited size
    described above, the hashes become irreversible (aliased);
    however, in most cases they still work (since they are
    handled the same way for query and reference).*/

	
	int nr,nc,nmaxes,maxix;
	int targetSR=8000;

	/*Estimate for how many maxes we keep - < 30/sec (to preallocate array)*/
	int maxespersec=30;
	int fft_ms=64,fft_hop=32,nfft=512;
	float Smax;
	int nmaxkeep;
	float temp,ddur,v,*t;

	/*overmasking factor?  Currently none.*/
	float s_sup=1.0;
	int nmaxthistime,nmaxes2,whichmaxes,maxpairsperpeak=3,nlmarks=0;
	int startt,F1,maxt,minf,maxf;
	DATAF cur,sthresh,s_this,sdiff,vv,eww,maxes2;
	DATAI xx;

	nr=D.n,nc=D.m;

//printf("welocome : %.4f\n",D.data[D.m-1]);

	/*Convert D to a mono row-vector*/
	if (nr==2 && nc>2)
	{
		for (i=0;i<length(D);i++)
			D.data[i]=(D.data[i]+D.data[nc+i])/2.0;
	}
	else if (nr>2 && nc==2)
	{
		for (i=0;i<nr;i++)
			D.data[i]=(D.data[i<<1]+D.data[i<<1|1])/2.0;
	}
	
	D.n=1;
	D.m=max(nr,nc);
	
	/*这里暂定，resample还没有写
	//Resample to target sampling rate*/
	if (SR!=targetSR)
	{
		//D=resample(D,startSR,SR);	
	} 


	/*Take spectral features
	We use a 64 ms window (512 point FFT) for good spectral resolution
	convert to log domain, and emphasize onsets*/
	specgram(S,D,nfft,nfft/2);


	Smax=find_max(*S)/1000000.0;

	/*Work on the log-magnitude surface
	s=log(max(Smax,s)) */
	temp=0;
	//forij(S->n,S->m) S->data[ii*S->m+jj]=log(max(Smax,S->data[ii*S->m+jj])),temp+=S->data[ii*S->m+jj];
	for (t=S->data,i=0;i<S->malloc_len;i++,t++) 
		*t=log(max(Smax,*t)),temp+=*t; 

	/*find the mean of S*/
	temp/=(float)(S->m*S->n);
	
	/*Make it zero-mean, so the start-up transients for the filter are minimized*/
	//forij(S->n,S->m) S->data[ii*S->m+jj]-=temp;
	for (i=0,t=S->data;i<S->malloc_len;i++,t++)
		*t-=temp;
	
	/*This is just a high pass filter, applied in the log-magnitude
    domain.  It blocks slowly-varying terms (like an AGC), but also 
    emphasizes onsets.  Placing the pole closer to the unit circle 
	(i.e. making the -.8 closer to -1) reduces the onset emphasis.*/
	filter(S);
//fput(*S);
	ddur=(float)length(D)/(float)targetSR;
	nmaxkeep=(int)(maxespersec*ddur+0.5);

	/*find all the local prominent peaks, store as maxes(i,:) = [t,f];*/
	init(maxes,3,nmaxkeep);

	//forij(maxes->n,maxes->m) maxes->data[ii*maxes->m+jj]=0;
//	for (i=0,t=maxes->data;i<maxes->malloc_len;i++,t++)
//		*t=0;

//puts("five");
	nmaxes=0;
	maxix=0;

	init(&cur,S->n,1);
	
//puts("six");
	/*initial threshold envelope based on peaks in first 10 frames*/
//	forij(cur.n,cur.m) cur.data[ii*cur.m+jj]=S->data[ii*S->m+jj];

	//init(&xx,257,10);
	//cur=sort(cur,&xx,2,0);
	//free(xx.data);
	
	for (i=0;i<cur.n;i++)
		cur.data[i]=S->data[i*S->m];
	for (i=0;i<cur.n;i++)                ///////////////////
		for (j=1;j<min(10,S->m);j++)
			cur.data[i]=max(cur.data[i],S->data[i*S->m+j]);


//puts("eight");
	init(&sthresh,257,1);
	spread(&sthresh,cur);
//put(sthresh);

//puts("seven");
	//init(T,S->n,S->m);
	//forij(T->n,T->m) T->data[ii*T->m+jj]=0;
//puts("four");	

	init(&s_this,S->n,1);

	init(&sdiff,S->n,1);
	init(&eww,S->n,1);
//puts("three");
	/*T stores the actual decaying threshold, for debugging*/
	
	init(&xx,sdiff.n,sdiff.m);
//puts("two");

	init(&vv,s_this.n,s_this.m);

	for (i=0;i<S->m-1;i++)
	{	
		for (j=0;j<S->n;j++)
			s_this.data[j]=S->data[j*S->m+i];

		for (j=0;j<S->n;j++)
			sdiff.data[j]=max(0.0,s_this.data[j]-sthresh.data[j]);
		
		/*find local maxima*/
		locmax(&sdiff);
		
		/*make sure last bin is never a local max since its index doesn't fit in 8 bits*/
		sdiff.data[length(sdiff)-1]=0;
/*		
if (i==557)
{
	for (j=0;j<257;++j)
	{
		printf("%d %.3f    ",j+1,S->data[j*S->m+i]);
	}puts("");
}*/


		/*take up to 5 largest*/
		sort(&vv,sdiff,&xx,1,0);
		
		
		nmaxthistime=0;
		/*store those peaks and update the decay envelope*/
		for (j=0;j<sdiff.n && nmaxthistime<maxpksperframe;j++) if (vv.data[j]>0.0)
		{
			p=xx.data[j];

			/* Check to see if this peak is under our updated threshold and keep only nonzero*/
			if (s_this.data[p]>sthresh.data[p])
			{
				
				maxes->data[nmaxes]=(float)i;
				maxes->data[maxes->m+nmaxes]=(float)p;
				maxes->data[2*maxes->m+nmaxes]=s_this.data[p];
				nmaxthistime++;
				nmaxes++;
				len=length(sthresh);
				for (k=0;k<len;k++)  //p is low 1
					eww.data[k]=exp(-0.5*pow((k-p)/(float)f_sd,2));  /*eww is computed every time...*/

				for (k=0;k<len;k++)
					sthresh.data[k]=max(sthresh.data[k],s_this.data[p]*eww.data[k]);
			}
		}
		else 
			break; /*end computing ealier*/
	
		//for (j=0;j<T->n;j++)
		//	T->data[j*T->m+i]=sthresh.data[j];
		for (j=0;j<sthresh.n;j++)
			sthresh.data[j]*=a_dec;

	}

//put(sthresh);
//put(*maxes);

//printf("%d \n",nmaxes);
 
	/*Backwards pruning of maxes*/
	nmaxes2=0;
	whichmaxes=nmaxes-1; //////
	free(cur.data);
	init(&cur,S->n,1);
	for (i=0;i<S->n;i++)
		cur.data[i]=S->data[i*S->m+S->m-1];
//put(cur);
	spread(&sthresh,cur);

	init(&maxes2,2,nmaxes);
	
	//forij(maxes2.n,maxes2.m) maxes2.data[maxes2.m*ii+jj]=-1.0;
//puts("one");


	for (i=S->m-2;i>=0 && whichmaxes>=0;i--)
	{
		
		while (whichmaxes>=0 && (int)maxes->data[whichmaxes]==i)
		{
			
			p=(int)maxes->data[maxes->m+whichmaxes];
			v=maxes->data[2*maxes->m+whichmaxes];
			if (v>=sthresh.data[p])
			{				
				/*keep this one*/
				maxes2.data[nmaxes2]=(float)i;
				maxes2.data[maxes2.m+nmaxes2]=(float)p;
				nmaxes2++;
				
				len=length(sthresh);

				for (k=0;k<len;k++)
					eww.data[k]=exp(-0.5*pow((k-p)/(float)f_sd,2));

				for (k=0;k<len;k++)
					sthresh.data[k]=max(sthresh.data[k],v*eww.data[k]);
			}
			whichmaxes--;
		}
		for (j=0;j<length(sthresh);j++)
			sthresh.data[j]*=a_dec;
	}


	for (j=0;j<nmaxes2;j++)
		maxes2.data[j+nmaxes2]=maxes2.data[maxes2.m+j];
	maxes2.m=nmaxes2;
	fliplr(&maxes2);


//printf("%d %d \n",nmaxes,nmaxes2);


//put(maxes2);
	/*Pack the maxes into nearby pairs = landmarks*/
	init(L,nmaxes2*maxpairsperpeak,4);
//	forij(L->n,L->m) L->data[ii*L->m+jj]=-1.0;
	nlmarks=0;
	for (i=0;i<nmaxes2;i++)
	{
		startt=(int)maxes2.data[i];
		F1=(int)maxes2.data[maxes2.m+i];
		maxt=startt+targetdt;
		minf=F1-targetdf;
		maxf=F1+targetdf;
		len=0;

		/*limit the number of pairs we make; take first ones, as they will be closest in time*/
		for (j=0;j<length(maxes2) && len<maxpairsperpeak;j++)
		{
			
			if ((int)maxes2.data[j]>startt && (int)maxes2.data[j]<maxt)
			if ((int)maxes2.data[maxes2.m+j]>minf && (int)maxes2.data[maxes2.m+j]<maxf)
			{
				len++;
				L->data[nlmarks*L->m]=startt;
				L->data[nlmarks*L->m+1]=F1;
				L->data[nlmarks*L->m+2]=maxes2.data[maxes2.m+j];
				L->data[nlmarks*L->m+3]=maxes2.data[j]-startt;
				nlmarks++;
			}
		}
	}

	L->n=nlmarks;
	free(maxes->data);
	*maxes=maxes2;
	
	
	/*free the space malloced before*/
	free(cur.data);
	//free(maxes2.data);
	free(sthresh.data);
	free(s_this.data);
	free(sdiff.data);
	free(vv.data);
	free(eww.data);
	free(xx.data);
	
}