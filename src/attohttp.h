/**
 * @file    src/attohttp.h
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
#ifndef __ATTOHTTP_H__
#define __ATTOHTTP_H__


#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_PUT "PUT"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_PATCH "PATCH"
#define HTTP_METHOD_DELETE "DELETE"
/**
 * @brief The return status from the HTTP code
 *
 * This is the return status for the main functions of the HTTP server.
 *  * `OK`           Everything worked.
 *  * `SERVER_ERROR` Some unknown thing went wrong.
 */
typedef enum
{
    OK,
    SERVER_ERROR,
} returncode_t;

/**
 * @brief These are the different HTTP methods
 *
 * These represent the different HTTP methods that we support.
 */
typedef enum
{
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
} httpmethod_t;

/**
 * @brief The version of HTTP
 *
 * These represent the version of HTTP that the client is using.
 */
typedef enum
{
    V1_0,
    V1_1
} httpversion_t;


#endif // #ifndef __ATTOHTTP_H__
