#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "../src/include/riak.h" 
#include "../src/include/riak_command.h"
#include "../src/include/riak_context.h"
#include "../src/include/riak_messages.h"
#include "../src/internal/riak_messages-internal.h"


int main(int argc, char* argv[]) {
  printf("Riak C client sample program\n");
  // connect to the first node of a Riak 2.0pre4 "devrel"

  riak_context *ctx;
  riak_error err = riak_context_new_default(&ctx, "127.0.0.1", "10017");
  if (err) {
    exit(1);
  }

  riak_binary *bucket_bin = riak_binary_new_from_string(ctx, "foo"); // Not copied
  riak_binary *key_bin    = riak_binary_new_from_string(ctx, "bar"); // Not copied
  riak_binary *value_bin  = riak_binary_new_from_string(ctx, "baz"); // Not copied

  if (bucket_bin == NULL ||
      key_bin    == NULL ||
      value_bin  == NULL) {
    fprintf(stderr, "Could not allocate bucket/key/value\n");
    exit(1);
  }

  riak_object *obj;
  obj = riak_object_new(ctx);
  if (obj == NULL) {
    return 1;
  }

  riak_object_set_bucket(obj, bucket_bin);
  riak_object_set_key(obj, key_bin);
  riak_object_set_value(obj, value_bin);

  if (riak_object_get_bucket(obj) == NULL ||
      riak_object_get_value(obj) == NULL) {
    fprintf(stderr, "Could not allocate bucket/value\n");
    riak_free(ctx, &obj);
    exit(1);
  }
  riak_put_options put_options;
  memset(&put_options, '\0', sizeof(riak_put_options));
  put_options.has_return_head = RIAK_TRUE;
  put_options.return_head = RIAK_TRUE;
  put_options.has_return_body = RIAK_TRUE;
  put_options.return_body = RIAK_TRUE;

  riak_put_response *put_response;
  err = riak_put(ctx, obj, &put_options, &put_response);
  if (err) {
    fprintf(stderr, "Put Problems [%s]\n", riak_strerror(err));
    exit(1);
  }
  printf("Key = %s\n", put_response->key->data);
  riak_free_put_response(ctx, &put_response);
  riak_object_free(ctx, &obj);
  return 0;
}
