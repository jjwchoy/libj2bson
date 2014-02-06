#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([j2bson_major_version], [0])
m4_define([j2bson_minor_version], [1])
m4_define([j2bson_micro_version], [0])
m4_define([j2bson_version], [j2bson_major_version.j2bson_minor_version.j2bson_micro_version])

m4_define([j2bson_interface_age], [0])
m4_define([j2bson_binary_age], [m4_eval(100 * j2bson_minor_version + j2bson_micro_version)])

m4_define([lt_current], [m4_eval(100 * j2bson_minor_version + j2bson_micro_version - j2bson_interface_age)])
m4_define([lt_revision], [j2bson_interface_age])
m4_define([lt_age], [m4_eval(j2bson_binary_age - j2bson_interface_age)])

AC_PREREQ([2.68])
AC_INIT([libj2bson], [j2bson_version], [https://github.com/jjwchoy/libj2bson/issues])
AC_CONFIG_SRCDIR([j2bson/j2bson.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT

# Initialize automake
AM_INIT_AUTOMAKE([1.10])
AM_PROG_CC_C_O

J2BSON_MAJOR_VERSION=j2bson_major_version
J2BSON_MINOR_VERSION=j2bson_minor_version
J2BSON_MICRO_VERSION=j2bson_micro_version
J2BSON_API_VERSION=1.0
J2BSON_VERSION=j2bson_version
AC_SUBST(J2BSON_MAJOR_VERSION)
AC_SUBST(J2BSON_MINOR_VERSION)
AC_SUBST(J2BSON_MICRO_VERSION)
AC_SUBST(J2BSON_API_VERSION)
AC_SUBST(J2BSON_VERSION)

J2BSON_LT_CURRENT=lt_current
J2BSON_LT_REVISION=lt_revision
J2BSON_LT_AGE=lt_age
J2BSON_LT_VERSION="$J2BSON_LT_CURRENT:$J2BSON_LT_REVISION:$J2BSON_LT_AGE"
J2BSON_LT_LDFLAGS="-version-info $J2BSON_LT_VERSION"

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([
	Makefile
	j2bson/j2bson-version.h
	j2bson/libj2bson-1.0.pc
])
AC_OUTPUT
