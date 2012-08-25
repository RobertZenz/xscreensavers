/*
 * lavanet.c
 *
 *      Author: Robert 'Bobby' Zenz
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include "vroot.h"


#define FALSE 0
#define TRUE 1
#define BLACK 0x000000


struct vector {
	float x;
	float y;
};

// Global, sorry.
int debug = FALSE;

float get_random() {
	return ((float) rand() / RAND_MAX - 0.5f) * 2;
}

ulong make_color(u_char red, u_char green, u_char blue) {
	ulong color = red;
	color = color << 8;

	color |= green;
	color = color << 8;

	color |= blue;

	return color;
}

float sign(float x) {
	float val = x > 0;
	return val - (x < 0);
}

void parse_arguments(int argc, char *argv[]) {
	int idx;
	for(idx = 1; idx < argc; idx++) {
		if(strcasecmp(argv[idx], "--debug") == 0) {
			debug = TRUE;
		}
	}
}

int main(int argc, char *argv[]) {
	int count = 200;
	float minimumDistance = 100;
	float targetFps = 1000 / 60;
	float topChange = 0.1f;
	float topSpeed = 0.5f;

	parse_arguments(argc, argv);

	// Create our display
	Display *dpy;
	dpy = XOpenDisplay(getenv("DISPLAY"));

	// Get the root window
	Window root;
	if (debug == FALSE) {
		// Get the root window
		root = DefaultRootWindow(dpy);
	} else {
		// Let's create our own window.
		int screen = DefaultScreen(dpy);
		root = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 24, 48, 640,
				640, 1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));
		XMapWindow(dpy, root);
	}

	// Get the window attributes
	XWindowAttributes wa;
	XGetWindowAttributes(dpy, root, &wa);

	// Create the buffer
	Pixmap double_buffer = XCreatePixmap(dpy, root, wa.width, wa.height,
			wa.depth);

	// And new create our graphics context to draw on
	GC g;
	g = XCreateGC(dpy, root, 0, NULL);

	srand(0);

	struct vector points[count];
	struct vector velocities[count];
	int counter = 0;
	for (counter = 0; counter < count; counter++) {
		points[counter].x = rand() % wa.width;
		points[counter].y = rand() % wa.height;

		velocities[counter].x = get_random() * topSpeed;
		velocities[counter].y = get_random() * topSpeed;
	}

	// I hate this...
	while (1) {
		XSetBackground(dpy, g, BLACK);
		XSetForeground(dpy, g, BLACK);
		XFillRectangle(dpy, double_buffer, g, 0, 0, wa.width, wa.height);

		// Move
		for (counter = 0; counter < count; counter++) {
			velocities[counter].x += get_random() * topChange;
			velocities[counter].y += get_random() * topChange;

			if (abs(velocities[counter].x) > topSpeed) {
				velocities[counter].x = topSpeed * sign(velocities[counter].x);
			}

			if (abs(velocities[counter].y) > topSpeed) {
				velocities[counter].y = topSpeed * sign(velocities[counter].y);
			}

			points[counter].x += velocities[counter].x;
			points[counter].y += velocities[counter].y;

			if (points[counter].x < 0) {
				points[counter].x = wa.width;
			}
			if (points[counter].x > wa.width) {
				points[counter].x = 0;
			}
			if (points[counter].y < 0) {
				points[counter].y = wa.height;
			}
			if (points[counter].y > wa.height) {
				points[counter].y = 0;
			}
		}

		// Draw
		for (counter = 0; counter < count; counter++) {
			struct vector pointA = points[counter];
			int second = 0;
			for (second = counter + 1; second < count; second++) {
				struct vector pointB = points[second];

				// Check distance between points
				int distanceX = abs(pointA.x - pointB.x);
				int distanceY = abs(pointA.y - pointB.y);

				double distance = sqrt(pow(distanceX, 2) + pow(distanceY, 2));

				if (distance < minimumDistance) {
					u_char value = (u_char) floor(distance / minimumDistance
							* 255);
					XSetForeground(dpy, g, make_color(255 - value, 255 - value,
							0));
					XDrawLine(dpy, double_buffer, g, pointA.x, pointA.y,
							pointB.x, pointB.y);
				}
			}
		}

		XCopyArea(dpy, double_buffer, root, g, 0, 0, wa.width, wa.height, 0, 0);

		XFlush(dpy);
		usleep(targetFps);
	}

	return 0;
}
