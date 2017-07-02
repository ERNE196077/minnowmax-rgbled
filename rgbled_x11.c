
#include <stdio.h>
#include "headers/rgbled_x11.h"

static int w, h;
static int topstep, bottomstep, rightstep, leftstep, totalleds;
static int tmpstepx, tmpstepy;
x11rawpixel_t* rawpixels;
Display* display;
led_t* pixels;
XImage* TheImage;
Window root;
XColor testxcolor;



Display* getScreen ( void ){

	Display* pdsp = NULL;
	
	pdsp = XOpenDisplay( NULL );
	if ( !pdsp ) {
		fprintf(stderr, "Failed to open default display.\n");
		return NULL;
	}

	return pdsp;
}


int x11rgbleds_init(int topleds, int leftleds, int rightleds, int bottomleds, int border, led_t *pleds ){
	Screen* pscr = NULL;
	int i;

	display = getScreen();
	if ( !display ){
		printf("Cannot init X11 rgbleds. Is the X server running?\n");
		return -1;
	}

	pscr = DefaultScreenOfDisplay( display );
	if ( !pscr ) {
		fprintf(stderr, "Failed to obtain the default screen of given display.\n");
		printf("Cannot init X11 rgbleds. Is the X server running?\n");
		XCloseDisplay(display);
		return -2;
	}

	w = pscr->width;
	h = pscr->height;

	topstep = ( w - (border * 2)) / topleds;
	bottomstep = ( w - (border * 2)) / bottomleds;
	rightstep = ( h - (border * 2)) / rightleds;
	leftstep = ( h - (border * 2)) / leftleds;

	totalleds = topleds + bottomleds + rightleds + leftleds;

	/* Allocate space for the LED information */
	rawpixels = (x11rawpixel_t *)malloc( totalleds * (sizeof(x11rawpixel_t)) );
	pixels = pleds;

	tmpstepx = border;
	tmpstepy = border;

	TheImage = XGetImage (display, DefaultRootWindow(display), 0, 0, w, h, AllPlanes, ZPixmap);

	/* Top LEDs */
	for (i = 0 ; i < topleds ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = TheImage;
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		tmpstepx += topstep;
	}

	/* Right LEDs */
	for (i = i ; i < (topleds + rightleds) ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = TheImage;
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		tmpstepy += rightstep;
	}
	
	/* Bottom LEDs */
	for (i = i ; i < (topleds + rightleds + bottomleds) ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = TheImage;
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		tmpstepx -= bottomstep;
	}

	/* Left LEDs */
	for (i = i ; i < totalleds ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = TheImage;
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		tmpstepy -= leftstep;
	}

	printf("width : %d :::: height : %d\n", w, h);
	printf("topstep : %d\n", topstep);
	printf("bottomstep : %d\n", bottomstep);
	printf("rightstep : %d\n", rightstep);
	printf("leftstep : %d\n", leftstep);


	return 0;

}

int x11rgbleds_query( void ){
	int i, eq = 0, ye = 0;
	/*
	for (i = 0 ; i < totalleds ; i++){
		XQueryColor (display, XDefaultColormap(display, XDefaultScreen (display)), &(rawpixels[i].xcolor));
		pixels[i].r =  rawpixels[i].xcolor.red / 256;
		pixels[i].g =  rawpixels[i].xcolor.green / 256;
		pixels[i].b =  rawpixels[i].xcolor.blue / 256;

		printf ("*****************\n");
		printf ("X : %d ::: Y : %d\n",rawpixels[i].x, rawpixels[i].y );
		printf ("Red : %d\n",pixels[i].r);
		printf ("Green : %d\n",pixels[i].g);
		printf ("Blue : %d\n",pixels[i].b);
	}

	*/
	display =  XOpenDisplay(NULL);
	if ( !display ){
		printf("Cannot init X11 rgbleds. Is the X server running?\n");
		return -1;
	}
	root = DefaultRootWindow(display);

	XMapRaised(display, root);
	XGetSubImage (display,root , 0, 0, 1100, 1100, AllPlanes, XYPixmap, TheImage, eq, ye);
	testxcolor.pixel = XGetPixel (TheImage, 0, 0);

	XFree(TheImage);
	XCloseDisplay(display);


	return 0;
}

int x11rgbleds_close( void ){
	int i;
	/* Free X11 buffers */
	for (i = 0 ; i < totalleds ; i++){
		XFree(rawpixels[i].image);
	}
	XCloseDisplay(display);

	/* Free memory allocated buffers */
	free(rawpixels);

	return 0;
}