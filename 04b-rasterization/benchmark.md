 # Source:
 ## init_model() or OBJ:
 * num_vertices
 * num_faces
 * model type (not really relevant though because conveyed through num_vertices / num_faces)
 ## SCENE:
 * when rendering larger scenes?
*  when in scene mode: mouse callback?
 ----------------
 # Vertex Modification:
 * projection mode: Orthographic vs Perspective
 ----------------
 # Pixel Stage:
 * draw mode
 ----------------
 # Post Processing:
 * some effects from image
 * depth of field
 * try other blurring techniques? num of blurs?
 -----
 # Other Program Settings:
 * texturing (diff textures: high res, etc)
* material 
* OBJs supply vertex normals

* drawing normals: NO_NORMALS, F_NORMALS, V_NORMALS
* drawing coord axes
* drawing bounding box



# Draw Settings
* alpha blending (another operation per pixel)
* depth testing (another check + another write)
* texturing (repeated from above: "Other Program Settings")
* modulating (just an extra multiplication per pixel)
* perspective correct (more arithmetic)
* shading mode (none, flat, phong)
  * flat: per triangle (vtx shader)
  * phong: per pixel
* bump mapping (texturing - avoid calculating v_normals)
* material (repeat from "Other Program Settings")
* specular highlight
* fog

* modulate with light or with color
* texture generation mode (NAIVE, CYLINDRICAL, SPHERICAL, ENVIRONMENTAL, CUBE MAP)


