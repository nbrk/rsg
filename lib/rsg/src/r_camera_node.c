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

G_DECLARE_FINAL_TYPE(RsgCameraNode, rsg_camera_node, RSG, CAMERA_NODE,
                     RsgAbstractNode)

struct _RsgCameraNode {
  RsgAbstractNode abstract;
  vec4s clearColor;
};

G_DEFINE_TYPE(RsgCameraNode, rsg_camera_node, RSG_TYPE_ABSTRACT_NODE)

enum { PROP_CLEAR_COLOR = 1, N_PROPERTIES };

static GParamSpec* properties[N_PROPERTIES] = {NULL};

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(node);

  glClearColor(cnode->clearColor.raw[0], cnode->clearColor.raw[1],
               cnode->clearColor.raw[2], cnode->clearColor.raw[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void set_property(GObject* object, guint property_id,
                         const GValue* value, GParamSpec* pspec) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(object);

  switch (property_id) {
    case PROP_CLEAR_COLOR:
      cnode->clearColor = *(vec4s*)g_value_get_boxed(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void get_property(GObject* object, guint property_id, GValue* value,
                         GParamSpec* pspec) {
  RsgCameraNode* cnode = RSG_CAMERA_NODE(object);

  switch (property_id) {
    case PROP_CLEAR_COLOR:
      g_value_set_boxed(value, &cnode->clearColor);
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
      g_param_spec_boxed("clear_color", "Clear color",
                         "RGBA Color to clear the OpenGL color buffer with",
                         vec4s_get_type(), G_PARAM_READWRITE);

  g_object_class_install_properties(G_OBJECT_CLASS(klass), N_PROPERTIES,
                                    properties);
}

static void rsg_camera_node_init(RsgCameraNode* cnode) {}

RsgNode* rsgCameraNodeCreatePerspectiveDefault(void);
RsgNode* rsgCameraNodeCreateOrthographicDefault(void);
RsgNode* rsgCameraNodeCreate(vec3s position, float horizAngle, float vertAngle,
                             float fov, float aspect, float nearPlane,
                             float farPlane, bool perspective);

// RsgNode* rsgCameraNodeCreate(void) {
//  RsgNode* node = g_object_new(rsg_camera_node_get_type(), NULL);
//  /*
//   * Defaults
//   */
//  rsgNodeSetProperty(node, "clear_color",
//                     rsgValueVec4((vec4s){0.0f, 0.0f, 0.0f, 1.0f}));
//  return node;
//}
