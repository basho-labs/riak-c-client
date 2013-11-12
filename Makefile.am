AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4
lib_LTLIBRARIES = libriak_c_client.la

libriak_c_client_la_SOURCES = src/riak.c \
			      src/riak_async.c \
			      src/riak_binary.c \
			      src/riak_bucket_props.c \
			      src/riak_call_backs.c \
			      src/riak_context.c \
			      src/riak_error.c \
			      src/riak_event.c \
			      src/riak_log.c \
			      src/riak_messages.c \
			      src/riak_network.c \
			      src/riak_object.c \
			      src/riak_options.c \
			      src/riak_print.c \
			      src/riak_utils.c \
			      src/proto/riak.pb-c.c \
			      src/proto/riak_dt.pb-c.c \
			      src/proto/riak_kv.pb-c.c \
			      src/proto/riak_search.pb-c.c \
			      src/proto/riak_yokozuna.pb-c.c

#%.pb-c.c %.pb-c.h: %.proto
#    $(PROTOC) --proto_path=$(dir $^) --c_out=$(dir $^) $^
#BUILT_SOURCES = src/proto/riak.pb-c.h

libriak_c_client_la_CFLAGS = -Isrc/internal -Isrc/proto $(CFLAG_VISIBILITY) 
libriak_c_client_la_LDFLAGS = -lprotobuf-c

#riak_c_client_includedir = src/include src/proto src/internal

riak_client_libincludedir = $(libdir)/riak_c_client/include

#bin_PROGRAMS = riakc
#riakc_SOURCES = src/riakc/riakc.cpp
#riakc_CXXFLAGS = $(libriak_client_la_CXXFLAGS)
# -I/usr/local/Cellar/readline/6.2.1/include
#riakc_LDFLAGS = -lriak_client
 # -L/usr/local/Cellar/readline/6.2.1/lib -lreadline

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = riak_c_client.pc

#check_PROGRAMS = riak_cxx_unit_test
#TESTS = $(check_PROGRAMS)

#riak_cxx_unit_test_SOURCES = test/test_riak_object.cpp test/test_basic.cpp
#riak_cxx_unit_test_CXXFLAGS = $(libriak_client_la_CXXFLAGS)
#riak_cxx_unit_test_LDFLAGS = -lriak_client -lriakboost_unit_test_framework-mt-riak_client

#riak_cxx_test_server_SOURCES = test/test_server.cpp src/cxx/pbc_header.cpp src/cxx/pbc_message.cpp src/cxx/riakclient.pb.cc
#riak_cxx_test_server_CXXFLAGS = $(libriak_client_la_CXXFLAGS) -Isrc/cxx
#riak_cxx_test_server_LDFLAGS =  $(libriak_client_la_LDFLAGS) -lriak_client 

#CLEANFILES = $(check_PROGRAMS)
#SUBDIRS = deps
#DIST_SUBDIRS = deps
