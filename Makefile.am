
bin_PROGRAMS = chrono
chrono_SOURCES = chrono.c chrono.h test.c gtk.c

if HAVE_NCURSES
chrono_SOURCES += ncurses.c
endif

if HAVE_X11
chrono_SOURCES += xlib.c
endif

