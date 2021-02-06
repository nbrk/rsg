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
#include <string.h>
#include "rsg_internal.h"

#define PROJECTION_ORTHO 1
#define PROJECTION_PERSP 2

struct RsgCameraNode {
  RsgNode node;

  int projection;
  vec3s position;
  float yaw;    // horizontal angle
  float pitch;  // vertical angle
  float aspect;
  float fov;  // field of view
  float nearPlane;
  float farPlane;

  // position displacement vector components cache
  float forward;
  float strafe;
  float jump;

  // matrix cache
  mat4s viewMatrix;
  mat4s projectionMatrix;
};

static void recalcMatrices(RsgCameraNode* node) {
  // view matrix: needs direction, right and up vectors
  vec3s direction = (vec3s){cos(node->pitch) * sin(node->yaw), sin(node->pitch),
                            cos(node->pitch) * cos(node->yaw)};
  vec3s right = (vec3s){sin(node->yaw - M_PI_2), 0, cos(node->pitch - M_PI_2)};
  vec3s up = glms_cross(right, direction);
  node->viewMatrix =
      glms_lookat(node->position, glms_vec3_add(node->position, direction), up);

  // projection matrix
  if (node->projection == PROJECTION_PERSP)
    node->projectionMatrix = glms_perspective_default(node->aspect);
  if (node->projection == PROJECTION_ORTHO)
    node->projectionMatrix = glms_ortho_default(node->aspect);
}

static void positionMoveBy(RsgCameraNode* node,
                           float forward,
                           float strafe,
                           float jump) {
  vec3s direction = (vec3s){cos(node->pitch) * sin(node->yaw), sin(node->pitch),
                            cos(node->pitch) * cos(node->yaw)};
  vec3s right = (vec3s){sin(node->yaw - M_PI_2), 0, cos(node->pitch - M_PI_2)};
  vec3s up = glms_cross(right, direction);

  direction = glms_vec3_scale(direction, forward);
  right = glms_vec3_scale(right, strafe);
  up = glms_vec3_scale(up, jump);

  vec3s newposition;
  newposition = glms_vec3_add(node->position, direction);
  newposition = glms_vec3_add(newposition, right);
  newposition = glms_vec3_add(newposition, up);

  node->position = newposition;
}

static const char* getType(void) {
  return "RsgCameraNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgCameraNode* cnode = (RsgCameraNode*)node;

  // tweak the position if we have a displacement vector components set
  if ((cnode->forward == cnode->strafe == cnode->jump == 0.0f) == false)
    positionMoveBy(cnode, cnode->forward, cnode->strafe, cnode->jump);

  /*
   * Recalc view & projection matrices in any case (uses yaw, pitch, position).
   */
  recalcMatrices(cnode);

  /*
   * Set the well-known uniform matrices in the context. Use cached values.
   */
  rsgLocalContextSetUniform(lctx, "u_view", rsgValueMat4(cnode->viewMatrix));
  rsgLocalContextSetUniform(lctx, "u_projection",
                            rsgValueMat4(cnode->projectionMatrix));
}

static RsgValue getProperty(RsgNode* node, const char* name) {
  RsgCameraNode* cnode = (RsgCameraNode*)node;
  if (strcmp(name, "position") == 0) {
    return rsgValueVec3(cnode->position);
  }
  if (strcmp(name, "yaw") == 0) {
    return rsgValueFloat(cnode->yaw);
  }
  if (strcmp(name, "pitch") == 0) {
    return rsgValueFloat(cnode->pitch);
  }
  if (strcmp(name, "aspect") == 0) {
    return rsgValueFloat(cnode->aspect);
  }
  if (strcmp(name, "fov") == 0) {
    return rsgValueFloat(cnode->fov);
  }

  assert("Unknown property" && 0);
}

static void setProperty(RsgNode* node, const char* name, RsgValue val) {
  RsgCameraNode* cnode = (RsgCameraNode*)node;
  if (strcmp(name, "position") == 0) {
    printf("Camera position update with value: %f, %f\n", val.asVec2.raw[0],
           val.asVec2.raw[1]);
    cnode->position = val.asVec3;
    recalcMatrices(cnode);
    return;
  }
  if (strcmp(name, "yaw") == 0) {
    cnode->yaw = val.asFloat;
    recalcMatrices(cnode);
    return;
  }
  if (strcmp(name, "pitch") == 0) {
    cnode->pitch = val.asFloat;
    recalcMatrices(cnode);
    return;
  }

  // set-only property
  if (strcmp(name, "yaw_delta") == 0) {
    rsgNodeSetProperty(node, "yaw", rsgValueFloat(cnode->yaw + val.asFloat));
    return;
  }

  // set-only property
  if (strcmp(name, "pitch_delta") == 0) {
    rsgNodeSetProperty(node, "pitch",
                       rsgValueFloat(cnode->pitch + val.asFloat));
    return;
  }

  // convenience set-only property to couple the angles with our trackball node
  if (strcmp(name, "trackball_xy_delta_yaw_pitch") == 0) {
    float yawDelta = -0.001f * val.asVec2.raw[0];
    float pitchDelta = -0.001f * val.asVec2.raw[1];
    rsgNodeSetProperty(node, "yaw_delta", rsgValueFloat(yawDelta));
    rsgNodeSetProperty(node, "pitch_delta", rsgValueFloat(pitchDelta));
    return;
  }

  // set-only property to couple with the kbd node: possibly set the x/z
  // components of the camera position displacement vector
  if (strcmp(name, "keyboard_wasd_presses") == 0) {
    int key = val.asInt;
    const float step = 0.1f;
    if (key == GLFW_KEY_W)
      cnode->forward = step;
    if (key == GLFW_KEY_S)
      cnode->forward = -step;
    if (key == GLFW_KEY_A)
      cnode->strafe = -step;
    if (key == GLFW_KEY_D)
      cnode->strafe = step;
    return;
  }

  if (strcmp(name, "keyboard_wasd_releases") == 0) {
    int key = val.asInt;
    if (key == GLFW_KEY_W)
      cnode->forward = 0.0f;
    if (key == GLFW_KEY_S)
      cnode->forward = 0.0f;
    if (key == GLFW_KEY_A)
      cnode->strafe = 0.0f;
    if (key == GLFW_KEY_D)
      cnode->strafe = 0.0f;
    return;
  }

  if (strcmp(name, "aspect") == 0) {
    cnode->aspect = val.asFloat;
    recalcMatrices(cnode);
    return;
  }
  if (strcmp(name, "fov") == 0) {
    cnode->fov = val.asFloat;
    recalcMatrices(cnode);
    return;
  }

  assert("Unknown property" && 0);
}

RsgCameraNode* rsgCameraNodeCreate(vec3s position,
                                   float horizAngle,
                                   float vertAngle,
                                   float fov,
                                   float aspect,
                                   float nearPlane,
                                   float farPlane,
                                   bool perspective) {
  RsgCameraNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;
  node->node.getPropertyFunc = getProperty;
  node->node.setPropertyFunc = setProperty;

  // others
  node->position = position;
  node->yaw = horizAngle;
  node->pitch = vertAngle;
  node->fov = fov;
  node->aspect = aspect;
  node->nearPlane = nearPlane;
  node->farPlane = farPlane;
  node->projection = perspective ? PROJECTION_PERSP : PROJECTION_ORTHO;
  node->forward = 0.0f;
  node->strafe = 0.0f;
  node->jump = 0.0f;

  // initally, calculate the matrices
  recalcMatrices(node);

  return node;
}

RsgCameraNode* rsgCameraNodeCreatePerspectiveDefault(float aspect) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f}, M_PI, 0.0f, 45.f,
                             aspect, 0.1f, 1000.f, true);
}

RsgCameraNode* rsgCameraNodeCreateOrthographicDefault(float aspect) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f}, M_PI, 0.0f, 45.f,
                             aspect, 0.1f, 1000.f, false);
}
