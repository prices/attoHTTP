/**
 * @file    test/attohttp_config.h
 * @author  Scott L. Price <prices@dflytech.com>
 * @note    (C) 2015  Scott L. Price
 * @brief   A small http server for embedded systems
 * @details
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Scott Price
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __ATTOHTTP_CONFIG_H__
#define __ATTOHTTP_CONFIG_H__

/**
 * @brief This is the size of the character buffer that printf uses
 *
 * This will be dynamically allocated when printf is called.  The bigger
 * it is, the larger the strings that can be made are.
 *
 * Defaults to 128 if not set
 */
#undef ATTOHTTP_PRINTF_BUFFER_SIZE

/**
 * @brief If this flag is set, all pages are expected to be gzipped.
 *
 * All pages must be gzipped if this flag is set.  API Pages should still
 * not be gzipped, however.
 *
 * Defaults to not set
 */
#undef ATTOHTTP_GZIP_PAGES

/**
 * @brief If this flag is set, digest auth is required for all pages.
 *
 * Defaults to not set
 */
#define ATTOHTTP_DIGEST_AUTH

/**
 * @brief User function to get a byte
 *
 * This function must be defined by the user.  It will allow this software to
 * get bytes from any source.
 *
 * @param read This is whatever it needs to be.  Could be a socket, or an object,
 *              or something totally different.  It will be called with whatever
 *              extra argument was given to the execute routine.
 * @param byte  A pointer to the byte we need to put the next character in.
 *
 * @return 1 if a character was read, 0 otherwise.
 */
uint16_t attoHTTPGetByte(void *read, uint8_t *byte);
/**
 * @brief User function to set a byte
 *
 * This function must be defined by the user.  It will allow this software to
 * set bytes to any destination.
 *
 * @param write This is whatever it needs to be.  Could be a socket, or an object,
 *              or something totally different.  It will be called with whatever
 *              extra argument was given to the execute routine.
 * @param byte  A pointer to the byte we need to put the next character in.
 *
 * @return 1 if a character was read, 0 otherwise.
 */
uint16_t attoHTTPSetByte(void *write, uint8_t byte);
/**
 * @brief Checks the Auth, based on what is given in the Authorization header
 *
 * @param auth The authentication type
 * @param cred The credential string, NULL terminated
 * 
 * @return 1 if the auth succeeded, 0 otherwise
 */
int8_t attoHTTPWrapperCheckAuth(uint8_t auth, int8_t *cred);

#endif // #ifndef __ATTOHTTP_CONFIG_H__
