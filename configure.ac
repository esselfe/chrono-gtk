
AC_INIT([chrono],[0.3.0],[bsfc1685@gmail.com])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.9 foreign])
AC_CONFIG_HEADERS(config.h)

CFLAGS="-O2 -I/usr/include/gtk-3.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0"

AC_PROG_CC
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(stdio.h)
LIBS="$LIBS -lc -lpthread"
AC_CHECK_FUNCS([atoi gettimeofday])
AC_CHECK_FUNCS([getopt_long gmtime timeradd timersub pthread_create usleep])
AC_CHECK_HEADER(assert.h)
AC_CHECK_FUNC([assert])

AC_ARG_ENABLE(ncurses,
	[AC_HELP_STRING([--disable-ncurses], [Do not compile ncurses code @<:@default=yes@:>@])],
	[enable_ncurses=$enableval], [enable_ncurses="yes"])

AC_MSG_CHECKING([whether to use ncurses])
if test x"$enable_ncurses" = xyes; then
	AC_MSG_RESULT(yes)
	LIBS="$LIBS -lncurses"
	AC_CHECK_HEADERS(ncursesw/ncurses.h,,enable_ncurses=no)
	AC_CHECK_FUNCS([getch initscr endwin printw refresh])
	AC_DEFINE(HAVE_NCURSES, 1, [Define to 1 if you have ncurses installed])
else
	AC_MSG_RESULT(no)
	AC_MSG_WARN([*** ncurses support will not be compiled ***])
fi
AM_CONDITIONAL([HAVE_NCURSES], [test x"$enable_ncurses" = xyes])

AC_ARG_ENABLE(x11,
	[AS_HELP_STRING([--disable-x11], [Do not compile X11 code @<:@default=yes@:>@])],
	[enable_x11=$enableval], [enable_x11="yes"])

AC_MSG_CHECKING([whether to use X11])
if test x"$enable_x11" = xyes; then
	AC_MSG_RESULT(yes)
	LIBS="$LIBS -lX11 -lXext"
	AC_CHECK_HEADERS(X11/Xlib.h X11/Xatom.h X11/Xutil.h,,enable_x11=no)
	AC_CHECK_FUNCS([XOpenDisplay XPending XNextEvent XSetErrorHandler XCreateWindow])
	AC_CHECK_FUNCS([XSetWMNormalHints XSetWMHints XSetWMName XStringListToTextProperty])
	AC_CHECK_FUNCS([XSetWMIconName XLoadQueryFont XCreateGC XMapWindow XInternAtom])
	AC_CHECK_FUNCS([XSendEvent XClearArea XDrawText XUnmapWindow XDestroyWindow])
	AC_DEFINE(HAVE_X11, 1, [Define to 1 if you have libX11 installed])
else
	AC_MSG_RESULT(no)
	AC_MSG_WARN([*** libX11 support will not be compiled ***])
fi
AM_CONDITIONAL([HAVE_X11], [test x"$enable_x11" = xyes])

AC_ARG_ENABLE(gtk,
	[AS_HELP_STRING([--disable-gtk], [Do not compile gtk code @<:@default=yes@:>@])],
	[enable_gtk=$enableval], [enable_gtk="yes"])

AC_MSG_CHECKING([wheter to use GTK])
if test x"$enable_gtk" = xyes; then
	AC_MSG_RESULT(yes)
	LIBS="$LIBS -lgtk-3 -lgobject-2.0 -lgdk-3 -lglib-2.0"
	AC_CHECK_HEADERS(gtk-3.0/gtk/gtk.h,,enable_gtk=no)
else
	AC_MSG_RESULT(no)
	AC_MSG_WARN([*** GTK support will not be compiled ***])
fi

if test x"$enable_x11" = xno && test x"$enable_ncurses" = xno && test x"$enable_gtk" = xno; then
	AC_MSG_WARN([At least on of GTK, X11 or ncurses support must be enabled.])
	AC_MSG_ERROR([Use configure options --enable-ncurses --enable-x11 or --enable-gtk])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

