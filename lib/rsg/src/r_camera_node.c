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
 * TODO:
 * Camera node.
 * When processed:
 * sets the following in the local context:
 * - "u_view" (View matrix uniform)
 * - "u_projection" (Projection matrix uniform)
 * clears buffers in the OpenGL state:
 * - using clearColor field
 *
 * Exposes following properties:
 * - "clearColor" of vec4s
 * - "position" of vec3s
 * - "yaw" of float (horizontal angle)
 * - "pitch" of float (vertical angle)
 */

#define PROJ_PERSP 1
#define PROJ_ORTHO 2

G_DECLARE_FINAL_TYPE(RsgCameraNode,
                     rsg_camera_node,
                     RSG,
                     CAMERA_NODE,
                     RsgAbstractNode)

struct _RsgCameraNode {
  RsgAbstractNode abstract;
  vec4s clearColor;
  int projection;
  vec3s position;
  float yaw;    // horizontal angle
  float pitch;  // vertical angle
  float aspect;
  float fov;
  float nearPlane;
  float farPlane;

  // matrix cache
  mat4s viewMatrix;
  mat4s projectionMatrix;
};

G_DEFINE_TYPE(RsgCameraNode, rsg_camera_node, RSG_TYPE_ABSTRACT_NODE)

enum {
  PROP_CLEAR_COLOR = 1,
  PROP_POSITION,
  PROP_YAW,
  PROP_PITCH,
  // write-only
  PROP_YAW_CHANGE,
  PROP_PITCH_CHANGE,
  //      PROP_POSITION_CHANGE,

  N_PROPERTIES
};

static GParamSpec* properties[N_PROPERTIES] = {NULL};

static void recalcMatrices(RsgCameraNode* node) {
  // view matrix: needs direction, right and up vectors
  vec3s direction = (vec3s){cos(node->pitch) * sin(node->yaw), sin(node->pitch),
                            cos(node->pitch) * cos(node->yaw)};
  vec3s right = (vec3s){sin(node->yaw - M_PI_2), 0, cos(node->yaw - M_PI_2)};
  vec3s up = glms_cross(right, direction);
  node->viewMatrix =
      glms_lookat(node->position, glms_vec3_add(node->position, direction), up);

  // projection matrix
  if (node->projection == PROJ_PERSP)
    node->projectionMatrix = glms_perspective(glm_rad(node->fov), node->aspect,
                                              node->nearPlane, node->farPlane);
  if (node->projection == PROJ_ORTHO)
    node->projectionMatrix = glms_ortho_default(node->aspect);
}

static vec3s positionMoveBy(const RsgCameraNode* node,
                            float forward,
                            float strafe,
                            float jump) {
  vec3s direction = (vec3s){cos(node->pitch) * sin(node->yaw), sin(node->pitch),
                            cos(node->pitch) * cos(node->yaw)};
  vec3s right = (vec3s){sin(node->yaw - M_PI_2), 0, cos(node->yaw - M_PI_2)};
  vec3s up = glms_cross(right, direction);

  direction = glms_vec3_scale(direction, forward);
  right = glms_vec3_scale(right, strafe);
  up = glms_vec3_scale(up, jump);

  vec3s newposition;
  newposition = glms_vec3_add(node->position, direction);
  newposition = glms_vec3_add(newposition, right);
  newposition = glms_vec3_add(newposition, up);

  return newposition;
}

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(node);
  /*
   * Write (modify) the OpenGL context
   */
  glClearColor(cnode->clearColor.raw[0], cnode->clearColor.raw[1],
               cnode->clearColor.raw[2], cnode->clearColor.raw[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*
   * Write to the local context
   */
  ctx->local->u_projection = cnode->projectionMatrix;
  ctx->local->u_view = cnode->viewMatrix;
}

static void set_property(GObject* object,
                         guint property_id,
                         const GValue* value,
                         GParamSpec* pspec) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(object);

  switch (property_id) {
    case PROP_CLEAR_COLOR:
      cnode->clearColor = *(vec4s*)g_value_get_boxed(value);
      break;
    case PROP_POSITION:
      cnode->position = *(vec3s*)g_value_get_boxed(value);
      recalcMatrices(cnode);
      break;
    case PROP_YAW:
      cnode->yaw = g_value_get_float(value);
      recalcMatrices(cnode);
      break;
    case PROP_PITCH:
      cnode->pitch = g_value_get_float(value);
      recalcMatrices(cnode);
      break;
    case PROP_YAW_CHANGE:
      cnode->yaw = cnode->yaw - g_value_get_float(value);
      g_object_notify_by_pspec(object, properties[PROP_YAW_CHANGE]);
      recalcMatrices(cnode);
      break;
    case PROP_PITCH_CHANGE:
      cnode->pitch = cnode->pitch - g_value_get_float(value);
      g_object_notify_by_pspec(object, properties[PROP_PITCH_CHANGE]);
      recalcMatrices(cnode);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void get_property(GObject* object,
                         guint property_id,
                         GValue* value,
                         GParamSpec* pspec) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(object);

  switch (property_id) {
    case PROP_CLEAR_COLOR:
      g_value_set_boxed(value, &cnode->clearColor);
      break;
    case PROP_POSITION:
      g_value_set_boxed(value, &cnode->position);
      break;
    case PROP_YAW:
      g_value_set_float(value, cnode->yaw);
      break;
    case PROP_PITCH:
      g_value_set_float(value, cnode->pitch);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void rsg_camera_node_class_init(RsgCameraNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;

  G_OBJECT_CLASS(klass)->set_property = set_property;
  G_OBJECT_CLASS(klass)->get_property = get_property;

  properties[PROP_CLEAR_COLOR] =
      g_param_spec_boxed("clearColor", "Clear color",
                         "RGBA Color to clear the OpenGL color buffer with",
                         vec4s_get_type(), G_PARAM_READWRITE);
  properties[PROP_POSITION] =
      g_param_spec_boxed("position", "Position", "Camera position",
                         vec3s_get_type(), G_PARAM_READWRITE);
  properties[PROP_YAW] =
      g_param_spec_float("yaw", "Yaw", "Horizontal angle", 0.f, 2.f * M_PI,
                         0.0f, G_PARAM_READWRITE);
  properties[PROP_PITCH] =
      g_param_spec_float("pitch", "Pitch", "Vertical angle", 0.f, 2.f * M_PI,
                         0.0f, G_PARAM_READWRITE);
  properties[PROP_YAW_CHANGE] =
      g_param_spec_float("yawChange", "Yaw change", "Horizontal angle delta",
                         -2.f * M_PI, 2.f * M_PI, 0.0f, G_PARAM_WRITABLE);
  properties[PROP_PITCH_CHANGE] =
      g_param_spec_float("pitchChange", "Pitch change", "Vertical angle delta",
                         -2.f * M_PI, 2.f * M_PI, 0.0f, G_PARAM_WRITABLE);

  g_object_class_install_properties(G_OBJECT_CLASS(klass), N_PROPERTIES,
                                    properties);
}

static void rsg_camera_node_init(RsgCameraNode* cnode) {}

RsgNode* rsgCameraNodeCreatePerspectiveDefault(void) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f}, M_PI, 0.0f, 45.f,
                             (float)rsgGetScreenWidth() / rsgGetScreenHeight(),
                             0.1f, 1000.f, true);
}
RsgNode* rsgCameraNodeCreateOrthographicDefault(void) {
  return rsgCameraNodeCreate((vec3s){0.0f, 0.0f, 10.0f}, M_PI, 0.0f, 45.f,
                             (float)rsgGetScreenWidth() / rsgGetScreenHeight(),
                             0.1f, 1000.f, false);
}

RsgNode* rsgCameraNodeCreate(vec3s position,
                             float horizAngle,
                             float vertAngle,
                             float fov,
                             float aspect,
                             float nearPlane,
                             float farPlane,
                             bool perspective) {
  RsgNode* node = g_object_new(rsg_camera_node_get_type(), NULL);
  RsgCameraNode* cnode = RSG_CAMERA_NODE(node);
  cnode->position = position;
  cnode->yaw = horizAngle;
  cnode->pitch = vertAngle;
  cnode->fov = fov;
  cnode->aspect = aspect;
  cnode->nearPlane = nearPlane;
  cnode->farPlane = farPlane;
  cnode->projection = perspective ? PROJ_PERSP : PROJ_ORTHO;

  // initally, calculate the matrices
  recalcMatrices(cnode);

  return node;
}
