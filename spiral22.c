
/* 2 centred spiral main function */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include "spiral2x.h"

/* Main function */

int main(int argc, char *argv[]) {
    Display *dpy; /* Graphic Display */
    Window win; /* programm main window */
    GC gc[2]; /* Black & White Graphic Context */
    int scr; /* screen number = 0 */
    XPiAr2 r; /* spiral structure */

    { /* Display Block */
        unsigned long tone; /* Light drawing tone */
        XFontStruct *fnptr; /* font structure address to 9x15 */
        dpy = XOpenDisplay(NULL);
        scr = DefaultScreen(dpy);
        win = DefaultRootWindow(dpy);
        scr = DefaultScreen(dpy);
        gc[0] = XCreateGC(dpy, win, 0, 0);
        gc[1] = XCreateGC(dpy, win, 0, 0);
        tone = WhitePixel(dpy, scr); /* = 0xFFFFFF; */
        XSetForeground(dpy, gc[0], tone);
        if ((fnptr = XLoadQueryFont(dpy, "9x15")) != NULL)
            XSetFont(dpy, gc[0], fnptr->fid);
    } /* Display block */

    { /* Window block */
        unsigned w, h; /* main window width & height */
        XSetWindowAttributes attr; /* window attributes structure */
        XGCValues gval; /* GC structure */
        unsigned long amask; /* window attributes mask */
        Window root = win; /* Display root window */
        XSizeHints hint; /* Geometry WM hints */
        Atom wdw[1]; /* WM delete window atom */
        if (argc < 2)
            argv[1] = "16x8";
        while (1) {
            if (maxisize(&r, argv[1]) == 0)
                maxisize(&r, argv[1] = "16x8");
            w = decent(&r);
            if ((h = reset(&r)) < DisplayHeight(dpy, scr))
                break;
            argv[1] = "16x8";
        } /* if */
        printf("Now: %s %s (RxN)\n", argv[0], argv[1]);
        fflush(stdout);
        amask = (CWOverrideRedirect | CWBackPixel);
        XGetGCValues(dpy, gc[1], GCBackground, &gval);
        attr.background_pixel = gval.background; /* = 0x0 */
        attr.override_redirect = False;
        win = XCreateWindow(dpy, root, 0, 0, w, h, 1, CopyFromParent,
                            InputOutput, CopyFromParent, amask, &attr);
        hint.flags = (PMinSize | PMaxSize);
        hint.min_width = hint.max_width = w;
        hint.min_height = hint.max_height = h;
        XSetNormalHints(dpy, win, &hint);
        XStoreName(dpy, win, "spiral");
        wdw[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(dpy, win, wdw, 1);
        XMapWindow(dpy, win);
    } /* window block */

    { /* Multi Block */

        unsigned long emask; /* window event mask */
        XEvent event; /* graphic event structure */
        int freeze = 0; /* window visibility stop state */
        unsigned delay = (1 << 18); /* multi delay period = 2^rate */
        int multi = (1 << 12); /* multi code */
        int count = 0; /* delay count */
        int g = 0; /* GC index */
        unsigned del;
        int z = 1;
        emask = (ExposureMask | KeyPressMask | FocusChangeMask |
                 VisibilityChangeMask);
        XSelectInput(dpy, win, emask);
        del = delay;
        while (multi != 0) { /* Async dispatch event with multic ground */
            event.type = 0;
            XCheckWindowEvent(dpy, win, emask, &event);
            switch (event.type) {
                case Expose:
                    redraw(&event, gc[0], &r, z);
                    break;
                case VisibilityNotify:
                    freeze = overlap(&event);
                    break;
                case FocusIn: //rep5355(dpy, AutoRepeatModeOff);
                    delay = del;
                    z = z * (-1);
                    break;
                case FocusOut: //rep5355(dpy, AutoRepeatModeOn);
                    delay = -32;
                    break;
                case KeyPress:
                    if ((multi = rapid(&event, delay)) > 0)
                        count = delay = multi;
                    break;
                default:
                    break;
            } /* switck */

            if ((multi < 0) || (freeze < 0)) /* Freeze display spiral */
                continue;
            if (count++ < delay) /* Delay display spiral */
                continue;
//count = 0; /* reset count to next delay */
            twist(dpy, win, gc[g], &r, z); /* 1 spiral arc step run g*/
            if ((count = reverse(&r)) < 2) /* spiral reverse & count reset */
                twist(dpy, win, gc[g = count], &r, z); /* return overpass */
        } /* while event */
    } /* multi block */

    { /* Exit block */
        rep5355(dpy, AutoRepeatModeOn);
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
        return (0);
    } /* exit block*/

} /* main */
