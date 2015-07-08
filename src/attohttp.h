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
/**
 * @page char_fcts User Defined Character Functions
 * @section char_fcts_get attoHTTPGetByte
 * @subsection char_fcts_get_prototype Prototype
 * @code
 * uint16_t attoHTTPGetByte(void *read, uint8_t *byte);
 * @endcode
 *
 * @subsection char_fcts_get_explain Explaination
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
 *
 *
 * @section char_fcts_set attoHTTPSetByte
 * @subsection char_fcts_set_prototype Prototype
 * @code
 * uint16_t attoHTTPSetByte(void *write, uint8_t byte);
 * @endcode
 *
 * @subsection char_fcts_set_explain Explaination
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
 *
 *
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

#define HTTPEOL "\r\n"


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
    NOT_IMPLEMENTED,
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
#define ATTOHTTP_MIME_TYPES 5

typedef returncode_t (*attoHTTPDefAPICallback)(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl);

/**
 * @brief This keeps track of our pages
 *
 * This struct keeps track of pages and what to load for them.
 */
typedef struct {
    char url[ATTOHTTP_PAGE_URL_SIZE];
    const uint8_t *content;
    uint16_t size;
    mimetypes_t type;
} attoHTTPPage_t;

/**
 * @brief This keeps track of our pages
 *
 * This struct keeps track of pages and what to load for them.
 */
typedef struct _attoHTTPRestAPI {
    char url[ATTOHTTP_PAGE_URL_SIZE];
    const uint8_t *content;
    uint16_t size;
    mimetypes_t type;
} attoHTTPRestAPI_t;

#ifdef __cplusplus
extern "C" {
#endif
returncode_t attoHTTPExecute(void *read, void *write);
uint8_t attoHTTPSendHeaders();
void attoHTTPInit(void);
uint8_t attoHTTPAddPage(const char *url, const uint8_t *page, uint16_t page_len, mimetypes_t type);
uint8_t attoHTTPDefaultPage(const char *url, const uint8_t *page, uint16_t page_len, mimetypes_t type);
uint16_t attoHTTPwrite(const uint8_t *buffer, uint16_t len);
uint16_t attoHTTPprintf(const char *format, ...);
uint16_t attoHTTPprint(const char *buffer);
uint8_t attoHTTPDefaultREST(attoHTTPDefAPICallback Callback);
uint16_t attoHTTPRESTSendHeaders(uint16_t code, char *type, char *headers);
uint16_t attoHTTPFirstLine(uint16_t code);

#ifdef __cplusplus
}
#endif

/***********************************************************************
 *                   Inline Functions
 ***********************************************************************/
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPOK()
{

    return attoHTTPFirstLine(200);

}
/**
 * @brief This prints out the Accepted message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPAccepted()
{
    return attoHTTPFirstLine(202);
}
/**
 * @brief This prints out the Bad Request Message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPBadRequest()
{
    return attoHTTPFirstLine(400);
}
/**
 * @brief This prints out the Not Found Message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPNotFound()
{
    return attoHTTPFirstLine(404);
}
/**
 * @brief This prints out the Internal Error message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPInternalError()
{
    return attoHTTPFirstLine(500);
}
/**
 * @brief This prints out the Not Impelemented message
 *
 * @return The number of characters printed
 */
static inline uint8_t
attoHTTPNotImplemented()
{
    return attoHTTPFirstLine(501);
}


#define __ATTOHTTP_H_DONE__
#endif // #ifndef __ATTOHTTP_H__
