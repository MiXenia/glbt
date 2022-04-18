cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_STANDARD 90) #89

project ("glbt_c")

file(GLOB_RECURSE SRCS ../../src/*.c ./src/*.c)
add_executable (${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ../../include/ ../../include/internal ./include)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE m dl)

ADD_CUSTOM_TARGET(
        copy_resources
         COMMAND ${CMAKE_COMMAND} -E copy_directory
             ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
         COMMENT "Copying resources" VERBATIM 
        )

ADD_DEPENDENCIES(${PROJECT_NAME} copy_resources)
