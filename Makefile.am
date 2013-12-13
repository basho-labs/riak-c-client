ACLOCAL_AMFLAGS = -I m4

SRCDIR = $(top_srcdir)/src
EXAMPLESDIR = $(top_srcdir)/examples
TESTCUNITDIR = $(top_srcdir)/test/cunit

include_HEADERS =	src/include/riak.h \
			src/include/riak_binary.h \
			src/include/riak_bucket_props.h \
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

lib_LTLIBRARIES =	libriak_c_client-0.1.la
libriak_c_client_0_1_la_SOURCES = \
			src/riak.c \
			src/riak_async.c \
			src/riak_binary.c \
			src/riak_bucket_props.c \
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
			src/riak.pb-c.c src/riak_kv.pb-c.c \
			src/riak_search.pb-c.c src/riak_yokozuna.pb-c.c \
			src/messages/riak_delete.c \
			src/messages/riak_error.c \
			src/messages/riak_get.c \
			src/messages/riak_get_bucketprops.c \
			src/messages/riak_get_clientid.c \
			src/messages/riak_listbuckets.c \
			src/messages/riak_listkeys.c \
			src/messages/riak_ping.c \
			src/messages/riak_put.c \
			src/messages/riak_reset_bucketprops.c \
			src/messages/riak_serverinfo.c \
			src/messages/riak_set_bucketprops.c \
			src/messages/riak_set_clientid.c

BUILT_SOURCES =		src/riak.pb-c.h src/riak.pb-c.c \
			src/riak_kv.pb-c.h src/riak_kv.pb-c.c \
			src/riak_search.pb-c.h src/riak_search.pb-c.c \
			src/riak_yokozuna.pb-c.h src/riak_yokozuna.pb-c.c

CLEANFILES =		$(BUILT_SOURCES)

libriak_c_client_0_1_la_CPPFLAGS = \
			$(PROTOBUFC_INCLUDES) $(PROTOBUF_INCLUDES) \
			$(EVENT_INCLUDES) \
			-I$(SRCDIR)/include -I$(SRCDIR)/internal -I$(SRCDIR) \
			-DUSE_DEBUG

libriak_c_client_0_1_la_LIBADD = \
			$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
			-lpthread

AM_CFLAGS =		-g -Wall

%.pb-c.c %.pb-c.h: proto/%.proto
	$(PROTOC_C) --proto_path=src/proto --c_out=src/ $^

EXTRA_DIST = $(BUILT_SOURCES) \
		$(SRCDIR)/include $(SRCDIR)/internal $(SRCDIR)/adapters \
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
			-I$(SRCDIR)/adapters \
			-I$(SRCDIR) \
			-Iexamples

riak_c_example_LDADD = \
		-lriak_c_client-0.1 \
		$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
		-lcunit -lpthread

riak_c_example_DEPENDENCIES = libriak_c_client-0.1.la

check_PROGRAMS = riak_c_cunit
riak_c_cunit_SOURCES = 	test/cunit/registry.c \
			test/cunit/test_binary.c \
			test/cunit/test_clientid.c \
			test/cunit/test_config.c \
			test/cunit/test_connection.c \
			test/cunit/test_delete.c \
			test/cunit/test_get.c \
			test/cunit/test_operation.c \
			test/cunit/test_listbuckets.c \
			test/cunit/test_listkeys.c \
			test/cunit/test_put.c \
			test/cunit/test_serverinfo.c

riak_c_cunit_CPPFLAGS = -I$(SRCDIR)/include \
			-I$(SRCDIR)/internal \
			-I$(SRCDIR) \
			-I$(TESTCUNITDIR)/include

riak_c_cunit_LDADD =	-lriak_c_client-0.1 \
			$(PROTOBUFC_LIBS) $(PROTOBUF_LIBS) $(EVENT_LIBS) \
			-lcunit -lpthread

riak_c_cunit_DEPENDENCIES = libriak_c_client-0.1.la

TESTS = riak_c_cunit
