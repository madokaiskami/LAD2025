AM_CPPFLAGS = -I$(top_srcdir)/src $(CHECK_CFLAGS)

check_PROGRAMS = gmbuf_tests
gmbuf_tests_SOURCES = test_buf.c
gmbuf_tests_LDADD = $(top_builddir)/src/libgrowablebuf.la $(CHECK_LIBS)

TESTS = gmbuf_tests
