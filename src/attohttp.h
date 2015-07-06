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

#include "attohttp_config.h"

#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_PUT "PUT"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_PATCH "PATCH"
#define HTTP_METHOD_DELETE "DELETE"

#define HTTP_VERSION_1_0 "HTTP/1.0"
#define HTTP_VERSION_1_1 "HTTP/1.1"

#define HTTP_VERSION HTTP_VERSION_1_0


/** This flag tells attoHTTP that all pages are gzipped */
#define ATTOHTTP_FLAG_GZIP (1<<0)


#ifndef ATTOHTTP_PAGE_URL_SIZE
#  define ATTOHTTP_PAGE_URL_SIZE 32
#endif

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
    UNSUPPORTED,
    BADREQUEST,
    INTERNAL_ERROR,
    NOT_FOUND
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
    PATCH,
    NOTSUPPORTED
} httpmethod_t;

/**
 * @brief The version of HTTP
 *
 * These represent the version of HTTP that the client is using.
 */
typedef enum
{
    VUNKNOWN,
    V1_0,
    V1_1
} httpversion_t;

/**
 * @brief The version of HTTP
 *
 * These represent the version of HTTP that the client is using.
 */
typedef enum
{
    APPLICATION_JSON = 0,
    TEXT_HTML = 1,
    TEXT_PLAIN = 2,
    TEXT_CSS = 3,
    APPLICATION_JAVASCRIPT = 4

} mimetypes_t;
#define ATTOHTTP_MIME_TYPES 3


/**
 * @brief This keeps track of our pages
 *
 * This struct keeps track of pages and what to load for them.
 */
typedef struct {
    char url[ATTOHTTP_PAGE_URL_SIZE];
    const char *content;
    uint16_t size;
    mimetypes_t type;
} attoHTTPPage;


returncode_t attoHTTPExecute(void *read, void *write);
uint8_t attoHTTPOK();
uint8_t attoHTTPAccepted();
uint8_t attoHTTPBadRequest();
uint8_t attoHTTPNotFound();
uint8_t attoHTTPInternalError();
uint8_t attoHTTPNotImplemented();
uint8_t attoHTTPSendHeaders();
void attoHTTPInit(uint16_t flags);
uint8_t attoHTTPAddPage(const char *url, const char *page, uint16_t page_len, mimetypes_t type);
uint8_t attoHTTPDefaultPage(const char *url, const char *page, uint16_t page_len, mimetypes_t type);
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
//uint16_t attoHTTPGetByte(void *read, char *byte);
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
//uint16_t attoHTTPSetByte(void *write, char byte);


#define __ATTOHTTP_H_DONE__
#endif // #ifndef __ATTOHTTP_H__
