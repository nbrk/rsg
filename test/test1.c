/*
 * Copyright 2020 Nikolay Burkov <nbrk@linklevel.net>
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
#include <GL/glew.h>
#include <rsg/rsg.h>
#include <stdio.h>
#include <unistd.h>

static const char* vertex_0 =
    "#version 330\n"
    "layout(location = 0) in vec3 a_position;\n"
    "uniform mat4 u_model;\n"
    "uniform mat4 u_view;\n"
    "uniform mat4 u_projection;\n"
    "void main()\n"
    "{\n"
    "//gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);\n"
    "gl_Position = vec4(a_position, 1.0);\n"
    "}\n"
    "\n";
static const char* fragment_0 =
    "#version 330\n"
    "uniform vec4 u_diffuse_color;\n"
    "void main()\n"
    "{\n"
    "gl_FragColor = u_diffuse_color;\n"
    "//gl_FragColor = vec4(0.3, 0.0, 0.0, 1.0);\n"
    "}\n"
    "\n";

static void func(void* cookie) {
  glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char** argv) {
  rsgInit(1024, 768, false);

  RsgCallbackNode* cn1 = rsgCallbackNodeCreate(func, NULL);
  RsgGroupNode* gn1 = rsgGroupNodeCreate();
  RsgMeshNode* mn1 = rsgMeshNodeCreateTriangle();
  RsgShaderNode* sn1 = rsgShaderNodeCreate(vertex_0, fragment_0);

  const char* names[] = {"u_int", "u_float", "u_diffuse_color"};
  RsgValue values[] = {
      rsgValueInt(100), rsgValueFloat(12.3f),
      rsgValueVec4((vec4s){.x = 0.f, .y = 1.0f, .z = 0.0f, .w = 1.f})};
  RsgUniformSetterNode* un1 = rsgUniformSetterNodeCreate(names, values, 3);

  rsgGroupNodeAddChild(gn1, (RsgNode*)cn1);
  rsgGroupNodeAddChild(gn1, (RsgNode*)un1);
  rsgGroupNodeAddChild(gn1, (RsgNode*)sn1);
  rsgGroupNodeAddChild(gn1, (RsgNode*)mn1);

  rsgMainLoop((RsgNode*)gn1);

  return 0;
}
