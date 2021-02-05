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

extern RsgGlobalContext* rsgGlobalContext;

void rsgMainLoop(RsgNode* rootNode) {
  RsgLocalContext* lctx = rsgMalloc(sizeof(*lctx));
  rsgLocalContextSetDefaults(lctx);

  RsgGlobalContext* gctx = rsgGlobalContext;

  /*
   * Process the node (presumably, a group or otherwise root node) using the
   * global context and the volatile local context frequenly changing from node
   * to node in a subtree.
   */
  while (glfwWindowShouldClose(gctx->window) == 0) {
    glfwWaitEvents();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rootNode->processFunc(rootNode, lctx, gctx);
    gctx->totalTraversals++;

    glfwSwapBuffers(gctx->window);
  }

  printf("RSG: main loop done after %zu traversals\n", gctx->totalTraversals);
}
