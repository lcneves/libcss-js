AC_INIT([example], [0.1], [lcneves@gmail.com])
: ${CFLAGS="$CFLAGS -g -W -Wall -I/usr/INCLUDEDIR -lcss -lparserutils -lwapcaplet"}
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CHECK_LIB([libcss])
AC_CHECK_HEADERS([libcss/libcss.h])
AC_CHECK_LIB([libwapcaplet])
AC_CHECK_HEADERS([libwapcaplet/libwapcaplet.h])
AC_CHECK_LIB([libparserutils])
AC_CHECK_HEADERS([libparserutils/parserutils.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
