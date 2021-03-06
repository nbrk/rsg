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
 * Mesh node.
 * Actually draws the geometry in OpenGL using values from the local context.
 *
 * Properties: none
 */

G_DECLARE_FINAL_TYPE(RsgMeshNode,
                     rsg_mesh_node,
                     RSG,
                     MESH_NODE,
                     RsgAbstractNode)

struct _RsgMeshNode {
  RsgAbstractNode abstract;
  GLuint vao;
};

G_DEFINE_TYPE(RsgMeshNode, rsg_mesh_node, RSG_TYPE_ABSTRACT_NODE)

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgMeshNode* cnode = RSG_MESH_NODE(node);

  /*
   * Actually draw the geometry setting various OpenGL values/shader uniforms
   * from the local context beforehand.
   */
  /*
   * TODO: Save current OpenGL state
   */

  // program
  glUseProgram(ctx->local->program);

  // uniforms
  GLint uniformLocation;
  uniformLocation = glGetUniformLocation(ctx->local->program, "u_view");
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE,
                     (GLfloat*)&ctx->local->u_view);
  uniformLocation = glGetUniformLocation(ctx->local->program, "u_projection");
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE,
                     (GLfloat*)&ctx->local->u_projection);

  //  size_t i;
  //  for (i = 0; i < lctx->numUniforms; i++) {
  //    /*
  //     * Set the value if the program in use has such a uniform location.
  //     * Determine the corresponding OpenGL typed set call using the Value
  //     type.
  //     */
  //    GLint uniformLocation =
  //        glGetUniformLocation(lctx->program, lctx->uniformNames[i]);
  //    if (uniformLocation != -1) {
  //      RsgValue* value = &lctx->uniformValues[i];
  //      if (value->type == RSG_VALUE_INT)
  //        glUniform1i(uniformLocation, value->asInt);
  //      if (value->type == RSG_VALUE_FLOAT)
  //        glUniform1f(uniformLocation, value->asFloat);
  //      if (value->type == RSG_VALUE_VEC3)
  //        glUniform3fv(uniformLocation, 1, (GLfloat*)&value->asVec3);
  //      if (value->type == RSG_VALUE_VEC4)
  //        glUniform4fv(uniformLocation, 1, (GLfloat*)&value->asVec4);
  //      if (value->type == RSG_VALUE_MAT4)
  //        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE,
  //                           (GLfloat*)&value->asMat4);
  //    }
  //  }

  // draw
  glBindVertexArray(cnode->vao);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

  /*
   * TODO: Restore OpenGL state
   */

  // for now, just reset to zeroes
  glBindVertexArray(0);
  glUseProgram(0);
}

static void rsg_mesh_node_class_init(RsgMeshNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;
}

static void rsg_mesh_node_init(RsgMeshNode* cnode) {}

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

RsgNode* rsgMeshNodeCreateTriangle(void) {
  GLuint vao = generateTriangle();
  assert(vao != 0);

  RsgNode* node = g_object_new(rsg_mesh_node_get_type(), NULL);

  RSG_MESH_NODE(node)->vao = vao;
  return node;
}
