# ptLeftarCore

- Sponza loading time - 1:02
- Sponza directional light (float optimized), 160x160 - 3:01
- Sponza directional light (AVX2 unoptimized), 160x160 - 0:18
- Sponza directional light (float octree-optimized), 1920x1080 - 19:40
- Sponza directional light (AVX2 octree-omptimized), 1920x1080 - 4:15

# Build
- Generate project
```
cmake -S . -B out
```
- Build project
```
cmake --build out --config Release
cmake --build out --config Debug
```