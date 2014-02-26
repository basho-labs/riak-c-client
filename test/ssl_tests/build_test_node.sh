#!/bin/sh
./gen_test_certs.sh

rm -rf ./riak

git clone git@github.com:/basho/riak.git
(cd riak && make && make rel)

dir=`pwd`
cat << EOF >> ./riak/rel/riak/etc/riak.conf
tls_protocols.tlsv1.1 = on
ssl.certfile = $dir/certificate/cert.pem
ssl.keyfile = $dir/certificate/key.pem
ssl.cacertfile = $dir/certificate/cacert.pem
EOF

