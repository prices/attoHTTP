/**
 * @file    src/attohttp.c
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


#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "attohttp.h"

#ifndef ATTOHTTP_PRINTF_BUFFER_SIZE
# define ATTOHTTP_PRINTF_BUFFER_SIZE 128
#endif
#ifndef ATTOHTTP_URL_BUFFER_SIZE
# define ATTOHTTP_URL_BUFFER_SIZE 64
#endif


/***************************************************************************
 *                              Private Parameters
 * @cond dev
 ***************************************************************************/
httpmethod_t _attoHTTPMethod;
httpversion_t _attoHTTPVersion;
char _attoHTTP_url[ATTOHTTP_URL_BUFFER_SIZE];
char *_attoHTTP_body;
uint16_t _attoHTTP_url_len;
uint16_t _attoHTTP_body_len;
void *_attoHTTP_read;
void *_attoHTTP_write;
uint8_t _headersDone;
returncode_t _returnCode;

/***************************************************************************
 * @endcond
 ***************************************************************************/


/***************************************************************************
 *                              Private Members
 * @cond dev
 ***************************************************************************/
/**
 * @brief Initiialized the variables
 *
 * @return none
 */
void
attoHTTPInit(void)
{
    _attoHTTPMethod = NOTSUPPORTED;
    _attoHTTPVersion = VUNKNOWN;
    _attoHTTP_body = NULL;
    _attoHTTP_url_len = 0;
    _attoHTTP_body_len = 0;
    _headersDone = 0;
    _returnCode = OK;
}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseEOL(char c)
{
    uint8_t ret = 1;
    int8_t eolCount = 0;
    // Remove any extra space
    while (isspace(c) && (ret != 0)) {
        if (c == '\n') {
            eolCount++;
        }
        ret = attoHTTPGetByte(_attoHTTP_read, &c);
    }
    if (eolCount > 1) {
        _headersDone = 1;
    } else if (eolCount == 0) {
        _returnCode = BADREQUEST;
    }
    return eolCount;
}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseMethod()
{
    uint8_t ret;
    char c;
    char buffer[10];
    uint16_t ptr;
    // Remove any extra space
    do {
        ret = attoHTTPGetByte(_attoHTTP_read, &c);
    } while (isblank(c) && ret);

    if (ret) {
        ptr = 0;
        do {
            buffer[ptr++] = c;
            ret = attoHTTPGetByte(_attoHTTP_read, &c);
        } while (!isblank(c) && ret && (ptr < 9));
        buffer[ptr] = 0;

        // Get rid of any white space
        if (strncmp(HTTP_METHOD_GET, buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = GET;
        } else if (strncmp(HTTP_METHOD_POST, buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = POST;
        } else if (strncmp(HTTP_METHOD_PUT, buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = PUT;
        } else if (strncmp(HTTP_METHOD_DELETE, buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = DELETE;
        } else if (strncmp(HTTP_METHOD_PATCH, buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = PATCH;
        } else {
            _returnCode = UNSUPPORTED;
        }
    }
    return ret;

}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseURI()
{
    uint8_t ret;
    char c;
    // Remove any extra space
    do {
        ret = attoHTTPGetByte(_attoHTTP_read, &c);
    } while (isblank(c) && (ret != 0));

    if (ret) {
        _attoHTTP_url_len = 0;
        do {
            _attoHTTP_url[_attoHTTP_url_len++] = c;
            ret = attoHTTPGetByte(_attoHTTP_read, &c);

        } while (!isblank(c) && ret && (_attoHTTP_url_len < (sizeof(_attoHTTP_url) - 1)));
        _attoHTTP_url[_attoHTTP_url_len] = 0;
    }
    return ret;

}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseVersion()
{
    uint8_t ret;
    char c;
    char buffer[10];
    uint16_t ptr;
    // Remove any extra space
    do {
        ret = attoHTTPGetByte(_attoHTTP_read, &c);
    } while (isblank(c) && (ret != 0));

    if (ret) {
        ptr = 0;
        do {
            buffer[ptr++] = c;
            ret = attoHTTPGetByte(_attoHTTP_read, &c);
        } while (!isspace(c) && ret && (ptr < (sizeof(buffer) - 1)));
        buffer[ptr] = 0;
        _attoHTTPVersion = VUNKNOWN;

        if (strncmp(HTTP_VERSION_1_0, buffer, ptr) == 0) {
            _attoHTTPVersion = V1_0;
        } else if (strncmp(HTTP_VERSION_1_1, buffer, ptr) == 0) {
            _attoHTTPVersion = V1_1;
        }
    }
    ret = attoHTTPParseEOL(c);

    return ret;
}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseHeaders()
{
    int8_t ret = 1;
    if (_headersDone == 0) {

    }
    return ret;
}
uint16_t
attoHTTPwrite(const char *buffer, uint16_t len)
{
    uint16_t ret = 0;
    while (len-- > 0) {
        ret += attoHTTPSetByte(_attoHTTP_write, *buffer++);
    }
    return ret;
}

uint16_t
attoHTTPprintf(const char *format, ...)
{
    char buffer[ATTOHTTP_PRINTF_BUFFER_SIZE];
    uint16_t count;
    va_list ap;
    va_start(ap, format);
    count = vsnprintf(buffer, 128, format, ap);
    va_end(ap);
    return attoHTTPwrite(buffer, count);
}
uint16_t
attoHTTPprint(const char *buffer)
{
    return attoHTTPwrite(buffer, strlen(buffer));
}
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
uint8_t
attoHTTPOK()
{
    return attoHTTPprint(HTTP_VERSION " 200 OK\r\n");
}
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
uint8_t
attoHTTPBadRequest()
{
    return attoHTTPprint(HTTP_VERSION " 400 Bad Request\r\n");
}
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
uint8_t
attoHTTPInternalError()
{
    return attoHTTPprint(HTTP_VERSION " 500 Internal Error\r\n");
}
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
uint8_t
attoHTTPNotImplemented()
{
    return attoHTTPprint(HTTP_VERSION " 501 Not Implemented\r\n");
}
/***************************************************************************
 * @endcond
 ***************************************************************************/

/**
 * @brief Main function that runs everything
 *
 * This function sets up the server.  It should be called once per session,
 * with extra being whatever key is needed to access the server
 *
 * @param read This will be sent as the first argument to the get
 *             data functions.  It could be anything.
 * @param write This will be sent as the first argument to the send
 *             data functions.  It could be anything.
 *
 * @return @see returncode_t for details
 */
returncode_t
attoHTTPExecute(void *read, void *write)
{
    _attoHTTP_read = read;
    _attoHTTP_write = write;

    // Init all of the variables.
    attoHTTPInit();
    // Parse the first line
    attoHTTPParseMethod();

    if (_returnCode == OK) {
        attoHTTPParseURI();
        attoHTTPParseVersion();
    }
    switch (_returnCode) {
        case OK:
            attoHTTPOK();
            break;
        case UNSUPPORTED:
            attoHTTPNotImplemented();
            break;
        case BADREQUEST:
            attoHTTPBadRequest();
            break;
        default:
            attoHTTPInternalError();
            break;
    }
    return _returnCode;

}
