EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =
noinst_PROGRAMS =

include j2bson/Makefile.include
include examples/Makefile.include

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
DISTCHECK_CONFIGURE_FLAGS = --enable-silent-rules

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =
pkgconfig_DATA += $(top_builddir)/j2bson/libj2bson-1.0.pc
