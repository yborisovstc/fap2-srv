
AC_INIT([grayb_srv], [0.1], [yuri.borisov.v@gmail.com])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_FILES([
		 Makefile
		 server/Makefile
		 client/Makefile
		 dmas/Makefile
		 test/Makefile
		 ])


AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Configure options: --enable-debug[=no].
AC_ARG_ENABLE([debug],
	      [AS_HELP_STRING([--enable-debug],
			      [enable debug code (default is no)])],
			      [debug="$withval"], [debug=no])

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.0.0], , [AC_MSG_ERROR([
You must have the GLib 2.0 and development headers installed to build.
])])

AC_CHECK_LIB(pthread, pthread_create, [PTHREAD_LIBS+=-lpthread],
[AC_MSG_ERROR([required library pthread missing])])

PKG_CHECK_MODULES(FAPWS2, [libgrayb >= 0.1], , [
    AC_MSG_ERROR([You must have 'libgrayb' dev headers (libgrayb-dev) installed
    ]) ])

AC_SUBST(FAPWS2_CFLAGS)
AC_SUBST(FAPWS2_LIBS)

if test x$debug = xyes; then
    CXXFLAGS="-g -O0"
else
#    CXXFLAGS="-g -O2"
    CXXFLAGS="-g -O0"
fi


CXXFLAGS="$CXXFLAGS $GLIB_CFLAGS $FAPWS2_CFLAGS" 
LDFLAGS="$LDFLAGS $GLIB_LIBS $PTHREAD_LIBS $FAPWS2_LIBS"

AC_OUTPUT

echo CXXFLAGS $CXXFLAGS
echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Doxygen........: ${DOXYGEN:-NONE}
"

