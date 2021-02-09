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

G_DEFINE_TYPE(RsgAbstractNode, rsg_abstract_node, G_TYPE_OBJECT)

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  const char* className = g_type_name_from_instance((GTypeInstance*)node);
  printf("ERROR: unimplemented process() is called for node type '%s'\n",
         className);
  assert(0);
}

static void rsg_abstract_node_class_init(RsgAbstractNodeClass* klass) {
  klass->processFunc = process;
}

static void rsg_abstract_node_init(RsgAbstractNode* node) {}

RsgValue rsgNodeGetProperty(RsgNode* node, const char* name) {
  assert(RSG_IS_ABSTRACT_NODE(node) != false);
  /*
   * Map from GValue
   */
  GValue value = G_VALUE_INIT;
  g_object_get_property(G_OBJECT(node), name, &value);
  return rsgGValueToValue(value);
}

void rsgNodeSetProperty(RsgNode* node, const char* name, RsgValue value) {
  assert(RSG_IS_ABSTRACT_NODE(node) != false);

  const char* className = g_type_name_from_instance((GTypeInstance*)node);
  char* valStr = rsgValueToString(value);
  printf("PROPERTY '%s' in %s: SET %s\n", name, className, valStr);

  /*
   * Map our values to GValue
   */
  GValue gvalue = rsgValueToGValue(value);
  //  GValue gvalue = G_VALUE_INIT;
  //  gvalue = rsgValueToGValue(value);
  g_object_set_property(G_OBJECT(node), name, &gvalue);
}

void rsgNodeBindProperty(RsgNode* node,
                         const char* name,
                         RsgNode* toNode,
                         const char* toName) {
  assert(RSG_IS_ABSTRACT_NODE(node));
  assert(RSG_IS_ABSTRACT_NODE(toNode));
  (void)g_object_bind_property(G_OBJECT(node), name, G_OBJECT(toNode), toName,
                               0);
}

void rsgNodeBindPropertyWithClosure(RsgNode* node,
                                    const char* name,
                                    RsgNode* toNode,
                                    const char* toName,
                                    RsgClosure* toTransform) {
  assert(RSG_IS_ABSTRACT_NODE(node));
  assert(RSG_IS_ABSTRACT_NODE(toNode));
  assert(toTransform != NULL);

  (void)g_object_bind_property_with_closures(G_OBJECT(node), name,
                                             G_OBJECT(toNode), toName, 0,
                                             toTransform->gclosure, NULL);
}
