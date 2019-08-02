OBJECTS = triangle.c vector.c test.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o triangle 

run: 
	make build && ./triangle

test: 	
	gcc -framework OpenGL -framework GLUT $(OBJECTS) test.c -o run_test && ./run_test