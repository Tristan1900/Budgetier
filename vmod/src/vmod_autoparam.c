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

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#include "vapi/vsm.h"
#include "vapi/vsl.h"
#include "vapi/voptget.h"
#include "vdef.h"
#include "vut.h"
#include "vtree.h"
#include "vas.h"
#include "vdef.h"
#include "vcs.h"
#include <time.h>
#include "vqueue.h"
#include "miniobj.h"
//test
struct cacheLT {
  unsigned                magic;
#define CACHELT_MAGIC             0xA7D4005C
  VTAILQ_ENTRY(cacheLT)     list;
  long hash;
  unsigned long size;
};
VTAILQ_HEAD(cacheLT_head1, cacheLT);
VTAILQ_HEAD(cacheLT_head2, cacheLT);

// varnish hashmaptree
struct cacheMT {
  long hash;
  unsigned long size;
  VRB_ENTRY(cacheMT) e_key;
  struct cacheLT * listEntry;
};

static VRB_HEAD(t_key1, cacheMT) h_key1 = VRB_INITIALIZER(&h_key1);
static VRB_HEAD(t_key2, cacheMT) h_key2 = VRB_INITIALIZER(&h_key2);

static inline int
lcmp_key(const struct cacheMT *a, const struct cacheMT *b)
{
  if (a->hash != b->hash)
    return (a->hash - b->hash);
  return (a->size - b->size);
}

VRB_PROTOTYPE_STATIC(t_key1, cacheMT, e_key, lcmp_key);
VRB_GENERATE_STATIC(t_key1, cacheMT, e_key, lcmp_key);
VRB_PROTOTYPE_STATIC(t_key2, cacheMT, e_key, lcmp_key);
VRB_GENERATE_STATIC(t_key2, cacheMT, e_key, lcmp_key);

struct cacheLT_head1 cacheList1;
struct cacheLT_head2 cacheList2;
long current_objectc1;
long current_objectc2;
long current_size1;
long current_size2;
long unsigned hitc1, hitc2;

// hillclimbing
double lookSize;
double stepSize;

int
init_function(const struct vrt_ctx *ctx, struct vmod_priv *priv,
        enum vcl_event_e e)
{
  if (e != VCL_EVENT_LOAD) {
    return (0);
  }

  /* init what you need */
  return (0);
}

VCL_BOOL
vmod_lru(VRT_CTX, VCL_STRING p, VCL_INT id, VCL_BOOL whichCache) {
  char ip[100];
  int port = 80;
  char page[100];
  sscanf(p, "http://%99[^:]:%99d/%99[^\n]", ip, &port, page);
  long id = atol(page)
  struct cacheMT *lp, l;
  l.hash = id;
  l.size = size;
  if(whichCache) {
    lp = VRB_FIND(t_key1, &h_key1, &l);
  } else {
    lp = VRB_FIND(t_key2, &h_key2, &l);
  }
  if(lp) {
    struct cacheLT *w2;
    w2 = lp->listEntry;
    if(whichCache) {
      VTAILQ_REMOVE(&cacheList1, w2, list);
      VTAILQ_INSERT_TAIL(&cacheList1, w2, list);
    } else {
      VTAILQ_REMOVE(&cacheList2, w2, list);
      VTAILQ_INSERT_TAIL(&cacheList2, w2, list);
    }
    return(1);
  }

  // check object size
  if(size>cache_size) {
    return(0); //cannot admit at all
  }
  // check if want to admit
  if( (strcmp("ThLRU",cacheType)==0 && pow(2,tparam) < size) || (strcmp("ExpLRU",cacheType)==0 && exp(-1*((double)size)/ pow(2,tparam)) < drand48()) ) {
    //    printf("%s: dont admit\n",cacheType);
    return(0);
  }

  // check if space
  long * current_size;
  if(whichCache)
    current_size = &current_size1;
  else
    current_size = &current_size2;
  while(*current_size + size>cache_size) {
    struct cacheLT *wr;
    if(whichCache) {
      wr = VTAILQ_FIRST(&cacheList1);
    } else {
      wr = VTAILQ_FIRST(&cacheList2);
    }
    assert(wr);
    struct cacheMT tpt, * tpr;
    tpt.hash = wr->hash;
    tpt.size = wr->size;
    if(whichCache) {
      tpr = VRB_FIND(t_key1, &h_key1, &tpt);
    } else {
      tpr = VRB_FIND(t_key2, &h_key2, &tpt);
    }
    if(!tpr) {
      printf("not found: %lu %lu [cache:%i]\n",wr->hash,wr->size,whichCache);
    }
    assert(tpr);
    //    printf("evict: %lu %lu (%li|%li|%li) \n",wr->hash,wr->size,*current_size,cache_size,size);
    fflush(stdout);
    *current_size -= wr->size;
    if(whichCache) {
      VRB_REMOVE(t_key1, &h_key1, tpr);
      VTAILQ_REMOVE(&cacheList1, wr, list);
      current_objectc1--;
    } else {
      VRB_REMOVE(t_key2, &h_key2, tpr);
      VTAILQ_REMOVE(&cacheList2, wr, list);
      current_objectc2--;
    }
    free(tpr);
    free(wr);
  }
  // insert into cacheList
  struct cacheLT *w;
  ALLOC_OBJ(w, CACHELT_MAGIC);
  w->hash = id;
  w->size = size;
  //  printf("insert: %lu %lu (%li|%li)\n",w->hash,w->size,*current_size,cache_size);
  fflush(stdout);
  *current_size += size;
  if(whichCache) {
    VTAILQ_INSERT_TAIL(&cacheList1, w, list);
  } else {
    VTAILQ_INSERT_TAIL(&cacheList2, w, list);
  }
  // insert into cacheMT tree
  lp = malloc(sizeof(struct cacheMT));
  lp->hash = id;
  lp->size = size;
  lp->listEntry = w;
  if(whichCache) {
    VRB_INSERT(t_key1, &h_key1, lp);
    current_objectc1++;
  } else {
    VRB_INSERT(t_key2, &h_key2, lp);
    current_objectc2++;
  }
  return(0);
}
