set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GOBJECT REQUIRED glib-2.0 gobject-2.0)

set(SRCS
  include/rsg/rsg.h

  src/rsg_internal.h
  src/r_malloc.c
  src/r_init.c
  src/r_context.c
  src/r_value.c
  src/r_value_gvalue.c
  src/r_closure.c
  src/r_shader_loader.c
  src/r_main_loop.c
  src/r_abstract_node.c
  src/r_callback_node.c
  src/r_group_node.c
  src/r_mesh_node.c
  src/r_screen_node.c # XXX
  src/r_mouse_manipulator_node.c
  src/r_camera_node.c
  src/r_shader_node.c
  src/r_property_printer_node.c
  )

set(NAME rsg)

add_library(${NAME} SHARED ${SRCS})
target_include_directories(${NAME} PUBLIC include/)
target_link_libraries(${NAME} PRIVATE GLEW GL glfw m cglm )
target_link_directories(${NAME} PRIVATE /usr/local/lib)
target_include_directories(${NAME} PRIVATE  /usr/local/include)

target_include_directories (${NAME} PRIVATE ${GOBJECT_INCLUDE_DIRS})
target_link_directories (${NAME} PRIVATE ${GOBJECT_LIBRARY_DIRS})
add_definitions (${GOBJECT_CFLAGS_OTHER})
target_link_libraries (${NAME} PRIVATE ${GOBJECT_LIBRARIES})
