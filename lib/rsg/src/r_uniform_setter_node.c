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

struct RsgUniformSetterNode {
  RsgNode node;
  SLIST_HEAD(, RsgUniform) uniforms;
};

static const char* getType(void) {
  return "RsgUniformSetterNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgUniformSetterNode* cnode = (RsgUniformSetterNode*)node;
  /*
   * Push all our uniforms to the local context.
   */
  RsgUniform* uniform;
  SLIST_FOREACH(uniform, &cnode->uniforms, entries) {
    rsgLocalContextSetUniform(lctx, uniform);
  }
}

RsgUniformSetterNode* rsgUniformSetterNodeCreate(const char** names,
                                                 RsgValue* values,
                                                 size_t size) {
  RsgUniformSetterNode* node = rsgMalloc(sizeof(*node));
  rsgNodeInit(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;

  // other data
  SLIST_INIT(&node->uniforms);

  // configure our uniforms list from the user data (initial values)
  size_t i;
  for (i = 0; i < size; i++) {
    RsgUniform* uniform = rsgMalloc(sizeof(*uniform));
    uniform->name = names[i];
    uniform->value = values[i];
    printf("RsgUniformSetterNode: inserting uniform '%s' of type %d\n",
           uniform->name, uniform->value.type);
    SLIST_INSERT_HEAD(&node->uniforms, uniform, entries);
  }

  return node;
}
