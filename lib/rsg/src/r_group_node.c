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

typedef struct RsgGroupNodeChild RsgGroupNodeChild;
struct RsgGroupNodeChild {
  RsgNode* node;
  STAILQ_ENTRY(RsgGroupNodeChild) entries;
};

struct RsgGroupNode {
  RsgNode node;
  STAILQ_HEAD(, RsgGroupNodeChild) children;
  size_t childrenCount;
};

static const char* getType(void) {
  return "RsgGroupNode";
}

static void process(RsgNode* node,
                    RsgLocalContext* lctx,
                    RsgGlobalContext* gctx) {
  RsgGroupNode* cnode = (RsgGroupNode*)node;

  // no children means empty group and no processing
  if (cnode->childrenCount == 0)
    return;

  // make a copy of the local context to be restored upon finish of children
  // processing
  RsgLocalContext* lctx_backup = rsgMalloc(sizeof(*lctx_backup));
  *lctx_backup = *lctx;

  // process children from left to right
  RsgGroupNodeChild* child;
  STAILQ_FOREACH(child, &cnode->children, entries) {
    child->node->processFunc(child->node, lctx, gctx);
  }

  // restore local/subtree context after all children are done, so the local ctx
  // changes do not propagate back up over into the tree (e.g. to siblings of
  // the group node)
  // TODO: deep clone the lctx (with all its uniform slists, etc..)
  //  *lctx = *lctx_backup;
  rsgFree(lctx_backup);
}

RsgGroupNode* rsgGroupNodeCreate(void) {
  RsgGroupNode* node = rsgMalloc(sizeof(*node));
  rsgNodeSetDefaults(&node->node);

  // base
  node->node.getTypeFunc = getType;
  node->node.processFunc = process;

  // other data
  STAILQ_INIT(&node->children);
  node->childrenCount = 0;

  return node;
}

void rsgGroupNodeAddChild(RsgGroupNode* node, RsgNode* childNode) {
  assert(childNode != NULL && "adding null child");

  RsgGroupNodeChild* child = rsgMalloc(sizeof(*child));
  child->node = childNode;
  STAILQ_INSERT_TAIL(&node->children, child, entries);

  node->childrenCount++;
}
