#ifndef CHRONO_H
#define CHRONO_H 1

#include "config.h"
#include <time.h>
#include <sys/time.h>

extern unsigned int use_x11;
extern unsigned int test, debug, endmainloop, paused, seconds_only;
extern unsigned int days, months, years;
extern unsigned int countdown_repeat, countdown;
extern struct timeval tv0, tv_countdown, tv_countdown_restore,
    tv_start, tv_current, tv_diff, tv_prev,
    tv_paused, tv_paused_current, tv_paused_start;
extern struct tm *tm0;

/* from chrono.c */
unsigned int ParseTimeToSeconds(char *timestr);

/* from test.c */
extern unsigned int test;
void Test(void);

#define HAVE_GTK 1
#ifdef HAVE_GTK
#include <gtk/gtk.h>
/* from gtk.c */
extern GtkWidget *mainwin;
extern GtkWidget *text_view;
extern GtkTextBuffer *buffer;
extern unsigned int gtk_has_init;

void gWindowInit(int *argc, char **argv);
#endif

/* from xlib.c */
#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern Display *display;
extern char *display_name;
extern Screen *screen;
extern int screen_num;
extern int screen_depth;
extern XSetWindowAttributes wattr;
extern int winX, winY, winW, winH;
extern Window root_window, window;
extern GC gc;
extern XEvent xev;
extern XSizeHints wmsize;
extern XWMHints wmhint;
extern XTextProperty wname, iname;
extern int text_item_size;
extern XTextItem text_item;
extern char *window_name, *icon_name;
extern unsigned int borderless, sticky;
#define _NET_WM_STATE_REMOVE        0	// remove/unset property
#define _NET_WM_STATE_ADD           1	// add/set property
#define _NET_WM_STATE_TOGGLE        2	// toggle property

int xlibErrorFunc(Display *dsp, XErrorEvent *error);
int xlibIOErrorFunc(Display *dsp);
void xlibKeysCheck(void);
void xlibUpdateTimeString(void);
void xlibWindowInit(void);
#endif

#ifdef HAVE_NCURSES
/* from ncurses.c */
void *ncursesKeysThreadFunc(void *ptr);
void ncursesUpdateTimeString(void);
#endif

#endif /* CHRONO_H */
