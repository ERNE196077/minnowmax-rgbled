
#include <stdio.h>
#include "headers/rgbled_x11.h"

static int w, h;
static int topstep, bottomstep, rightstep, leftstep, totalleds;
static int tmpstepx, tmpstepy;
x11rawpixel_t* rawpixels;
Display* display;
led_t* pixels;
XImage* TheImage ;
XImage* otherimage;
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

	root = DefaultRootWindow(display);

	w = pscr->width;
	h = pscr->height;

	topstep = ( w - (border * 2)) / (topleds - 1);
	bottomstep = ( w - (border * 2)) / (bottomleds - 1);
	rightstep = ( h - (border * 2)) / (rightleds + 2);
	leftstep = ( h - (border * 2)) / (leftleds + 2);

	totalleds = topleds + bottomleds + rightleds + leftleds;

	/* Allocate space for the LED information */
	rawpixels = (x11rawpixel_t *)malloc( totalleds * (sizeof(x11rawpixel_t)) );
	pixels = pleds;

	tmpstepx = border;
	tmpstepy = border;

	
	/* Top LEDs */
	for (i = 0 ; i < topleds ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = XGetImage (display, root, tmpstepx, tmpstepy, 1, 1, AllPlanes, XYPixmap);
		
		if (i < topleds - 1)
		tmpstepx += topstep;
	}

	/* Right LEDs */
	for (i = i ; i < (topleds + rightleds) ; i++){
		tmpstepy += rightstep;
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = XGetImage (display, root, tmpstepx, tmpstepy, 1, 1, AllPlanes, XYPixmap);
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		if (i == (topleds + rightleds - 1))
			tmpstepy += rightstep;
	}
	
	/* Bottom LEDs */
	for (i = i ; i < (topleds + rightleds + bottomleds) ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = XGetImage (display, root, tmpstepx, tmpstepy, 1, 1, AllPlanes, XYPixmap);
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

		if (i < (topleds + rightleds + bottomleds - 1))
			tmpstepx -= bottomstep;
	}

	/* Left LEDs */
	for (i = i ; i < totalleds ; i++){

		tmpstepy -= leftstep;
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		rawpixels[i].image = XGetImage (display, root, tmpstepx, tmpstepy, 1, 1, AllPlanes, XYPixmap);
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);

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
	
	for (i = 0 ; i < totalleds ; i++){
		XGetSubImage(display,root, rawpixels[i].x,rawpixels[i].y,1,1,AllPlanes, XYPixmap, rawpixels[i].image, 0,0); 
		rawpixels[i].xcolor.pixel = XGetPixel (rawpixels[i].image, 0, 0);
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

	printf ("###################################\n");

	
	/*
	display =  XOpenDisplay(NULL);
	if ( !display ){
		printf("Cannot init X11 rgbleds. Is the X server running?\n");
		return -1;
	}
	root = DefaultRootWindow(display);

	//XMapRaised(display, root);
	otherimage = malloc(sizeof(XImage));
	TheImage =  XGetImage (display,root , 1200, 1200, 1, 1, AllPlanes, XYPixmap);

	//XGetSubImage(display,root, 1200,1200,1,1,AllPlanes, XYPixmap, TheImage, 0,0); 
	testxcolor.pixel = XGetPixel (TheImage, 0, 0);
	XQueryColor (display, DefaultColormap(display, DefaultScreen (display)), &testxcolor);

	printf ("Red : %d\n",testxcolor.red/256);
		printf ("Green : %d\n",testxcolor.green/256);
		printf ("Blue : %d\n",testxcolor.blue/256);



	sleep(2);
	//XGetSubImage(display,root, 1200,1200,1,1,AllPlanes, XYPixmap, TheImage, 0,0); 
	testxcolor.pixel = XGetPixel (TheImage, 0, 0);
		XQueryColor (display, DefaultColormap(display, DefaultScreen (display)), &testxcolor);

	printf ("Red : %d\n",testxcolor.red/256);
		printf ("Green : %d\n",testxcolor.green/256);
		printf ("Blue : %d\n",testxcolor.blue/256);




sleep(2);
//XGetSubImage(display,root, 1200,1200,1,1,AllPlanes, XYPixmap, TheImage, 0,0); 
	testxcolor.pixel = XGetPixel (TheImage, 0, 0);
		XQueryColor (display, DefaultColormap(display, DefaultScreen (display)), &testxcolor);

	printf ("Red : %d\n",testxcolor.red/256);
		printf ("Green : %d\n",testxcolor.green/256);
		printf ("Blue : %d\n",testxcolor.blue/256);






	XFree(TheImage);

	free(otherimage);
	XCloseDisplay(display);
*/

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