ACLOCAL_AMFLAGS = -I m4

SRCDIR = $(top_srcdir)/src
RIAK_PB = $(top_srcdir)/riak_pb
EXAMPLESDIR = $(top_srcdir)/examples
TESTCUNITDIR = $(top_srcdir)/test/cunit

include_HEADERS =	src/include/riak.h \
			src/include/riak_binary.h \
			src/include/riak_bucketprops.h \
			src/include/riak_config.h \
			src/include/riak_connection.h \
			src/include/riak_error.h \
			src/include/riak_log.h \
			src/include/riak_log_config.h \
			src/include/riak_messages.h \
			src/include/riak_network.h \
			src/include/riak_object.h \
			src/include/riak_operation.h \
			src/include/riak_types.h
msgincludedir =		$(includedir)/messages
dist_msginclude_DATA =	src/include/messages/riak_2index.h \
			src/include/messages/riak_delete.h \
			src/include/messages/riak_error.h \
			src/include/messages/riak_get.h \
			src/include/messages/riak_get_bucketprops.h \
			src/include/messages/riak_get_clientid.h \
			src/include/messages/riak_listbuckets.h \
			src/include/messages/riak_listkeys.h \
			src/include/messages/riak_mapreduce.h \
			src/include/messages/riak_ping.h \
			src/include/messages/riak_put.h \
			src/include/messages/riak_reset_bucketprops.h \
			src/include/messages/riak_search.h \
			src/include/messages/riak_serverinfo.h \
			src/include/messages/riak_set_bucketprops.h \
			src/include/messages/riak_set_clientid.h

lib_LTLIBRARIES =	libriak_c_client-0.5.la
libriak_c_client_0_5_la_SOURCES = \
			src/riak.c \
			src/riak_async.c \
			src/riak_binary.c \
			src/riak_bucketprops.c \
			src/riak_config.c \
			src/riak_connection.c \
			src/riak_error.c \
			src/riak_log.c \
			src/riak_messages.c \
			src/riak_network.c \
			src/riak_object.c \
			src/riak_operation.c \
			src/riak_print.c \
			src/riak_utils.c \
			src/messages/riak_2index.c \
			src/messages/riak_delete.c \
			src/messages/riak_error.c \
			src/messages/riak_get.c \
			src/messages/riak_get_bucketprops.c \
			src/messages/riak_get_clientid.c \
			src/messages/riak_listbuckets.c \
			src/messages/riak_listkeys.c \
			src/messages/riak_mapreduce.c \
			src/messages/riak_ping.c \
			src/messages/riak_put.c \
			src/messages/riak_reset_bucketprops.c \
			src/messages/riak_search.c \
			src/messages/riak_serverinfo.c \
			src/messages/riak_set_bucketprops.c \
			src/messages/riak_set_clientid.c \
			riak_pb/c/riak.pb-c.c \
			riak_pb/c/riak_dt.pb-c.c \
			riak_pb/c/riak_kv.pb-c.c \
			riak_pb/c/riak_search.pb-c.c \
			riak_pb/c/riak_yokozuna.pb-c.c

libriak_c_client_0_5_la_CPPFLAGS = \
			$(PROTOBUFC_INCLUDES) $(PROTOBUF_INCLUDES) \
			$(EVENT_INCLUDES) \
			-I$(SRCDIR)/include \
			-I$(SRCDIR)/internal \
			-I$(RIAK_PB)/c -I$(SRCDIR) \
			-DUSE_DEBUG

libriak_c_client_0_5_la_LIBADD = \
			$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
			-lpthread

AM_CFLAGS =		-g -Wall

EXTRA_DIST =	$(SRCDIR)/include $(SRCDIR)/internal $(SRCDIR)/adapters \
		$(EXAMPLESDIR)/example_call_backs.h \
		$(EXAMPLESDIR)/riak_command.h \
		$(TESTCUNITDIR)/include

bin_PROGRAMS = riak_c_example
riak_c_example_SOURCES = examples/example.c \
			examples/example_call_backs.c \
			examples/riak_command.c

riak_c_example_CPPFLAGS = \
			-DUSE_DEBUG -D_RIAK_DEBUG \
			-I$(SRCDIR)/include \
			-I$(SRCDIR)/internal \
			-I$(RIAK_PB)/c \
			-I$(SRCDIR)/adapters \
			-I$(SRCDIR) \
			-Iexamples

riak_c_example_LDADD = \
		-lriak_c_client-0.5 \
		$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
		-lcunit -lpthread

riak_c_example_DEPENDENCIES = libriak_c_client-0.5.la

check_PROGRAMS = riak_c_cunit
riak_c_cunit_SOURCES = 	test/cunit/registry.c \
			test/cunit/test_2index.c \
			test/cunit/test_binary.c \
			test/cunit/test_bucketprops.c \
			test/cunit/test_clientid.c \
			test/cunit/test_config.c \
			test/cunit/test_connection.c \
			test/cunit/test_delete.c \
			test/cunit/test_get.c \
			test/cunit/test_mapreduce.c \
			test/cunit/test_operation.c \
			test/cunit/test_listbuckets.c \
			test/cunit/test_listkeys.c \
			test/cunit/test_object.c \
			test/cunit/test_put.c \
			test/cunit/test_search.c \
			test/cunit/test_serverinfo.c

riak_c_cunit_CPPFLAGS = -I$(SRCDIR)/include \
			-I$(SRCDIR)/internal \
			-I$(RIAK_PB)/c \
			-I$(SRCDIR) \
			-I$(TESTCUNITDIR)/include

riak_c_cunit_LDADD =	-lriak_c_client-0.5 \
			$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
			-lcunit -lpthread

riak_c_cunit_DEPENDENCIES = libriak_c_client-0.5.la

TESTS = riak_c_cunit


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = riak-c-client.pc
