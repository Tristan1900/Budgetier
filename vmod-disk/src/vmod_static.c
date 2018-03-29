#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

#include "cache/cache.h"

#include "vtim.h"
#include "vcc_static_if.h"


#define FILE_NAME "/home/wenqim/result/admission.dat"
// helper funciton
void *timer();

int counter = 0;
long count = 0;
pthread_t tid;
/*
 * handle vmod internal state, vmod init/fini and/or varnish callback
 * (un)registration here.
 *
 * malloc'ing the info buffer is only indended as a demonstration, for any
 * real-world vmod, a fixed-sized buffer should be a global variable
 */

int v_matchproto_(vmod_event_f)
event_function(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
	(void)ctx;
	(void)priv;
	if (e != VCL_EVENT_LOAD)
		return (0);

  pthread_create(&tid, NULL, timer, NULL);
	return (0);
}

void *timer() {
	while(1) {
		sleep(5);
		FILE *f;
		f = fopen(FILE_NAME,"a+");
		fprintf(f, "%ld \n", count);
		fclose(f);
		count = 0;
	}
}

VCL_BOOL
vmod_lru(VRT_CTX, VCL_STRING p, VCL_STRING s, VCL_BOOL whichCache, VCL_STRING f) {
    double ran = (double)rand() / (double)RAND_MAX;
    if(ran <= atof(f)) {
    	long size = atol(s);
    	count += size;
        return 1;
    }
    return 0;
}
