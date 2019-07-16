OBJECTS = vector.c util.c texture.c image.c color.c depth.c raster.c point.c main.c model.c light.c scene.c material.c post-processing.c math_lib/mat4.c time.c benchmark.c app.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o build/model 

debug:
	gcc -g -framework OpenGL -framework GLUT $(OBJECTS) -o build/model-debug && lldb build/model-debug

print-debug:
	gcc -DPRINT_DEBUG_OBJ -framework OpenGL -framework GLUT $(OBJECTS) -o build/model_print-debug