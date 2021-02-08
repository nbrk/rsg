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

/*
 * Shader node.
 * Creates OpenGL shader program object from sources in memory or files.
 *
 * On process: sets active shader program in the local context.
 *
 * Properties: none
 */

G_DECLARE_FINAL_TYPE(RsgShaderNode,
                     rsg_shader_node,
                     RSG,
                     SHADER_NODE,
                     RsgAbstractNode)

struct _RsgShaderNode {
  RsgAbstractNode abstract;
  GLuint program;
};

G_DEFINE_TYPE(RsgShaderNode, rsg_shader_node, RSG_TYPE_ABSTRACT_NODE)

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgShaderNode* cnode = RSG_SHADER_NODE(node);

  /*
   * Set our program in the current local context.
   */
  ctx->local->program = cnode->program;
}

static void rsg_shader_node_class_init(RsgShaderNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;
}

static void rsg_shader_node_init(RsgShaderNode* cnode) {}

RsgNode* rsgShaderNodeCreate(unsigned int program) {
  RsgNode* node = g_object_new(rsg_shader_node_get_type(), NULL);

  RSG_SHADER_NODE(node)->program = program;
  return node;
}

RsgNode* rsgShaderNodeCreateFromMemory(const char* vertexText,
                                       const char* fragmentText) {
  GLuint program =
      rsgShaderProgramAssembleFromStrings(vertexText, fragmentText);
  assert(program != 0);
  return rsgShaderNodeCreate(program);
}

RsgNode* rsgShaderNodeCreateFromFiles(const char* vertexPath,
                                      const char* fragmentPath) {
  GLuint program = rsgShaderProgramAssembleFromFiles(vertexPath, fragmentPath);
  assert(program != 0);
  return rsgShaderNodeCreate(program);
}
