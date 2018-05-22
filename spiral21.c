/* 2 centred spiral implementation */

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <string.h>
#include "spiral2x.h"
#include "math.h"

/* Check maximal (extern) semi-circle radius */

int maxisize(XPiAr2 *pr, char *R0xN) {
    int R0; /* Inner SemiRing Radiur */
    int N; /* Twist number */
    int empty; /* for x & y */
    XParseGeometry(R0xN, &empty, &empty, &R0, &N);
    if (((pr->dR = pr->R = R0) < 1) || (N < 1))
        N = R0 = 0; /* invoking defalts after rerun */
    return (pr->Rm = 2 * R0 * N); /* return maximal radius */
}

/* Compute Spiral center location */

int decent(XPiAr2 *pr) {
    int w = 2 * (pr->Rm + pr->R) + (8 + 8); /* R = R0 = dR now */
    pr->c[0].x = w / 2 - (pr->R / 2);
    pr->c[1].x = pr->c[0].x + pr->R; /* = w/2 + (pr->R/2); */
    pr->c[0].y = pr->c[1].y = w / 2 + 8;
    return (w); /* return frame width for window */
}

/* Reset spiral from center */

int reset(XPiAr2 *pr) {
    pr->A = (0 * 64);
    pr->dA = (1 * 64);
    pr->R = 0;
    if (pr->dR < 0)
        pr->dR = -pr->dR;
    return (2 * (pr->c[0].y)); /* return frame height */
}

/* redraw exposed spiral fragment from center */

int redraw(XEvent *ev, GC gc, XPiAr2 *pr, int z) {
    int y; /* ALT-X script y-location */
    XPiAr2 r; /* spiral copy */
    static XRectangle clip[32]; /* clip rectangles buffer */
    static int n = 0; /* clip rectangles number */
    clip[n].x = ev->xexpose.x;
    clip[n].y = ev->xexpose.y;
    clip[n].width = ev->xexpose.width;
    clip[n].height = ev->xexpose.height;
    n++;
    if ((ev->xexpose.count > 0) && (n < 32))
        return (0);
    XSetClipRectangles(ev->xexpose.display, gc, 0, 0, clip, n, Unsorted);
    r = *pr; /* = memcpy(&r, pr, (sizeof(XPiAr2))); */
    y = reset(&r) - 8;
    while (twist(ev->xexpose.display, ev->xexpose.window, gc, &r, z) < pr->R);
    r.dA = (pr->A - r.A);
    twist(ev->xexpose.display, ev->xexpose.window, gc, &r, z);
    XSetClipMask(ev->xexpose.display, gc, None);
    return (n = 0);
} /* redraw */

/* Reverse spiral direction */

int reverse(XPiAr2 *pr) {
    int g = 2; /* spiral GC number */
    if ((pr->R > pr->Rm) || (pr->R == 0)) {
        pr->dR = -pr->dR;
        pr->dA = -pr->dA;
        g = (pr->dA > 0) ? 0 : 1;
    } /* if */
    return (g);
} /* reverse */

/* Restrick twist angle from 0 to 2pi range */

int amod2pi(XPiAr2 *pr) {
    pr->A += (pr->dA);
    if (pr->A == (360 * 64))
        return (pr->A = (0 * 64));
    if (pr->A == (0 * 64))
        pr->A = (360 * 64);
    return (pr->A);
} /* amod2pi */

/* Change center for semi-circle (semi-step) */

int recent(XPiAr2 *pr) {
    if ((pr->A % (180 * 64)) != 0)
        return (pr->A / (180 * 64));
    pr->R += pr->dR;
    return ((pr->A + pr->dA) / (180 * 64));
} /* recent */

/* 1 arc step by spiral track */

int twist(Display *dpy, Window win, GC gc, XPiAr2 *pr, int z) {
    int i = recent(pr);
    int x1 = 300 + 300 * cos((pr->A) * (3.14) * z);
    int y1 = 300 + 200 * sin((pr->A) * (3.14) * z);
    XClearWindow(dpy, win);
//XGCvalues.line_style=LineOnOffDash;
    //XDrawArc(dpy, win, gc, pr->c[i].x - pr->R, pr->c[i].y - pr->R, (2*pr->R), (2*pr->R), pr->A, pr->dA);
    XDrawArc(dpy, win, gc, -3, 300 - 200, 600, 400, 0, 360 * 64);
    XDrawLine(dpy, win, gc, 300, 300, 2 * 300 - x1, 2 * 300 - y1);
    XDrawLine(dpy, win, gc, 300, 300, x1, y1);
    XDrawString(dpy, win, gc, 6, 25, "Ctrl-E", 6);
    XFlush(dpy);
    amod2pi(pr);
    return (pr->R);
} /* twist */

/* Turn On/Off autorepeat plus or minus key */

int rep5355(Display *dpy, int r) {
    XKeyboardControl kbval;
    kbval.key = XKeysymToKeycode(dpy, XK_KP_Add);
    kbval.auto_repeat_mode = r;
    XChangeKeyboardControl(dpy, (KBKey | KBAutoRepeatMode), &kbval);
    kbval.key = XKeysymToKeycode(dpy, XK_KP_Subtract);
    XChangeKeyboardControl(dpy, (KBKey | KBAutoRepeatMode), &kbval);
    return (r);
} /* rep5355 */

/* Keyboard Control to ac(de)celerate spiral rapid or exit */

int rapid(XEvent *ev, int t) {
    char sym[1]; /* symbol ASCII code */
    KeySym code[1]; /* key pressed symbol X code */
    XLookupString((XKeyEvent *) ev, NULL, 0, code, NULL);
    switch (code[0]) {
        case XK_plus: /* accelerate tempo or decrease delay */
        case XK_KP_Add:
            if (t > 1)
                t >>= 1;
            break;
        case XK_minus: /* decerarate tempo or  increase delay */
        case XK_KP_Subtract:
            if (t < (1 << 30))
                t <<= 1;
            break;
        case XK_space:
            t = (-32); /* freeze spiral */
            break;
        case XK_e:
            if (ev->xkey.state & ControlMask)
                t = 0;  /* Ctrl+E to exit */
            break;
        default:
            break; /* unfreeze spiral */
    } /* switch */
    return (t);
} /* rapid */

/* Check Main Window Visibility to (un)freeze */

int overlap(XEvent *ev) {
    if (ev->xvisibility.state != VisibilityUnobscured)
        return (-32);
    return (0);
} /* overlap */
