OBJECTS = texture.c util.c triangle.c vector.c test.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o triangle 

run: 
	make build && ./triangle

test: 	
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o run_test && ./run_test

debug:
	gcc -g $(OBJECTS) -o tri-debug && lldb tri-debug