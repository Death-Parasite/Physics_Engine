cmake_minimum_required(VERSION 3.20)
project(NextGenPhysicsEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Math
    ${CMAKE_CURRENT_SOURCE_DIR}/Cloth
)

set(ENGINE_SOURCES
    "Core/Types/PhysicsTypes.h"
    "Core/Utils/OBJExporter.h"
    "Core/Utils/OBJExporter.cpp"
    
    "Math/PhysicsMath.h"
    "Math/VectorMath.h"
    "Math/MatrixMath.h"
    "Math/QuaternionMath.h"
    "Math/SIMDMath.h"

    "Cloth/ClothSolver.h"
    "Cloth/ClothSolver.cpp"
    "Cloth/ClothMesh.h"
    "Cloth/ClothMesh.cpp"
)

add_library(NextGenPhysics STATIC ${ENGINE_SOURCES})

add_executable(TestCloth "Test/TestCloth.cpp")
target_link_libraries(TestCloth PRIVATE NextGenPhysics)

find_package(raylib CONFIG QUIET)
if(raylib_FOUND)
    add_executable(TestClothLive "Test/TestClothLive3D.cpp")
    target_link_libraries(TestClothLive PRIVATE NextGenPhysics raylib)
else()
    add_executable(TestClothLive "Test/TestClothLive3D.cpp")
    target_link_libraries(TestClothLive PRIVATE NextGenPhysics -lraylib -lgdi32 -lwinmm)
endif()