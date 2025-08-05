#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define FPS 60

void test01(Display *, Window , GC , uint32_t , uint32_t );
void test02(Display *, Window , Pixmap , GC , uint32_t , uint32_t );

void draw_pixel(Display *d, Pixmap p, GC gc, int x, int y, unsigned long color)
{
    XSetForeground(d,gc,color);
    XDrawPoint(d,p,gc,x,y);
}

int main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    int screen = DefaultScreen(dpy);

    int width=250, height=250, border_width=2;
    int x=DisplayWidth(dpy,screen)/2-(width/2),
        y=DisplayHeight(dpy,screen)/2-(height/2);

    XSetWindowAttributes xswa = {
        .override_redirect = true,
        .event_mask = ExposureMask|
            SubstructureNotifyMask,
    };

    Window win = XCreateWindow(dpy, 
                               XDefaultRootWindow(dpy), 
                               x, y, width, height, border_width, 
                               DefaultDepth(dpy,screen), 
                               InputOutput, 
                               DefaultVisual(dpy,screen), 
                               CWEventMask|CWOverrideRedirect, 
                               &xswa);

    GC gc = XCreateGC(dpy,win,0,NULL);
    if(!gc) err(1,"effd\n");

    uint32_t* pixels = malloc(width*height*sizeof(uint32_t));
    /* Pixmap pm = XCreatePixmap(dpy,win,width,height,DefaultDepth(dpy,screen)); */
    XImage *xi = XCreateImage(dpy,
                              DefaultVisual(dpy,screen),
                              DefaultDepth(dpy,screen),
                              ZPixmap, 0,
                              (char*)pixels,
                              width, height,
                              32, width*sizeof(uint32_t));


    XMapWindow(dpy,win);
    XSync(dpy,false);

    int xoffset = 0;
    int yoffset = 0;
    while(1){
        int idx = 0;
        for(int col=0; col<height; ++col){
            for(int row=0; row<width; ++row){
                pixels[col*width+row] = (uint32_t) 0 << 24 |
                                        (uint32_t) 0 << 16 |
                 (uint32_t) (uint8_t)((row+xoffset)) <<  8 |
                 (uint32_t) (uint8_t)((col+yoffset)) <<  0 ;

            }
        }
        xoffset+=1;
        yoffset+=1;
        XPutImage(dpy,win,gc,xi,0,0,0,0,width,height);
        
        usleep(1000*1000/FPS);
    }

    /* test02(dpy,win,pm,gc,width,height); */
    /* test01(dpy,win,gc,width,height); */

    return(0);
}

/* pixmap backbuffer */
void test02(Display *dpy, Window win, Pixmap pm, GC gc, uint32_t width, uint32_t height)
{
    int xoffset = 0;
    int yoffset = 0;
    while(1){
        for(int row=0; row<width; ++row){
            for(int col=0; col<height; ++col){
                unsigned long pixel = (uint32_t) 0  << 16 |
                                      (uint32_t) (uint8_t)((row+yoffset)) << 8 |
                                      (uint32_t) (uint8_t)((col+xoffset)) << 0 ;
                XSetForeground(dpy,gc,pixel);
                XDrawPoint(dpy,pm,gc,col,row);
                XCopyArea(dpy,pm,win,gc,0,0,width,height,0,0);
            }
        }
        xoffset+=2;
    }
}

/* Pixel-by-pixel */
void test01(Display *dpy, Window win, GC gc, uint32_t width, uint32_t height)
{
    XEvent event;
    int xoffset = 0;
    int yoffset = 0;
    while(1){ 
        /* XClearWindow(dpy,win); */
        for(int row=0; row<width; ++row){
            for(int col=0; col<height; ++col){
                unsigned long pixel = (uint32_t) 0  << 16 |
                                      (uint32_t) (uint8_t)((row+yoffset)) << 8 |
                                      (uint32_t) (uint8_t)((col+xoffset)) << 0 ;
                                      /* (uint32_t) (uint8_t)((row+yoffset)/100)*255 << 8 | */
                                      /* (uint32_t) (uint8_t)((col+xoffset)/100)*255 << 0 ; */
                draw_pixel(dpy,win,gc,col,row,pixel);
            }
        }
        xoffset+=5;
    }
}
