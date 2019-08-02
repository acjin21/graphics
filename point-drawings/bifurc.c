#include "point.h"

void draw_bifurcate( void )
{
	double	x;
	double	y	= 0;
	double	r	= 2;
	double	inc	= 0.004;
	
	/*
	 *	calculate bifurcation plot
	 */
	for( int i = 0; i < 800; i++ )
	{
		x = 0.5;
		r += inc;
		
		for( int j = 0; j < 200; j++ )
		{
			x = r * x * (1 - x);
			draw_point( x * 800 - 400, y - 400 );
		}
	
		y = y + 1;
	}
}

