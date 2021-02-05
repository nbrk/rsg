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

RsgValue rsgValueInt(int val) {
  return (RsgValue){.type = RSG_VALUE_INT, .asInt = val};
}
RsgValue rsgValueFloat(float val) {
  return (RsgValue){.type = RSG_VALUE_FLOAT, .asFloat = val};
}
RsgValue rsgValueVec3(vec3s val) {
  return (RsgValue){.type = RSG_VALUE_VEC3, .asVec3 = val};
}
RsgValue rsgValueVec4(vec4s val) {
  return (RsgValue){.type = RSG_VALUE_VEC4, .asVec4 = val};
}
RsgValue rsgValueMat4(mat4s val) {
  return (RsgValue){.type = RSG_VALUE_MAT4, .asMat4 = val};
}
