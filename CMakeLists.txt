cmake_minimum_required(VERSION 3.8)

project(SoleilRenderer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)

add_library(LibSoleil src/scene_io.cpp include/scene.hpp src/scene.cpp
    include/renderer.hpp src/renderer.cpp include/camera.hpp include/lights/point.hpp
        include/primitives/sphere.hpp include/materials/base.hpp include/primitives/mesh.hpp 
        include/vertex.hpp src/sphere.cpp include/aabb.hpp include/kd_tree.hpp 
		src/kd_tree.cpp src/mesh.cpp include/dir_light.hpp include/tinyobjloader.hpp 
		include/scene_loaders.hpp  include/payload.hpp include/utils.hpp
        include/area_light.hpp	include/tinyxml/tinyxml2.h include/tinyxml/tinyxml2.cpp 
		include/photon_map.hpp src/photon_map.cpp include/photon.hpp src/photon_integrator.cpp 
		include/photon_integrator.hpp include/KDTree.hpp src/KDTree.cpp include/integrators/progressive_integrator.hpp
	    include/integrator.hpp src/integrators/progressive_integrator.cpp src/scene_loaders/tungsten_loader.cpp
		src/integrators/mc_integrator.cpp src/scene_loaders/obj_loader.cpp src/scene_loaders/veach_loader.cpp
		include/integrators/mc_integrator.hpp include/textures/sampler2D.hpp
		include/lights/directional.hpp include/lights/light.hpp src/lights/point.cpp src/lights/directional.cpp
		include/lights/area.hpp src/lights/area.cpp src/primitives/quad.cpp
		#textures
		src/textures/checkerboard.cpp src/textures/texture.cpp
		#bsdfs
		src/bsdfs/lambert.cpp src/bsdfs/orennayar.cpp
		# materials
		src/materials/mirror.cpp src/materials/plastic.cpp src/materials/base.cpp src/materials/matte.cpp
		#tonemappers
		src/tonemappers/gamma.cpp src/tonemappers/reinhard.cpp include/textures/single_color.hpp include/bsdfs/cooktorrance.hpp src/bsdfs/cooktorrance.cpp include/bsdfs/microfacet_distribution/beckmann.hpp src/bsdfs/microfacet_distributions/beckmann.cpp include/bsdfs/fresnel/dielectric.hpp src/bsdfs/fresnel/dielectric.cpp src/core/utils.cpp)

target_link_libraries(LibSoleil PUBLIC ${OpenCV_LIBS})
target_include_directories(LibSoleil PUBLIC include)
target_include_directories(LibSoleil PUBLIC /usr/local/include )

target_compile_features(LibSoleil PUBLIC cxx_std_17)

#target_compile_options(LibSoleil PUBLIC -pg)
#target_link_libraries(LibSoleil PUBLIC -pg)

#target_compile_options(LibSoleil PUBLIC -fsanitize=undefined)
#target_link_options(LibSoleil PUBLIC -fsanitize=undefined)

add_executable(SoleilRenderer main.cpp)
add_executable(SoleilDebugger debug_main.cpp)
target_link_libraries(SoleilRenderer PUBLIC LibSoleil)
target_link_libraries(SoleilDebugger PUBLIC LibSoleil)

enable_testing()
add_subdirectory(tests)
