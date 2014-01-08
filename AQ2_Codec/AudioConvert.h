#ifndef AUDIOCONVERT_H
#define AUDIOCONVERT_H

#ifdef AUDIOCONVERT_EXPORTS  
#define AUDIOCONVERT_API __declspec(dllexport)  
#else  
#define AUDIOCONVERT_API __declspec(dllimport)  
#endif 

#ifdef __cplusplus
extern "C" AUDIOCONVERT_API int AudioConvert( const char *inputfilename,int sample_rate,int channels,int sec,const char *outfilename);
#else
AUDIOCONVERT_API int AudioConvert( const char *inputfilename,int sample_rate,int channels,int sec,const char *outfilename);
#endif 


#ifdef __cplusplus
extern "C" AUDIOCONVERT_API int AudioConvert_Buffer( const char *inputfilename,int sample_rate,int channels,int sec, short *pcmbuffer,int *pcmbuff_size);
#else
AUDIOCONVERT_API int AudioConvert_Buffer( const char *inputfilename,int sample_rate,int channels,int sec, short *pcmbuffer,int *pcmbuff_size);
#endif 

#endif
