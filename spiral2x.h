/* 2 centred spiral header file */

/* Semi-Circle (pi) spiral structure */

typedef struct {
    int A; /* Arc's coordinate (0 t - 180 - 360) */
    int dA; /* Arc in(de)crement (+1 or -1) */
    XPoint c[2]; /* 2 Semi-circles centeres c[1].x - c[0].x = R0 */
    int Rm; /* Max spiral radius */
    int R; /* Semi-circle radius from 0 to Rm */
    int dR; /* radius in(de)crement (+R0 or -R0) */
} XPiAr2;

/* Spiral Implementation function */

int maxisize(XPiAr2 *, char *); /* check max spiral radius */
int reset(XPiAr2 *); /* Reset spiral from center & window height */
int decent(XPiAr2 *); /* xy-spiral centeres & window width */
int redraw(XEvent *, GC, XPiAr2 *, int); /* redraw spiral fragment */
int reverse(XPiAr2 *); /* reverse spiral direction */
int amod2pi(XPiAr2 *); /* Restrick arc from 0 to 2pi range */
int recent(XPiAr2 *); /* Change center for semi-circle */
int twist(Display *, Window, GC, XPiAr2 *, int); /* Rotate by spiral track */
int rep5355(Display *, int); /* On/Off autorepeat + or - key */
int rapid(XEvent *, int); /* Control to spiral temp & freeze or exit */
int overlap(XEvent *); /* check visibility status */
