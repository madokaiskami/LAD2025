lib_LTLIBRARIES = libgrowablebuf.la

libgrowablebuf_la_SOURCES = buf.c
libgrowablebuf_la_LDFLAGS = -version-info 0:0:0 -no-undefined

# Install public header
include_HEADERS = buf.h

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)/src
