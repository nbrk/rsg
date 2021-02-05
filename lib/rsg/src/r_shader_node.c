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

extern GLuint rsgShaderProgramAssembleFromStrings(const char* vertex_str,
                                                  const char* fragment_str);

struct RsgShaderNode {
  RsgNode node;
  GLuint program;
};

static const char* getType(void) {
  return "RsgShaderNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgShaderNode* cnode = (RsgShaderNode*)node;
  lctx->program = cnode->program;
}

RsgShaderNode* rsgShaderNodeCreate(const char* vertexText,
                                   const char* fragmentText) {
  GLuint program = 0;

  program = rsgShaderProgramAssembleFromStrings(vertexText, fragmentText);
  assert(program != 0 && "shader program creation error");

  /*
   * Linking successful
   */
  RsgShaderNode* node = rsgMalloc(sizeof(*node));

  rsgNodeInit(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;

  // other data
  node->program = program;

  return node;
}
