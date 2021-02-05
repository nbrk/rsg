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

#define PROJECTION_ORTHO 1
#define PROJECTION_PERSP 2

struct RsgCameraNode {
  RsgNode node;

  int projection;
  vec3s position;
  vec3s target;
  vec3s up;
  float aspect;

  // matrix cache
  mat4s viewMatrix;
  mat4s projectionMatrix;
};

static const char* getType(void) {
  return "RsgCameraNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgCameraNode* cnode = (RsgCameraNode*)node;

  /*
   * Set the well-known uniform matrices uniforms in the context. Use cache.
   */
  rsgLocalContextSetUniform(lctx, "u_view", rsgValueMat4(cnode->viewMatrix));
  rsgLocalContextSetUniform(lctx, "u_projection",
                            rsgValueMat4(cnode->projectionMatrix));
}

static void recalcMatrices(RsgCameraNode* node) {
  // view matrix
  node->viewMatrix = glms_lookat(node->position, node->target, node->up);

  // projection matrix
  if (node->projection == PROJECTION_PERSP)
    node->projectionMatrix = glms_perspective_default(node->aspect);
  if (node->projection == PROJECTION_ORTHO)
    node->projectionMatrix = glms_ortho_default(node->aspect);
}

RsgCameraNode* rsgCameraNodeCreate(vec3s position,
                                   vec3s target,
                                   vec3s up,
                                   float aspect,
                                   bool perspective) {
  RsgCameraNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;

  // others
  node->position = position;
  node->target = target;
  node->up = up;
  node->aspect = aspect;
  node->projection = perspective ? PROJECTION_PERSP : PROJECTION_ORTHO;

  // initally, calculate the matrices
  recalcMatrices(node);

  return node;
}

RsgCameraNode* rsgCameraNodeCreatePerspectiveDefault(float aspect) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f},
                             (vec3s){0.0f, 0.0f, 0.0f},
                             (vec3s){0.0f, 1.0f, 0.0f}, aspect, true);
}

RsgCameraNode* rsgCameraNodeCreateOrthographicDefault(float aspect) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f},
                             (vec3s){0.0f, 0.0f, 0.0f},
                             (vec3s){0.0f, 1.0f, 0.0f}, aspect, false);
}
