
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "headers/rgbled_x11.h"

struct shmimage
{
    XShmSegmentInfo shminfo ;
    XImage * ximage ;
    unsigned int * data ; 
} ;

static int w, h;
static int topstep, bottomstep, rightstep, leftstep, totalleds;
static int tmpstepx, tmpstepy;
Display* display;
led_t* pixels;
Window root;
x11rawpixel_t *rawpixels;
unsigned long rawpixel;
struct shmimage image ;
int pscr;


void initimage( struct shmimage * image )
{
    image->ximage = NULL ;
    image->shminfo.shmaddr = (char *) -1 ;
}

void destroyimage( Display * dsp, struct shmimage * image )
{
    if( image->ximage )
    {
        XShmDetach( dsp, &image->shminfo ) ;
        XDestroyImage( image->ximage ) ;
        image->ximage = NULL ;
    }

    if( image->shminfo.shmaddr != ( char * ) -1 )
    {
        shmdt( image->shminfo.shmaddr ) ;
        image->shminfo.shmaddr = ( char * ) -1 ;
    }
}


int createimage( Display * dsp, struct shmimage * image, int width, int height )
{
    // Create a shared memory area 
    image->shminfo.shmid = shmget( IPC_PRIVATE, width * height * 4, IPC_CREAT | 0600 ) ;
    if( image->shminfo.shmid == -1 )
    {
        perror( "rgbledX11" ) ;
        return 0 ;
    }

    // Map the shared memory segment into the address space of this process
    image->shminfo.shmaddr = (char *) shmat( image->shminfo.shmid, 0, 0 ) ;
    if( image->shminfo.shmaddr == (char *) -1 )
    {
        perror( "rgbledX11" ) ;
        return 0 ;
    }

    image->data = (unsigned int*) image->shminfo.shmaddr ;
    image->shminfo.readOnly = 0 ;

    // Mark the shared memory segment for removal
    // It will be removed even if this program crashes
    shmctl( image->shminfo.shmid, IPC_RMID, 0 ) ;

    // Allocate the memory needed for the XImage structure
    image->ximage = XShmCreateImage( dsp, XDefaultVisual( dsp, DefaultScreen( dsp ) ),
                        DefaultDepth( dsp, DefaultScreen( dsp ) ), ZPixmap, 0,
                        &image->shminfo, width, height ) ;
    if( !image->ximage )
    {
        destroyimage( dsp, image ) ;
        printf( "rgbledX11: could not allocate the XImage structure\n" ) ;
        return 0 ;
    }

    image->ximage->data = (char *)image->data ;
    image->ximage->width = width ;
    image->ximage->height = height ;

    // Ask the X server to attach the shared memory segment and sync
    XShmAttach( dsp, &image->shminfo ) ;
    XSync( dsp, False ) ;
    return 1 ;
}

int x11rgbleds_init(int topleds, int leftleds, int rightleds, int bottomleds, int border, led_t *pleds ){
	
	int i;

	display = XOpenDisplay( NULL );

	if ( !display ){
		printf("Cannot init X11 rgbleds. Is the X server running?\n");
		return -1;
	}

	if( !XShmQueryExtension( display ) )
    {
        XCloseDisplay( display ) ;
        printf( "rgbledX11: the X server does not support the XSHM extension\n" ) ;
        return 1 ;
    }
    
	pscr = DefaultScreen( display );
	
	initimage( &image );
	w = XDisplayWidth( display, pscr );
	h = XDisplayHeight( display, pscr );

	printf("Width : %d\n",w);
	printf("Height : %d\n",h);


	if( !createimage( display, &image, w, h ) )
    {
        XCloseDisplay( display ) ;
        return 1 ;
    }

    /* Get X11 root window */
	root = XDefaultRootWindow(display);

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
		
		if (i < topleds - 1)
		tmpstepx += topstep;
	}

	/* Right LEDs */
	for (i = i ; i < (topleds + rightleds) ; i++){
		tmpstepy += rightstep;
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		
		if (i == (topleds + rightleds - 1))
			tmpstepy += rightstep;
	}
	
	/* Bottom LEDs */
	for (i = i ; i < (topleds + rightleds + bottomleds) ; i++){
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		
		if (i < (topleds + rightleds + bottomleds - 1))
			tmpstepx -= bottomstep;
	}

	/* Left LEDs */
	for (i = i ; i < totalleds ; i++){

		tmpstepy -= leftstep;
		rawpixels[i].x = tmpstepx;
		rawpixels[i].y = tmpstepy;
		
	}

	return 0;

}

int x11rgbleds_query( void ){
	
	int i;
	
	/* Refresh image contents */
	XShmGetImage( display, root, image.ximage, 0, 0, AllPlanes ) ;

	/* Get pixel color for all LEDs */
	for (i = 0 ; i < totalleds ; i++){
		
		rawpixel = XGetPixel (image.ximage, rawpixels[i].x, rawpixels[i].y);
		
		pixels[i].r =  (rawpixel >> 16) & 0xff;
		pixels[i].g =  (rawpixel >> 8) & 0xff;
		pixels[i].b =  rawpixel & 0xff;

	}

	return 0;
}

int x11rgbleds_close( void ){

	/* Free X11 buffers */
	destroyimage( display, &image ) ;
	XCloseDisplay(display);

	/* Free memory allocated buffers */
	free(rawpixels);

	return 0;
}
