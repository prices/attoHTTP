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

/***************************************************************************
 *                              Private Parameters
 * @cond dev
 ***************************************************************************/
httpmethod_t _attoHTTPCode;
httpversion_t _attoHTTPVersion;
char *_attoHTTP_url;
char *_attoHTTP_headers;
char *_attoHTTP_body;
uint16_t _attoHTTP_url_len;
uint16_t _attoHTTP_body_len;
uint16_t _attoHTTP_headers_len;
void *_attoHTTP_read;
void *_attoHTTP_write;

#ifndef ATTOHTTP_PRINTF_BUFFER_SIZE
# define ATTOHTTP_PRINTF_BUFFER_SIZE 128
#endif

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
    _attoHTTPCode = NOTSUPPORTED;
    _attoHTTPVersion = VUNKNOWN;
    _attoHTTP_url = NULL;
    _attoHTTP_headers = NULL;
    _attoHTTP_body = NULL;
    _attoHTTP_url_len = 0;
    _attoHTTP_body_len = 0;
    _attoHTTP_headers_len = 0;
}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * This function sets up the server.  It should be called once per session,
 * with the data from the client stored in *buffer.
 *
 * @param buffer This is the buffer that the incoming client data is stored in.
 *               The calling program can decide how big it should be, but it must
 *               be big enough for all of the headers.
 * @param len    This is the total length of the data in the buffer.
 *
 * @return @see returncode_t for details
 */
uint8_t
attoHTTPParseInitialRequestLine(char *buffer, uint16_t len)
{
    // Remove any extra space
    while (isblank(*buffer) && (len > 0)) {
        buffer++;
        len--;
    }
    uint8_t ctr = 0;
    // Get rid of any white space
    if (strncmp(HTTP_METHOD_GET, buffer, sizeof(HTTP_METHOD_GET)) == 0) {
        _attoHTTPCode = GET;
        len -= sizeof(HTTP_METHOD_GET) + 1;
        buffer += sizeof(HTTP_METHOD_GET) + 1;
    } else if (strncmp(HTTP_METHOD_POST, buffer, sizeof(HTTP_METHOD_POST)) == 0) {
        _attoHTTPCode = POST;
        len -= sizeof(HTTP_METHOD_POST) + 1;
        buffer += sizeof(HTTP_METHOD_GET) + 1;
    } else if (strncmp(HTTP_METHOD_PUT, buffer, sizeof(HTTP_METHOD_PUT)) == 0) {
        _attoHTTPCode = PUT;
        len -= sizeof(HTTP_METHOD_PUT) + 1;
        buffer += sizeof(HTTP_METHOD_PUT) + 1;
    } else if (strncmp(HTTP_METHOD_DELETE, buffer, sizeof(HTTP_METHOD_DELETE)) == 0) {
        _attoHTTPCode = POST;
        len -= sizeof(HTTP_METHOD_DELETE) + 1;
        buffer += sizeof(HTTP_METHOD_DELETE) + 1;
    } else if (strncmp(HTTP_METHOD_PATCH, buffer, sizeof(HTTP_METHOD_PATCH)) == 0) {
        _attoHTTPCode = POST;
        len -= sizeof(HTTP_METHOD_PATCH) + 1;
        buffer += sizeof(HTTP_METHOD_PATCH) + 1;
    }
    // Remove any extra space
    while (isblank(*buffer) && (len > 0)) {
        buffer++;
        len--;
    }
    _attoHTTP_url_len = 0;
    _attoHTTP_url = buffer;
    // Get the URL Length
    while (!isblank(*buffer) && (len > 0)) {
        buffer++;
        _attoHTTP_url_len++;
        len--;
    }
    // Remove any extra space
    while (isblank(*buffer) && (len > 0)) {
        buffer++;
        len--;
    }
    if (len > 0) {
        _attoHTTPVersion = VUNKNOWN;
        if (strncmp(HTTP_VERSION_1_0, buffer, sizeof(HTTP_VERSION_1_0)) == 0) {
            _attoHTTPVersion = V1_0;
            len-=sizeof(HTTP_VERSION_1_0);
            buffer += sizeof(HTTP_VERSION_1_0);
        } else if (strncmp(HTTP_VERSION_1_1, buffer, sizeof(HTTP_VERSION_1_1)) == 0) {
            _attoHTTPCode = V1_1;
            len-=sizeof(HTTP_VERSION_1_1);
            buffer += sizeof(HTTP_VERSION_1_1);
        }
    }
    while (((*buffer == '\n') || (*buffer == '\r')) && (len > 0) && ctr < 2) {
        buffer++;
        _attoHTTP_url_len++;
        len--;
        ctr++;
    }
    _attoHTTP_headers = buffer;
    return (len >= 0);
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
    returncode_t ret = OK;
    _attoHTTP_read = read;
    _attoHTTP_write = write;

    attoHTTPInit();

    attoHTTPOK();

    return ret;

}
