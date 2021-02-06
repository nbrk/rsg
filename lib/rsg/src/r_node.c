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

static void emptyProcess(RsgNode* node,
                         RsgLocalContext* lctx,
                         RsgGlobalContext* gctx) {}

void rsgNodeSetDefaults(RsgNode* node) {
  // ops (some must be set in containing nodes)
  node->getTypeFunc = getType;
  node->destroyFunc = NULL;
  node->getPropertyFunc = NULL;
  node->setPropertyFunc = NULL;
  node->processFunc = emptyProcess;

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
  // set our property, if the set function is provided
  if (node->setPropertyFunc != NULL)
    node->setPropertyFunc(node, name, val);

  // anyways, update connected properties (use value adapters, if set)
  RsgPropertyConnection* conn;
  STAILQ_FOREACH(conn, &node->propertyConnections, entries) {
    if (strcmp(conn->name, name) == 0) {
      // just set the value if no conversions are needed
      if (conn->numAdapterFuncs == 0)
        rsgNodeSetProperty(conn->targetNode, conn->targetName, val);
      else {
        // apply all the adapters
        RsgValue convertedVal = val;
        size_t i;
        for (i = 0; i < conn->numAdapterFuncs; i++)
          convertedVal = conn->adapterFuncs[i](convertedVal);
        // finally, set the converted value instead of val parameter
        rsgNodeSetProperty(conn->targetNode, conn->targetName, convertedVal);
      }
    }
  }
}

void rsgNodeConnectProperty(RsgNode* node,
                            const char* name,
                            RsgNode* targetNode,
                            const char* targetName) {
  rsgNodeConnectPropertyWithAdapter(node, name, targetNode, targetName, NULL,
                                    0);
}

void rsgNodeConnectPropertyWithAdapter(RsgNode* node,
                                       const char* name,
                                       RsgNode* targetNode,
                                       const char* targetName,
                                       RsgValue (**funcs)(RsgValue val),
                                       size_t nfuncs) {
  RsgPropertyConnection* conn;
  // check for existence of exactly the same connection
  STAILQ_FOREACH(conn, &node->propertyConnections, entries) {
    if (conn->targetNode == targetNode)
      if (strcmp(conn->name, name) == 0)
        if (strcmp(conn->targetName, targetName) == 0)
          return;  // NOTE: disallow duplicate connections
  }

  // just add the connection, even if the name prop doesn't exist in the target
  conn = rsgMalloc(sizeof(*conn));
  conn->name = name;
  conn->targetNode = targetNode;
  conn->targetName = targetName;
  conn->adapterFuncs = funcs;
  conn->numAdapterFuncs = nfuncs;
  STAILQ_INSERT_TAIL(&node->propertyConnections, conn, entries);
}
