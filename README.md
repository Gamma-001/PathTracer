# Basic PathTracer
A very basic path tracer written in c++

# Features
- Support for diffuse and specular shading
- Support for hdr
- Result is rendered as a bimap

# Build Instructions
```
cmake -S . -B ./build -G "MinGW Makefiles"

cmake --build ./build

./build/RayTracing
```

# Dependencies
[stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)

# Screenshots
![](https://github.com/Gamma-001/PathTracer/blob/main/results/png_result.png)

# Todo
- Scene lights
- Mesh loading
- Refractive materials
