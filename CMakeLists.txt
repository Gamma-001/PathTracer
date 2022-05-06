# Specify the minimum version of cmake
cmake_minimum_required (VERSION 3.10)

# name of the project
project (RayTracing)

# specifiy a c++ standard
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# subdirectories containing CMakeLists.txt for additiona build commands
add_subdirectory ("./src")

# add the executable
add_executable (RayTracing "src/main.cpp")

# specify target libraries
target_link_libraries (
	RayTracing PUBLIC
	Material
	Util
	Sphere
	Vec3
)

target_include_directories (
	RayTracing PUBLIC 
	"./include"
)