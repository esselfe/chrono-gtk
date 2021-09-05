#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "chrono.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

Display *display;
char *display_name;
Screen *screen;
int screen_num;
int screen_depth;
XSetWindowAttributes wattr;
int winX = 200, winY = 200, winW = 180, winH = 20;
Window root_window, window;
GC gc;
XEvent xev;
XSizeHints wmsize;
XWMHints wmhint;
XTextProperty wname, iname;
int text_item_size;
XTextItem text_item;
char *window_name = PACKAGE_STRING,
	*icon_name = PACKAGE_NAME;
unsigned int borderless, sticky;

int xlibErrorFunc(Display *display, XErrorEvent *error) {
	printf("Error code %d: ", error->error_code);
	switch(error->error_code) {
	case BadRequest:	// 1
		printf("bad request code\n");
		break;
	case BadValue:	  // 2
		printf("int parameter out of range\n");
		break;
	case BadWindow:	 // 3
		printf("parameter not a Window\n");
		break;
	case BadPixmap:	 // 4
		printf("parameter not a Pixmap\n");
		break;
	case BadAtom:	   // 5
		printf("parameter not an Atom\n");
		break;
	case BadCursor:	 // 6
		printf("parameter not a Cursor\n");
		break;
	case BadFont:	   // 7
		printf("parameter not a Font\n");
		break;
	case BadMatch:	  // 8
		printf("parameter mismatch\n");
		break;
	case BadDrawable:   // 9
		printf("parameter not a Pixmap or Window\n");
		break;
	case BadAccess:	 // 10
		printf("depending on context:\n");
		break;
	case BadAlloc:	  // 11
		printf("insufficient resources\n");
		break;
	case BadColor:	  // 12
		printf("no such colormap\n");
		break;
	case BadGC:		 // 13
		printf("parameter not a GC\n");
		break;
	case BadIDChoice:   // 14
		printf("choice not in range or already used\n");
		break;
	case BadName:	   // 15
		printf("font or color name doesn't exist\n");
		break;
	case BadLength:	 // 16
		printf("request length incorrect\n");
		break;
	case BadImplementation: // 17
		printf("server is defective / bad implementation\n");
		break;
	}
	return 0;
}

int xlibIOErrorFunc(Display *dsp) {
	return 0;
}

void xlibKeysCheck(void) {
	if (XPending(display)) {
		XNextEvent(display, &xev);
		switch (xev.type) {
		case KeyPress:
			if (debug && use_x11)
				printf("keycode: %u\n", xev.xkey.keycode);

			if (xev.xkey.keycode == 40) // 'D'
				debug = !debug;
			else if (xev.xkey.keycode == 9 || xev.xkey.keycode == 24) // <escape> & 'q'
				endmainloop = 1;
			else if (xev.xkey.keycode == 27) { // 'r'
				gettimeofday(&tv0, NULL);
				tv_start = tv_prev = tv0;
				days = 0;
			}
			else if (xev.xkey.keycode == 33 || xev.xkey.keycode == 65) { // 'p' & <space>
				paused = !paused;
				if (paused) {
					gettimeofday(&tv0, NULL);
					tv_paused_start = tv0;
				}
				else {
					gettimeofday(&tv0, NULL);
					tv_paused_current = tv0;
//				  tv_paused += tv_paused_current - tv_paused_start;
					timersub(&tv_paused_current, &tv_paused_start, &tv_diff);
					timeradd(&tv_paused, &tv_diff, &tv_paused);
				}
			}
			else if (xev.xkey.keycode == 39) // 's'
				seconds_only = !seconds_only;
			break;
		case ButtonPress:
			if (xev.xbutton.button == 1) {
				paused = !paused;
				if (paused) {
					gettimeofday(&tv0, NULL);
					tv_paused_start = tv0;
				}
				else {
					gettimeofday(&tv_paused_current, NULL);
					timersub(&tv_paused_current, &tv_paused_start, &tv_diff);
					timeradd(&tv_paused, &tv_diff, &tv_paused);
				}
			}
			else if (xev.xbutton.button == 2)
				endmainloop = 1;
			else if (xev.xbutton.button == 3) {
				gettimeofday(&tv0, NULL);
				tv_start = tv0;
				tv_paused.tv_sec = 0;
				tv_paused.tv_usec = 0;
			}
			break;
		}

	}
}

void xlibUpdateTimeString(void) {
	memset(text_item.chars, 0, text_item_size);
	if (years)
		sprintf(text_item.chars, "%dy", years);
	if (months)
		sprintf(text_item.chars, "%s%dm", text_item.chars, months);
	if (days)
		sprintf(text_item.chars, "%s%dd", text_item.chars, days);
	if (seconds_only)
		sprintf(text_item.chars, "%s%02d:%02d:%02d", text_item.chars,
			tm0->tm_hour, tm0->tm_min, tm0->tm_sec);
	else {
		if (countdown)
			sprintf(text_item.chars, "%s%02d:%02d:%02d.%03lu", text_item.chars,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_countdown.tv_usec/1000);
		else
			sprintf(text_item.chars, "%s%02d:%02d:%02d.%03lu", text_item.chars,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_current.tv_usec/1000);
	}

	text_item.nchars = strlen(text_item.chars);
	XClearArea(display, window, 4, 4, winW-4, winH-4, True);
	XDrawText(display, window, gc, 4, 15, &text_item, 1);
}

void xlibWindowInit(void) {
	display = XOpenDisplay(display_name);
	if (display == NULL) {
		fprintf(stderr, "Cannot open a X display \"%s\"\n", display_name);
		exit(1);
	}

	Screen *screen = XDefaultScreenOfDisplay(display);
	int screen_num = XScreenNumberOfScreen(screen);
	int screen_depth = XDefaultDepthOfScreen(screen);
	root_window = XDefaultRootWindow(display);

	XSetWindowAttributes wattr;
	//wattr.background_pixel = BlackPixel(display, screen_num);
	//wattr.background_pixel = 0xaaaa00;
	wattr.background_pixel = 0x081018;
	wattr.event_mask = KeyPressMask | ButtonPressMask;
	wattr.cursor = None;
	window = XCreateWindow(display, RootWindow(display, screen_num),
		winX, winY, winW, winH, 2, screen_depth, InputOutput, DefaultVisual(display, screen_num),
		CWBackPixel | CWCursor | CWEventMask, &wattr);

	XSizeHints wmsize;
	wmsize.flags = USPosition | USSize;
	XSetWMNormalHints(display, window, &wmsize);
	
	XWMHints wmhint;
	wmhint.initial_state = NormalState;
	wmhint.flags = StateHint;
	XSetWMHints(display, window, &wmhint);

	XTextProperty wname, iname;
	char *window_name = PACKAGE_STRING,
		*icon_name = PACKAGE_NAME;
	XStringListToTextProperty(&window_name, 1, &wname);
	XSetWMName(display, window, &wname);
	XStringListToTextProperty(&icon_name, 1, &iname);
	XSetWMIconName(display, window, &iname);

	XFontStruct *xfont;
	xfont = XLoadQueryFont(display, "9x15");
	if (xfont == NULL) {
		fprintf(stderr, "chrono error: XLoadQueryFont(\"9x15\") cannot retrieve X11 font\n");
		exit(1);
	}

	XGCValues gcv;
	gcv.foreground = 0xa0a0a0;
	gcv.background = 0x000000;
	gcv.font = xfont->fid;
	gc = XCreateGC(display, window, GCForeground | GCBackground, &gcv);

	//text_item_size = strlen("10000 days 00:00:00.000")+1;
	text_item_size = 32;
	text_item.chars = (char *)malloc(text_item_size);
	memset(text_item.chars, 0, text_item_size);
	sprintf(text_item.chars, "00:00:00				");
	text_item.nchars = strlen(text_item.chars);
	text_item.delta = 0;
	text_item.font = xfont->fid;

	XMapWindow(display, window);

	if (borderless) {
		Atom window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
		long value = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
		XChangeProperty(display, window, window_type,
			XA_ATOM, 32, PropModeReplace, (unsigned char *)&value, 1);
	}
	if (sticky) {
		Atom wmStateSticky = XInternAtom(display, "_NET_WM_STATE_STICKY", 1);
		Atom wmNetWmState = XInternAtom(display, "_NET_WM_STATE", 1);
		XClientMessageEvent xclient;
		memset( &xclient, 0, sizeof (xclient) );
		xclient.type = ClientMessage;
		xclient.window = window;
		xclient.message_type = wmNetWmState;
		xclient.format = 32;
		xclient.data.l[0] = _NET_WM_STATE_ADD;
		xclient.data.l[1] = wmStateSticky;
		xclient.data.l[3] = 0;
		xclient.data.l[4] = 0;
		XSendEvent( display, root_window, False,
			SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *)&xclient );
	}
}

