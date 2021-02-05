/*
 * Copyright 2021 Nikolay Burkov <nbrk@linklevel.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "rsg_internal.h"

#include <stdio.h>
#include <string.h>

void rsgLocalContextSetUniform(RsgLocalContext* lctx,
                               const char* name,
                               RsgValue value) {
  size_t i;
  // overwrite
  for (i = 0; i < lctx->numUniforms; i++) {
    if (strcmp(lctx->uniformNames[i], name) == 0) {
      lctx->uniformValues[i] = value;
      //      printf("Local context: overwritten uniform '%s'\n", name);
      return;
    }
  }

  // no place for new uniform
  if (lctx->numUniforms >= RSG_LOCAL_CONTEXT_MAX_UNIFORMS) {
    printf(
        "WARNING: Local context: max uniforms exceeded, ignoring insertion of "
        "'%s'\n",
        name);
    return;
  }

  // insertion of new uniform
  size_t uniformIndex = lctx->numUniforms;
  lctx->numUniforms += 1;
  lctx->uniformNames[uniformIndex] = name;
  lctx->uniformValues[uniformIndex] = value;
  printf("Local context: inserted uniform '%s' (%zu/%d)\n", name,
         lctx->numUniforms, RSG_LOCAL_CONTEXT_MAX_UNIFORMS);
}

void rsgLocalContextSetDefaults(RsgLocalContext* lctx) {
  // default variables
  //  lctx->program = 0;
  // default lists
  //  SLIST_INIT(&lctx->uniforms);

  bzero(lctx, sizeof(*lctx));
}

RsgLocalContext* rsgLocalContextCreateCopy(RsgLocalContext* origlctx) {
  RsgLocalContext* lctx = rsgMalloc(sizeof(*lctx));

  rsgLocalContextSetDefaults(lctx);

  // values copy (shallow)
  lctx->program = origlctx->program;
  lctx->numUniforms = origlctx->numUniforms;

  // indirection/lists copy
  size_t i;
  for (i = 0; i < RSG_LOCAL_CONTEXT_MAX_UNIFORMS; i++) {
    lctx->uniformNames[i] = origlctx->uniformNames[i];
    lctx->uniformValues[i] = origlctx->uniformValues[i];
  }

  return lctx;
}

void rsgLocalContextDestroy(RsgGlobalContext* lctx) {
  // NOTE: no indirection, just free the whole lctx memory block
  rsgFree(lctx);
}
