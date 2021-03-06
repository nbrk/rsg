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

void rsgInit(int width, int height, int flags) {
  assert(rsgGetGlobalContext() == NULL);
  glfwInit();

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwSwapInterval(1);  // TODO: what about the events?
  GLFWwindow* window = NULL;
  if ((flags & RSG_INIT_FLAG_FULLSCREEN) != 0) {
    window =
        glfwCreateWindow(mode->width, mode->height, "RSG/GLFW", monitor, NULL);
  } else {
    window = glfwCreateWindow(width, height, "RSG/GLFW", NULL, NULL);
  }
  glfwMakeContextCurrent(window);
  int realWidth, realHeight;
  glfwGetWindowSize(window, &realWidth, &realHeight);
  //  glfwSetCursorPos(window, (double)realWidth / 2.0, (double)realHeight
  //  / 2.0);

  if ((flags & RSG_INIT_FLAG_HIDECURSOR) != 0)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glewExperimental = GL_TRUE;
  glewInit();

  printf("RSG: screen %dx%d, GLFW %s, GLEW %s\nRSG: OpenGL context %s\n",
         realWidth, realHeight, glfwGetVersionString(),
         glewGetString(GLEW_VERSION), glGetString(GL_VERSION));

  /*
   * Create and configure the global context
   */
  RsgGlobalContext* gctx = rsgMalloc(sizeof(*gctx));
  gctx->window = window;
  gctx->totalTraversals = 0L;

  rsgSetGlobalContext(gctx);
}
