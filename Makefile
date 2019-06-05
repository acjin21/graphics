POINT_OBJS = point.c pixels.c trig.c hopalong_fract.c mandelbrot.c sierpinski.c bifurc.c lorenz.c func.c

build-point:
	gcc -framework OpenGL -framework GLUT $(POINT_OBJS) -o point

open: 
	open $(OBJECTS) point.h

LINE_OBJS = line.c 

build-line: 
	gcc -framework OpenGL -framework GLUT $(LINE_OBJS) -o line