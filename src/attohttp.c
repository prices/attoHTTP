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
char * _attoHTTP_url;
/***************************************************************************
 * @endcond
 ***************************************************************************/


/***************************************************************************
 *                              Private Members
 * @cond dev
 ***************************************************************************/
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
attoHTTPGetMethod(char *buffer, uint16_t len)
{
    // Get rid of any white space
    while (isspace(*buffer) && (len > 0)) {
        buffer++;
        len--;
    }
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
    _attoHTTP_url = buffer;
    return 1;
}

/***************************************************************************
 * @endcond
 ***************************************************************************/

/**
 * @brief Main function that runs everything
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
returncode_t
attoHTTPExecute(char *buffer, uint16_t len)
{
    returncode_t ret = OK;


    return ret;

}
