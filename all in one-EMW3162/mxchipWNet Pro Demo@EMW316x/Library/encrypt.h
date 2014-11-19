#ifndef __ENCRYPT_H_
#define __ENCRYPT_H_

#include "mxchipWNET.h"

typedef struct
{
    u32 total[2];
    u32 state[4];
    u8  buffer[64];

    u8  ipad[64];
    u8  opad[64];
} md5_context;


/**
 * @brief          MD5 context setup
 *
 * @param ctx      context to be initialized
 */
void md5_starts( md5_context *ctx );

/**
 * @brief          MD5 process buffer
 *
 * @param ctx      MD5 context
 * @param input    buffer holding the  data
 * @param ilen     length of the input data
 */
void md5_update( md5_context *ctx, unsigned char *input, int ilen );

/**
 * @brief          MD5 final digest
 *
 * @param ctx      MD5 context
 * @param output   MD5 checksum result
 */
void md5_finish( md5_context *ctx, unsigned char output[16] );


#endif /* ENCRYPT_H_ */

