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

static const char* getType(void) {
  assert(0 && "Node getType function not set");
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  assert(0 && "Node process function not set");
}

void rsgNodeSetDefaults(RsgNode* node) {
  // ops (some must be set in containing nodes)
  node->getTypeFunc = getType;
  node->destroyFunc = NULL;
  node->getPropertyFunc = NULL;
  node->setPropertyFunc = NULL;
  node->processFunc = process;

  // other data
  STAILQ_INIT(&node->propertyConnections);
}

const char* rsgNodeGetType(RsgNode* node) {
  return node->getTypeFunc();
}

RsgValue rsgNodeGetProperty(RsgNode* node, const char* name) {
  return node->getPropertyFunc(node, name);
}

void rsgNodeSetProperty(RsgNode* node, const char* name, RsgValue val) {
  node->setPropertyFunc(node, name, val);
}

void rsgNodeConnectProperty(RsgNode* node,
                            const char* name,
                            RsgNode* targetNode,
                            const char* targetName) {
  RsgPropertyConnection* conn;
  STAILQ_FOREACH(conn, &node->propertyConnections, entries) {
    if (conn->targetNode == targetNode)
      if (strcmp(conn->name, name) == 0)
        if (strcmp(conn->targetName, targetName) == 0)
          return;  // NOTE: no duplicate connections
  }

  conn = rsgMalloc(sizeof(*conn));
  conn->name = name;
  conn->targetNode = targetNode;
  conn->targetName = targetName;
  STAILQ_INSERT_TAIL(&node->propertyConnections, conn, entries);
}
