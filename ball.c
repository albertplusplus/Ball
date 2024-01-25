#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HEIGHT 32
#define WIDTH 32
#define FPS 15
#define RADIUS WIDTH / 4
#define GRAV 100.0f
#define DT (1.0f / (float)FPS)
#define DAMPER 0.8f

typedef struct {float x, y; } fvec;
typedef enum
{
	BACK = 0,
	FORE,
	PIXEL_N,
} Pixel;

static Pixel display[WIDTH*HEIGHT];

fvec to_fvec(float, float);
fvec fvec_add(fvec, fvec);
fvec fvec_sub(fvec, fvec);
fvec fvec_mult(fvec, fvec);
void circle(fvec, float);

void show()
{
	static char row[WIDTH];
	static char table[PIXEL_N][PIXEL_N] = 
	{
		{' ','_'},
		{'^','C'},
	};

	for (int y = 0; y < HEIGHT / 2; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			Pixel t = display[(2*y + 0) * WIDTH + x];
			Pixel b = display[(2*y + 1) * WIDTH + x];
			row[x] = table[t][b];
		}
		fwrite(row, WIDTH, 1, stdout);
		fputc('\n',stdout);
	}
}

void fill(Pixel p)
{
	Pixel *ptr = display;
	size_t n = WIDTH * HEIGHT;
	while (n-- > 0) *ptr++ = p;
}

void back()
{
	printf("\x1b[%dD", WIDTH);
	printf("\x1b[%dA", HEIGHT);
}

int main()
{
	fvec pos = to_fvec(WIDTH / 4.0f, HEIGHT / 4.0f);
	fvec vel = to_fvec(50.0f,1.0f);
	fvec gravity = to_fvec(0.0f, GRAV);
	
	for (;;)
	{
		vel = fvec_add(vel, fvec_mult(gravity,to_fvec(0.0f,DT)));
		pos = fvec_add(pos,fvec_mult(vel,to_fvec(DT,DT)));
		fill(BACK);
		circle(pos,RADIUS);
		show();
		back();
		usleep(1000*1000 / FPS);

		if (pos.y + RADIUS >= HEIGHT) {
			vel.y *= -1 * DAMPER;
			pos.y = HEIGHT - RADIUS;
		}
		if (pos.y - RADIUS <= 0) {
			vel.y *= -1 * DAMPER;
			pos.y = RADIUS;
		}
		if (pos.x + RADIUS >= WIDTH) {
			vel.x *= -1 * DAMPER;
			pos.x = WIDTH - RADIUS;
		}
		if (pos.x - RADIUS <= 0) {
			vel.x *= -1 * DAMPER;
			pos.x = RADIUS;
		}		
	}
	return 0;
}

void circle(fvec c, float r)
{
	fvec b = fvec_sub(c, to_fvec(r,r));
	fvec e = fvec_add(c, to_fvec(r,r));
	for (int y = b.y; y <= e.y; ++y)
	{
		for (int x = b.x; x <= e.x; ++x)
		{
			float dx = c.x - (0.5f + x);
			float dy = c.y - (0.5f + y);
			if (dx*dx + dy*dy <= r*r)
				if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT)
					display[x + y*WIDTH] = FORE;
		}
	}
}

fvec fvec_sub(fvec a, fvec b)
{
	fvec ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	return ret;
}

fvec fvec_add(fvec a, fvec b)
{
	fvec ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	return ret;
}

fvec fvec_mult(fvec a, fvec b)
{
	fvec ret;
	ret.x = a.x * b.x;
	ret.y = a.y * b.y;
	return ret;
}

fvec to_fvec(float a, float b)
{
	fvec ret;
	ret.x = a;
	ret.y = b;
	return ret;
}
