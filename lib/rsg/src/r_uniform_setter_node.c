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

struct RsgUniformSetterNode {
  RsgNode node;
  size_t numUniforms;
  const char* uniformNames[RSG_LOCAL_CONTEXT_MAX_UNIFORMS];
  RsgValue uniformValues[RSG_LOCAL_CONTEXT_MAX_UNIFORMS];
};

static const char* getType(void) {
  return "RsgUniformSetterNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgUniformSetterNode* cnode = (RsgUniformSetterNode*)node;

  /*
   * Write all our uniforms to the local context.
   */
  size_t i;
  for (i = 0; i < cnode->numUniforms; i++) {
    rsgLocalContextSetUniform(lctx, cnode->uniformNames[i],
                              cnode->uniformValues[i]);
  }
}

static RsgValue getProperty(RsgNode* node, const char* name) {
  RsgUniformSetterNode* cnode = (RsgUniformSetterNode*)node;
  /*
   * Look through our uniforms and make them available as properties
   */
  size_t i;
  for (i = 0; i < cnode->numUniforms; i++) {
    if (strcmp(name, cnode->uniformNames[i]) == 0)
      return cnode->uniformValues[i];
  }
  assert("Unknown property" && 0);
}

static void setProperty(RsgNode* node, const char* name, RsgValue val) {
  RsgUniformSetterNode* cnode = (RsgUniformSetterNode*)node;
  size_t i;
  for (i = 0; i < cnode->numUniforms; i++) {
    if (strcmp(name, cnode->uniformNames[i]) == 0) {
      cnode->uniformValues[i] = val;
      return;
    }
  }
  assert("Unknown property" && 0);
}

RsgUniformSetterNode* rsgUniformSetterNodeCreate(const char** names,
                                                 RsgValue* values,
                                                 size_t numValues) {
  assert(numValues < RSG_LOCAL_CONTEXT_MAX_UNIFORMS);

  RsgUniformSetterNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;
  node->node.getPropertyFunc = getProperty;
  node->node.setPropertyFunc = setProperty;

  // copy uniform names/values from the function args
  node->numUniforms = numValues;
  size_t i;
  for (i = 0; i < numValues; i++) {
    node->uniformNames[i] = names[i];
    node->uniformValues[i] = values[i];
  }

  return node;
}
