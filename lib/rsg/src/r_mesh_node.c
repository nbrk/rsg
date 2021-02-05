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

struct RsgMeshNode {
  RsgNode node;
  GLuint vao;
};

static const char* getType(void) {
  return "RsgMeshNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgMeshNode* cnode = (RsgMeshNode*)node;

  // set program from the context
  glUseProgram(lctx->program);

  // set uniforms from the context (if eligible for this program)
  RsgUniform* uniform;
  SLIST_FOREACH(uniform, &lctx->uniforms, entries) {
    GLint loc = glGetUniformLocation(lctx->program, uniform->name);
    if (loc != -1) {
      if (uniform->value.type == RSG_VALUE_INT)
        glUniform1i(loc, uniform->value.asInt);
      if (uniform->value.type == RSG_VALUE_FLOAT)
        glUniform1f(loc, uniform->value.asFloat);
      if (uniform->value.type == RSG_VALUE_VEC4)
        glUniform4fv(loc, 1, (GLfloat*)&uniform->value.asVec4);
    }
  }

  // draw
  glBindVertexArray(cnode->vao);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);

  glUseProgram(0);
}

static void destroy(RsgNode* node) {
  RsgMeshNode* cnode = (RsgMeshNode*)node;
  glDeleteVertexArrays(1, &cnode->vao);
}

static GLuint generateTriangle(void) {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint bo;
  // position data
  glGenBuffers(1, &bo);
  glBindBuffer(GL_ARRAY_BUFFER, bo);
  glEnableVertexAttribArray(0);
  glBufferData(
      GL_ARRAY_BUFFER, 9 * sizeof(GLfloat),
      (float[9]){-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f},
      GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // index/elements
  glGenBuffers(1, &bo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 /* trig 1 */ * sizeof(GLuint),
               (GLuint[3]){0, 1, 2}, GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // bind calls stored in the VAO

  return vao;
}

static RsgMeshNode* create(void) {
  RsgMeshNode* node = rsgMalloc(sizeof(*node));

  rsgNodeInit(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.destroyFunc = destroy;
  node->node.processFunc = process;
  return node;
}

RsgMeshNode* rsgMeshNodeCreateTriangle(void) {
  RsgMeshNode* node = create();

  // other
  node->vao = generateTriangle();

  return node;
}
