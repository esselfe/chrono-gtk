#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include <gtk/gtk.h>

GtkWidget *mainwin;
GtkWidget *text_view;
GtkTextBuffer *buffer;
unsigned int gtk_has_init;

void gWindowInit(int *argc, char **argv) {
	printf("gWindowInit() started\n");

//	buffer = gtk_text_buffer_new(NULL);

	gtk_init(argc, &argv);

	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwin), "chrono 0.3.0");
	gtk_window_set_default_size(GTK_WINDOW(mainwin), 200, 50);

	text_view = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(mainwin), text_view);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	char *text = malloc(1024);
	sprintf(text, "0:00:000");
	gtk_text_buffer_set_text(buffer, text, -1);

	g_signal_connect(G_OBJECT(mainwin), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(mainwin);

	gtk_has_init = 1;
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	exit(0);

	return;
}

