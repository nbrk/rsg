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

#include <stdio.h>

#include "rsg_internal.h"

G_DECLARE_FINAL_TYPE(RsgPropertyPrinterNode, rsg_property_printer_node, RSG,
                     PROPERTY_PRINTER_NODE, RsgAbstractNode)

struct _RsgPropertyPrinterNode {
  RsgAbstractNode abstract;
};

G_DEFINE_TYPE(RsgPropertyPrinterNode, rsg_property_printer_node,
              RSG_TYPE_ABSTRACT_NODE)

enum {
  PROP_POINTER = 1,
  PROP_INT_CH1,
  PROP_INT_CH2,
  PROP_INT_CH3,
  PROP_INT_CH4,
  PROP_FLOAT,
  PROP_VEC2,
  PROP_VEC3,
  PROP_VEC4,
  N_PROPERTIES
};

static GParamSpec* properties[N_PROPERTIES] = {NULL};

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgPropertyPrinterNode* cnode = RSG_PROPERTY_PRINTER_NODE(node);
}

static void set_property(GObject* object, guint property_id,
                         const GValue* value, GParamSpec* pspec) {
  RsgPropertyPrinterNode* cnode = RSG_PROPERTY_PRINTER_NODE(object);
  vec2s v2;
  vec3s v3;
  vec4s v4;

  switch (property_id) {
    case PROP_POINTER:
      printf("PropertyPrinterNode: pointer property set to %p\n",
             g_value_get_pointer(value));
      break;
    case PROP_INT_CH1:
      printf("PropertyPrinterNode: int1 property set to %d\n",
             g_value_get_int(value));
      break;
    case PROP_INT_CH2:
      printf("PropertyPrinterNode: int2 property set to %d\n",
             g_value_get_int(value));
      break;
    case PROP_INT_CH3:
      printf("PropertyPrinterNode: int3 property set to %d\n",
             g_value_get_int(value));
      break;
    case PROP_INT_CH4:
      printf("PropertyPrinterNode: int4 property set to %d\n",
             g_value_get_int(value));
      break;
    case PROP_FLOAT:
      printf("PropertyPrinterNode: float property set to %f\n",
             g_value_get_float(value));
      break;
    case PROP_VEC2:
      v2 = *(vec2s*)g_value_get_boxed(value);
      printf("PropertyPrinterNode: vec2 property set to %f, %f\n", v2.raw[0],
             v2.raw[1]);
      break;
    case PROP_VEC3:
      v3 = *(vec3s*)g_value_get_boxed(value);
      printf("PropertyPrinterNode: vec3 property set to %f, %f, %f\n",
             v3.raw[0], v3.raw[1], v3.raw[2]);
      break;
    case PROP_VEC4:
      v4 = *(vec4s*)g_value_get_boxed(value);
      printf("PropertyPrinterNode: vec4 property set to %f, %f, %f, %f\n",
             v4.raw[0], v4.raw[1], v4.raw[2], v4.raw[3]);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void rsg_property_printer_node_class_init(
    RsgPropertyPrinterNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;

  G_OBJECT_CLASS(klass)->set_property = set_property;

  properties[PROP_POINTER] = g_param_spec_pointer(
      "pointer", "Pointer", "Pseudo-property", G_PARAM_WRITABLE);
  properties[PROP_INT_CH1] =
      g_param_spec_int("int1", "Integer", "Pseudo-property", G_MININT, G_MAXINT,
                       0, G_PARAM_WRITABLE);
  properties[PROP_INT_CH2] =
      g_param_spec_int("int2", "Integer", "Pseudo-property", G_MININT, G_MAXINT,
                       0, G_PARAM_WRITABLE);
  properties[PROP_INT_CH3] =
      g_param_spec_int("int3", "Integer", "Pseudo-property", G_MININT, G_MAXINT,
                       0, G_PARAM_WRITABLE);
  properties[PROP_INT_CH4] =
      g_param_spec_int("int4", "Integer", "Pseudo-property", G_MININT, G_MAXINT,
                       0, G_PARAM_WRITABLE);
  properties[PROP_FLOAT] =
      g_param_spec_float("float", "Float", "Pseudo-property", G_MINFLOAT,
                         G_MAXFLOAT, 1.0f, G_PARAM_WRITABLE);
  properties[PROP_VEC2] = g_param_spec_boxed(
      "vec2", "Vec2s", "Pseudo-property", vec2s_get_type(), G_PARAM_WRITABLE);
  properties[PROP_VEC3] = g_param_spec_boxed(
      "vec3", "Vec3s", "Pseudo-property", vec3s_get_type(), G_PARAM_WRITABLE);
  properties[PROP_VEC4] = g_param_spec_boxed(
      "vec4", "Vec4s", "Pseudo-property", vec4s_get_type(), G_PARAM_WRITABLE);

  g_object_class_install_properties(G_OBJECT_CLASS(klass), N_PROPERTIES,
                                    properties);
}

static void rsg_property_printer_node_init(RsgPropertyPrinterNode* cnode) {}

RsgNode* rsgPropertyPrinterNodeCreate(void) {
  return g_object_new(rsg_property_printer_node_get_type(), NULL);
}
