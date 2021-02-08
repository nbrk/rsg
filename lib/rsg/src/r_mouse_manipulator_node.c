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
 * Mouse manipulator node.
 *
 * Properties:
 * - "xy" (Read-only) of vec2 (current position)
 * - "xyChange" (Read-only) of vec2 (delta between old and current xy sample)
 *
 * On process:
 * - read mouse position
 * - update fields & notify on props change
 */

G_DECLARE_FINAL_TYPE(RsgMouseManipulatorNode, rsg_mouse_manipulator_node, RSG,
                     MOUSE_MANIPULATOR_NODE, RsgAbstractNode)

struct _RsgMouseManipulatorNode {
  RsgAbstractNode abstract;
  //  vec2s xy;
  //  vec2s xyChange;
  int x;
  int y;
  int xChange;
  int yChange;
};

G_DEFINE_TYPE(RsgMouseManipulatorNode, rsg_mouse_manipulator_node,
              RSG_TYPE_ABSTRACT_NODE)

enum {
  // PROP_XY = 1,
  //    PROP_XY_CHANGE,
  PROP_X = 1,
  PROP_X_CHANGE,
  PROP_Y,
  PROP_Y_CHANGE,
  N_PROPERTIES
};

static GParamSpec* properties[N_PROPERTIES] = {NULL};

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgMouseManipulatorNode* cnode = RSG_MOUSE_MANIPULATOR_NODE(node);

  double x, y;
  glfwGetCursorPos(ctx->global->window, &x, &y);

  if (x != cnode->x || y != cnode->y) {
    // x or y has been changed
    int xChange = x - cnode->x;
    int yChange = y - cnode->y;
    cnode->x = x;
    cnode->y = y;
    //    printf("Mouse manip: x/y changed to %d, %d\n", cnode->x, cnode->y);
    g_object_notify_by_pspec(G_OBJECT(node), properties[PROP_X]);
    g_object_notify_by_pspec(G_OBJECT(node), properties[PROP_Y]);

    // delta x or delta y has been changed
    if (xChange != cnode->xChange || yChange != cnode->yChange) {
      cnode->xChange = xChange;
      cnode->yChange = yChange;
      //      printf("Mouse manip: delta x/y changed to %d, %d\n",
      //      cnode->xChange,
      //             cnode->yChange);
      g_object_notify_by_pspec(G_OBJECT(node), properties[PROP_X_CHANGE]);
      g_object_notify_by_pspec(G_OBJECT(node), properties[PROP_Y_CHANGE]);
    }
  }
}

static void get_property(GObject* object, guint property_id, GValue* value,
                         GParamSpec* pspec) {
  RsgMouseManipulatorNode* cnode = RSG_MOUSE_MANIPULATOR_NODE(object);

  switch (property_id) {
    case PROP_X:
      g_value_set_int(value, cnode->x);
      break;
    case PROP_X_CHANGE:
      g_value_set_int(value, cnode->xChange);
      break;
    case PROP_Y:
      g_value_set_int(value, cnode->y);
      break;
    case PROP_Y_CHANGE:
      g_value_set_int(value, cnode->yChange);
      break;
      //    case PROP_XY:
      //      g_value_set_boxed(value, &cnode->xy);
      //      break;
      //    case PROP_XY_CHANGE:
      //      g_value_set_boxed(value, &cnode->xyChange);
      //      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void rsg_mouse_manipulator_node_class_init(
    RsgMouseManipulatorNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;

  //  G_OBJECT_CLASS(klass)->set_property = set_property;
  G_OBJECT_CLASS(klass)->get_property = get_property;

  //  properties[PROP_XY] =
  //      g_param_spec_boxed("xy", "XY", "XY of mouse coordinates",
  //                         vec2s_get_type(), G_PARAM_READABLE);
  //  properties[PROP_XY_CHANGE] = g_param_spec_boxed(
  //      "xyChange", "XY Change", "Change (delta) in XY of mouse coordinates",
  //      vec2s_get_type(), G_PARAM_READABLE);
  properties[PROP_X] =
      g_param_spec_int("x", "X", "X of mouse coordinates", G_MININT, G_MAXINT,
                       0, G_PARAM_READABLE);
  properties[PROP_Y] =
      g_param_spec_int("y", "Y", "Y of mouse coordinates", G_MININT, G_MAXINT,
                       0, G_PARAM_READABLE);
  properties[PROP_X_CHANGE] =
      g_param_spec_int("xChange", "X change", "Change in X mouse coordinate",
                       G_MININT, G_MAXINT, 0, G_PARAM_READABLE);
  properties[PROP_Y_CHANGE] =
      g_param_spec_int("yChange", "Y change", "Change in Y mouse coordinate",
                       G_MININT, G_MAXINT, 0, G_PARAM_READABLE);

  g_object_class_install_properties(G_OBJECT_CLASS(klass), N_PROPERTIES,
                                    properties);
}

static void rsg_mouse_manipulator_node_init(RsgMouseManipulatorNode* cnode) {
  /*
   * Defaults
   */
  double xPos, yPos;
  glfwGetCursorPos(rsgGetGlobalContext()->window, &xPos, &yPos);
  //  cnode->xy = (vec2s){xPos, yPos};
  //  cnode->xyChange = (vec2s){0.0f, 0.0f};
  cnode->x = xPos;
  cnode->y = yPos;
  cnode->xChange = 0;
  cnode->yChange = 0;
}

RsgNode* rsgMouseManipulatorNodeCreate(void) {
  RsgNode* node = g_object_new(rsg_mouse_manipulator_node_get_type(), NULL);
  //  double xPos, yPos;
  //  glfwGetCursorPos(rsgGetGlobalContext()->window, &xPos, &yPos);
  //  rsgNodeSetProperty(node, "xy",
  //                     rsgValueVec2((vec2s){(float)xPos, (float)yPos}));
  //  rsgNodeSetProperty(node, "xyChange", rsgValueVec2((vec2s){0.0f, 0.0f}));

  return node;
}
