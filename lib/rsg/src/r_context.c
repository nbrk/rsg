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

static RsgGlobalContext* globalContext = NULL;

RsgGlobalContext* rsgGetGlobalContext(void) {
  return globalContext;
}

void rsgSetGlobalContext(RsgGlobalContext* gctx) {
  globalContext = gctx;
}

int rsgGetScreenWidth(void) {
  assert(globalContext != NULL);
  int width;
  int height;
  glfwGetWindowSize(globalContext->window, &width, &height);
  return width;
}

int rsgGetScreenHeight(void) {
  assert(globalContext != NULL);
  int width;
  int height;
  glfwGetWindowSize(globalContext->window, &width, &height);
  return height;
}

void rsgLocalContextReset(RsgLocalContext* lctx) {
  lctx->program = 0;
  lctx->u_projection = glms_mat4_identity();
  lctx->u_view = glms_mat4_identity();
}
