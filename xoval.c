# 3 "xoval.c"
#include <X11/Xlib.h>
#include <math.h>
#include "xsort.h"



typedef XRectangle XFig;



#define REFIG(F) (F->rec)

#define EXTRAFILL 1

#define FIXEDFILL 0



#define ISFILL(t) ((t) > 0 ? EXTRAFILL : FIXEDFILL)



int radical(int s) {
int r, R, r2;
for(R = r = 0, r2 = 0; r2 < s; r = R++)
 r2 = R*R;
if((s - r*r) > (r2 - s))
  r = R;
return(r);
}



int fragon(XFragment* ff, int x, int y, int t) {
XFig* f = REFIG(ff);
int dr;
int d[2];
int d2;
int c;
int c2;
int r;
int a = (f->width)/2;
int b = (f->height)/2;
XPoint p[2];
if((x < (f->x - 1)) || (x > (f->x + f->width + 1)) ||
   (y < (f->y - 1)) || (y > (f->y + f->height + 1)))
   return(0);
if((c2 = (a*a - b*b)) < 0)
  c2 = -c2;
c = radical(c2);
if(f->width > f->height) {
  p[0].x = f->x + a - c;
  p[1].x = f->x + a + c;
  p[0].y = p[1].y = f->y + b;
  r = f->width;
}
else {
  p[0].y = f->y + b - c;
  p[1].y = f->y + b + c;
  p[0].x = p[1].x = f->x + a;
  r = f->height;
}
d2 = (p[0].x - x)*(p[0].x - x) + (p[0].y - y)*(p[0].y - y);
d[0] = radical(d2);
d2 = (p[1].x - x)*(p[1].x - x) + (p[1].y - y)*(p[1].y - y);
d[1] = radical(d2);
dr = (d[0] + d[1] - r);
if(dr > 2)
  return(0);
if(t < 0)
  return(-t);
if(ISFILL(t) > 0)
  return(1);
if(dr < -4)
  return(0);
return(1);
}



int fragover(XFragment* ff1, XFragment* ff2) {
XFig* f1 = REFIG(ff1);
XFig* f2 = REFIG(ff2);
XSegment s1, s2;
XSegment r;
int x, y;
s1.x1 = f1->x; s1.x2 = f1->x + f1->width;
s1.y1 = f1->y; s1.y2 = f1->y + f1->height;
s2.x1 = f2->x; s2.x2 = f2->x + f2->width;
s2.y1 = f2->y; s2.y2 = f2->y + f2->height;
if((s1.x1 > s2.x2) || (s2.x1 > s1.x2) ||
   (s1.y1 > s2.y2) || (s2.y1 > s1.y2))
  return(0);
r.x1 = (s1.x1 > s2.x1) ? s1.x1 : s2.x1;
r.y1 = (s1.y1 > s2.y1) ? s1.y1 : s2.y1;
r.x2 = (s1.x2 < s2.x2) ? s1.x2 : s2.x2;
r.y2 = (s1.y2 < s2.y2) ? s1.y2 : s2.y2;
r.x1 -= 2; r.y1 -= 2; r.x2 += 2; r.y2 += 2;
for(y = r.y1; y < r.y2; y++)
  for(x = r.x1; x < r.x2; x++)
    if(fragon(ff1, x, y, -1) > 0)
      if(fragon(ff2, x, y, -1) > 0)
        return(1);
return(0);
}



int difrag(XFragment* ff1, XFragment* ff2) {
XFig* f1 = REFIG(ff1);
XFig* f2 = REFIG(ff2);
return(f1->width * f1->height - f2->width * f2->height);
}



int fragextra(int (*fe[])()) {
//return((*fe[GRP2FRAG])());
return((*fe[DIFFFRAG])());
}



int fragsize(XFragment* ff) {
XFig* f = REFIG(ff);
return(f->width * f->height);
}



int tinyfrag(XFragment* ff) {
XFig* f = REFIG(ff);
if((f->width < 8) || (f->height < 8))
  return(1);
return(0);
}



int frag0(XFragment* ff, int x, int y) {
XFig* f = REFIG(ff);
f->x = x; f->y = y;
f->width = f->height = 0;
//f->angle1 = (0*64);
//f->angle2 = (360*64);
return(0);
}



int fragvar(XFragment* ff, int x, int y) {
XFig* f = REFIG(ff);
static int fx0, fy0;
int dx, dy;
int rx, ry;
int ax, ay;
if(f->width == 0)
  fx0 = f->x;
if(f->height == 0)
  fy0 = f->y;
if((dx = x - fx0) < 0)
  dx = -dx;
if((dy = y - fy0) < 0)
  dy = -dy;
ax = (int) (M_SQRT2 * dx);
ay = (int) (M_SQRT2 * dy);
rx = fx0 - ax; ry = fy0 - ay;
if((rx < 0) || (ry < 0))
  return(-1);
f->x = rx; f->y = ry;
f->width = 2*ax;
f->height = 2*ay;
return(0);
}



int fragmaxix(XFragment* ff) {
XFig* f = REFIG(ff);
return(f->x + f->width);
}



int fragmaxiy(XFragment* ff) {
XFig* f = REFIG(ff);
return(f->y + f->height);
}



int XContour(Display* dpy, Window win, GC gc, XFragment* ff) {
XFig* f = REFIG(ff);
XDrawRectangles(dpy, win, gc, f, 1);
return(0);
}



int XFix(Display* dpy, Window win, GC gc, XFragment* ff) {
XFig* f = REFIG(ff);
if(FIXEDFILL > 0)
  XFillRectangles(dpy, win, gc, f, 1);
XDrawRectangles(dpy, win, gc, f, 1);
return(0);
}



int XExtra(Display* dpy, Window win, GC gc, XFragment* ff) {
XFig* f = REFIG(ff);
if(EXTRAFILL > 0)
  XFillRectangles(dpy, win, gc, f, 1);
XDrawRectangles(dpy, win, gc, f, 1);
return(0);
}
