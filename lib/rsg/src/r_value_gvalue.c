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
 * Our Value to GValue (boxed GType) coupling
 */
vec2s* vec2s_copy(const vec2s* data) {
  vec2s* copy = g_new(vec2s, 1);
  copy->raw[0] = data->raw[0];
  copy->raw[1] = data->raw[1];
  return copy;
}

vec3s* vec3s_copy(const vec3s* data) {
  vec3s* copy = g_new(vec3s, 1);
  copy->raw[0] = data->raw[0];
  copy->raw[1] = data->raw[1];
  copy->raw[2] = data->raw[2];
  return copy;
}

vec4s* vec4s_copy(const vec4s* data) {
  vec4s* copy = g_new(vec4s, 1);
  copy->raw[0] = data->raw[0];
  copy->raw[1] = data->raw[1];
  copy->raw[2] = data->raw[2];
  copy->raw[3] = data->raw[3];
  return copy;
}

mat4s* mat4s_copy(const mat4s* data) {
  mat4s* copy = g_new(mat4s, 1);
  *copy = *data;
  return copy;
}

void vec2s_free(vec2s* data) { g_free(data); }
void vec3s_free(vec3s* data) { g_free(data); }
void vec4s_free(vec4s* data) { g_free(data); }
void mat4s_free(mat4s* data) { g_free(data); }

G_DEFINE_BOXED_TYPE(vec2s, vec2s, vec2s_copy, vec2s_free)
G_DEFINE_BOXED_TYPE(vec3s, vec3s, vec3s_copy, vec3s_free)
G_DEFINE_BOXED_TYPE(vec4s, vec4s, vec4s_copy, vec4s_free)
G_DEFINE_BOXED_TYPE(mat4s, mat4s, mat4s_copy, mat4s_free)

GValue rsgValueToGValue(RsgValue value) {
  GValue gvalue = G_VALUE_INIT;
  if (value.type == RSG_VALUE_POINTER) {
    g_value_init(&gvalue, G_TYPE_POINTER);
    g_value_set_pointer(&gvalue, value.asPointer);
    return gvalue;
  }
  if (value.type == RSG_VALUE_INT) {
    g_value_init(&gvalue, G_TYPE_INT);
    g_value_set_int(&gvalue, value.asInt);
    return gvalue;
  }
  if (value.type == RSG_VALUE_FLOAT) {
    g_value_init(&gvalue, G_TYPE_FLOAT);
    g_value_set_int(&gvalue, value.asFloat);
    return gvalue;
  }
  if (value.type == RSG_VALUE_VEC2) {
    g_value_init(&gvalue, vec2s_get_type());
    g_value_set_boxed(&gvalue, &value.asVec2);
    return gvalue;
  }
  if (value.type == RSG_VALUE_VEC3) {
    g_value_init(&gvalue, vec3s_get_type());
    g_value_set_boxed(&gvalue, &value.asVec3);
    return gvalue;
  }
  if (value.type == RSG_VALUE_VEC4) {
    g_value_init(&gvalue, vec4s_get_type());
    g_value_set_boxed(&gvalue, &value.asVec4);
    //    g_value_take_boxed(&gvalue, &value.asVec4);
    return gvalue;
  }
  if (value.type == RSG_VALUE_MAT4) {
    g_value_init(&gvalue, mat4s_get_type());
    g_value_set_boxed(&gvalue, &value.asMat4);
    return gvalue;
  }
  assert(0 && "Unsupported RsgValue");
}

RsgValue rsgGValueToValue(GValue value) {
  if (G_VALUE_HOLDS_POINTER(&value) == true) {
    return rsgValuePointer(g_value_get_pointer(&value));
  }
  if (G_VALUE_HOLDS_INT(&value) == true) {
    return rsgValueInt(g_value_get_int(&value));
  }
  if (G_VALUE_HOLDS_FLOAT(&value) == true) {
    return rsgValueFloat(g_value_get_float(&value));
  }
  if (G_VALUE_HOLDS(&value, vec2s_get_type()) == true) {
    return rsgValueVec2(*(vec2s*)g_value_get_boxed(&value));
  }
  if (G_VALUE_HOLDS(&value, vec3s_get_type()) == true) {
    return rsgValueVec3(*(vec3s*)g_value_get_boxed(&value));
  }
  if (G_VALUE_HOLDS(&value, vec4s_get_type()) == true) {
    return rsgValueVec4(*(vec4s*)g_value_get_boxed(&value));
  }
  if (G_VALUE_HOLDS(&value, mat4s_get_type()) == true) {
    return rsgValueMat4(*(mat4s*)g_value_get_boxed(&value));
  }
  assert(0 && "Unsupported GValue");
}
