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
    "gl_Position = u_projection * u_view * vec4(a_position, 1.0);\n"
    "//gl_Position = vec4(a_position, 1.0);\n"
    "}\n"
    "\n";
static const char* fragment_0 =
    "#version 330\n"
    "uniform vec4 u_diffuse_color;\n"
    "void main()\n"
    "{\n"
    "//gl_FragColor = u_diffuse_color;\n"
    "gl_FragColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
    "}\n"
    "\n";

static void func(void* cookie) {
  glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void printIntCookie(void* cookie) {
  if (cookie != NULL)
    printf("Int cookie is %d\n", (int)cookie);
}

static void mult(void* b, RsgValue* from, RsgValue* to, float* f) {
  to->asFloat = from->asFloat * *f;
}

int main(int argc, char** argv) {
  //  rsgInit(1024, 768, RSG_INIT_FLAG_FULLSCREEN | RSG_INIT_FLAG_HIDECURSOR);
  rsgInit(1024, 768, 0);

  RsgNode* group1 = rsgGroupNodeCreate();
  RsgNode* camera1 = rsgCameraNodeCreatePerspectiveDefault();
  //  RsgNode* screen1 = rsgScreenNodeCreate();
  RsgNode* mouse1 = rsgMouseManipulatorNodeCreate();
  RsgNode* printer1 = rsgPropertyPrinterNodeCreate();
  RsgNode* mesh1 = rsgMeshNodeCreateTriangle();
  RsgNode* shader1 = rsgShaderNodeCreateFromMemory(vertex_0, fragment_0);

  //  rsgNodeBindProperty(mouse1, "x", printer1, "int1");
  //  rsgNodeBindProperty(mouse1, "y", printer1, "int2");

  float f = 0.01f;
  rsgNodeBindPropertyWithClosure(mouse1, "xChange", camera1, "yawChange",
                                 rsgClosureCreate(mult, &f, sizeof(float)));
  //  rsgNodeBindProperty(mouse1, "yChange", camera1, "pitchChange");

  //  rsgNodeSetProperty(callback1, "callback", rsgValuePointer(func));
  //  rsgNodeSetProperty(screen1, "clearColor",
  //                     rsgValueVec4((vec4s){{1.0f, 1.f, 0.0f, 1.0f}}));
  //  rsgNodeBindProperty(mouse1, "x", screen1, "clearRed");
  //  rsgNodeBindProperty(mouse1, "y", screen1, "clearGreen");
  //  rsgNodeBindProperty(mouse1, "y", screen1, "clearBlue");

  //  rsgGroupNodeAddChild(group1, screen1);
  rsgGroupNodeAddChild(group1, mouse1);
  rsgGroupNodeAddChild(group1, camera1);
  rsgGroupNodeAddChild(group1, printer1);
  rsgGroupNodeAddChild(group1, shader1);
  rsgGroupNodeAddChild(group1, mesh1);
  //  rsgGroupNodeAddChild(group1, callback1);
  //  rsgGroupNodeAddChild(group1, callback2);

  //  rsgMainLoop(group1, 60);
  rsgMainLoop(group1, 0);
  return 0;
}
