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

#include <string.h>

/*
 * Properties:
 * connect-only: "key_action": vec2 (key, action)
 */

// NOTE: glfw3 callbacks do no provide cookie args, so use global node pointer
static RsgKeyboardManipulatorNode* callbackNode = NULL;

struct RsgKeyboardManipulatorNode {
  RsgNode node;
  int lastKey;
  int lastKeyAction;
};

static const char* getType(void) {
  return "RsgKeyboardManipulatorNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgKeyboardManipulatorNode* cnode = (RsgKeyboardManipulatorNode*)node;
  // connect-only property
  rsgNodeSetProperty((RsgNode*)callbackNode, "key_action",
                     rsgValueVec2((vec2s){(float)cnode->lastKey,
                                          (float)cnode->lastKeyAction}));
}

static void setProperty(RsgNode* node, const char* name, RsgValue val) {
  RsgKeyboardManipulatorNode* cnode = (RsgKeyboardManipulatorNode*)node;
  if (strcmp(name, "key_action") == 0) {
    // virtual set-only prop
    return;
  }
  assert("Unknown property" && 0);
}

static void keyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
  callbackNode->lastKey = key;
  callbackNode->lastKeyAction = action;
}

RsgKeyboardManipulatorNode* rsgKeyboardManipulatorNodeCreate(void) {
  RsgKeyboardManipulatorNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;
  node->node.setPropertyFunc = setProperty;

  // other data
  node->lastKey = GLFW_KEY_UNKNOWN;
  node->lastKeyAction = 0;

  callbackNode = node;
  glfwSetKeyCallback(rsgGlobalContextGet()->window, keyCallback);

  return node;
}
