BUFFERS = color_buffer.c depth_buffer.c g_buffer.c stencil_buffer.c
UTILS = vector.c util.c math_lib/mat4.c math_lib/mat3.c time.c opengl.c window.c
OBJECTS = light.c camera.c model.c scene.c 
RENDER_PIP = point.c raster.c frustum.c material.c texture.c

MODULES = image.c post-processing.c fourier.c $(BUFFERS) $(UTILS) $(OBJECTS) $(RENDER_PIP) main.c app.c

run-basic: 
	gcc -framework OpenGL -framework GLUT $(MODULES) -o build/model && ./build/model BASIC

run-benchmark: 
	gcc -framework OpenGL -framework GLUT $(MODULES) -o build/model && ./build/model BENCHMARK

run-scene:
	gcc -framework OpenGL -framework GLUT $(MODULES) -o build/model && ./build/model SCENE $(file)

run-fourier:
	gcc -framework OpenGL -framework GLUT $(MODULES) -o build/model && ./build/model FOURIER

debug:
	gcc -g -framework OpenGL -framework GLUT $(MODULES) -o build/model-debug && lldb build/model-debug

print-debug:
	gcc -DPRINT_DEBUG_OBJ -framework OpenGL -framework GLUT $(MODULES) -o build/model_print-debug

test-math:
	gcc -DTESTING camera.c vector.c math_lib/mat3.c math_lib/mat4.c math_lib/test.c -o test/math && ./test/math

run-windows:
	gcc -std=c11 -DWINDOWS -c -o $(OBJECTS) -I"C:\Program Files\MinGW\freeglut\include"