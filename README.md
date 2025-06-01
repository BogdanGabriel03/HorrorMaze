## Overview

**HorrorMaze** is an immersive horror-genre application implemented with OpenGL and freeglut. The core objective of this project is to simulate an indefinitely extending corridor through which the player traverses. As the player progresses, the environment dynamically modifies itself, becoming increasingly unsettling. The scene is predominantly dark, necessitating the use of a virtual flashlight to navigate and reveal the corridor’s geometry. Repetition of corridor segments is employed to create the illusion of an infinite space, while subtle environmental variations at each iteration intensify the atmospheric tension.

---

## Technologies and Tools

- **Rendering Framework**: OpenGL (version 3.3 Core Profile)

  - Utilized for all rendering operations, including shader management, buffer allocation, and texture binding.
  - freeglut serves as the windowing and context‐creation toolkit, providing input callbacks and event management.

- **Programming Languages**: C and C++

  - Core engine and application logic are developed in C++.
  - Performance‐critical routines (e.g., memory allocation for geometry buffers) utilize idiomatic C++ constructs, while certain low‐level utility functions (such as mathematical helpers) remain in C for minimal overhead.

- **Image Loading**: [stb_image.h](https://github.com/nothings/stb)

  - Employed to load texture data (PNG and JPEG) into CPU memory.
  - Facilitates decoding of image formats and generation of OpenGL‐compatible texture arrays.

- **Shader Development**: GLSL (OpenGL Shading Language)
  - Vertex and fragment shaders handle per‐pixel lighting, normal mapping, and texture sampling.
  - A modular struct (`LightSource`) supports both static point lights and a dynamic spotlight (flashlight) attached to the player’s viewpoint.

---

## Implementation Details

### Corridor Generation

1. **Segment Definition**

   - Each corridor segment is represented by a `HallwaySegment` class, encapsulating position, rotation, scale, and bounding‐box collision data (OBB).
   - Four concrete subclasses (`WallSegment`, `DoorSegment`, `FloorSegment` and `LightSource`) inherit from `HallwaySegment`, allowing for polymorphic update and rendering routines.

2. **Boundary Collision (OBB)**

   - An oriented bounding box (OBB) is computed per segment using `glm` transforms.
   - Collision detection employs a standard “slab method,” iterating over the three principal axes of each OBB to determine (ray)‐intersection intervals.

3. **Procedural Looping**
   - The corridor is partitioned into a finite number of distinct segment templates (e.g., straight walls, corner pieces, door frames, floor tiles, lights).
   - As the player moves forward, segment templates are recycled beyond the camera’s view to create the effect of infinite extension.
   - Each template’s transformation (translation, rotation) is updated based on the player’s position modulo the segment length, ensuring continuity of geometry.

### Lighting Model

1. **Global Illumination Sources**

   - Four static point lights are positioned at predetermined coordinates along the corridor.
   - A boolean array (`hitByLight[4]`) is updated per‐frame using raycasting: for each segment, a ray is cast from each light source to the segment’s centroid. If any OBB intersects this ray before reaching the segment, the segment is marked as occluded from that light.

2. **Flashlight Mechanics (Spotlight)**

   - The player’s flashlight is represented as a `LightSource` with position tied to the camera, direction aligned with the viewing vector, and inner/outer cutoff angles for a smooth spotlight cone.
   - In the fragment shader, a dot‐product comparison between the normalized direction of the fragment relative to the flashlight and the flashlight’s forward vector yields `theta`.
   - A smooth interpolation (`clamp((theta − outerCutoff) / (innerCutoff − outerCutoff), 0.0, 1.0)`) determines the spotlight intensity falloff at the cone’s periphery.
   - Attenuation is computed using the standard formula:
     ```
     attenuation = 1.0 / (constant + linear * d + quadratic * (d^2))
     ```
     where `d` is the distance from the fragment to the light source.

3. **Shader Structure**
   - **Vertex Shader**: Transforms object‐space vertex positions to clip space, passes normal vectors and texture coordinates to the fragment stage.
   - **Fragment Shader**:
     1. Normalizes interpolated normals (`normalize(Normal)`).
     2. Computes view direction (`normalize(viewPos − FragPos)`).
     3. Iterates over each point light (if `hitByLight[i]` is true) to accumulate ambient, diffuse, and specular contributions.
     4. If the flashlight is enabled, invokes `CalculateFlashlightImpact()`, which:
        - Computes the angle `theta` between the flashlight direction and the fragment direction.
        - Applies a smooth cutoff for the inner/outer cone.
        - Adds attenuation and specular components.
     5. Samples the appropriate texture (floor, ceiling, wall, or door) based on a uniform flag `useTextures`.
     6. Multiplies the final lighting term by the material color and outputs `FragColor`.

---

## Gameplay Mechanics

- **Player Spawn**

  - Upon initialization, the player is placed at the center of the first corridor segment, facing forward.
  - The camera’s initial position is `(0.0, 0.0, startZ)`, where `startZ` corresponds to the center of the segment.

- **Movement and Collision**

  - Standard FPS controls (`W, A, S, D`) move the camera in the horizontal plane; `mouse` controls yaw and pitch.
  - An OBB representing the player’s bounding volume is tested each frame against all corridor OBBs to prevent intersection.
  - When a collision is detected, the player’s position is reverted to the previous valid coordinate.

- **Interactive Doors**

  - Doors are modeled as separate `DoorSegment` instances containing an animation state (open/closed) and an angular offset (`_angle`).
  - If the player is within a threshold distance (e.g., 10 units) and presses the interaction key (`F`), the door’s animation is triggered, toggling `_angle` between 0° and 90°.
  - The door’s OBB is recomputed each frame during animation to update collision boundaries.

- **Endless Corridor Illusion**

  - As the player crosses from one segment to the next (based on Z‐coordinate), segments behind the camera are recycled and appended ahead with updated transformation parameters.
  - Each recycled segment’s textures and materials may incorporate progressively darker or distorted textures, producing a **creepier visual** with each loop.
  - Ambient audio cues (e.g., distant footsteps, creaking doors) are triggered at predetermined intervals to reinforce immersion.

- **Flashlight and Darkness**
  - The corridor is otherwise unlit except for the flashlight and the static point lights (which may be toggled or disabled by pressing `F`).
  - The default fragment shader enforces a minimal ambient value (`vec3(0.024, 0.002, 0.002)`) if no light contributes, ensuring that silhouettes remain faintly visible rather than completely black.

---

## Asset Management

- **Textures**

  - All image assets (floor, ceiling, walls, doors) are stored as `.png` or `.jpg` files in a designated `imagini/` directory.
  - `stbi_load()` is invoked to decode image bytes and construct OpenGL textures via `glGenTextures`, `glBindTexture`, `glTexImage2D`, and `glGenerateMipmap`.

- **Mesh Data**

  - A generic `createQuadVAO()` function encapsulates the generation of a 2×2 square in the XZ or XY plane. This VAO is specialized via model‐matrix transforms to serve as a floor tile, wall segment, or door panel.
  - Floor meshes are oriented horizontally (normals pointing upward), while walls and doors are vertical quads (normals facing inward toward the corridor).

- **Shaders**
  - Vertex and fragment shader sources reside in `shaders/vertex.vert`, `shaders/fragment.frag`, and `shaders/light_source.vert` (for visualizing light volumes).
  - Shader compilation and linking are managed via a `Shader` utility class that checks for compile errors, logs status, and exposes `setMat4`, `setVec3`, `setFloat`, and `setBool` methods for uniform updates.

---

## Development References

- Core rendering tutorials and architectural patterns were derived from [LearnOpenGL](https://learnopengl.com/).
- Collision detection references follow the “Slab Method” for OBB intersection as described in various academic collision detection papers.
- GLSL lighting equations (ambient, diffuse, specular, attenuation) adhere to classical Phong shading as documented in seminal graphics texts (e.g., Foley, van Dam, Feiner, Hughes).

---

## Acknowledgments

- **LearnOpenGL**: For comprehensive explanations of modern OpenGL pipeline stages, G-buffer techniques, and advanced lighting.
- **stb_image.h**: For lightweight, public‐domain image loading functionality.
- **freeglut**: For streamlined window/context creation and input handling in cross-platform environments.

---

_This project was developed with an emphasis on modular, reusable code, real‐time performance optimizations, and an academically rigorous approach to procedural environment generation and dynamic lighting._
