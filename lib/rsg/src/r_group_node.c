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
#include <stdio.h>

#include "rsg_internal.h"

G_DECLARE_FINAL_TYPE(RsgGroupNode, rsg_group_node, RSG, GROUP_NODE,
                     RsgAbstractNode)

struct _RsgGroupNode {
  RsgAbstractNode abstract;
  GList* children;
  RsgLocalContext* lctxBackup;
};

G_DEFINE_TYPE(RsgGroupNode, rsg_group_node, RSG_TYPE_ABSTRACT_NODE)

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgGroupNode* cnode = RSG_GROUP_NODE(node);
  if (cnode->children == NULL) {
    // no children
    return;
  }
  /*
   * Save the local context copy, process all children from left to right, and
   * restore the local context.
   */
  *cnode->lctxBackup = *ctx->local;

  GList* elem = NULL;
  for (elem = cnode->children; elem != NULL; elem = elem->next) {
    RsgAbstractNode* childNode = RSG_ABSTRACT_NODE(elem->data);
    RSG_ABSTRACT_NODE_GET_CLASS(childNode)->processFunc(childNode, ctx);
  }

  *ctx->local = *cnode->lctxBackup;
}

static void finalize(GObject* node) {
  RsgGroupNode* cnode = RSG_GROUP_NODE(node);
  g_list_free(cnode->children);  // NOTE: free only list elems, not the child
                                 // nodes themselves
  rsgFree(cnode->lctxBackup);
}

static void rsg_group_node_class_init(RsgGroupNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;
  G_OBJECT_CLASS(klass)->finalize = finalize;
}

static void rsg_group_node_init(RsgGroupNode* cnode) {
  cnode->children = NULL;
  cnode->lctxBackup = rsgMalloc(sizeof(*cnode->lctxBackup));
}

RsgNode* rsgGroupNodeCreate(void) {
  return g_object_new(rsg_group_node_get_type(), NULL);
}

void rsgGroupNodeAddChild(RsgNode* node, RsgNode* childNode) {
  assert(RSG_IS_GROUP_NODE(node) != false);
  assert(RSG_IS_ABSTRACT_NODE(childNode) != false);

  RsgGroupNode* cnode = RSG_GROUP_NODE(node);

  cnode->children = g_list_append(cnode->children, childNode);
}
