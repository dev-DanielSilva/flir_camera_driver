cmake_minimum_required(VERSION 3.8)
project(vehicle_can)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Encontrar dependências
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# 2. Gerar as mensagens customizadas
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Canmsg.msg"
  DEPENDENCIES std_msgs
)

# =================================================================
# A) COMPILAÇÃO DAS BIBLIOTECAS DE DRIVER
# =================================================================

add_library(${PROJECT_NAME}_drivers STATIC
  src/utils.cpp
  src/decode_ABS_h_FrP00.cpp
  src/decode_ABS_h_FrP02.cpp
  src/decode_SASM_h_FrP00.cpp
)

rosidl_get_typesupport_target(cpp_typesupport_target_lib "${PROJECT_NAME}" "rosidl_typesupport_cpp")
target_link_libraries(${PROJECT_NAME}_drivers "${cpp_typesupport_target_lib}")

target_include_directories(${PROJECT_NAME}_drivers PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# =================================================================
# B) BUSCA AUTOMÁTICA DE BIBLIOTECAS EXTERNAS (.a)
# =================================================================

# Captura todos os arquivos .a na pasta lib
file(GLOB EXTERNAL_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.a")
message(STATUS "Bibliotecas externas encontradas: ${EXTERNAL_LIBS}")

# =================================================================
# C) COMPILAÇÃO DO NÓ EXECUTÁVEL
# =================================================================

add_executable(can_node_exec src/can_node.cpp)

set_target_properties(can_node_exec PROPERTIES POSITION_INDEPENDENT_CODE OFF)
set_target_properties(can_node_exec PROPERTIES LINK_FLAGS "-no-pie")

# Linkagem robusta: 
# Usamos --start-group para que o linker procure símbolos em todos os .a da pasta lib
target_link_libraries(can_node_exec 
  "${cpp_typesupport_target}"
  ${PROJECT_NAME}_drivers
  -Wl,--start-group
  ${EXTERNAL_LIBS}
  -Wl,--end-group
  pthread
  dl
  m
)

target_include_directories(can_node_exec PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(can_node_exec
  rclcpp
  std_msgs
)

# =================================================================
# D) INSTALAÇÃO E EXPORTAÇÃO
# =================================================================

install(TARGETS
  can_node_exec
  ${PROJECT_NAME}_drivers
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include/)


ament_export_dependencies(rclcpp std_msgs rosidl_default_runtime)
ament_export_libraries(${PROJECT_NAME}_drivers)
ament_export_include_directories(include)

ament_package()
