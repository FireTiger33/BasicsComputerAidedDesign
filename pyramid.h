/* Paint pyramid graph face header file */

#define MB0 10                        /* extern face top number */
#define NF4 3                        /* 4 top inner face number */
#define NF5 8

#define NFACE (NF4+NF5)                  /* all inner face number (=4) */
#define NEDGE ((4*NF4+5*NF5+MB0)/2)        /* edge number (=8) */
/* define NEDGE ((MB0+3*NF3+4*NF4+...)/2) */
#define NVERT (NEDGE-(NFACE+1)+2)    /* vertex number (=5) by Euler */
#define NCOLOR 4        /* face color number */
#define NUNIT 16         /* unit cell number by X & Y */
#define EWIDTH 2        /* contour line width */
#define VDOT 8          /* vertex dot size */
#define DEFTONE 0       /* default face tone */

/* Vertex & Edge type redeclaration */
typedef XPoint XVertex;
typedef XSegment XEdge;

/* Face declaratiom */

typedef struct {
    XPoint* top;   /* face vertex location array */
    int Cn;        /* face vertex  number */
    int tone;      /* face tone color index */
    Region regi;   /* face space region */
} XFace; 

/* Polytop graph  declaration */
typedef struct {
    XVertex* vertex;    /* vertex array */
    XFace* face;       /* face array */
    XEdge* edge;    /* edge path array */
} XPolyGraph;

/* Graph display function declaration (pyramid2) */
int relink(XPolyGraph*);
GC congraph(Display*);
Window canvas(Display*);
int colorite(Display*);
int regraph(Display*, Window, GC, int);
int reset(Display*, Window, int);
int reface(Display*, Window, GC, int);

/* Graph geometry function declaration (pyramid1) */
int assoc(XPolyGraph*);
int resize(unsigned, unsigned);
int rescale(unsigned, unsigned);
int rebuild();
int retrace();
int reconf(unsigned, unsigned);
int zotone(int, int);

/* Control function declaration (pyramid0) */
int rekey(XEvent*);
int dispatch(Display*, Window, GC);
