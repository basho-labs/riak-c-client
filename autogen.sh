autoreconf --install || exit 1
rm -rf riak_pb
git clone https://github.com/basho/riak_pb
cd riak_pb
git checkout -b `cat ../.riak_pb.vsn` `cat ../.riak_pb.vsn`
MAKE=make
# Use GNU make, if it exists
if hash gmake 2>/dev/null; then
    MAKE=gmake
fi
${MAKE} c_compile
cd ..
