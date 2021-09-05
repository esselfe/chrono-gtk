#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include "chrono.h"

#include <gtk/gtk.h>

GtkTextBuffer *buffer;

void gWindowInit(int *argc, char **argv) {
	printf("gWindowInit() started\n");

//	buffer = gtk_text_buffer_new(NULL);

	gtk_init(argc, &argv);

	GtkWidget *mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwin), "chrono 0.3.0");
	gtk_window_set_default_size(GTK_WINDOW(mainwin), 200, 50);

	GtkWidget *text_view = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(mainwin), text_view);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	char *text = malloc(1024);
	if (seconds_only)
		sprintf(text, "0y0m0d 00:00:00");
	else
		sprintf(text, "0y0m0d 00:00:00.000");
	gtk_text_buffer_set_text(buffer, text, -1);

	g_signal_connect(G_OBJECT(mainwin), "destroy",
	  G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(mainwin);

	guint timeoutID;
	if (seconds_only)
		timeoutID = g_timeout_add(1000, gUpdate, NULL);
	else
		timeoutID = g_timeout_add(1, gUpdate, NULL);

	gettimeofday(&tv_start, NULL);

	gtk_main();

	exit(0);

	return;
}

int gUpdate(void *argp){
	char str[1024];
	memset(str, 0, 1024);
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
//				else
//					break;
		}
		tm0 = gmtime(&tv_countdown.tv_sec);
	}
	else
		tm0 = gmtime(&tv_current.tv_sec);

	days = tm0->tm_mday - 1;
	months = tm0->tm_mon;
	years = tm0->tm_year - 70;

	if (seconds_only) {
		sprintf(str, "%uy%um%ud %02d:%02d:%02d", years, months, days,
			tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_current.tv_usec/1000);
	}
	else {
		if (countdown) {
			sprintf(str, "%uy%um%ud %02d:%02d:%02d.%03ld", years, months, days,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_countdown.tv_usec/1000);
		}
		else {
			sprintf(str, "%uy%um%ud %02d:%02d:%02d.%03ld", years, months, days,
				tm0->tm_hour, tm0->tm_min, tm0->tm_sec, tv_current.tv_usec/1000);
		}
	}
	gtk_text_buffer_set_text(buffer, str, -1);

	return 1;
}

