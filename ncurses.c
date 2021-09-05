#include "config.h"
#include <ncurses.h>
#include <sys/time.h>
#include "chrono.h"

void *ncursesKeysThreadFunc(void *ptr) {
	int c;
	while (!endmainloop) {
		c = getch();
		switch (c) {
		case 'D':
			debug = !debug;
			break;
		case 'p':
		case ' ':
			paused = !paused;
			if (paused) {
				gettimeofday(&tv0, NULL);
				tv_paused_start = tv0;
			}
			else {
				gettimeofday(&tv0, NULL);
				tv_paused_current = tv0;
//			  tv_paused += tv_paused_current - tv_paused_start;
				timersub(&tv_paused_current, &tv_paused_start, &tv_diff);
				timeradd(&tv_paused, &tv_diff, &tv_paused);
			}
			break;
		case 'q':
			endmainloop = 1;
			break;
		case 'r':
			gettimeofday(&tv0, NULL);
			tv_start = tv0;
			tv_paused.tv_sec = 0;
			tv_paused.tv_usec = 0;
			if (paused)
				tv_paused_start = tv0;
			break;
		case 's':
			seconds_only = !seconds_only;
			break;
		}
	}
	return NULL;
}

void ncursesUpdateTimeString(void) {
	printw("\r");
	if (years)
			printw("%dy", years);
	if (months)
			printw("%dm", months);
	if (days)
			printw("%dd", days);

	if (seconds_only)
		printw("%02d:%02d:%02d							",
			tm0->tm_hour, tm0->tm_min, tm0->tm_sec);
	else {
		if (countdown)
			printw("%02d:%02d:%02d.%03lu					  ",
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_countdown.tv_usec/1000);
		else
			printw("%02d:%02d:%02d.%03lu					  ",
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_current.tv_usec/1000);
	}
	refresh();
}

