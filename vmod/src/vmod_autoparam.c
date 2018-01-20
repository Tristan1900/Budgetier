#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"
//test
int count = 0;
int
init_function(const struct vrt_ctx *ctx, struct vmod_priv *priv,
	      enum vcl_event_e e)
{
  if (e != VCL_EVENT_LOAD)
    return (0);

  /* init what you need */
  return (0);
}

VCL_BOOL
vmod_lru(VRT_CTX, VCL_STRING p)
{
  if(count == 0) {
    return(1);
  } else {
    count++;
    return(0);
  }
}
