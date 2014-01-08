#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"some_function.h"

/*************************************************************************
   [L,S,T,maxes] = find_landmarks(D,SR,N)
   SR、FS是采样率
   Make a set of spectral feature pair landmarks from some audio data
   D is an audio waveform at sampling rate SR
   L returns as a set of landmarks, as rows of a 4-column matrix
                  {start-time-col start-freq-row end-freq-row delta-time}
   N is the target hashes-per-sec (approximately; default 5)
   S returns the filtered log-magnitude surface
   maxes returns a list of the actual time-frequency peaks extracted.

  REVISED VERSION FINDS PEAKS INCREMENTALLY IN TIME WITH DECAYING THRESHOLD
 *************************************************************************/



void find_landmarks(DATAI *L,DATAF *S,DATAF *maxes,DATAF D,float SR,float N);