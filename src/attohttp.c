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
#include <strings.h>
#include <ctype.h>
#include "attohttp.h"

#ifndef ATTOHTTP_PRINTF_BUFFER_SIZE
# define ATTOHTTP_PRINTF_BUFFER_SIZE 128
#endif
#ifndef ATTOHTTP_URL_BUFFER_SIZE
# define ATTOHTTP_URL_BUFFER_SIZE 64
#endif
#ifndef ATTOHTTP_HEADER_NAME_SIZE
# define ATTOHTTP_HEADER_NAME_SIZE 32
#endif
#ifndef ATTOHTTP_HEADER_VALUE_SIZE
# define ATTOHTTP_HEADER_VALUE_SIZE 64
#endif


#define _attoHTTPPushC(char) _attoHTTP_extra_c = char

/***************************************************************************
 *                              Private Parameters
 * @cond dev
 ***************************************************************************/
httpmethod_t _attoHTTPMethod;
httpversion_t _attoHTTPVersion;
char _attoHTTP_url[ATTOHTTP_URL_BUFFER_SIZE];
char _attoHTTP_extra_c;
char *_attoHTTP_body;
uint16_t _attoHTTP_url_len;
uint16_t _attoHTTP_body_len;
void *_attoHTTP_read;
void *_attoHTTP_write;
uint8_t _headersDone;
returncode_t _returnCode;
mimetypes_t _attoHTTP_accept;

/** This is a map of our mime types */
static const char *_mimetypes[] = {
    [application_json] = "application/json",
    [text_html] = "text/html",
    [text_plain] = "text/plain",
};

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
    _attoHTTP_extra_c = -1;
}

/**
 * @brief Finds one or more EOL
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPReadC(char *c)
{
    int8_t ret = 1;
    if (_attoHTTP_extra_c > 0) {
        *c = _attoHTTP_extra_c;
        _attoHTTP_extra_c = -1;
    } else if (_attoHTTP_extra_c == 0) {
        *c = 0;
    } else {
        ret = attoHTTPGetByte(_attoHTTP_read, c);
    }
    return ret;
}
/**
 * @brief Finds one or more EOL
 *
 * @return 1 if there is more to read, 0 if done reading
 */
static inline int8_t
attoHTTPWriteC(char c)
{
    return attoHTTPSetByte(_attoHTTP_write, c);
}
/**
 * @brief Finds one or more EOL
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseSpace(void)
{
    int8_t ret;
    char c;
    do {
        ret = attoHTTPReadC(&c);
    } while (isblank(c) && (ret != 0));
    _attoHTTPPushC(c);
    return ret;
}
/**
 * @brief Finds one or more EOL
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseEOL(void)
{
    uint8_t ret = 1;
    char c;
    int8_t eolCount = 0;
    // Remove any extra space
    do {
        ret = attoHTTPReadC(&c);
        if (c == '\n') {
            eolCount++;
        }
    } while (isspace(c) && (ret != 0));
    _attoHTTPPushC(c);
    if (eolCount > 1) {
        _headersDone = 1;
    } else if (eolCount == 0) {
        _returnCode = BADREQUEST;
    }
    return ret;
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
    char buffer[10];
    uint16_t ptr;
    // Remove any extra space
    ret = attoHTTPParseSpace();

    if (ret) {
        ptr = 0;
        do {
            ret = attoHTTPReadC(&buffer[ptr]);
            if (isblank(buffer[ptr])) {
                break;
            } else {
                ptr++;
            }
        } while (ret && (ptr < (sizeof(buffer) - 1)));
        buffer[ptr] = 0;

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
    // Remove any extra space
    ret = attoHTTPParseSpace();

    if (ret) {
        _attoHTTP_url_len = 0;
        do {
            ret = attoHTTPReadC(&_attoHTTP_url[_attoHTTP_url_len]);
            if (isblank(_attoHTTP_url[_attoHTTP_url_len])) {
                break;
            } else {
                _attoHTTP_url_len++;
            }
        } while (ret && (_attoHTTP_url_len < (sizeof(_attoHTTP_url) - 1)));
        _attoHTTPPushC(_attoHTTP_url[_attoHTTP_url_len]);
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
    char buffer[10];
    uint16_t ptr;
    // Remove any extra space
    ret = attoHTTPParseSpace();

    if (ret) {
        ptr = 0;
        do {
            ret = attoHTTPReadC(&buffer[ptr]);
            if (isspace(buffer[ptr])) {
                break;
            } else {
                ptr++;
            }
        } while (ret && (ptr < (sizeof(buffer) - 1)));
        _attoHTTPPushC(buffer[ptr]);
        buffer[ptr] = 0;
        _attoHTTPVersion = VUNKNOWN;
        if (strncmp(HTTP_VERSION_1_0, buffer, ptr) == 0) {
            _attoHTTPVersion = V1_0;
        } else if (strncmp(HTTP_VERSION_1_1, buffer, ptr) == 0) {
            _attoHTTPVersion = V1_1;
        }
    }

    ret = attoHTTPParseEOL();

    return ret;
}
/**
 * @brief Finds the method, url, and HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseHeader(char *name, uint16_t namesize, char *value, uint16_t valuesize)
{
    uint8_t ret;
    do {
        ret = attoHTTPReadC(name);
        if (*name == ':') {
            break;
        } else {
            name++;
            namesize--;
        }
    } while ((ret > 0) && (namesize > 0));
    *name = 0; // Terminate the string

    ret = attoHTTPParseSpace();

    do {
        ret = attoHTTPReadC(value);
        if ((*value == '\r') || (*value == '\n')) {
            break;
        } else {
            value++;
            valuesize--;
        }
    } while ((ret > 0) && (valuesize > 0));
    _attoHTTPPushC(*value);
    *value = 0;  // Terminate the string

    ret = attoHTTPParseEOL();
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
    uint8_t i;
    char name[ATTOHTTP_HEADER_NAME_SIZE];
    char value[ATTOHTTP_HEADER_VALUE_SIZE];

    while ((_headersDone == 0) && (ret != 0)) {
        ret = attoHTTPParseHeader(name, sizeof(name), value, sizeof(value));
        if (strncasecmp(name, "accept", sizeof(name)) == 0) {
            for (i = 0; i < ATTOHTTP_MIME_TYPES; i++) {
                if (strncasecmp(value, _mimetypes[i], sizeof(value)) == 0) {
                    _attoHTTP_accept = i;
                }
            }
        }
    }
    return ret;
}
uint16_t
attoHTTPwrite(const char *buffer, uint16_t len)
{
    uint16_t ret = 0;
    while (len-- > 0) {
        ret += attoHTTPWriteC(*buffer++);
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
