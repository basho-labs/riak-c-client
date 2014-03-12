/*********************************************************************
 *
 * riak_object.c: Riak C Object
 *
 * Copyright (c) 2007-2014 Basho Technologies, Inc.  All Rights Reserved.
 *
 * This file is provided to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *********************************************************************/
#include <time.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_binary-internal.h"
#include "riak_object-internal.h"
#include "riak_config-internal.h"

//
// P A I R S
//

riak_pair*
riak_pair_new(riak_config *cfg) {
    return riak_config_clean_allocate(cfg, sizeof(riak_pair));
}

static riak_error
riak_pairs_copy_to_pb(riak_config  *cfg,
                      RpbPair    ***pbpair_target,
                      riak_pair   **pair,
                      int           num_pairs) {
    RpbPair **pbpair = riak_config_clean_allocate(cfg, sizeof(RpbPair*) * num_pairs);
    if (pbpair == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_pairs; i++) {
        pbpair[i] = riak_config_clean_allocate(cfg, sizeof(RpbPair));
        if (pbpair[i] == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        rpb_pair__init(pbpair[i]);

        riak_binary_copy_to_pb(&(pbpair[i]->key), pair[i]->key);
        
        if (pair[i]->has_value) {
            pbpair[i]->has_value = RIAK_TRUE;
            riak_binary_copy_to_pb(&(pbpair[i]->value), pair[i]->value);
        }
    }
    // Finally assign the pointer to the list of pair pointers
    *pbpair_target = pbpair;

    return ERIAK_OK;
}

static void
riak_pairs_free_pb(riak_config  *cfg,
                   RpbPair    ***pbpair_target,
                   int           num_pairs) {
    RpbPair **pbpair = *pbpair_target;
    int i;
    for(i = 0; i < num_pairs; i++) {
        riak_free(cfg, &(pbpair[i]));
    }
    riak_free(cfg, pbpair_target);
}

riak_error
riak_pairs_copy_from_pb(riak_config *cfg,
                        riak_pair  **pair_target,
                        RpbPair     *pbpair) {

    riak_pair *pair = riak_config_clean_allocate(cfg, sizeof(riak_pair));
    if (pair == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    pair->key = riak_binary_copy_from_pb(cfg, &(pbpair->key));
    if (pair->key == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    if (pbpair->has_value) {
        pair->has_value = RIAK_TRUE;
        pair->value = riak_binary_copy_from_pb(cfg, &(pbpair->value));
        if (pair->value == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    // Finally assign the pointer to the list of pair pointers
    *pair_target = pair;

    return ERIAK_OK;
}

riak_error
riak_pairs_copy_array_from_pb(riak_config  *cfg,
                              riak_pair  ***pair_target,
                              RpbPair     **pbpair,
                              int           num_pairs) {
    riak_pair **pair = riak_config_clean_allocate(cfg, sizeof(riak_pair*) * num_pairs);
    if (pair == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_pairs; i++) {
        riak_error err = riak_pairs_copy_from_pb(cfg, &(pair[i]), pbpair[i]);
        if (err != ERIAK_OK) {
            return err;
        }
    }
    // Finally assign the pointer to the list of pair pointers
    *pair_target = pair;

    return ERIAK_OK;
}

riak_int32_t
riak_pairs_print(riak_print_state *state,
                 riak_pair       **pair,
                 riak_uint32_t     num_pairs) {
    riak_int32_t wrote = 0;

    for(int i = 0; i < num_pairs; i++) {
        wrote += riak_print_label_binary(state, "Key", pair[i]->key);
        if (pair[i]->has_value) {
            wrote += riak_print_label_binary(state, "Value", pair[i]->value);
        }
    }

    return wrote;
}

void
riak_pairs_free(riak_config  *cfg,
                riak_pair  ***pair_target,
                int           num_pairs) {
    riak_pair **pair = *pair_target;
    int i;
    for(i = 0; i < num_pairs; i++) {
        riak_binary_free(cfg, &(pair[i]->key));
        riak_binary_free(cfg, &(pair[i]->value));
        riak_free(cfg, &(pair[i]));
    }
    riak_free(cfg, pair_target);
}

riak_binary*
riak_pair_get_key(riak_pair *pair)
{
    return pair->key;
}

riak_boolean_t
riak_pair_get_has_value(riak_pair *pair)
{
    return pair->has_value;
}

riak_binary*
riak_pair_get_value(riak_pair *pair)
{
    return pair->value;
}

riak_error
riak_pair_set_key(riak_config *cfg,
                  riak_pair   *pair,
                  riak_binary *value) {
    pair->key = riak_binary_copy(cfg, value);
    if (pair->key == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_pair_set_value(riak_config *cfg,
                    riak_pair   *pair,
                    riak_binary *value) {
    pair->value = riak_binary_copy(cfg, value);
    if (pair->value == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    pair->has_value = RIAK_TRUE;
    return ERIAK_OK;
}

//
// L I N K S
//

riak_link*
riak_link_new(riak_config *cfg) {
    return riak_config_clean_allocate(cfg, sizeof(riak_link));
}

static riak_error
riak_links_copy_to_pb(riak_config  *cfg,
                      RpbLink    ***pblink_target,
                      riak_link   **link,
                      int           num_links) {
    RpbLink **pblink = riak_config_clean_allocate(cfg, sizeof(RpbLink*) * num_links);
    if (pblink == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_links; i++) {
        pblink[i] = riak_config_clean_allocate(cfg, sizeof(RpbLink));
        if (pblink[i] == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        rpb_link__init(pblink[i]);
        if (link[i]->has_bucket) {
            pblink[i]->has_bucket = RIAK_TRUE;
            riak_binary_copy_to_pb(&(pblink[i]->bucket), link[i]->bucket);
        }
        if (link[i]->has_key) {
            pblink[i]->has_key = RIAK_TRUE;
            riak_binary_copy_to_pb(&(pblink[i]->key), link[i]->key);
        }
        if (link[i]->has_tag) {
            pblink[i]->has_tag = RIAK_TRUE;
            riak_binary_copy_to_pb(&(pblink[i]->tag), link[i]->tag);
        }
    }
    // Finally assign the pointer to the list of link pointers
    *pblink_target = pblink;

    return ERIAK_OK;
}

static void
riak_links_free_pb(riak_config  *cfg,
                   RpbLink    ***pblink_target,
                   int           num_links) {
    RpbLink **pblink = *pblink_target;
    int i;
    for(i = 0; i < num_links; i++) {
        riak_free(cfg, &(pblink[i]));
    }
    riak_free(cfg, pblink_target);
}

static riak_error
riak_links_copy_from_pb(riak_config  *cfg,
                        riak_link  ***link_target,
                        RpbLink     **pblink,
                        int           num_links) {
    riak_link **link = riak_config_clean_allocate(cfg, sizeof(riak_link*) * num_links);
    if (pblink == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_links; i++) {
        link[i] = riak_config_clean_allocate(cfg, sizeof(riak_link));
        if (link[i] == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        if (pblink[i]->has_bucket) {
            link[i]->has_bucket = RIAK_TRUE;
            link[i]->bucket = riak_binary_copy_from_pb(cfg, &(pblink[i]->bucket));
            if (link[i]->bucket == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
        }
        if (pblink[i]->has_key) {
            link[i]->has_key = RIAK_TRUE;
            link[i]->key = riak_binary_copy_from_pb(cfg, &(pblink[i]->key));
            if (link[i]->key == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
        }
        if (pblink[i]->has_tag) {
            link[i]->has_tag = RIAK_TRUE;
            link[i]->tag = riak_binary_copy_from_pb(cfg, &(pblink[i]->tag));
            if (link[i]->tag == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
        }
    }
    // Finally assign the pointer to the list of link pointers
    *link_target = link;

    return ERIAK_OK;
}

void
riak_links_free(riak_config  *cfg,
                riak_link  ***link_target,
                int           num_links) {
    riak_link **link = *link_target;
    int i;
    for(i = 0; i < num_links; i++) {
        riak_binary_free(cfg, &(link[i]->bucket));
        riak_binary_free(cfg, &(link[i]->key));
        riak_binary_free(cfg, &(link[i]->tag));
        riak_free(cfg, &(link[i]));
    }
    riak_free(cfg, link_target);
}

riak_int32_t
riak_links_print(riak_print_state *state,
                 riak_link        *link) {
    riak_int32_t wrote = 0;
    if (link->has_bucket) {
        wrote += riak_print_label_binary(state, "Link Bucket", link->bucket);
    }
    if (link->has_key) {
        wrote += riak_print_label_binary(state, "Link Key", link->key);
    }
    if (link->has_tag) {
        wrote += riak_print_label_binary(state, "Link Tag", link->tag);
    }
    return wrote;
}

riak_boolean_t
riak_link_get_has_bucket(riak_link *link)
{
    return link->has_bucket;
}

riak_binary*
riak_link_get_bucket(riak_link *link)
{
    return link->bucket;
}

riak_boolean_t
riak_link_get_has_key(riak_link *link)
{
    return link->has_key;
}

riak_binary*
riak_link_get_key(riak_link *link)
{
    return link->key;
}

riak_boolean_t
riak_link_get_has_tag(riak_link *link)
{
    return link->has_tag;
}

riak_binary*
riak_link_get_tag(riak_link *link)
{
    return link->tag;
}

riak_error
riak_link_set_bucket(riak_config *cfg,
                     riak_link   *link,
                     riak_binary *value) {
      link->bucket = riak_binary_copy(cfg, value);
      if (link->bucket == NULL) {
          return ERIAK_OUT_OF_MEMORY;
      }
      link->has_bucket = RIAK_TRUE;
      return ERIAK_OK;
}
riak_error
riak_link_set_key(riak_config *cfg,
                  riak_link   *link,
                  riak_binary *value) {
    link->key = riak_binary_copy(cfg, value);
    if (link->key == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    link->has_key = RIAK_TRUE;
    return ERIAK_OK;
}

riak_error
riak_link_set_tag(riak_config *cfg,
                  riak_link   *link,
                  riak_binary *value) {
    link->tag = riak_binary_copy(cfg, value);
    if (link->tag == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    link->has_tag = RIAK_TRUE;
    return ERIAK_OK;
}

//
// R I A K   O B J E C T
//
riak_object*
riak_object_new(riak_config *cfg) {
    return riak_config_clean_allocate(cfg, sizeof(riak_object));
}

riak_error
riak_object_new_array(riak_config   *cfg,
                      riak_object ***array,
                      riak_size_t    len) {
    riak_object **result = riak_config_clean_allocate(cfg, sizeof(riak_object)*len);
    if (result == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *array = result;

    return ERIAK_OK;
}

void
riak_object_free_array(riak_config   *cfg,
                       riak_object ***array_target,
                       riak_size_t    len) {
    int i;
    riak_object **array = *array_target;
    for(i = 0; i < len; i++) {
        riak_object_free(cfg, &(array[i]));
    }
    riak_free(cfg, array_target);
}

riak_error
riak_object_to_pb_copy(riak_config *cfg,
                       RpbContent  *to,
                       riak_object *from) {
    riak_binary_copy_to_pb(&(to->value), from->value);
    if (from->has_charset) {
        to->has_charset = RIAK_TRUE;
        riak_binary_copy_to_pb(&(to->charset), from->charset);
    }
    if (from->has_content_encoding) {
        to->has_content_encoding = RIAK_TRUE;
        riak_binary_copy_to_pb(&(to->content_encoding), from->encoding);
    }
    if (from->has_content_type) {
        to->has_content_type = RIAK_TRUE;
        riak_binary_copy_to_pb(&(to->content_type), from->content_type);
    }
    if (from->has_deleted) {
        to->has_deleted = RIAK_TRUE;
        to->deleted = from->deleted;
    }
    if (from->has_last_mod) {
        to->has_last_mod = RIAK_TRUE;
        to->last_mod = from->last_mod;
    }
    if (from->has_last_mod_usecs) {
        to->has_last_mod_usecs = RIAK_TRUE;
        to->last_mod_usecs = from->last_mod_usecs;
    }
    if (from->has_vtag) {
        to->has_vtag = RIAK_TRUE;
        riak_binary_copy_to_pb(&(to->vtag), from->vtag);
    }

    riak_error err = ERIAK_OK;
    // Indexes
    if (from->n_indexes > 0) {
        to->n_indexes = from->n_indexes;
        err = riak_pairs_copy_to_pb(cfg, &(to->indexes), from->indexes, to->n_indexes);
    }

    // User-Metadave
    if ((from->n_usermeta > 0) && (err == ERIAK_OK)) {
        to->n_usermeta = from->n_usermeta;
        err = riak_pairs_copy_to_pb(cfg, &(to->usermeta), from->usermeta, to->n_usermeta);
    }

    // Links
    if ((from->n_links > 0) && (err == ERIAK_OK)) {
        to->n_links = from->n_links;
        err = riak_links_copy_to_pb(cfg, &(to->links), from->links, to->n_links);
    }

    return err;
}

riak_error
riak_object_new_from_pb(riak_config  *cfg,
                        riak_object **target,
                        RpbContent   *from) {
    *target = riak_object_new(cfg);
    if (*target == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_object *to = *target;

    to->value = riak_binary_copy_from_pb(cfg, &(from->value));
    if (to->value == NULL) {
        riak_free(cfg, target);
        return ERIAK_OUT_OF_MEMORY;
    }
    if (from->has_charset) {
        to->has_charset = RIAK_TRUE;
        to->charset= riak_binary_copy_from_pb(cfg, &(from->charset));
        if (to->charset == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (from->has_content_encoding) {
        to->has_content_encoding = RIAK_TRUE;
        to->encoding = riak_binary_copy_from_pb(cfg, &(from->content_encoding));
        if (to->encoding == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (from->has_content_type) {
        to->has_content_type = RIAK_TRUE;
        to->content_type = riak_binary_copy_from_pb(cfg, &(from->content_type));
        if (to->content_type == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (from->has_deleted) {
        to->has_deleted = RIAK_TRUE;
        to->deleted = from->deleted;
    }
    if (from->has_last_mod) {
        to->has_last_mod = RIAK_TRUE;
        to->last_mod = from->last_mod;
    }
    if (from->has_last_mod_usecs) {
        to->has_last_mod_usecs = RIAK_TRUE;
        to->last_mod_usecs = from->last_mod_usecs;
    }
    if (from->has_vtag) {
        to->has_vtag = RIAK_TRUE;
        to->vtag = riak_binary_copy_from_pb(cfg, &(from->vtag));
        if (to->vtag == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }

    riak_error err = ERIAK_OK;
    // Indexes
    if (from->n_indexes > 0) {
        to->n_indexes = from->n_indexes;
        err = riak_pairs_copy_array_from_pb(cfg, &(to->indexes), from->indexes, to->n_indexes);
    }

    // User-Metadave
    if ((from->n_usermeta > 0) && (err == ERIAK_OK)) {
        to->n_usermeta = from->n_usermeta;
        err = riak_pairs_copy_array_from_pb(cfg, &(to->usermeta), from->usermeta, to->n_usermeta);
    }

    // Links
    if ((from->n_links > 0) && (err == ERIAK_OK)) {
        to->n_links = from->n_links;
        err = riak_links_copy_from_pb(cfg, &(to->links), from->links, to->n_links);
    }

    return err;
}

static int
riak_object_compare_binary(const char             *message,
                                 riak_print_state *state,
                                 riak_binary      *bin1,
                                 riak_binary      *bin2) {
    int result = riak_binary_compare(bin1, bin2);
    if (state && result) {
        riak_print(state, "Object %s differ: ", message);
        riak_print_binary(state, bin1);
        riak_print(state, " vs ");
        riak_print_binary(state, bin2);
        riak_print(state, "\n");
    }
    return result;
}

static int
riak_object_compare_int(const char       *message,
                        riak_print_state *state,
                        riak_uint32_t     int1,
                        riak_uint32_t     int2) {
    int result = (int1 != int2);
    if (state && result) {
        riak_print(state, "Object %s differ: %d vs %d\n", message, int1, int2);
    }
    return result;
}

static int
riak_object_compare_internal(riak_object      *obj1,
                             riak_object      *obj2,
                             riak_print_state *debug) {
    int result = riak_object_compare_binary("Buckets", debug, obj1->bucket, obj2->bucket);
    if (result) return result;
    result = riak_object_compare_binary("Keys", debug, obj1->key, obj2->key);
    if (result) return result;
    result = riak_object_compare_binary("Values", debug, obj1->value, obj2->value);
    if (result) return result;
    result = riak_object_compare_int("Charset Flags", debug, obj1->has_charset, obj2->has_charset);
    if (result) return result;
    result = riak_object_compare_binary("Charset", debug, obj1->charset, obj2->charset);
    if (result) return result;
    result = riak_object_compare_int("Content Type Flags", debug, obj1->has_content_type, obj2->has_content_type);
    if (result) return result;
    result = riak_object_compare_binary("Content Types", debug, obj1->content_type, obj2->content_type);
    if (result) return result;
    result = riak_object_compare_int("Encoding Flags", debug, obj1->has_content_encoding, obj2->has_content_encoding);
    if (result) return result;
    result = riak_object_compare_binary("Encodings", debug, obj1->encoding, obj2->encoding);
    if (result) return result;
    // Only compare VTag and Lost Modified times if the are both non-NULL
    if (obj1->has_vtag && obj2->has_vtag) {
        result = riak_object_compare_binary("VTags", debug, obj1->vtag, obj2->vtag);
        if (result) return result;
    }
    if (obj1->has_last_mod && obj2->has_last_mod) {
        result = riak_object_compare_int("Last Modified Dates", debug, obj1->last_mod, obj2->last_mod);
        if (result) return result;
    }
    if (obj1->has_last_mod_usecs && obj2->has_last_mod_usecs) {
        result = riak_object_compare_int("Last Modified uSecs", debug, obj1->last_mod_usecs, obj2->last_mod_usecs);
        if (result) return result;
    }
    result = riak_object_compare_int("Delete Flags", debug, obj1->deleted, obj2->deleted);
    if (result) return result;
    result = riak_object_compare_int("Number of Indexes", debug, obj1->n_indexes, obj2->n_indexes);
    if (result) return result;
    result = riak_object_compare_int("Number of Links", debug, obj1->n_links, obj2->n_links);
    if (result) return result;
    result = riak_object_compare_int("Number of User Metadata", debug, obj1->n_usermeta, obj2->n_usermeta);
    if (result) return result;

    for(int i = 0; i < obj1->n_indexes; i++) {
        result = riak_object_compare_int("Index Value Flags", debug, obj1->indexes[i]->has_value, obj2->indexes[i]->has_value);
        if (result) return result;
        result = riak_object_compare_binary("Index Keys", debug, obj1->indexes[i]->key, obj2->indexes[i]->key);
        if (result) return result;
        if (obj1->indexes[i]->has_value) {
            result = riak_object_compare_binary("Index Values", debug, obj1->indexes[i]->value, obj2->indexes[i]->value);
            if (result) return result;
        }
    }
    for(int i = 0; i < obj1->n_links; i++) {
        result = riak_object_compare_int("Link Bucket Flags", debug, obj1->links[i]->has_bucket, obj2->links[i]->has_bucket);
        if (result) return result;
        result = riak_object_compare_int("Link Key Flags", debug, obj1->links[i]->has_key, obj2->links[i]->has_key);
        if (result) return result;
        result = riak_object_compare_int("Link Tag Flags", debug, obj1->links[i]->has_tag, obj2->links[i]->has_tag);
        if (result) return result;
        if (obj1->links[i]->has_bucket) {
            result = riak_object_compare_binary("Link Buckets", debug, obj1->links[i]->bucket, obj2->links[i]->bucket);
            if (result) return result;
        }
        if (obj1->links[i]->has_key) {
            result = riak_object_compare_binary("Link Keys", debug, obj1->links[i]->key, obj2->links[i]->key);
            if (result) return result;
        }
        if (obj1->links[i]->has_tag) {
            result = riak_object_compare_binary("Link Tags", debug, obj1->links[i]->tag, obj2->links[i]->tag);
            if (result) return result;
        }
    }
    for(int i = 0; i < obj1->n_usermeta; i++) {
        result = riak_object_compare_int("User Meta-data Value Flags", debug, obj1->usermeta[i]->has_value, obj2->usermeta[i]->has_value);
        if (result) return result;
        result = riak_object_compare_binary("User Meta-data Keys", debug, obj1->usermeta[i]->key, obj2->usermeta[i]->key);
        if (result) return result;
        if (obj1->usermeta[i]->has_value) {
            result = riak_object_compare_binary("User Meta-data Values", debug, obj1->usermeta[i]->value, obj2->usermeta[i]->value);
            if (result) return result;
        }
    }
    return 0;
}

int
riak_object_compare(riak_object   *obj1,
                    riak_object   *obj2) {
    return riak_object_compare_internal(obj1, obj2, NULL);
}

int
riak_object_compare_debug(riak_object      *obj1,
                          riak_object      *obj2,
                          riak_print_state *state) {
    return riak_object_compare_internal(obj1, obj2, state);
}

riak_int32_t
riak_object_print(riak_print_state *state,
                  riak_object      *obj) {
    riak_int32_t wrote = 0;
    wrote += riak_print_label_binary(state, "Bucket", obj->bucket);

    if (obj->has_bucket_type) {
        wrote += riak_print_label_binary(state, "Bucket type", obj->bucket_type);
    } 
    if (obj->has_key) {
        wrote += riak_print_label_binary(state, "Key", obj->key);
    }
    wrote += riak_print_label_binary(state, "Value", obj->value);

    if (obj->has_charset) {
        wrote += riak_print_label_binary(state, "Charsets", obj->charset);
    }
    if (obj->has_last_mod) {
        wrote += riak_print_label_time(state, "Last Mod", obj->last_mod);
    }
    if (obj->has_last_mod_usecs) {
        wrote += riak_print_label_int(state, "Last Mod uSecs", obj->last_mod_usecs);
    }
    if (obj->has_content_type) {
        wrote += riak_print_label_binary(state, "Content Type", obj->content_type);
    }
    if (obj->has_content_encoding) {
        wrote += riak_print_label_binary(state, "Content Encoding", obj->encoding);
    }
    if (obj->has_deleted) {
        wrote += riak_print_label_bool(state, "Deleted", obj->deleted);
    }
    if (obj->has_vtag) {
        wrote += riak_print_label_binary(state, "VTag", obj->vtag);
    }

    for(int i = 0; i < obj->n_links; i++) {
        wrote += riak_links_print(state, obj->links[i]);
    }
    wrote += riak_print(state, "%s\n", "User Meta-data");
    wrote += riak_pairs_print(state, obj->usermeta, obj->n_usermeta);
    wrote += riak_print(state, "%s\n", "Indexes");
    wrote += riak_pairs_print(state, obj->indexes, obj->n_indexes);
    return wrote;
}

void
riak_object_free(riak_config *cfg,
                 riak_object **obj) {
    if (obj == NULL) return;
    riak_object* object = *obj;
    if (object == NULL) return;

    riak_binary_free(cfg, &(object->bucket));
    riak_binary_free(cfg, &(object->charset));
    riak_binary_free(cfg, &(object->content_type));
    riak_binary_free(cfg, &(object->encoding));
    riak_binary_free(cfg, &(object->key));
    riak_binary_free(cfg, &(object->value));
    riak_binary_free(cfg, &(object->vtag));
    riak_pairs_free(cfg, &(object->indexes), object->n_indexes);
    riak_pairs_free(cfg, &(object->usermeta), object->n_usermeta);
    riak_links_free(cfg, &(object->links), object->n_links);
    riak_free(cfg, obj);
}

void
riak_object_free_pb(riak_config *cfg,
                    RpbContent  *obj) {
    riak_pairs_free_pb(cfg, &(obj->indexes), obj->n_indexes);
    riak_pairs_free_pb(cfg, &(obj->usermeta), obj->n_usermeta);
    riak_links_free_pb(cfg, &(obj->links), obj->n_links);
}

//
// ACCESSORS
//
riak_binary*
riak_object_get_bucket(riak_object *obj)
{
    return obj->bucket;
}

riak_binary*
riak_object_get_bucket_type(riak_object *obj)
{
    return obj->bucket_type;
}

riak_boolean_t
riak_object_has_bucket_type(riak_object *obj) {
    return obj->has_bucket_type;
}

riak_boolean_t
riak_object_get_has_key(riak_object *obj)
{
    return obj->has_key;
}

riak_binary*
riak_object_get_key(riak_object *obj)
{
    return obj->key;
}

riak_binary*
riak_object_get_value(riak_object *obj)
{
    return obj->value;
}

riak_boolean_t
riak_object_get_has_charset(riak_object *obj)
{
    return obj->has_charset;
}

riak_binary*
riak_object_get_charset(riak_object *obj)
{
    return obj->charset;
}

riak_boolean_t
riak_object_get_has_last_mod(riak_object *obj)
{
    return obj->has_last_mod;
}

riak_uint32_t
riak_object_get_last_mod(riak_object *obj)
{
    return obj->last_mod;
}

riak_boolean_t
riak_object_get_has_last_mod_usecs(riak_object *obj)
{
    return obj->has_last_mod_usecs;
}

riak_uint32_t
riak_object_get_last_mod_usecs(riak_object *obj)
{
    return obj->last_mod_usecs;
}

riak_boolean_t
riak_object_get_has_content_type(riak_object *obj)
{
    return obj->has_content_type;
}

riak_binary*
riak_object_get_content_type(riak_object *obj)
{
    return obj->content_type;
}

riak_boolean_t
riak_object_get_has_content_encoding(riak_object *obj)
{
    return obj->has_content_encoding;
}

riak_binary*
riak_object_get_content_encoding(riak_object *obj)
{
    return obj->encoding;
}

riak_boolean_t
riak_object_get_has_deleted(riak_object *obj)
{
    return obj->has_deleted;
}

riak_boolean_t
riak_object_get_deleted(riak_object *obj)
{
    return obj->deleted;
}

riak_boolean_t
riak_object_get_has_vtag(riak_object *obj)
{
    return obj->has_vtag;
}

riak_binary*
riak_object_get_vtag(riak_object *obj)
{
    return obj->vtag;
}

riak_int32_t
riak_object_get_n_links(riak_object *obj)
{
    return obj->n_links;
}

riak_error
riak_object_get_link(riak_object  *obj,
                     riak_link   **link,
                     riak_uint32_t n)
{
    if (n >= obj->n_links) {
        return ERIAK_OUT_OF_RANGE;
    }
    *link = obj->links[n];
    return ERIAK_OK;
}

riak_int32_t
riak_object_get_n_usermeta(riak_object *obj)
{
    return obj->n_usermeta;
}

riak_error
riak_object_get_usermeta(riak_object  *obj,
                         riak_pair   **pair,
                         riak_uint32_t n)
{
    if (n >= obj->n_usermeta) {
        return ERIAK_OUT_OF_RANGE;
    }
    *pair = obj->usermeta[n];
    return ERIAK_OK;
}

riak_int32_t
riak_object_get_n_indexes(riak_object *obj)
{
    return obj->n_indexes;
}

riak_error
riak_object_get_index(riak_object  *obj,
                      riak_pair   **index,
                      riak_uint32_t n)
{
    if (n >= obj->n_indexes) {
        return ERIAK_OUT_OF_RANGE;
    }
    *index = obj->indexes[n];
    return ERIAK_OK;
}

riak_error
riak_object_set_bucket(riak_config *cfg,
                       riak_object *obj,
                       riak_binary *value) {
    obj->bucket = riak_binary_copy(cfg, value);
    if (obj->bucket == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_object_set_bucket_type(riak_config *cfg,
                            riak_object *obj,
                            riak_binary *value) {
    obj->bucket_type = riak_binary_copy(cfg, value);
    if (obj->bucket_type == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    obj->has_bucket_type = RIAK_TRUE;
    return ERIAK_OK;
}


riak_error
riak_object_set_key(riak_config *cfg,
                    riak_object *obj,
                    riak_binary *value) {
     obj->key = riak_binary_copy(cfg, value);
     if (obj->key == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     obj->has_key = RIAK_TRUE;
     return ERIAK_OK;
}

riak_error
riak_object_set_value(riak_config *cfg,
                      riak_object *obj,
                      riak_binary *value) {
     obj->value = riak_binary_copy(cfg, value);
     if (obj->value == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     return ERIAK_OK;
}

riak_error
riak_object_set_value_shallow_copy(riak_config *cfg,
                                   riak_object *obj,
                                   riak_binary *value) {
     obj->value = riak_binary_copy_shallow(cfg, value);
     if (obj->value == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     return ERIAK_OK;
}

riak_error
riak_object_set_charset(riak_config *cfg,
                        riak_object *obj,
                        riak_binary *value) {
     obj->charset = riak_binary_copy(cfg, value);
     if (obj->charset == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     obj->has_charset = RIAK_TRUE;
     return ERIAK_OK;
}

void
riak_object_set_last_mod(riak_object  *obj,
                         riak_uint32_t value) {
     obj->last_mod = value;
     obj->has_last_mod = RIAK_TRUE;
}

void
riak_object_set_last_mod_usecs(riak_object  *obj,
                               riak_uint32_t value) {
     obj->last_mod_usecs = value;
     obj->has_last_mod_usecs = RIAK_TRUE;
}

riak_error
riak_object_set_content_type(riak_config *cfg,
                             riak_object *obj,
                             riak_binary *value) {
     obj->content_type = riak_binary_copy(cfg, value);
     if (obj->content_type == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     obj->has_content_type = RIAK_TRUE;
     return ERIAK_OK;
}

riak_error
riak_object_set_content_encoding(riak_config *cfg,
                                 riak_object *obj,
                                 riak_binary *value) {
     obj->encoding = riak_binary_copy(cfg, value);
     if (obj->encoding == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     obj->has_content_encoding = RIAK_TRUE;
     return ERIAK_OK;
}

void
riak_object_set_deleted(riak_object   *obj,
                        riak_boolean_t value) {
     obj->deleted = value;
     obj->has_deleted = RIAK_TRUE;
}

riak_error
riak_object_set_vtag(riak_config *cfg,
                     riak_object *obj,
                     riak_binary *value) {
     obj->vtag = riak_binary_copy(cfg, value);
     if (obj->vtag == NULL) {
         return ERIAK_OUT_OF_MEMORY;
     }
     obj->has_vtag = RIAK_TRUE;
     return ERIAK_OK;
}

riak_link*
riak_object_new_link(riak_config *cfg,
                     riak_object *obj) {
    if (obj->links == NULL) {
        obj->links = riak_config_clean_allocate(cfg, sizeof(riak_link*));
    } else {
         riak_array_realloc(cfg,
                           (void***)&(obj->links),
                           sizeof(riak_link*),
                           obj->n_links,
                           obj->n_links +1);
    }
    if (obj->links == NULL) {
        return NULL;
    }
    riak_link *result = riak_link_new(cfg);
    obj->links[obj->n_links] = result;
    if (result != NULL) {
        obj->n_links++;
    }
    return result;
}

riak_pair*
riak_object_new_usermeta(riak_config *cfg,
                         riak_object *obj) {
    if (obj->usermeta == NULL) {
        obj->usermeta = riak_config_clean_allocate(cfg, sizeof(riak_pair*));
    } else {
         riak_array_realloc(cfg,
                           (void***)&(obj->usermeta),
                           sizeof(riak_pair*),
                           obj->n_usermeta,
                           obj->n_usermeta +1);
    }
    if (obj->usermeta == NULL) {
        return NULL;
    }
    riak_pair *result = riak_pair_new(cfg);
    obj->usermeta[obj->n_usermeta] = result;
    if (result != NULL) {
        obj->n_usermeta++;
    }

    return result;
}

riak_pair*
riak_object_new_index(riak_config *cfg,
                      riak_object *obj) {
    if (obj->indexes == NULL) {
        obj->indexes = riak_config_clean_allocate(cfg, sizeof(riak_pair*));
    } else {
         riak_array_realloc(cfg,
                           (void***)&(obj->indexes),
                           sizeof(riak_pair*),
                           obj->n_indexes,
                           obj->n_indexes +1);
    }
    if (obj->indexes == NULL) {
        return NULL;
    }
    riak_pair *result = riak_pair_new(cfg);
    obj->indexes[obj->n_indexes] = result;
    if (result != NULL) {
        obj->n_indexes++;
    }

    return result;
}
