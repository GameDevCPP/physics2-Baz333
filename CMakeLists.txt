cmake_minimum_required(VERSION 3.21)
project(CMakeSFMLProject LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


# B2D - Box2D physics library
add_subdirectory("lib/b2d/Box2D")
include_directories("lib/b2d/Box2D/")
set(B2D_INCS "lib/b2d/Box2D/")
link_directories("${CMAKE_BINARY_DIR}/lib/sfml/lib")

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)
add_executable(5_PHYSICS ${SOURCES})
target_include_directories(5_PHYSICS SYSTEM PRIVATE ${SFML_INCS} ${B2D_INCS})
target_link_libraries(5_PHYSICS Box2D sfml-graphics)
set(EXECUTABLES ${EXECUTABLES} PRACTICAL_5_PHYSICS)

add_executable(CMakeSFMLProject src/main.cpp)
target_link_libraries(CMakeSFMLProject PRIVATE sfml-graphics Box2D)
target_compile_features(CMakeSFMLProject PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET CMakeSFMLProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS CMakeSFMLProject)

#add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND}
#        -E copy directory
#        "${PROJECT_SOURCE_DIR}/res"
#        "${CMAKE_BINARY_DIR}/res"
#)
#
#add_dependencies(CMakeSFMLProject copy_resources)
#
#install(TARGETS CMakeSFMLProject)