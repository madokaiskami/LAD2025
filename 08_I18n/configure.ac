AC_PREREQ([2.69])
AC_INIT([guessnum],[0.1],[n/a])
AC_CONFIG_SRCDIR([src/guessnum.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-xz no-dist-gzip])
AC_PROG_CC

dnl ---- gettext (external) ----
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.21])

dnl Provide localedir to Makefiles; compile-time injection done in src/Makefile.am
AC_SUBST([localedir], ["${datarootdir}/locale"])

AC_CONFIG_FILES([
Makefile
src/Makefile
po/Makefile.in
])
AC_OUTPUT
