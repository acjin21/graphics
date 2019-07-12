OBJECTS = vector.c util.c texture.c image.c color.c depth.c raster.c point.c main.c model.c light.c scene.c material.c post-processing.c math_lib/mat4.c time.c command.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o model 

debug:
	gcc -g -framework OpenGL -framework GLUT $(OBJECTS) -o model-debug && lldb model-debug

print-debug:
	gcc -DPRINT_DEBUG -framework OpenGL -framework GLUT $(OBJECTS) -o model_print-debug