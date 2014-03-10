## Riak C Client


# Status

**This project is currently under development and is NOT ready to use in a production environment. Expect
significant changes to the API.**

## Current tasks:
* cleanup around libevent threading (currently broken)
* support Riak 2.0 messages
   * Bucket Types
   * CRDTs (riak_dt)
   * Yokozuna
   * Security
   * Strong consistency
* testing
* general code cleanup
* documentation 
* performance tuning

# API Documentation

Doxygen generated docs located [here](http://basho.github.io/riak-c-client/globals_func.html)

# Dependencies

* automake
* autoconf
* autoconf-archive
* libtool
* libevent-2.0.21
* protobuf-2.5.0
* protobuf-c-0.15
* cunit-2.1-2
* OpenSSL 1.0.1f
* cunit
* pkg-config
* pthreads
* doxygen (if you are building docs)
* riak_pb
	* see note below
	
### riak_pb 

The riak-c-client depends on [riak_pb](https://github.com/basho/riak_pb), which is automatically downloaded via `./autogen.sh` as part of a normal build. The branch/tag that riak_pb uses is recorded in `.\.riak_pb.vsn`.

# Building

### OSX 10.8.5 and 10.9.1 Build

Be sure Xcode, command-line tools and [Home Brew](http://brew.sh/) installed first.

```
brew install git automake autoconf autoconf-archive libtool pkg-config protobuf protobuf-c libevent cunit doxygen openssl

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
# if you don't use a newer version of OpenSSL than what's shipped with OSX,
# you'll get several deprecation errors during the build
./configure -with-openssl=/usr/local/Cellar/openssl/1.0.1f
make
make check
doxygen
make install
```

### Ubuntu 12.04 LTS and 13.10 Build

*Note:* There appears to a conflict between the required version of **libprotobuf** installed on the system by default, so we need to put our version in its own directory.

```
sudo apt-get update
sudo apt-get install build-essential git automake libtool doxygen

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
 
wget http://sourceforge.net/projects/cunit/files/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
tar fx CUnit-2.1-2
./configure
make
make check
sudo make install
cd ..

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
LDFLAGS="-Wl,-rpath=/usr/local/protobuf-2.5.0/lib,--enable-new-dtags" ./configure PKG_CONFIG_PATH=/usr/local/protobuf-2.5.0/lib/pkgconfig
make
make check
doxygen
make install
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

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
./configure PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
make
make check
doxygen
make install
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

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
./configure CFLAGS="-m64" PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/lib/amd64/pkgconfig
make
make check
doxygen
make install
```

### FreeBSD 10.0

```
pkg install sudo-1.8.8 (as root)
sudoedit /usr/local/etc/sudoers
sudo pkg install git-1.8.5.2
sudo pkg install gcc-4.6.4
sudo pkg install autoconf-2.69
sudo pkg install automake-1.14
sudo pkg install libtool-2.4.2_2
sudo pkg install libevent2-2.0.21
sudo pkg install protobuf-2.5.0_1
sudo pkg install protobuf-c-0.15_1
sudo pkg install cunit-2.1.0_2
sudo pkg install gmake-3.82_1

git clone https://github.com/basho/riak-c-client
cd riak-c-client
./autogen.sh
CC=gcc46 ./configure
make
make check
doxygen
make install
```

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
