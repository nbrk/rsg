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

G_DECLARE_FINAL_TYPE(RsgCallbackNode, rsg_callback_node, RSG, CALLBACK_NODE,
                     RsgAbstractNode)

struct _RsgCallbackNode {
  RsgAbstractNode abstract;
  void (*callbackFunc)(void* cookie);
  void* cookie;
};

G_DEFINE_TYPE(RsgCallbackNode, rsg_callback_node, RSG_TYPE_ABSTRACT_NODE)

enum { PROP_FUNC = 1, PROP_COOKIE, N_PROPERTIES };

static GParamSpec* properties[N_PROPERTIES] = {NULL};

static void process(RsgAbstractNode* node, RsgContext* ctx) {
  RsgCallbackNode* cnode = RSG_CALLBACK_NODE(node);
  if (cnode->callbackFunc != NULL) {
    cnode->callbackFunc(cnode->cookie);
  }
}

static void set_property(GObject* object, guint property_id,
                         const GValue* value, GParamSpec* pspec) {
  RsgCallbackNode* cnode = RSG_CALLBACK_NODE(object);

  switch (property_id) {
    case PROP_FUNC:
      cnode->callbackFunc = g_value_get_pointer(value);
      printf("Callback node @%p, function set to %p\n", cnode,
             cnode->callbackFunc);
      break;
    case PROP_COOKIE:
      cnode->cookie = g_value_get_pointer(value);
      printf("Callback node @%p, cookie set to %p\n", cnode, cnode->cookie);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void get_property(GObject* object, guint property_id, GValue* value,
                         GParamSpec* pspec) {
  RsgCallbackNode* cnode = RSG_CALLBACK_NODE(object);

  switch (property_id) {
    case PROP_FUNC:
      g_value_set_pointer(value, cnode->callbackFunc);
      break;
    case PROP_COOKIE:
      g_value_set_pointer(value, cnode->cookie);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
  }
}

static void rsg_callback_node_class_init(RsgCallbackNodeClass* klass) {
  RSG_ABSTRACT_NODE_CLASS(klass)->processFunc = process;

  G_OBJECT_CLASS(klass)->set_property = set_property;
  G_OBJECT_CLASS(klass)->get_property = get_property;

  properties[PROP_FUNC] = g_param_spec_pointer(
      "function", "Callback function",
      "Pointer to user callback function to call in process()",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  properties[PROP_COOKIE] = g_param_spec_pointer(
      "cookie", "Callback argument",
      "Pointer to a user data to be passed in the callback",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  g_object_class_install_properties(G_OBJECT_CLASS(klass), N_PROPERTIES,
                                    properties);
}

static void rsg_callback_node_init(RsgCallbackNode* cnode) {}

RsgNode* rsgCallbackNodeCreate(void (*func)(void* cookie), void* cookie) {
  return g_object_new(rsg_callback_node_get_type(), NULL);
  /*  return g_object_new(rsg_callback_node_get_type(), "callback", func,
                        "argument", cookie, NULL);*/
}
