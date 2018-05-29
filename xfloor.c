/* Chain Arm Ring background */
#define DEFTONE 0       /* default face tone */
#define R 27
#define G 42
#define B 56
#define EXT 9

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <stdio.h>

#define WID 50     /* repeat element width */
#define HEI 50    /* repeat element height */

typedef struct {
    char ch[1];
    XPoint Coord;
} Liter;

static unsigned long palette[5];
static int depth;                      /* bit per color pixel */
static int scr;                        /* screen number */
static int RootWid, RootHei;
static Display *dpy;     /* X Display */
static GC gc[3];         /* Graphic Context */
static Window root;      /* Main root window */
static Window desk;
int X0;         /* main root wintow x-location */
int Y0;         /* main root wintow y-location */
int Nbox = 0;
static unsigned NX = 10;   /* Default Boxes' number in each row */
static unsigned NY = 10;    /* Default Row's number in game desk */
static int BW, BH;
static Window *wind;      /* NY row window  box containe array  */
//================colorite====================================		//СОЗДАНИЕ ПАЛИТРЫ   palette[i]-цвет
int colorite(Display *dpy) {
    Colormap cmap; /* color map */
    XColor rgb;    /* Structure for color allocation */
    int i;         /* color index */
    static char *spector[] = {
            "#ffffff", /* white */
            "#ff0000", /* red */
            "green", /* green */
            "#0000ff", /* blue */
            "#000000"  /* black */
    }; /* color spector */
    cmap = DefaultColormap(dpy, DefaultScreen(dpy));
    for (i = 0; i < 5; i++) {
        XParseColor(dpy, cmap, spector[i], &rgb);        //ИЩЕМ RGB ПО НАЗВАНИЮ
        XAllocColor(dpy, cmap, &rgb);
        palette[i] = rgb.pixel;
    } /* for */
    return (0);
} /* colorite */
//==========Create main window==========================================

Window CreateWin(Display *dpy, Liter **L, Liter **L2) {                                         /* return Window id */
    XSetWindowAttributes attr;                         /* window attributes */
    int i, j;
    int x = WID, y = 0;                                            /* window location */
    gc[2] = XCreateGC(dpy, DefaultRootWindow(dpy), 0, 0);
    XSetForeground(dpy, gc[2], palette[0]);
    attr.override_redirect = True;
    attr.event_mask = (ButtonPressMask | ButtonReleaseMask | KeyPressMask | ExposureMask | ButtonMotionMask);
    attr.background_pixel = palette[0];
    for (i = 0; i < Nbox; i++) {
        wind[i] = XCreateWindow(dpy, root, x, y, WID, HEI, 1, depth,
                                InputOutput, CopyFromParent,
                                (CWOverrideRedirect | CWBackPixel | CWEventMask), &attr);

        L[0][i].Coord.x = x;
        L[0][i].Coord.y = y;
        x += WID;
    }
    y += HEI;
    x = WID;
    for (j = 0; j < Nbox; i++, j++) {
        wind[i] = XCreateWindow(dpy, root, x, y, WID, HEI, 1, depth,
                                InputOutput, CopyFromParent,
                                (CWOverrideRedirect | CWBackPixel | CWEventMask), &attr);

        L2[0][j].Coord.x = x;
        L2[0][j].Coord.y = y;
        x += WID;
    }
    XMapWindow(dpy, root);
    XMapSubwindows(dpy, root);
    XStoreName(dpy, root, "xpat");
    return (root);
}

//==========alloc==========================================

int alloc() {
    void *r;                /* row array pointer */
    r = calloc(Nbox*2, sizeof(unsigned long));
//printf("%d\n",Nbox);
    wind = (Window *) r;
    return (0);
}


//=========tr=================================================
int tr(char *str, Liter **L) {
    int i = 0;
    while (str[i] != '\0')
        i++;
    Nbox = i;
    *L = (Liter *) calloc(i, sizeof(Liter));
    i = 0;
    while (str[i] != '\0') {
        L[0][i].ch[0] = str[i];
        printf("%c", L[0][i].ch[0]);
        i++;
    }
}

//=========rootWnd=================================================
int rootWnd(Display *dpy) {
    XSetWindowAttributes attr;                         /* window attributes */
    XSizeHints hint;                         /* connect with Window Maneger */
    int i;
    int x = 0, y = 0;                                            /* window location */                                          /* window size */
    gc[0] = DefaultGC(dpy, scr);
    attr.override_redirect = False;           /* set window maneger control */
    attr.background_pixel = palette[DEFTONE];           /* background color */
    attr.event_mask = (ButtonPressMask | KeyPressMask);
    scr = DefaultScreen(dpy);
    root = XCreateWindow(dpy, DefaultRootWindow(dpy), x, y, WID * (Nbox + 2), HEI*2,            //Основное окно
                         1, depth, InputOutput, CopyFromParent,
                         (CWOverrideRedirect | CWBackPixel | CWEventMask), &attr);
    hint.flags = (PMinSize | PPosition | PMaxSize | PResizeInc);
    hint.min_width = hint.max_width = WID * (Nbox + 2);
    hint.min_height = hint.max_height = HEI*2;
    hint.x = x;
    hint.y = y;
    XSetNormalHints(dpy, root, &hint);
}

//=========findWinNum====================================
int findWinNum(Window win, int Nbox) {
    for (int i = 0; i < Nbox*2; i++)
        if (wind[i] == win)
            return (i);
}

//==========wrtLit========================================
int wrtLit(Liter **L, Liter **L2) {
    int i, j;
    for (i = 0; i < Nbox; i++) {
        const char *c = L[0][i].ch;
        XDrawString(dpy, wind[i], gc[0], WID / 2, HEI / 2, c, 1);
    }
    for (j = 0; j < Nbox; i++, j++) {
        const char *c = L2[0][j].ch;
        XDrawString(dpy, wind[i], gc[0], WID / 2, HEI / 2, c, 1);
    }
}


int main(int argc, char *argv[]) {
    static Liter *L1;
    static Liter *L2;
    int num;
    int perecritie;
    int startX, startY;
    XFontStruct *fnptr;
    int motion = 0;
    char dopolnenie[sizeof(argv[1])];
    for (int i = 0; i < sizeof(argv[1]); i++) {
        if (argv[1][i] == '0')
            dopolnenie[i] = '1';
        else
            dopolnenie[i] = '0';
    }
    tr(argv[1], &L1);
    tr(dopolnenie, &L2);
    XEvent event;                   /* Event structure */
    unsigned int done = 0;          /* exit code */
    dpy = XOpenDisplay(NULL);
    scr = DefaultScreen(dpy);
    alloc();
    depth = DefaultDepth(dpy, scr);
    colorite(dpy);
    rootWnd(dpy);
    desk = CreateWin(dpy, &L1, &L2);

    puts("1");
    if ((fnptr = XLoadQueryFont(dpy, "9x15")) != NULL)
        XSetFont(dpy, gc[2], fnptr->fid);
    while (done == 0) {
        XNextEvent(dpy, &event);
        wrtLit(&L1, &L2);
        switch (event.type) {
            case ButtonRelease:
                if (perecritie == 1) {
                    //num=findWinNum(event.xbutton.window,argc-1);
                    //goBack(dpy,&event,num,X0,Y0);
                }
                break;
            case ButtonPress:
                num = findWinNum(event.xbutton.window, sizeof(argv[1]));
                printf("%d\n", num);
                if (num < sizeof(argv[1])) {
                    XClearWindow(dpy, event.xbutton.window);
                    XClearWindow(dpy, event.xbutton.window + sizeof(argv[1]));
                    if (L1[num].ch[0] == '1') {
                        L1[num].ch[0] = '0';
                        L2[num].ch[0] = '1';
                    }
                    else {
                        L1[num].ch[0] = '1';
                        L2[num].ch[0] = '0';
                    }
                } else {
                    XClearWindow(dpy, event.xbutton.window);
                    XClearWindow(dpy, event.xbutton.window - sizeof(argv[1]));
                    num -= sizeof(argv[1]);
                    if (L2[num].ch[0] == '1') {
                        L2[num].ch[0] = '0';
                        L1[num].ch[0] = '1';
                    }
                    else {
                        L2[num].ch[0] = '1';
                        L1[num].ch[0] = '0';
                    }
                }
                //XGrabPointer(dpy, wind[num], False, (ButtonReleaseMask |ButtonPressMask| ButtonMotionMask),GrabModeAsync, GrabModeAsync, wind[num], None, CurrentTime);
                //XWarpPointer(dpy, None, root, 0, 0, 0, 0, Sizes[num].Coord.x+Sizes[num].wid/2, Sizes[num].Coord.y+Sizes[num].hei/2);
                //startX=event.xbutton.x_root;
                //startY=event.xbutton.y_root;
                //X0=Sizes[num].Coord.x;
                //Y0=Sizes[num].Coord.y;
                break;
            case KeyPress:
            case KeyRelease:
                if (event.xkey.keycode == EXT)
                    done = 1;
                break;
            case MotionNotify:
                //motion=1;
                //num=findWinNum(event.xmotion.window,argc-1);
                //perecritie=booksir(dpy,&event, argc-1,num,startX,startY);
                //startX=event.xmotion.x_root;
                //startY=event.xmotion.y_root;
                break;
            default:
                break;
        }
    }

    XDestroySubwindows(dpy, root);
    XDestroyWindow(dpy, root);
    XCloseDisplay(dpy);
    return (done);
} 






