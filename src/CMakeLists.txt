# add a library to the project
add_library (Sphere sphere.cpp)
add_library (Vec3 vec3.cpp)

target_include_directories (
	Sphere PUBLIC
	"../include"
)

target_include_directories (
	Vec3 PUBLIC
	"../include"
)
