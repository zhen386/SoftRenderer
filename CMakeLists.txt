cmake_minimum_required(VERSION 3.10)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3/Eigen)

add_executable(SoftRenderer src/RTRenderer/main.cpp src/frame/Object.hpp src/frame/Vector.cpp src/frame/Vector.hpp src/frame/Sphere.hpp src/frame/global.hpp src/frame/Triangle.hpp src/frame/Scene.cpp
        src/frame/Scene.hpp src/Rasterizer/RasTriangle.cpp src/Rasterizer/RasTriangle.hpp src/RTRenderer/Light.hpp src/RTRenderer/AreaLight.hpp src/RTRenderer/BVH.cpp src/RTRenderer/BVH.hpp src/RTRenderer/Bounds3.hpp src/RTRenderer/Ray.hpp src/frame/Material.hpp src/RTRenderer/Intersection.hpp
        src/RTRenderer/Renderer.cpp src/RTRenderer/Renderer.hpp src/Rasterizer/MVP.hpp src/Rasterizer/DrawTriangle.hpp src/Rasterizer/rasterizer.hpp src/Rasterizer/rasterizer.cpp
)
target_link_libraries(SoftRenderer Eigen3::Eigen ${OpenCV_LIBRARIES})