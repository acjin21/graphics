OBJECTS = vector.c util.c texture.c image.c color.c depth.c test.c raster.c point.c main.c model.c light.c scene.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o model 

run: 
	make build && ./triangle

debug:
	gcc -g $(OBJECTS) -o tri-debug && lldb tri-debug