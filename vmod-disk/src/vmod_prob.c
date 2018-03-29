#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>

#include "cache/cache.h"
#include <pthread.h>
#include "vtim.h"

#include "vcc_prob_if.h"

// //helper function
void *timer();

// cache
long cache_size = 1024L*1024L*1024L*200;
long pquota = 1024L*1024L*100L;
long cur_write = 0;
double prob = 1;
pthread_t tid;

int v_matchproto_(vmod_event_f)
event_function(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
	(void)ctx;
	(void)priv;
	if (e != VCL_EVENT_LOAD)
		return(0);

  pthread_create(&tid, NULL, timer, NULL);
	return (0);
}

void *timer() {
  while(1) {
    sleep(10);
    if(cur_write <= pquota) {
      cur_write = 0;
    } else {
      cur_write -= pquota;
    }
    prob = exp(-((double)1 - (((double)pquota - (double)cur_write) / (double)pquota)));
  }
}

VCL_BOOL
vmod_probability(VRT_CTX, VCL_STRING p, VCL_STRING s) {
  double ran = (double)rand() / (double)RAND_MAX;
  if(ran <= prob) {
    return 1;
  }
  return 0;
}