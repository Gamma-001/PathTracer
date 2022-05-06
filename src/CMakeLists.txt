# add a library to the project
add_library (Material "material.cpp")
add_library (Util "util.cpp")
add_library (Sphere "sphere.cpp")
add_library (Vec3 "vec3.cpp")

target_include_directories (
	Material PUBLIC
	"../include"
)

target_include_directories (
	Util PUBLIC
	"../include"
)

target_include_directories (
	Sphere PUBLIC
	"../include"
)

target_include_directories (
	Vec3 PUBLIC
	"../include"
)