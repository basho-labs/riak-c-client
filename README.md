riak-c-client
=============


# Status

**This project is currently under heavy development and is NOT ready to use in a production environment.**

# Dependencies

* libevent
* protobuf
* protobuf-c
* pthreads
* [zlog](https://github.com/basho/zlog/tree/feature/add-pkgconfig) 
* doxygen (if you are building docs)


# Building

### OSX Build

	git clone https://github.com/basho/zlog.git
	cd zlog
	git checkout feature/add-pkgconfig
	
	make
	sudo make install
		or 
	make PREFIX=/usr/local/
	sudo make PREFIX=/usr/local/ install
	
	brew install protobuf protobuf-c scons libevent
	git clone git@github.com:/basho/riak-c-client.git
	cd riak-c-client
	scons


### Ubuntu 13.04 build

```
sudo apt-get install scons libevent-dev protobuf-c-compiler dev-libprotobuf dev-libprotoc libcunit1 libcunit1-ncurses-dev
 
git clone https://github.com/basho/zlog.git
cd zlog
git checkout feature/add-pkgconfig
	
make
sudo make install
	or 
make PREFIX=/usr/local/
sudo make PREFIX=/usr/local/ install 
 
wget https://protobuf-c.googlecode.com/files/protobuf-c-0.15.tar.gz
cd protobuf-c-0.15
./configure && make && sudo make install
cd ..

git clone git@github.com:/basho/riak-c-client.git
cd riak-c-client
scons
```

### API Documentation

[Here](http://basho.github.io/riak-c-client/index.html)

### Building local API docs

To build API documentation + man pages, you'll need `doxygen` installed. 

	scons docs


# Tutorial

## Include files

## 

```
 // TODO: should we create a single .h to include?
 #include "riak.h" 
 #include "riak.pb-c.h"
 #include "riak_binary-internal.h"
```

## Connecting to Riak

A **riak_context** needs to be created to communicate with Riak.

```
 #define riak_context_new_default(C,H,P) riak_context_new((C),(H),(P),NULL,NULL,NULL,NULL,NULL,NULL)

 // OR
 
 riak_error
 riak_context_new(riak_context    **context,
                  const char       *hostname,
                  const char       *portnum,
                  riak_alloc_fn     alloc,
                  riak_realloc_fn   realloc,
                  riak_free_fn      freeme,
                  riak_pb_alloc_fn  pb_alloc,
                  riak_pb_free_fn   pb_free,
                  const char       *logging_category);
```

See *riak_context_new* in `riak_context.h` to customize the connection, logging, and memory management.


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

In order to accomodate multiple character sets, all values stored with the Riak C client use a **riak_binary** type. Any character set conversion can be done before a value is stored in a **riak_binary**. 


To allocate a new **riak_binary**, call the *riak_binary_new* function:

```
riak_binary*
riak_binary_new(riak_context *ctx,
                riak_size_t   len,
                riak_uint8_t *data); 
```

If you are working with **char*** values, use the *riak_binary_new_from_string* function:

```
riak_binary*
riak_binary_new_from_string(riak_context *ctx,
                            const char   *from) 
```

####Example

```
riak_binary *bucket_bin = riak_binary_new_from_string(ctx, "my_bucket"); // Not copied
riak_binary *key_bin    = riak_binary_new_from_string(ctx, "my_key"); // Not copied
riak_binary *value_bin  = riak_binary_new_from_string(ctx, "my_value"); // Not copied
```

## Synchronous communications

### Get

### Put
### Delete
### Ping
### Server info
### List buckets
### List keys
### Get/set client id
### Get/set/reset bucket properties


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

### TODO

## Asynchronous communications

### Asynch setup

```
riak_event *rev;
err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
if (err) {
	return err;
}
// have user callback know about its riak_event
riak_event_set_cb_data(rev, rev);
```

### Asynch Get

``` 
void my_get_cb(riak_get_response *response, void *ptr) {
  // process the get callback
}

int main() {
	…  
	riak_event  *rev = NULL;
	riak_encode_get_request(rev, bucket_bin, key_bin, NULL, &(rev->pb_request));
}

```



#### Example
```
riak_event_set_response_cb(rev, (riak_response_callback)get_cb);
riak_encode_get_request(rev, bucket_bin, key_bin, NULL, &(rev->pb_request));

```


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

1. Fork the `riak-c-client` repo
2. Clone the repo from your fork
3. Create a branch:
	* `git checkout -b my_branch`
4. git push origin my_branch
5. Submit a pull request
    - if it's a major feature request, please email riak-users 
	- please provide CUnit test cases for your PR 

#License

http://www.apache.org/licenses/LICENSE-2.0.html

---

© 2013 Basho Technologies  
