#!/bin/sh

rm -rf ./riak

git clone git@github.com:/basho/riak.git
(cd riak && make && make rel)

dir=`pwd`
cat << EOF > ./riak/rel/riak/etc/advanced.config
[
  {riak_core, [
              {default_bucket_props,
              [{allow_mult, true}]}
            ]},
  {riak_api, [
            {certfile,  "$dir/certs/cert.pem"},
            {keyfile,    "$dir/certs/key.pem"},
            {cacertfile, "$dir/certs/cacert.pem"}
            ]}
].
EOF

TEST_RIAK=./riak/rel/riak/
$TEST_RIAK/bin/riak start
sleep 25

$TEST_RIAK/bin/riak-admin security enable
$TEST_RIAK/bin/riak-admin security add-user foo password=bar
$TEST_RIAK/bin/riak-admin security add-source all 127.0.0.1/32 password
$TEST_RIAK/bin/riak-admin security grant riak_kv.get,riak_kv.put,riak_kv.delete,riak_kv.index,riak_kv.list_keys,riak_kv.list_buckets,riak_core.get_bucket,riak_core.set_bucket,riak_core.get_bucket_type,riak_core.set_bucket_type ON ANY TO foo
