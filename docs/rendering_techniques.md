# Rendering Techniques

How do we render a scene on the screen?

There are plenty of methods we can use to do that, each one with its
own style and requirements. Here is a general overview of the
rendering techniques I have played with:

- rasterization: this is the most popular technique for real time
  rendering since we have hardware that can do it fast, and it is the
  default technique in the engine. Its primitive is the triangle,
  which is composed of three vertices in 3D space. A vertex may also
  store a color or a position in a texture (often called `uv`
  coordinates). Given a triangle, after projecting it to the screen,
  the renderer (whether software or hardware accelerated) will
  interpolate over discrete pixels inside the triangle and color them
  based on a coloring algorithm or program (fragment shader).  This
  "projection" is achieved by multiplying together three matrices: the
  `model` or `world` matrix which translates a vertex to its position
  in world space, the `view` matrix which shift and rotates the world
  based on the camera position (If the camera moves 5 feet to the
  right, it’s mathematically the same as moving the entire world 5
  feet to the left), and `projection` matrix which applies perspective
  and field-of-view; this ultimately maps the vertices inside a cube
  called the Canonical Cube where the GPU can work with.
- ray tracing: the general idea is to shoot a ray (which is just a
  line) for each pixel (sometimes more than one ray per pixel) which
  interact with the scene and computes the color of the pixel. We
  simulate lights and reflections by scattering a ray when it collides
  with object based on the object's material. This achieves very good
  looking images but it is costly since you may want to simulate every
  ray bouncing over many surfaces. We also need to have a way to check
  if a ray collided with an object, which is "trivial" only for simple
  shapes but gets easily more complex (requiring Bounding Volume
  Hierarchy or KD-Trees). There are several techniques to calculate
  the color, including:
  - path tracing: instead of just "bouncing" a ray, it uses Monte
    Carlo integration to randomly sample many paths for light.
  - ray marching: we step through the ray line by some amount, checking
  for intersections with the scene.
    - Voxel based: we step through a grid which divides space in
      discrete squares or cubes. This creates a "blocky" look.
    - Sphere tracing: we calculate the step size by measuring the
      distance between the current ray position and the closest
      object.
  - raycasting: this is the famous technique used by Wolfestein 3D,
    where we iterate over the X axis of the framebuffer and shoot a
    ray in a 2D space. The illusion of 3D is achieved by drawing
    vertical lines based on the distance of the camera and the object
    that has been hit.
- point cloud: here we want to render a set of points. We may want to
  do this when we have a dataset of points, for example while doing
  photogrammetry where we want to reconstruct 3D objects from
  images. Or we may want to convert these points to a mesh.
  We could use both ray tracing or rasterization for rendering.
  - gaussian / triangle splatting: instead of using points as
    primitives, we use 3D gaussians or triangles. We learn the
    properties of the primitive (color, dimensions...) via a neural
    network. We can then rasterize it to the screen.
