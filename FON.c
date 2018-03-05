#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIDTH 400
#define HEIGHT 100

int main(int argc, char* argv[]) {
    Display* dpy = XOpenDisplay(NULL);
    int src = DefaultScreen(dpy);
    int depth = DefaultDepth(dpy, src);
    GC gc = DefaultGC(dpy, src);       
    Window root = DefaultRootWindow(dpy);
    Pixmap pix;
    Window win;
    XSetWindowAttributes attr;
    unsigned long amask;
    unsigned long emask;
    XEvent event;
    unsigned int done = 0;

    pix = XCreatePixmap(dpy, root, WIDTH, HEIGHT, depth);
    XSetBackground(dpy, gc, WhitePixel(dpy, src));
    XSetForeground(dpy, gc, WhitePixel(dpy, src));
    XFillRectangle(dpy, pix, gc, 0, 0, WIDTH, HEIGHT);
    XSetForeground(dpy, gc, BlackPixel(dpy, src));
    // XDrawRectangle(dpy, pix, gc, 0, 0, WIDTH, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH/4, 0, WIDTH/4, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH/4, HEIGHT, 0, HEIGHT/2);
    XDrawLine(dpy, pix, gc, WIDTH/4, 0, 0, HEIGHT/2);
    XDrawLine(dpy, pix, gc, WIDTH/4, 0, WIDTH/2, 0);
    XDrawLine(dpy, pix, gc, WIDTH/4, HEIGHT, WIDTH/2, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH/2, 0, WIDTH/2, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH/2, 0, WIDTH/4*3, HEIGHT/2);
    XDrawLine(dpy, pix, gc, WIDTH/2, HEIGHT, WIDTH/4*3, HEIGHT/2);
    XDrawLine(dpy, pix, gc, WIDTH/4*3, 0, WIDTH/4*3, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH-1, 0, WIDTH-1, HEIGHT);
    XDrawLine(dpy, pix, gc, WIDTH/4*3, HEIGHT/2, WIDTH, HEIGHT/2); 
    XFlush(dpy);
    amask = (CWOverrideRedirect | CWBackPixmap);
    attr.override_redirect = False;
    attr.background_pixmap = pix;
    win = XCreateWindow(dpy, root, 0, 0, 800, 600, 1, depth,
                        InputOutput, CopyFromParent, amask, &attr);
    emask = (ButtonPressMask | ButtonReleaseMask |
            EnterWindowMask | LeaveWindowMask |
            KeyPressMask);
    XSelectInput(dpy, win, emask);
    XMapRaised(dpy, win);
    XStoreName(dpy, win, "floor");
    XSetFunction(dpy, gc, GXinvert);

    while(done == 0) {
        XNextEvent(dpy, &event);
        switch(event.type) {
            case EnterNotify:
            case LeaveNotify:
            case ButtonPress:
            case ButtonRelease:
                XCopyArea(dpy, pix, pix, gc, 0, 0,
                        WIDTH, HEIGHT, 0, 0);
                        XSetWindowBackgroundPixmap(dpy, win, pix);
                        XClearWindow(dpy, win);
                break;
            case KeyPress:
                done = event.xkey.keycode;
                break;
            default: break;
        }
    }

    XFreePixmap(dpy, pix);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return done;
}
