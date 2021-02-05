set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)


set(SRCS
  include/rsg/rsg.h

  src/rsg_internal.h
  src/r_malloc.c
  src/r_value.c
  src/r_init.c
  src/r_context.c
  src/r_main_loop.c
  src/r_node.c
  src/r_callback_node.c
  src/r_group_node.c
  src/r_mesh_node.c
  src/r_uniform_setter_node.c
  src/r_shader_node.c
  src/r_shader_program.c

#  src/r_main.c
#  src/r_shader_program.c
#  src/r_node.c
#  src/r_data_node.c
#  src/r_group_node.c
#  src/r_code_node.c
#  src/r_mesh_node.c
#  src/r_shader_program_node.c
#  src/r_shader_uniform_node.c
  )

set(NAME rsg)

add_library(${NAME} SHARED ${SRCS})
target_include_directories(${NAME} PUBLIC include/)
target_link_libraries(${NAME} PRIVATE GLEW GL glfw m cglm )
target_link_directories(${NAME} PRIVATE /usr/local/lib)
target_include_directories(${NAME} PRIVATE  /usr/local/include)

