AC_INIT([numblr], [1.0], [sp1ff@pobox.com])
AC_CONFIG_SRCDIR([src/1.cc])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CXX
AM_PROG_AR

AX_BOOST_BASE([1.63], [], [AC_MSG_ERROR([numblr requires boost_base 1.63 or later.])])
AX_BOOST_SYSTEM
AX_BOOST_LOCALE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
