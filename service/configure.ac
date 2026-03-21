AC_INIT([rcr], [1.0], [aomdoa@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src])
AC_PROG_CC
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_ERROR([pthreads not found])])
PKG_CHECK_MODULES([MICROHTTPD], [libmicrohttpd >= 1.0.0], [AC_DEFINE([HAVE_MICROHTTPD], [1], [Define to 1 if you have libmicrohttpd])],[AC_MSG_ERROR([libmicrohttpd not found or version too old])])
PKG_CHECK_MODULES([LIBCJSON], [libcjson >= 1.7.0], [AC_DEFINE([HAVE_CJSON], [1], [Define to 1 if you have cjson])],[AC_MSG_ERROR([cjson not found or version too old])])
AC_OUTPUT