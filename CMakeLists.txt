cmake_minimum_required(VERSION 3.8)

project(PathTracer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)

add_library(PathTracerLib src/scene_io.cpp include/scene.hpp src/scene.cpp 
    include/renderer.hpp src/renderer.cpp include/camera.hpp include/lights/point.hpp
        include/primitives/sphere.hpp include/materials/base.hpp include/primitives/mesh.hpp 
        include/vertex.hpp src/sphere.cpp include/aabb.hpp include/kd_tree.hpp 
		src/kd_tree.cpp src/mesh.cpp include/dir_light.hpp include/tinyobjloader.hpp 
		include/scene_loaders.hpp src/material.cpp include/payload.hpp include/utils.hpp
        include/area_light.hpp	include/tinyxml/tinyxml2.h include/tinyxml/tinyxml2.cpp 
		include/photon_map.hpp src/photon_map.cpp include/photon.hpp src/photon_integrator.cpp 
		include/photon_integrator.hpp include/KDTree.hpp src/KDTree.cpp include/integrators/progressive_integrator.hpp
	    include/integrator.hpp src/integrators/progressive_integrator.cpp src/scene_loaders/tungsten_loader.cpp
		src/integrators/mc_integrator.cpp src/scene_loaders/obj_loader.cpp)
target_link_libraries(PathTracerLib PUBLIC ${OpenCV_LIBS})
target_include_directories(PathTracerLib PUBLIC include)
target_include_directories(PathTracerLib PUBLIC /usr/local/include)

add_executable(PathTracer main.cpp include/integrators/mc_integrator.hpp include/materials/texture.hpp include/lights/directional.hpp include/lights/light.hpp src/lights/point.cpp)
target_link_libraries(PathTracer PUBLIC PathTracerLib)

enable_testing()
add_subdirectory(tests)