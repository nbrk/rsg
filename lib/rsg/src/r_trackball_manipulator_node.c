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

struct RsgTrackballManipulatorNode {
  RsgNode node;
  vec2s currentPosition;
  vec2s deltaPosition;
  bool hasCurrentPosition;
};

static const char* getType(void) {
  return "RsgTrackballManipulatorNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgTrackballManipulatorNode* cnode = (RsgTrackballManipulatorNode*)node;

  double x, y;
  glfwGetCursorPos(gctx->window, &x, &y);
  vec2s currentPosition = (vec2s){(float)x, (float)y};
  vec2s deltaPosition = (vec2s){0.0f, 0.0f};
  if (cnode->hasCurrentPosition == true) {
    // if we have previous sample, calculate the delta
    deltaPosition = glms_vec2_sub(currentPosition, cnode->currentPosition);
  }

  // use property machinery to update our values, possibly triggering updates in
  // other nodes' properties
  rsgNodeSetProperty(node, "position", rsgValueVec2(currentPosition));
  rsgNodeSetProperty(node, "delta_position", rsgValueVec2(deltaPosition));
}

static RsgValue getProperty(RsgNode* node, const char* name) {
  RsgTrackballManipulatorNode* cnode = (RsgTrackballManipulatorNode*)node;
  if (strcmp(name, "position") == 0) {
    return rsgValueVec2(cnode->currentPosition);
  }
  if (strcmp(name, "delta_position") == 0) {
    return rsgValueVec2(cnode->deltaPosition);
  }
  assert("Unknown property" && 0);
}

static void setProperty(RsgNode* node, const char* name, RsgValue val) {
  RsgTrackballManipulatorNode* cnode = (RsgTrackballManipulatorNode*)node;
  if (strcmp(name, "position") == 0) {
    cnode->currentPosition = val.asVec2;
    cnode->hasCurrentPosition = true;
    return;
  }
  if (strcmp(name, "delta_position") == 0) {
    cnode->deltaPosition = val.asVec2;
    return;
  }
  assert("Unknown property" && 0);
}

RsgTrackballManipulatorNode* rsgTrackballManipulatorNodeCreate(void) {
  RsgTrackballManipulatorNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;
  node->node.getPropertyFunc = getProperty;
  node->node.setPropertyFunc = setProperty;

  // other data
  node->currentPosition = (vec2s){0.0f, 0.0f};
  node->deltaPosition = (vec2s){0.0f, 0.0f};
  node->hasCurrentPosition = false;

  return node;
}