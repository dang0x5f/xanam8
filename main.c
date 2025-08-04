#include <err.h>
#include <unistd.h>
#include <X11/Xlib.h>

void draw_pixel(Display *d, Window w, GC gc, int x, int y, unsigned long color)
{
    XSetForeground(d,gc,color);
    XDrawPoint(d,w,gc,x,y);
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

    XMapWindow(dpy,win);
    XSync(dpy,false);

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

    return(0);
}
