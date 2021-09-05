/* 190618
chrono is a Linux terminal/ncurses and/or X11 based chronometer with
optional millisecond precision, repeateable countdown, pause and reset function
*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <pthread.h>
#include <getopt.h>
#include <assert.h>
#include "chrono.h"

#define HAVE_GTK 1
#ifdef HAVE_GTK
#include <gtk/gtk.h>
#endif

const char *chrono_version_string = VERSION;

#ifdef HAVE_NCURSES
#include <ncursesw/ncurses.h>
#include <pthread.h>
#endif

#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif

unsigned int use_x11;
//#ifdef HAVE_GTK
unsigned int use_gtk;
//#endif
unsigned int debug, endmainloop, paused, seconds_only;
unsigned int days, months, years;
unsigned int countdown_repeat, countdown;
struct timeval tv0, tv_countdown, tv_countdown_restore,
	tv_start, tv_current, tv_diff, tv_prev,
	tv_paused, tv_paused_current, tv_paused_start;
struct tm *tm0;

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
#ifdef HAVE_X11
	{"borderless", no_argument, NULL, 'b'},
#endif
	{"countdown", required_argument, NULL, 'c'},
	{"debug", no_argument, NULL, 'D'},
	{"display", required_argument, NULL, 'd'},
//#ifdef HAVE_GTK
	{"gtk", no_argument, NULL, 'g'},
//#endif
	{"repeat", no_argument, NULL, 'r'},
	{"seconds", no_argument, NULL, 's'},
	{"test", no_argument, NULL, 't'},
	{"version", no_argument, NULL, 'V'},
#if defined HAVE_X11 && defined HAVE_NCURSES
	{"X11", no_argument, NULL, 'x'},
#endif
#ifdef HAVE_X11
	{"sticky", no_argument, NULL, 'S'},
	{"position-x", required_argument, NULL, 'X'},
	{"position-y", required_argument, NULL, 'Y'},
	{"width", required_argument, NULL, 'W'},
	{"height", required_argument, NULL, 'H'},
#endif
	{NULL, 0, NULL, 0}
};
#if defined HAVE_X11 && defined HAVE_NCURSES && defined HAVE_GTK
static const char *short_options = "hbc:DdgrSstVxX:Y:W:H:";
#elif defined HAVE_X11 && defined HAVE_NCURSES && !defined HAVE_GTK
static const char *short_options = "hbc:Dd:rSstVxX:Y:W:H:";
#elif defined HAVE_X11 && !defined HAVE_NCURSES && defined HAVE_GTK
static const char *short_options = "hbc:DdgrSstVX:Y:W:H:";
#elif defined HAVE_X11 && !defined HAVE_NCURSES && defined !HAVE_GTK
static const char *short_options = "hbc:Dd:rSstVX:Y:W:H:";
#elif !defined HAVE_X11 && !defined HAVE_NCURSES && defined HAVE_GTK
static const char *short_options = "hbc:Dd:rSstVX:Y:W:H:";
#elif !defined HAVE_X11 && !defined HAVE_GTK
static const char *short_options = "hc:Dd:rSstV";
#else
static const char *short_options = "hbc:DrstV";
#endif

void ShowHelp(void) {
	printf("chrono options:\n"
"\t-h, --help		Show this help message\n"
"\t-c, --countdown  Countdown using one of these formats: 15, 2:30 or 1:15:05\n"
"\t-D, --debug      Show detailed debug informations\n"
"\t-d, --display    Use specified X11 display name (ie \":0.0\" or \":1.0\")\n"
#ifdef HAVE_GTK
"\t-g, --gtk        Use gtk as GUI\n"
#endif
"\t-r, --repeat     Restart countdown once reaching 0\n"
#ifdef HAVE_X11
"\t-S, --sticky     Show window on all desktops\n"
#endif
"\t-s, --seconds	Display seconds instead of milliseconds\n"
"\t-t, --test       Run correctness tests and exit\n"
"\t-V, --version	Show the program version and exit\n"
#if HAVE_X11 && HAVE_NCURSES
"\t-x, --X11        Use XOrg X11 Xlib as GUI\n"
#endif
#ifdef HAVE_X11
"\t-X, --position-x Set the horizontal window position\n"
"\t-Y, --position-y Set the vertical window position\n"
"\t-W, --width      Set window width\n"
"\t-H, --height     Set window height\n"
#endif
"Once chrono has started, press 'q' to quit, 'r' to reset and 'p' or <space> to pause\n");
}

void *ThreadFunc(void *argp){
	char str[1024];
	memset(str, 0, 1024);
	gettimeofday(&tv_start, NULL);
    while (!endmainloop) {
		if (!gtk_has_init) {
			usleep(100000);
			continue;
		}
        gettimeofday(&tv0, NULL);
        timersub(&tv0, &tv_start, &tv_diff);
        timersub(&tv_diff, &tv_paused, &tv_current);
	    if (countdown) {
	        timersub(&tv_current, &tv_prev, &tv_diff);
	        tv_prev = tv_current;
	        timersub(&tv_countdown, &tv_diff, &tv_countdown);
	        if (tv_countdown.tv_sec <= 0 && tv_countdown.tv_usec <= 2000) {
	            if (countdown_repeat) {
	                tv_countdown = tv_countdown_restore;
	                tv_paused.tv_sec = 0;
	                tv_paused.tv_usec = 0;
	            }
	            else
	                break;
	        }
	        tm0 = gmtime(&tv_countdown.tv_sec);
	    }
	    else
	        tm0 = gmtime(&tv_current.tv_sec);

		days = tm0->tm_mday - 1;
	    months = tm0->tm_mon;
	    years = tm0->tm_year - 70;

		if (countdown) {
			sprintf(str, "%uy%um%ud%02d:%02d:%02d.%03ld", years, months, days,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_current.tv_usec/1000);
		}
		else {
			sprintf(str, "%uy%um%ud%02d:%02d:%02d.%03ld", years, months, days,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_countdown.tv_usec/1000);
		}
		//gtk_window_activate_focus(mainwin);
		gtk_window_get_focus(mainwin);
		gtk_text_buffer_set_text(buffer, str, -1);
	}


}

// timestr should be like "01:45:30" or "2:15" or "10"
unsigned int ParseTimeToSeconds(char *timestr) {
	unsigned int cnt = strlen(timestr)-1, seconds_total, hours, minutes, seconds,
		doing_seconds = 1, doing_minutes = 0, doing_hours = 0, doing_days = 0,
		doing_months = 0, doing_years = 0;
	char *c = timestr, buffer[3];
	if (debug)
		printf("\"%s\" ", timestr);
	while (1) {
		buffer[2] = '\0';
		if (cnt == 0) {
			buffer[1] = '\0';
			buffer[0] = *c;
		}
		else {
			if (*(c+cnt-1) == ':') {
				buffer[1] = '\0';
				buffer[0] = *(c+cnt);
			}
			else {
				buffer[1] = *(c+cnt);
				buffer[0] = *(c+cnt-1);
			}
		}
		if (debug)
			printf("<%s>", buffer);

		if (doing_seconds) {
			seconds = atoi(buffer);
			seconds_total = seconds;
			doing_seconds = 0;
			doing_minutes = 1;
			if (debug)
				printf(" (%us) [%ut]", seconds, seconds_total);
		}
		else if (doing_minutes) {
			minutes = atoi(buffer);
			seconds_total += minutes * 60;
			doing_minutes = 0;
			doing_hours = 1;
			if (debug)
				printf(" (%um) [%ut]", minutes, seconds_total);
		}
		else if (doing_hours) {
			hours = atoi(buffer);
			seconds_total += hours * 60 * 60;
			doing_hours = 0;
			doing_days = 1;
			if (debug)
				printf(" (%uh) [%ut]", hours, seconds_total);
		}
		else if (doing_days) {
			days = atoi(buffer);
			seconds_total += days * 24 * 60 * 60;
			doing_days = 0;
			doing_months = 1;
			if (debug)
				printf(" (%ud) [%ut]", days, seconds_total);
		}
		else if (doing_months) {
			months = atoi(buffer);
			seconds_total += months * 31 * 24 * 60 * 60;
			doing_months = 0;
			doing_years = 1;
			if (debug)
				printf(" (%um) [%ut]", months, seconds_total);
		}
		else if (doing_years) {
			years = atoi(buffer);
			seconds_total += years * 365 * 24 * 60 * 60;
			doing_years = 0;
			if (debug)
				printf(" (%uy) [%ut]", years, seconds_total);
		}
		if (debug)
			printf(" #%u %u\n", cnt, seconds_total);

		if (cnt == 0 || cnt == 1) {
			if (debug)
				printf("break, cnt: %u\n", cnt);
			break;
		}
		else if (cnt == 2)
			cnt -= 2;
		else if (cnt >= 3) {
			if (*(c+cnt-2) == ':')
				cnt -= 3;
			else if (*(c+cnt-1) == ':')
				cnt -= 2;
		}
	}

	return seconds_total;
}

int main(int argc, char **argv) {
	int c;
	while (1) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1) break;

		switch (c) {
		case 'h':
			ShowHelp();
			exit(0);
#ifdef HAVE_X11
		case 'b':
			borderless = 1;
			break;
#endif
		case 'c':
			countdown = 1;
			tv_countdown.tv_sec = (long)ParseTimeToSeconds(optarg);
			if (tv_countdown.tv_sec <= 0)
				tv_countdown.tv_sec = 1;
			break;
		case 'D':
			debug = 1;
			break;
#ifdef HAVE_X11
		case 'd':
			display_name = (char *)malloc(strlen(optarg)+1);
			sprintf(display_name, "%s", optarg);
			break;
#endif
#ifdef HAVE_GTK
		case 'g':
			use_gtk = 1;
			break;
#endif
		case 'r':
			countdown_repeat = 1;
			break;
		case 's':
			seconds_only = 1;
			break;
		case 't':
			test = 1;
			debug = 1;
			break;
		case 'V':
			printf("chrono %s\n", chrono_version_string);
			exit(0);
#if defined HAVE_X11 && defined HAVE_NCURSES
		case 'x':
			use_x11 = 1;
			break;
#endif
#ifdef HAVE_X11
		case 'S':
			sticky = 1;
			break;
		case 'W':
			winW = atoi(optarg);
			break;
		case 'H':
			winH = atoi(optarg);
			break;
		case 'X':
			winX = atoi(optarg);
			break;
		case 'Y':
			winY = atoi(optarg);
			break;
#endif
		default:
			fprintf(stderr, "chrono error: Unknown option: %d:%c:\n", c, (char) c);
			break;
		}
	}

	if (test) {
		Test();
		exit(0);
	}
	
	if (countdown || countdown_repeat)
		tv_countdown_restore = tv_countdown;

#if defined HAVE_X11 && !defined HAVE_NCURSES
	use_x11 = 1;
#endif

#ifdef HAVE_X11
	if (use_x11) {
		XSetErrorHandler(xlibErrorFunc);
		XSetIOErrorHandler(xlibIOErrorFunc);
		xlibWindowInit();
	}
#endif

#ifdef HAVE_GTK
	if (use_gtk) {
		pthread_t gthr;
        pthread_attr_t gattr;
        pthread_attr_init(&gattr);
        pthread_attr_setdetachstate(&gattr, PTHREAD_CREATE_DETACHED);
        pthread_create(&gthr, &gattr, ThreadFunc, NULL);
        pthread_detach(gthr);
        pthread_attr_destroy(&gattr);
		gWindowInit(&argc, argv);
	}
#endif

#ifdef HAVE_NCURSES
	if (!use_x11) {
		initscr();
		noecho();
		pthread_t thr;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thr, &attr, ncursesKeysThreadFunc, NULL);
		pthread_detach(thr);
		pthread_attr_destroy(&attr);
	}
#endif
	
	gettimeofday(&tv_start, NULL);
	while (!endmainloop) {
#ifdef HAVE_X11
		if (use_x11)
			xlibKeysCheck();
#endif
		gettimeofday(&tv0, NULL);
		timersub(&tv0, &tv_start, &tv_diff);
		timersub(&tv_diff, &tv_paused, &tv_current);
		if (countdown) {
			timersub(&tv_current, &tv_prev, &tv_diff);
			tv_prev = tv_current;
			timersub(&tv_countdown, &tv_diff, &tv_countdown);
			if (tv_countdown.tv_sec <= 0 && tv_countdown.tv_usec <= 2000) {
				if (countdown_repeat) {
					tv_countdown = tv_countdown_restore;
					tv_paused.tv_sec = 0;
					tv_paused.tv_usec = 0;
				}
				else
					break;
			}
			tm0 = gmtime(&tv_countdown.tv_sec);
		}
		else
			tm0 = gmtime(&tv_current.tv_sec);

		days = tm0->tm_mday - 1;
		months = tm0->tm_mon;
		years = tm0->tm_year - 70;
		
		if (!paused) {
#ifdef HAVE_X11
			if (use_x11)
				xlibUpdateTimeString();
#endif
#ifdef HAVE_NCURSES
			if(!use_x11) {
				ncursesUpdateTimeString();
			}
#endif
		}
		if (seconds_only)
			usleep(100000);
		else
			usleep(1000);
	}

#ifdef HAVE_X11
	if (use_x11) {
		XUnmapWindow(display, window);
		XDestroyWindow(display, window);
		XCloseDisplay(display);
	}
#endif
#ifdef HAVE_NCURSES
	if (!use_x11)
		endwin();
#endif
	return 0;
}

