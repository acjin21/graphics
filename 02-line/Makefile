OBJECTS = exercises.c line.c vector.c draw_lines.c

build: 
	gcc -framework OpenGL -framework GLUT $(OBJECTS) -o line 

run: 
	make build && ./line

test: 	
	gcc -framework OpenGL -framework GLUT $(OBJECTS) test_draw_triangle.c -o triangle_test && ./triangle_test