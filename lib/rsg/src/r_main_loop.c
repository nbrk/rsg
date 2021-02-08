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
#include <stdio.h>
#include <unistd.h>

#include "rsg_internal.h"

void rsgMainLoop(RsgNode* root, int traversalFreq) {
  assert(rsgGetGlobalContext() != NULL);
  assert(root != NULL);
  void (*checkEventsFunc)(void) = NULL;
  int (*usecSleepFunc)(useconds_t usec) = NULL;
  useconds_t usecSleepPeriod = 0;
  RsgAbstractNode* abstractRoot = RSG_ABSTRACT_NODE(root);
  /*
   * Set up the context
   */
  RsgContext* ctx = rsgMalloc(sizeof(*ctx));
  ctx->global = rsgGetGlobalContext();
  ctx->local = rsgMalloc(sizeof(*ctx->local));

  if (traversalFreq <= 0) {
    // event-driven retained mode
    printf("RSG: main loop in retained mode\n");
    checkEventsFunc = glfwWaitEvents;
    usecSleepFunc = NULL;
  } else {
    // continunous update mode
    printf("RSG: main loop in immediate mode (%d traversals per sec)\n",
           traversalFreq);
    checkEventsFunc = glfwPollEvents;
    usecSleepFunc = usleep;
    usecSleepPeriod = (int)((1.0f / traversalFreq) * 1000000);
  }

  while (glfwWindowShouldClose(ctx->global->window) == 0) {
    checkEventsFunc();

    // re-set the local context with default values before each traversal
    rsgLocalContextReset(ctx->local);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RSG_ABSTRACT_NODE_GET_CLASS(abstractRoot)->processFunc(abstractRoot, ctx);
    ctx->global->totalTraversals++;

    glfwSwapBuffers(ctx->global->window);
    if (usecSleepFunc != NULL) usleep(usecSleepPeriod);
  }

  printf("RSG: main loop done after %zu traversals\n",
         ctx->global->totalTraversals);
}
