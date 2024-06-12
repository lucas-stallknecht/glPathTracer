# OpenGL PathTracer

![img.png](img.png)

Achievements :
- [x] Compute shaders
- [x] Camera controls
- [x] Accumulation
- [x] Glossy and specular reflections 
- [x] OBJ imports
- [x] Acceleration structures, BVH (Bounding Volume hierarchy for every object)
- [x] Smooth shading
- [X] HDRI Cubemap
- [ ] Scenes presets / import with ImGUI
- [ ] ?? Denoising algorithm ??

Remaining fixes : 
- Loop over a list of filepath and construct buffers automatically
- Stop cutting every Bounding Volume in half, uniformly distribute the triangles instead
- Possibility to disable smooth shading
- Fix jitter and multiple rays per pixel