# -*- Autoconf -*-
AC_PREREQ([2.61])
AC_INIT([riak_c_client],[0.5])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_LIBTOOL
LT_INIT

PKG_CHECK_MODULES([EVENT], [libevent_pthreads])
EVENT_INCLUDES=`pkg-config libevent_pthreads --cflags`
PKG_CHECK_MODULES([PROTOBUFC], [libprotobuf-c])
PROTOBUFC_INCLUDES=`pkg-config libprotobuf-c --cflags-only-I`
PKG_CHECK_MODULES([PROTOBUF], [protobuf])
PROTOBUF_INCLUDES=`pkg-config protobuf --cflags-only-I`
PKG_CHECK_MODULES([CUNIT], [cunit])
CUNIT_INCLUDES=`pkg-config cunit --cflags-only-I`
PKG_CHECK_MODULES([OPENSSL], [openssl])
OPENSSL_INCLUDES=`pkg-config openssl --cflags-only-I`

AX_CHECK_OPENSSL

AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

AC_CHECK_FUNCS([memset socket strerror])

AC_CHECK_HEADERS([arpa/inet.h stddef.h stdint.h stdlib.h])
AC_CHECK_HEADERS([string.h strings.h unistd.h])
AC_FUNC_MALLOC

AC_ARG_WITH([protoc-c],
    [AS_HELP_STRING([--with-protoc-c=PATH],
        [location of the protoc protocol buffer C compiler binary, default protoc-c])],
    [PROTOC_C="$withval"],
    [PROTOC_C='protoc-c'])
AC_SUBST([PROTOC_C])

AC_CONFIG_FILES([Makefile riak-c-client-async.pc riak-c-client.pc])
AC_OUTPUT
