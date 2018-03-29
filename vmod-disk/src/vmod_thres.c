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

#include "vcc_thres_if.h"

#include "vtree.h"
// //helper function
void *thres();
//test
struct cacheLT {
  unsigned                magic;
#define CACHELT_MAGIC             0xA7D4005C
  VTAILQ_ENTRY(cacheLT)     list;
  long hash;
  unsigned long size;
};
VTAILQ_HEAD(cacheLT_head1, cacheLT);

// varnish hashmaptree
struct cacheMT {
  long               hash;
  unsigned                magic;
#define CACHEMT_MAGIC   0x9553B65C
  unsigned long size;
  VRB_ENTRY(cacheMT) e_key;
  struct cacheLT * listEntry;
  int               hit_times;
};

static VRB_HEAD(t_key1, cacheMT) h_key1 = VRB_INITIALIZER(&h_key1);

static inline int
lcmp_key(const struct cacheMT *a, const struct cacheMT *b)
{
  if (a->hash - b->hash > 0L) 
    return (1);
  else if(a->hash - b->hash < 0L)
    return(-1);
  else {
    if(a->size - b->size > 0L)
      return(1);
    else if(a->size - b->size < 0L)
      return(-1);
    else return(0);
  }
}

VRB_PROTOTYPE_STATIC(t_key1, cacheMT, e_key, lcmp_key);
VRB_GENERATE_STATIC(t_key1, cacheMT, e_key, lcmp_key);

struct cacheLT_head1 cacheList1;
long current_objectc1;
long current_size1 = 0L;
long unsigned hitc1;

// hillclimbing
double lookSize;
double stepSize;

// cache
long cache_size = 1024L*1024L*1024L*200;

long quota = 1024L*1024L*1024L*3;
long pquota = 1024L*1024L*100L;
struct cacheMT *start;
long hit_num = 0L;
long receive_num = 0L;
long count_num = 0L;
int threshold = 1;
long cur_write = 0;
double prob = 1;
pthread_mutex_t lock;
pthread_t tid;

int v_matchproto_(vmod_event_f)
event_function(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
	(void)ctx;
	(void)priv;
	if (e != VCL_EVENT_LOAD)
		return (0);

  VTAILQ_INIT(&cacheList1);
  pthread_mutex_init(&lock, NULL);
  pthread_create(&tid, NULL, thres, NULL);
	return (0);
}

void *thres() {
  while(1) {
    sleep(10);
    pthread_mutex_lock(&lock);
    if(cur_write > pquota) {
      threshold = -1;
      cur_write -= pquota;
    } else {
      threshold = 1;
      cur_write = 0;
    }
    pthread_mutex_unlock(&lock);
  }
}

VCL_BOOL 
vmod_threshold(VRT_CTX, VCL_STRING p, VCL_STRING s, VCL_BOOL whichCache, VCL_STRING th) {
  // set differnet method
  // whichCache = 0;
  pthread_mutex_lock(&lock);
  count_num++;
  receive_num++;
  char page[100];
  sscanf(p, "/%99[^\n]", page);
  long id = atol(page);
  if(s == NULL) {
    printf("object not exists, wrong url");
    pthread_mutex_unlock(&lock);
    return 0;
  }
  long size = atol(s);
  struct cacheMT *lp, l;
  l.hash = id;
  l.size = size;
  // just want to get size
  if(whichCache == 0) {
    double ran = (double)rand() / (double)RAND_MAX;
    if(ran <= prob) {
      cur_write += l.size;
      pthread_mutex_unlock(&lock);
      return(1);
    } else {
      pthread_mutex_unlock(&lock);
      return(0);
    }
  } 
  lp = VRB_FIND(t_key1, &h_key1, &l);

  if(whichCache && threshold == -1) {
    pthread_mutex_unlock(&lock);
    return(0);
  }
  if(lp) {
    hit_num++;
    struct cacheLT *w2;
    lp->hit_times++;
    w2 = lp->listEntry;

    VTAILQ_REMOVE(&cacheList1, w2, list);
    VTAILQ_INSERT_TAIL(&cacheList1, w2, list);

    if(lp->hit_times <= threshold) {
      pthread_mutex_unlock(&lock);
      return(0);
    }
    // increment the current size and update the threshold
    cur_write += l.size;
    if(cur_write > quota && cur_write < 2 * quota) {
      threshold = 2;
    } else if(cur_write >= 2 * quota && cur_write < 3 * quota) {
      threshold = 3;
    } else if(cur_write >= 3 * quota) {
      threshold = -1;
    }
    pthread_mutex_unlock(&lock);
    return(1);
  }

   // check object size
  if(size>cache_size) {
    pthread_mutex_unlock(&lock);
    return(0); //cannot admit at all
  }

   // check if space
  long * current_size;
  current_size = &current_size1;

  while(*current_size + size>cache_size ) {
    struct cacheLT *wr;
    if(VTAILQ_EMPTY(&cacheList1)) {
      printf("Empty queue?? error\n");
    }
    //if list is empty
    wr = VTAILQ_FIRST(&cacheList1);
    assert(wr);

    struct cacheMT tpt, * tpr;
    tpt.hash = wr->hash;
    tpt.size = wr->size;

    tpr = VRB_FIND(t_key1, &h_key1, &tpt);

    if(!tpr) {
     printf("not found: %lu %lu [cache:%i]\n",wr->hash,wr->size,whichCache);
    }
    //if that item can be find in map, this ususally has error
    assert(tpr);

    *current_size -= wr->size;

    VRB_REMOVE(t_key1, &h_key1, tpr);
    VTAILQ_REMOVE(&cacheList1, wr, list);
    current_objectc1--;

    free(tpr);
    free(wr);
  }

  // insert into cacheList
  struct cacheLT *w;
  ALLOC_OBJ(w, CACHELT_MAGIC);
  w->hash = id;
  w->size = size;
  *current_size += size;
  
  VTAILQ_INSERT_TAIL(&cacheList1, w, list);

  //insert into cacheMT tree
  ALLOC_OBJ(lp, CACHEMT_MAGIC);
  if(lp == NULL) {
    pthread_mutex_unlock(&lock);
    return(0);
  }

  lp->hash = id;
  lp->size = size;
  lp->listEntry = w;
  lp->hit_times = 1;

  current_objectc1++;
  VRB_INSERT(t_key1, &h_key1, lp);

  pthread_mutex_unlock(&lock);
  return(0); 
}
