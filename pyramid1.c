/* Pyramid Graph configure functions */
/* Change retrace() & rebuild() code */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "pyramid.h"

/* Polytop Graph static space definition */

static XVertex vertex[NVERT];
static XFace face[(NFACE+1)];
static XEdge edge[NEDGE];

static XPoint face4[NF4][5]; /* 3-top faces top location */
static XPoint face5[NF5][6];
static XPoint scale;             /* scale (pixel/cell) for x & y */ 

/* Associate graph address with graph space */
int assoc(XPolyGraph* pg) {
    pg->vertex = vertex;
    pg->edge = edge;
    pg->face = face;
    return 0;
}

/* Check Resize window when configure event */
int resize(unsigned w, unsigned h) {
    static XRectangle bak = {0, 0, 0, 0 };
    if((bak.width == w) && (bak.height == h))
	    return 0;      /* remove window */
    bak.width = w; bak.height = h; 
    return NFACE;          /* resize window */
}

/* Check window scale when resize */
int rescale(unsigned w, unsigned h) {
    int x, y;      /* pixel for cell by x & y */
    x = w / NUNIT; y = h / NUNIT;
    if((scale.x == x) && (scale.y == y))
        return 0;    /* small resize without change scale */
    scale.x = x; scale.y = y;
    return NFACE;  /* change scale */
}

/* Rebuild graph configuration */
int rebuild() { /* depending on graph ! */
    static XPoint vconf[] = { /* vertex location in x,y cells */
        {8, 3},
        {4, 1}, 
        {12, 1}, 
        {15, 10},
        {8, 15},
        {1, 10},
        {5, 3},
        {11, 3},
        {13, 9},
        {8, 13},
        {3 ,9},
        {8, 5},
        {10, 7},
        {9, 9},
        {7, 9},
        {6, 7},
        {12, 6},
        {10, 11},
        {6, 11},
        {4, 6}
    };
    static int fconf4[NF4][(4+1)] = {    /* Vertex index */
        {6, 1, 2, 7, 6},
        {6, 1, 5, 10, 6},
        {3, 8, 7, 2, 3}
    };
    static int fconf5[NF5][6] = {
        {11, 0, 6, 19, 15, 11},
        {11, 0, 7, 16, 12, 11},
        {12, 16, 8, 17, 13, 12},
        {13, 17, 9, 18, 14, 13},
        {14, 15, 19, 10, 18, 14},
        {11, 12, 13, 14, 15, 11},
        {5, 10, 18, 9, 4, 5},
        {9, 4, 3, 8, 17, 9}
    };
    static int econf[NEDGE][2] = { /* 2 Vertex index for each edge */
        {1, 2},
        {1, 6},
        {1, 5},
        {2, 3},           
        {2, 7},
        {3, 4},
        {3, 8},
        {4, 9},
        {4, 5},
        {5, 10},
        {6, 0},
        {6, 19},
        {7, 0},
        {7, 16},
        {8, 16},
        {8, 17},
        {9, 17},
        {9, 18},
        {10, 18},
        {10, 19},
        {11, 0},
        {11, 15},
        {11, 12},
        {12, 16},
        {12, 13},
        {13, 17},
        {13, 14},
        {14, 15},
        {14, 18},
        {15, 19}
    };
    int i, j;                          /* vertex, edge, face index */
    for(i=0; i < NVERT; i++) {    /* compute vertex pixel location */
        vertex[i].x = scale.x * vconf[i].x;
        vertex[i].y = scale.y * vconf[i].y;
    }
    for(i=0; i < NEDGE; i++) {   /* vertex pixel location for edge */
        edge[i].x1 = vertex[econf[i][0]].x;
        edge[i].y1 = vertex[econf[i][0]].y;
        edge[i].x2 = vertex[econf[i][1]].x;
        edge[i].y2 = vertex[econf[i][1]].y;
    }
    for(i=0; i < NF4; i++)                /* vertex pixel location */ 
        for(j=0; j<5; j++) {                  /* for 3-top faces */    
            face4[i][j].x = vertex[fconf4[i][j]].x;
            face4[i][j].y = vertex[fconf4[i][j]].y;
    }
    for(i=0; i < NF5; i++)
        for(j=0; j < 6; j++) {
            face5[i][j].x = vertex[fconf5[i][j]].x;
            face5[i][j].y = vertex[fconf5[i][j]].y;
        }
/* for(i=0; i < NF4; i++) */
/*   for(j=0; j<(4+1); j++) { ... } & etc */

    return 0;
}

/* Trace face array to join all n-face array */
int retrace() { /* depending on graph ! */
    int i=0;  /* total face index */
    int j;    /* n-top face index */
    for(j=0; j<NF4; j++, i++) {    /* fix 3-top faces in face array */
        face[i].top = face4[j];       /* fix 3-top face array address */
        face[i].Cn = 4;                /* fix 3-top face top number=3 */
        face[i].tone = DEFTONE;        /* set face default tone color */
        face[i].regi = XCreateRegion();      /* Empty region for face */
    }
    for(j=0; j<NF5; j++, i++) {
        face[i].top = face5[j];
        face[i].Cn = 5;
        face[i].tone = DEFTONE;
        face[i].regi = XCreateRegion();
    }
/* for(j=0; j<NF4; j++, i++) { ... } & etc. */

    face[i].tone = DEFTONE;  /* store extern face default tone color */  
    return 0;
}

/* Reconfigure graph when window resize & rescale */
int reconf(unsigned w, unsigned h) {
    if(resize(w, h) == 0)
        return 0;
    if(rescale(w, h) != 0)
        rebuild();
    return NFACE;
}

/* Ident face by inside point to repaint */
int zotone(int x, int y) {
    static XPoint past = {0, 0}; /* past scale */
    int f=0;                     /* face index */
    if((past.x == scale.x) && (past.y == scale.y)) /* Scale control */
        f = NFACE;      /* when no change scale */
    for( ; f < NFACE; f++) {          /* New regional zone for face */
        XDestroyRegion(face[f].regi);
        face[f].regi = XPolygonRegion(face[f].top, face[f].Cn, 0);
    }
    past.x = scale.x; past.y = scale.y;              /* Store scale */
    for(f=0; f < NFACE; f++)   /* find face with (x,y) inside */
        if(XPointInRegion(face[f].regi, x, y) == True)
            break;
    face[f].tone = (face[f].tone + 1) % NCOLOR;    /* new face tone */
    return f;                   /* return pointed face for repaint */
}
