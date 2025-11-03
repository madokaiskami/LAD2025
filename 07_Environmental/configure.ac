AC_INIT([rhasher], [0.1], [n/a])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/rhasher.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
PKG_PROG_PKG_CONFIG

# ---- LibRHash (required) ----
PKG_CHECK_MODULES([RHASH], [librhash], [], [AC_MSG_ERROR([librhash (LibRHash) not found. Install librhash-dev/rhash-devel.])])
AC_SUBST([RHASH_CFLAGS])
AC_SUBST([RHASH_LIBS])

# ---- Readline (optional, can be forced off) ----
AC_ARG_WITH([readline],
  AS_HELP_STRING([--without-readline], [build without readline even if present]),
  [],
  [with_readline=maybe])

have_readline=no
AS_IF([test "x$with_readline" != "xno"], [
  AC_CHECK_HEADERS([readline/readline.h readline/history.h], [], [have_readline=no_missing_headers=yes])
  AC_CHECK_LIB([readline], [readline], [have_readline=yes; READLINE_LIBS="-lreadline"], [have_readline=no_missing_lib=yes])
])

# If user forced off
AS_IF([test "x$with_readline" = "xno"], [have_readline=no])

# Define macro USE_READLINE when using it
AS_IF([test "x$have_readline" = "xyes"], [
  AC_DEFINE([USE_READLINE], [1], [Define to 1 to use GNU Readline instead of getline()])
  AC_SUBST([READLINE_LIBS])
  AC_MSG_RESULT([readline: enabled])
], [
  AC_MSG_RESULT([readline: disabled])
])

AM_CONDITIONAL([HAVE_READLINE], [test "x$have_readline" = "xyes"])

AC_CONFIG_FILES([
Makefile
src/Makefile
tests/Makefile
])
AC_OUTPUT
