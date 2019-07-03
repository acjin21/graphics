OBJECTS = vector.c util.c texture.c image.c color.c depth.c raster.c point.c main.c model.c light.c scene.c material.c post-processing.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o model 

debug:
	gcc -g $(OBJECTS) -o run-debug && lldb run-debug