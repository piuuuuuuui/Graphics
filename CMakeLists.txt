CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(UE4 CXX)

IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE Release)
ENDIF()

FIND_PACKAGE(OpenMP REQUIRED)

ADD_SUBDIRECTORY(deps/eigen)

SET(UE4_SOURCES
        src/image.cpp
        src/main.cpp
        src/mesh.cpp
        src/scene_parser.cpp
)

SET(UE4_INCLUDES
        include/camera.hpp
        include/disk.hpp
        include/fractal.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/portal.hpp
        include/ray.hpp
        include/render.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/utils.hpp
)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${UE4_SOURCES} ${UE4_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE eigen OpenMP::OpenMP_CXX)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
