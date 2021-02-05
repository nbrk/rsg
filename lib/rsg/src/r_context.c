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

void rsgLocalContextSetUniform(RsgLocalContext* lctx, RsgUniform* uniform) {
  RsgUniform* someUniform;

  // check if the name is already present in the ctx (overwrite)
  SLIST_FOREACH(someUniform, &lctx->uniforms, entries) {
    if (strcmp(someUniform->name, uniform->name) == 0) {
      someUniform->value = uniform->value;
      printf("Local ctx: overwritten uniform '%s'\n", uniform->name);
      return;
    }
  }

  // insert a new uniform
  someUniform = rsgMalloc(sizeof(*someUniform));
  someUniform->name = uniform->name;
  someUniform->value = uniform->value;
  SLIST_INSERT_HEAD(&lctx->uniforms, someUniform, entries);
  printf("Local ctx: inserted uniform '%s'\n", uniform->name);
}

void rsgLocalContextInitDefaults(RsgLocalContext* lctx) {
  lctx->program = 0;
  SLIST_INIT(&lctx->uniforms);
}

RsgLocalContext* rsgLocalContextDeepCopy(RsgLocalContext* origlctx) {
  RsgLocalContext* lctx = rsgMalloc(sizeof(*lctx));

  rsgLocalContextInitDefaults(lctx);

  // values copy (shallow)
  lctx->program = origlctx->program;

  // lists copy (deep)
  RsgUniform* uniform;
  SLIST_FOREACH(uniform, &origlctx->uniforms, entries) {
    RsgUniform* newUniform = rsgMalloc(sizeof(*newUniform));
    SLIST_INSERT_HEAD(&lctx->uniforms, newUniform, entries);
  }

  return lctx;
}
