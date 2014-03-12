/*********************************************************************
 *
 * riak_operation.h: A single Riak Operation
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

#ifndef RIAK_OPERATION_H_
#define RIAK_OPERATION_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_operation riak_operation;

// Generic placeholder for message-specific callbacks
typedef void (*riak_response_callback)(void *response, void *ptr);

/**
 * @brief Construct a Riak event
 * @param cxn Riak Connection
 * @param rop Riak Operation
 * @param response_cb Reaponse callback function (user-supplied)
 * @param cb_data Pointer passed to `response_cb` when it is called
 * @returns Error code
 */
riak_error
riak_operation_new(riak_connection        *cxn,
                    riak_operation       **rop,
                    riak_response_callback response_cb,
                    riak_response_callback error_cb,
                    void                  *cb_data);

/**
 * @brief Set the event's callback data
 * @param rop Riak Operation
 * @param cb_data Pointer to data used in user's callback
 */
void
riak_operation_set_cb_data(riak_operation     *rop,
                           void               *cb_data);

/**
 * @brief Set the event's response callback
 * @param rop Riak Operation
 * @param cb Function pointer to response callback
 */
void
riak_operation_set_response_cb(riak_operation          *rop,
                                riak_response_callback  cb);

/**
 * @brief Set the event's error callback
 * @param rop Riak Operation
 * @param cb Function pointer to error callback
 */
void
riak_operation_set_error_cb(riak_operation         *rop,
                            riak_response_callback  cb);

/**
 * @brief Cleanup memory used by a Riak Operation
 * @param re Riak Operation
 */
void
riak_operation_free(riak_operation** rop);

/**
 * @brief Return the Riak Configuration
 * @param rop Riak Operation
 * @returns The corresponding Riak Configuration
 */
riak_config*
riak_operation_get_config(riak_operation *rop);

/**
 * @brief Return the Riak Connection
 * @param rop Riak Operation
 * @returns The corresponding Riak Connection
 */
riak_connection*
riak_operation_get_connection(riak_operation *rop);

/**
 * @brief Get the optional server error
 * @param rop Riak Operation
 * @returns Error message sent from the server
 */
riak_server_error*
riak_operation_get_server_error(riak_operation *rop);

/**
 * @brief Set the bucket on the current operation
 * @param rop Riak Operation
 * @param bucket Name of bucket in Riak
 */
void
riak_operation_set_bucket(riak_operation *rop,
                          riak_binary    *bucket);

/**
 * @brief Set the bucket type on the current operation
 * @param rop Riak Operation
 * @param bucket Name of bucket type in Riak
 */
void
riak_operation_set_bucket_type(riak_operation *rop,
                          riak_binary    *bucket_type);


/**
 * @brief Set the bucket on the current operation
 * @param rop Riak Operation
 * @param key Name of key in Riak bucket
 */
void
riak_operation_set_key(riak_operation *rop,
                       riak_binary    *key);

/**
 * @brief Set the Seconardary Index on the current operation
 * @param rop Riak Operation
 * @param key Name of 2i in Riak bucket
 */
void
riak_operation_set_index(riak_operation *rop,
                         riak_binary    *key);

/**
 * @brief Gets the bucket on the current operation
 * @param rop Riak Operation
 * @returns Name of bucket in Riak
 */
riak_binary*
riak_operation_get_bucket(riak_operation *rop);

/**
 * @brief Gets the bucket type on the current operation
 * @param rop Riak Operation
 * @returns Name of bucket type in Riak
 */
riak_binary*
riak_operation_get_bucket_type(riak_operation *rop);

/**
 * @brief returns TRUE if bucket_type is defined
 * @param rop Riak Operation
 * @returns returns TRUE if bucket_type is defined
 */
riak_boolean_t
riak_operation_has_bucket_type(riak_operation *rop);


/**
 * @brief Gets the key on the current operation
 * @param rop Riak Operation
 * @returns Name of key in Riak bucket
 */
riak_binary*
riak_operation_get_key(riak_operation *rop);

/**
 * @brief Gets the Secondary Index on the current operation
 * @param rop Riak Operation
 * @returns Name of 2i in Riak bucket
 */
riak_binary*
riak_operation_get_index(riak_operation *rop);

#ifdef __cplusplus
}
#endif

#endif
