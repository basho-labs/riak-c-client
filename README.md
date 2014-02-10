## Riak C Client


# Status

**This project is currently under development and is NOT ready to use in a production environment. Expect
significant changes to the API.**

## Current tasks:
* cleanup around libevent threading (currently broken)
* support Riak 2.0 messages
   * CRDTs (riak_dt)
   * Yokozuna
* testing
* general code cleanup
* documentation 
* performance tuning


# Dependencies

* automake
* autoconf
* libtool
* libevent
* protobuf
* protobuf-c
* cunit
* pkg-config
* pthreads
* doxygen (if you are building docs)


# Building

### OSX 10.8.5 and 10.9.1 Build

Be sure Xcode, command-line tools and [Home Brew](http://brew.sh/) installed first

```
brew install git automake autoconf libtool pkg-config protobuf protobuf-c libevent cunit doxygen
git clone https://github.com/basho/riak_pb
cd riak_pb
make C_PREFIX=/usr/local/riak_pb_c c_compile
sudo make C_PREFIX=/usr/local/riak_pb_c c_release
cd ..

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
./configure --with-riak-pb=/usr/local/riak_pb_c
make
make check
doxygen
```

### Ubuntu 12.04 LTS and 13.10 Build

*Note:* There appears to a conflict between the required version of **libprotobuf** installed on the system by default, so we need to put our version in its own directory.

```
sudo apt-get update
sudo apt-get install build-essential git automake libtool libcunit1-dev doxygen

wget https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz
tar fx protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure --prefix=/usr/local/protobuf-2.5.0
make
make check
sudo make install
cd ..

wget https://protobuf-c.googlecode.com/files/protobuf-c-0.15.tar.gz
tar fx protobuf-c-0.15.tar.gz
cd protobuf-c-0.15
./configure CXXFLAGS=-I/usr/local/protobuf-2.5.0/include LDFLAGS=-L/usr/local/protobuf-2.5.0/lib PATH=/usr/local/protobuf-2.5.0/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
make
make check
sudo make install
cd ..

wget https://github.com/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz
tar fx libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
./configure
make
LD_LIBRARY_PATH=/usr/local/protobuf-2.5.0/lib:/usr/local/lib make check
sudo make install
cd ..
 
git clone https://github.com/basho/riak_pb
cd riak_pb
make C_PREFIX=/usr/local/riak_pb_c c_compile
sudo make C_PREFIX=/usr/local/riak_pb_c c_release
cd ..

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
LDFLAGS="-Wl,-rpath=/usr/local/protobuf-2.5.0/lib,--enable-new-dtags" ./configure --with-riak-pb=/usr/local/riak_pb_c PKG_CONFIG_PATH=/usr/local/protobuf-2.5.0/lib/pkgconfig
make
make check
doxygen
```

### Centos Developer Desktop Build

```
wget https://github.com/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz
tar fx libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
./configure
make
make check
sudo make install
cd ..

wget https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz
tar fx protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure
make
make check
sudo make install
cd ..

wget https://protobuf-c.googlecode.com/files/protobuf-c-0.15.tar.gz
tar fx protobuf-c-0.15.tar.gz
cd protobuf-c-0.15
./configure LDFLAGS="-L/usr/local/lib"
make
make check
sudo make install
cd ..

wget http://downloads.sourceforge.net/project/cunit/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
tar fx CUnit-2.1-2-src.tar.bz2
cd CUnit-2.1-2
./configure
make
make check
sudo make install
cd ..

git clone https://github.com/basho/riak_pb
cd riak_pb
make C_PREFIX=/usr/local/riak_pb_c c_compile
sudo PATH=/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin make C_PREFIX=/usr/local/riak_pb_c c_release
cd ..

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
./configure --with-riak-pb=/usr/local/riak_pb_c PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
make
make check
doxygen
```

### Solaris 11

Add to PATH:
```
/usr/local/bin
```

```
sudo pkg install -v pkg:/developer/gcc-45@4.5.2-0.175.1.0.0.24.0
sudo pkg install -v pkg:/developer/build/automake-111@1.11.2-0.175.1.0.0.24.0
sudo pkg install -v pkg:/developer/build/autoconf@2.68-0.175.1.0.0.24.0
sudo pkg install -v pkg:/developer/versioning/git@1.7.9.2-0.175.1.0.0.24.0
sudo pkg install -v pkg:/developer/documentation-tool/doxygen@1.7.6.1-0.175.1.0.0.24.0

wget https://github.com/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz
tar fx libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
./configure CFLAGS="-m64"
make
make check
sudo make install
cd ..

wget https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz
tar fx protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure CFLAGS="-m64"
make
make check
sudo make install
cd ..

wget https://protobuf-c.googlecode.com/files/protobuf-c-0.15.tar.gz
tar fx protobuf-c-0.15.tar.gz
cd protobuf-c-0.15
./configure  CFLAGS="-m64" CXXFLAGS="-I/usr/local/include -m64" LDFLAGS="-L/usr/local/lib/amd64" PATH="/usr/local/bin:/usr/bin:/usr/sbin"  LIBS="-lnsl -lsocket -lresolv"
make
make check
sudo make install
cd ..

wget http://downloads.sourceforge.net/project/cunit/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
tar fx CUnit-2.1-2-src.tar.bz2
cd CUnit-2.1-2
./configure CFLAGS="-m64"
make
make check
sudo make install
cd ..

git clone https://github.com/basho/riak_pb
cd riak_pb
gmake C_PREFIX=/usr/local/riak_pb_c c_compile
sudo gmake C_PREFIX=/usr/local/riak_pb_c c_release
cd ..

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
./configure --with-riak-pb=/usr/local/riak_pb_c CFLAGS="-m64" PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/lib/amd64/pkgconfig
make
make check
doxygen
```

# API Documentation

[Here](http://basho.github.io/riak-c-client/index.html)

### Building local API docs

To build API documentation + man pages, you'll need `doxygen` installed. 
```
	doxygen
```

# Tutorial (outdated, work in progress)

**Note** 
Links to documentation below are against the Git *develop* branch. 

**Note** 
Links to specific line numbers in the html API documentation may not work as some files are not long enough to scroll to a specific line numbers.


## Include files

## 

```
 // TODO: more cleanup around .h files
 #include "riak.h"
 #include "riak_command.h"
 #include "riak_event.h"
 #include "riak_call_backs.h"
```

## Connecting to Riak

A **riak_context** needs to be created to communicate with Riak.

A convenience macro is available if you wish to specify only host and port.

`#define riak_context_new_default(C,H,P) riak_context_new((C),(H),(P),NULL,NULL,NULL,NULL,NULL,NULL)`
	* defined in [riak_context.h](http://basho.github.io/riak-c-client/riak__context_8h_source.html#l00055)

To create a full riak_context with custom memory management and logging, see [riak_context_new](http://basho.github.io/riak-c-client/riak__context_8h_source.html#l00048)

#### Example
```
char *hostname = "127.0.0.1";
char *pbport   = "10017";
riak_context *ctx;
riak_error err = riak_context_new_default(&ctx, hostname, pbport);
if (err) {
	// handle connection error
}
```

## Working with binary values

In order to accomodate multiple character sets, all values stored with the Riak C client use a [riak_binary](http://basho.github.io/riak-c-client/riak__binary_8h_source.html). Any character set conversion can be done before a value is stored in a **riak_binary**. 

To allocate a new [riak_binary](http://basho.github.io/riak-c-client/riak__binary_8h_source.html), call the [riak_binary_new](http://basho.github.io/riak-c-client/riak__binary_8h_source.html#l00035) function:

Allocated riak_binaries must be freed with the [riak_binary_free](http://basho.github.io/riak-c-client/riak__binary_8h_source.html#l00064) function.


If you are working with **char*** values, use the [riak_binary_new_from_string](http://basho.github.io/riak-c-client/riak__binary_8h_source.html#l00109) function:


There are several riak_binary utility functions available, please see the [API docs](http://basho.github.io/riak-c-client/riak__binary_8h_source.html).

####Example

```
riak_binary *bucket_bin = riak_binary_copy_from_string(ctx, "my_bucket"); // Not copied
riak_binary *key_bin    = riak_binary_copy_from_string(ctx, "my_key"); // Not copied
riak_binary *value_bin  = riak_binary_copy_from_string(ctx, "my_value"); // Not copied
```

## Synchronous communications

### Get

To perform a *get* against Riak, use the [riak_get](http://basho.github.io/riak-c-client/riak_8h_source.html#l00078) function defined in [riak.h](https://github.com/basho/riak-c-client/blob/develop/src/include/riak.h).

A [riak_get_options](http://basho.github.io/riak-c-client/struct__riak__get__options.html) struct needs to be provided to the function. 

If the return value != [ERIAK_OK](http://basho.github.io/riak-c-client/riak__error_8h_source.html#l00027), the get request failed.

Once you are finished with a riak_get_response, it needs to be freed via [riak_free_get_response](http://basho.github.io/riak-c-client/riak__messages-internal_8h_source.html#l00479). This is currently defined in [riak_messages-internal.h](https://github.com/basho/riak-c-client/blob/develop/src/internal/riak_messages-internal.h), but will be moved soon.

```
riak_error
riak_get(riak_context              *ctx,
         riak_binary               *bucket,
         riak_binary               *key,
         riak_get_options          *opts,
         riak_get_response        **response);
```

#### Example
```
riak_get_response *get_response;
riak_error err = riak_get(ctx, bucket_bin, key_bin, NULL, &get_response);
if (err) {
	fprintf(stderr, "Get Problems\n");
}
riak_print_get_response(get_response, output, sizeof(output));
printf("%s\n", output);
riak_free_get_response(ctx, &get_response);
```

### Put
### Delete
### Ping
### Server info
### List buckets
### List keys
### Get/set client id
### Get/set/reset bucket properties




### TODO

## Asynchronous communications
### Asynch setup
### Get
### Put
### Delete
### Ping
### Server info
### List buckets
### List keys
### Get/set client id
### Get/set/reset bucket properties

### TODO


# Questions / Comments

Please use the *riak-users* mailing list for questions + comments:

	http://lists.basho.com/mailman/listinfo/riak-users_lists.basho.com

# Contributing

If you want to help code, write documentation, or test the Riak C Client:

1. Fork the `riak-c-client` repo
2. Clone the repo from your fork
3. Create a branch:
	* `git checkout -b my_branch`
4. git push origin my_branch
5. Submit a pull request
    - if it's a major feature change/request, please email riak-users 
	- please provide CUnit test cases for your PR 
	
#License

http://www.apache.org/licenses/LICENSE-2.0.html

---

© 2014 Basho Technologies  
