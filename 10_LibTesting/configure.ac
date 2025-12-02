AC_PREREQ([2.69])
AC_INIT([growable-buf-lib], [0.1], [n/a])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])

AC_CONFIG_SRCDIR([src/buf.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_AR
LT_INIT

# pkg-config + check
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

# gcov for coverage helper
AC_CHECK_TOOL([GCOV], [gcov], [gcov])
AC_SUBST([GCOV])

AC_SUBST([CHECK_CFLAGS])
AC_SUBST([CHECK_LIBS])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    tests/Makefile
])

AC_OUTPUT
