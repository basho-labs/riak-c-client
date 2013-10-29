![riak-c-client](./docs/riak_c_client_large.png "Riak C Client") 



# Status

**This project is currently under heavy development and is NOT ready to use in a production environment.**

#### TODO:

* cleanup around libevent threading (currently broken)
* clean up header files, only require a single riak.h 
* testing testing testing
* general code cleanup
* documentation 
* performance tuning

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


# Tutorial (outdated, work in progress)

**Note** 
Links to documentation below are against the Git *master* branch. 

**Note** 
Links to specific line numbers in the html API documentation may not work as some files are not long enough to scroll to a specific line numbers.


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
riak_binary *bucket_bin = riak_binary_new_from_string(ctx, "my_bucket"); // Not copied
riak_binary *key_bin    = riak_binary_new_from_string(ctx, "my_key"); // Not copied
riak_binary *value_bin  = riak_binary_new_from_string(ctx, "my_value"); // Not copied
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
