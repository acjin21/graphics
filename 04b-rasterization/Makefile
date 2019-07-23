OBJECTS = vector.c util.c texture.c image.c color.c depth.c raster.c point.c main.c model.c light.c scene.c material.c post-processing.c math_lib/mat4.c time.c benchmark.c app.c camera.c frustum.c 

run-basic: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o build/model && ./build/model BASIC

run-benchmark: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o build/model && ./build/model BENCHMARK

run-scene:
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o build/model && ./build/model SCENE $(file)

run-fourier:
	gcc -framework OpenGL -framework GLUT $(OBJECTS) fourier.c -o build/model && ./build/model FOURIER

debug:
	gcc -g -framework OpenGL -framework GLUT $(OBJECTS) -o build/model-debug && lldb build/model-debug

print-debug:
	gcc -DPRINT_DEBUG_OBJ -framework OpenGL -framework GLUT $(OBJECTS) -o build/model_print-debug

