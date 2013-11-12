AC_INIT([libriak_c_client], [1.0], [eng@basho.com], [riak_c_client],
        [http://github.com/basho/riak-c-client])
AC_PREREQ([2.59])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.0 -Wall no-define foreign no-dist])
AM_MAINTAINER_MODE([enable])
AM_PROG_AR
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])
AC_PROG_LIBTOOL
AC_PROG_CC
AC_LANG([C])
gl_VISIBILITY
AC_HEADER_STDC
AC_ARG_WITH([protobuf-c-libdir],
    [AS_HELP_STRING([--with-protobuf-c-libdir=LIB_DIR],
        [location of the protocol buffers C libraries, defaults to /usr/lib])],
    [PROTOBUF_C_LIBDIR="$withval"],
    [PROTOBUF_C_LIBDIR='/usr/lib'])
AC_SUBST([PROTOBUF_C_LIBDIR])

LDFLAGS="$LDFLAGS -L$PROTOBUF_C_LIBDIR"

AC_CHECK_LIB([protobuf-c], [main], [], [AC_MSG_ERROR([cannot find protobuf-c library])])

AC_ARG_WITH([protocc],
    [AS_HELP_STRING([--with-protoc=PATH],
        [location of the protoc protocol buffer compiler binary, defaults to protoc])],
    [PROTOC="$withval"],
    [PROTOC='protocc'])
AC_SUBST([PROTOCC])
AC_CHECK_PROG([protocc], [protoc-c], [yes], [no])
LT_INIT([])
AC_CONFIG_FILES([Makefile riak_c_client.pc])
AC_OUTPUT
