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
#include <stdlib.h>
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
#ifndef ATTOHTTP_PAGE_BUFFERS
# define ATTOHTTP_PAGE_BUFFERS 8
#endif
#ifndef ATTOHTTP_API_BUFFERS
# define ATTOHTTP_API_BUFFERS 8
#endif
#ifndef ATTOHTTP_API_LEVELS
# define ATTOHTTP_API_LEVELS 3
#endif


#define _attoHTTPCheckPage(page)  (!_attoHTTPPageEmpty(page) && (0 == strncmp((char *)_attoHTTP_url, (char *)page.url, sizeof(page.url))))
#define _attoHTTPDefaultPage() (!_attoHTTPPageEmpty(_attoHTTPDefaultPage) && (strncmp((char *)_attoHTTP_url, "/", sizeof(_attoHTTP_url)) == 0) && (_attoHTTP_url_len == 1))
#define _attoHTTPPushC(char) _attoHTTP_extra_c = char
#define _attoHTTPPageEmpty(page) (page.content == NULL)

/***************************************************************************
 *                              Private Parameters
 * @cond dev
 ***************************************************************************/
httpmethod_t _attoHTTPMethod;
httpversion_t _attoHTTPVersion;
uint8_t _attoHTTP_url[ATTOHTTP_URL_BUFFER_SIZE];
// This needs to be bigger than a character to save a character + sign information
int16_t _attoHTTP_extra_c;
uint8_t *_attoHTTP_body;
uint8_t *_attoHTTP_url_params;
uint16_t _attoHTTP_url_params_start;
uint16_t _attoHTTP_url_len;
uint16_t _attoHTTP_body_len;
void *_attoHTTP_read;
void *_attoHTTP_write;
uint8_t _attoHTTP_headersDone;
uint8_t _attoHTTP_headersSent;
uint8_t _attoHTTP_firstlineSent;
returncode_t _attoHTTP_returnCode;
uint16_t _attoHTTP_accept;
mimetypes_t _attoHTTP_contenttype;
uint32_t _attoHTTP_contentlength;
attoHTTPPage_t _attoHTTPPages[ATTOHTTP_PAGE_BUFFERS];
attoHTTPPage_t _attoHTTPDefaultPage;
attoHTTPDefAPICallback _attoHTTPDefaultCallback;

/** @var The curly brace level we are at */
uint8_t _attoHTTPParseJSONParam_cblevel;
/** @var The square brace level we are at */
uint8_t _attoHTTPParseJSONParam_sblevel;
/** @var The level that we are at */
uint8_t _attoHTTPParseJSONParam_baselevel;
/** @var A counter for the JSON parser */
uint8_t _attoHTTPParseJSONParam_counter;

/** @var This is a map of our mime types */
static const uint8_t *_mimetypes[] = {
    [APPLICATION_JSON] = (uint8_t *)"application/json",
    [TEXT_HTML] = (uint8_t *)"text/html",
    [TEXT_PLAIN] = (uint8_t *)"text/plain",
    [TEXT_CSS] = (uint8_t *)"text/css",
    [APPLICATION_JAVASCRIPT] = (uint8_t *)"application/javascript",
    [APPLICATION_XWWWFORMURLENCODED] = (uint8_t *)"application/x-www-form-urlencoded"
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
static inline void
attoHTTPInitRun(void)
{
    _attoHTTPMethod = NOTSUPPORTED;
    _attoHTTPVersion = VUNKNOWN;
    _attoHTTP_body = NULL;
    _attoHTTP_url_len = 0;
    _attoHTTP_body_len = 0;
    _attoHTTP_headersDone = 0;
    _attoHTTP_headersSent = 0;
    _attoHTTP_firstlineSent = 0;
    _attoHTTP_returnCode = OK;
    _attoHTTP_extra_c = -1;
    _attoHTTP_url_params = NULL;
    _attoHTTP_url_params_start = ATTOHTTP_URL_BUFFER_SIZE;
    _attoHTTP_accept = TEXT_HTML;
    _attoHTTP_contenttype = TEXT_HTML;
    _attoHTTP_contentlength = 0;
    _attoHTTPParseJSONParam_cblevel = 0;
    _attoHTTPParseJSONParam_sblevel = 0;
    _attoHTTPParseJSONParam_baselevel = 0;
    _attoHTTPParseJSONParam_counter = 0;

}

/**
 * @brief Reads a character in
 *
 * @param c A pointer to the location to store the read character into
 *
 * @return 1 if a character was read, 0 if not
 */
int8_t
attoHTTPReadC(uint8_t *c)
{
    int8_t ret = 1;
    if (_attoHTTP_extra_c > 0) {
        *c = _attoHTTP_extra_c;
        _attoHTTP_extra_c = -1;
        /*
    } else if (_attoHTTP_extra_c == 0) {
        *c = 0;
        ret = 0;
        */
    } else {
        ret = attoHTTPGetByte(_attoHTTP_read, c);
    }
    return ret;
}
/**
 * @brief Writes a character out
 *
 * @param c The character to write
 *
 * @return 1 if the byte was written, 0 if it was not
 */
static inline int8_t
attoHTTPWriteC(uint8_t c)
{
    return attoHTTPSetByte(_attoHTTP_write, c);
}
/**
 * @brief Read characters until a non-space character is encountered.
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseSpace(void)
{
    int8_t ret;
    uint8_t c;
    do {
        ret = attoHTTPReadC(&c);
    } while (isblank(c) && (ret != 0));
    _attoHTTPPushC(c);
    return ret;
}
/**
 * @brief Finds one or two EOL
 *
 * This will remove any character from the stream until it hits an EOL, then
 * it will only take space characters.
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseEOL(void)
{
    uint8_t ret = 1;
    uint8_t c;
    int8_t eolCount = 0;
    // Remove any extra space
    do {
        ret = attoHTTPReadC(&c);
        if (c == '\n') {
            eolCount++;
        }
    } while ((isspace(c) || (eolCount == 0)) && (ret != 0) && (eolCount < 2));
    _attoHTTPPushC(c);
    if (eolCount > 1) {
        _attoHTTP_headersDone = 1;
    }
    return ret;
}
/**
 * @brief Finds the HTTP method
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseMethod(void)
{
    uint8_t ret = 1;
    uint8_t buffer[10];
    uint16_t ptr;
    _attoHTTP_extra_c = -1;
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

        if (strncmp(HTTP_METHOD_GET, (char *)buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = GET;
        } else if (strncmp(HTTP_METHOD_POST, (char *)buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = POST;
        } else if (strncmp(HTTP_METHOD_PUT, (char *)buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = PUT;
        } else if (strncmp(HTTP_METHOD_DELETE, (char *)buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = DELETE;
        } else if (strncmp(HTTP_METHOD_PATCH, (char *)buffer, sizeof(buffer)) == 0) {
            _attoHTTPMethod = PATCH;
        } else {
            _attoHTTP_returnCode = UNSUPPORTED;
        }
#ifdef __DEBUG__
        printf("Got Method '%s' (%d)" HTTPEOL, buffer, _attoHTTPMethod);
#endif

    }
    return ret;

}
/**
 * @brief Finds the url and stores it for later usage
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseURI(void)
{
    uint8_t ret;
    uint8_t c;
    // Remove any extra space
    ret = attoHTTPParseSpace();

    if (ret) {
        _attoHTTP_url_len = 0;
        do {
            ret = attoHTTPReadC(&_attoHTTP_url[_attoHTTP_url_len]);
            if (isblank(_attoHTTP_url[_attoHTTP_url_len])) {
                break;
            } else {
                if (_attoHTTP_url[_attoHTTP_url_len] == '?') {
                    _attoHTTP_url[_attoHTTP_url_len] = 0;
                    _attoHTTP_url_params = &_attoHTTP_url[_attoHTTP_url_len + 1];
                    _attoHTTP_url_params_start = _attoHTTP_url_len + 1;
                }
                _attoHTTP_url_len++;
            }
        } while (ret && (_attoHTTP_url_len < (sizeof(_attoHTTP_url) - 1)));
        // Remove any extra that doesn't fit into our buffer
        // We are not done with the URL
        c = _attoHTTP_url[_attoHTTP_url_len];
        while (ret && !isblank(c)) {
            ret = attoHTTPReadC(&c);
        }
        _attoHTTPPushC(c);
        _attoHTTP_url[_attoHTTP_url_len] = 0;
#ifdef __DEBUG__
        printf("URL: '%s'" HTTPEOL, _attoHTTP_url);
#endif

    }
    return ret;

}
/**
 * @brief Finds the HTTP version
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseVersion()
{
    uint8_t ret;
    uint8_t buffer[10];
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
        if (strncmp(HTTP_VERSION_1_0, (char *)buffer, ptr) == 0) {
            _attoHTTPVersion = V1_0;
        } else if (strncmp(HTTP_VERSION_1_1, (char *)buffer, ptr) == 0) {
            _attoHTTPVersion = V1_1;
        }
    }

    ret = attoHTTPParseEOL();

    return ret;
}
/**
 * @brief Parses the next header
 *
 * @param name      The buffer to store the name in
 * @param namesize  The size of the name buffer
 * @param value     The buffer to store the value in
 * @param valuesize The size of the value buffer
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseHeader(uint8_t *name, uint16_t namesize, uint8_t *value, uint16_t valuesize)
{
    uint8_t ret;
    namesize--; // Account for the termination character
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

    valuesize--; // Account for the termination character
    do {
        ret = attoHTTPReadC(value);
        if ((*value == '\r') || (*value == '\n')) {
            break;
        } else {
            value++;
            valuesize--;
        }
    } while ((ret > 0) && (valuesize > 0));
    if (valuesize > 0) {
        _attoHTTPPushC(*value);
    }
    *value = 0;  // Terminate the string
    ret = attoHTTPParseEOL();
    return ret;
}
/**
 * @brief Parses headers and saves inforamtion it needs out of them.
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
attoHTTPParseHeaders(void)
{
    int8_t ret = 1;
    uint8_t i;
    uint8_t name[ATTOHTTP_HEADER_NAME_SIZE];
    uint8_t value[ATTOHTTP_HEADER_VALUE_SIZE];

    while ((_attoHTTP_headersDone == 0) && (ret != 0)) {
        ret = attoHTTPParseHeader(name, sizeof(name), value, sizeof(value));
        if (strncasecmp((char *)name, "accept", sizeof(name)) == 0) {
            for (i = 0; i < ATTOHTTP_MIME_TYPES; i++) {
                if (strncasecmp((char *)value, (char *)_mimetypes[i], sizeof(value)) == 0) {
                    _attoHTTP_accept = (1<<i);
                }
            }
        } else if (strncasecmp((char *)name, "content-type", sizeof(name)) == 0) {
            for (i = 0; i < ATTOHTTP_MIME_TYPES; i++) {
                if (strncasecmp((char *)value, (char *)_mimetypes[i], sizeof(value)) == 0) {
                    _attoHTTP_contenttype = i;
                    break;
                }
            }
        }
    }
    return ret;
}


/**
 * @brief Finds API callback
 *
 * This function parses the URL into component parts, and feeds that to the
 * callback function.  This allows it to figure out what it should do, based
 * on the URL.
 *
 * @return 1 if a function was called, 0 otherwise
 */
int8_t
attoHTTPFindAPICallback(void)
{
    int8_t ret = 0;
    attoHTTPDefAPICallback Callback = NULL;
    uint8_t *command[ATTOHTTP_API_LEVELS];
    uint8_t *id[ATTOHTTP_API_LEVELS];
    uint8_t i;
    uint8_t *url_ptr = _attoHTTP_url;
    uint16_t ctr = _attoHTTP_url_len;
    uint8_t cmdlvl = 0;
    uint8_t idlvl = 0;
    // Find the Callback
    if (_attoHTTPDefaultCallback != NULL) {
        Callback = _attoHTTPDefaultCallback;
    }
    if (Callback != NULL) {
        ret = 1;
        // Init the buffers
        for (i = 0; i < ATTOHTTP_API_LEVELS; i++) {
            command[i] = NULL;
            id[i] = NULL;
        }
        // Parse the URL
        while ((*url_ptr != 0) && (ctr > 0)) {
            if (*url_ptr == '/') {
                if ((cmdlvl == idlvl) && (cmdlvl < ATTOHTTP_API_LEVELS)) {
                    command[cmdlvl] = url_ptr + 1;
                    cmdlvl++;
                } else if ((cmdlvl > idlvl) && (idlvl < ATTOHTTP_API_LEVELS)) {
                    // Don't get a level if there is nothing after the slash.
                    if (ctr > 1) {
                        id[idlvl] = url_ptr + 1;
                        idlvl++;
                    }
                }
                *url_ptr = 0;
            }
            url_ptr++;
            ctr--;
        }
        _attoHTTP_returnCode = Callback(_attoHTTPMethod, _attoHTTP_accept, command, id, cmdlvl, idlvl);
    }
    return ret;
}
/**
 * @brief Finds the page associated with the URL.
 *
 * If no pages are found, it will try to run an API callback function using
 * attoHTTPFindAPICallback()
 *
 * @return 1 if a page was found (or function called), 0 otherwise.
 */
int8_t
attoHTTPFindPage(void)
{
    int8_t ret = 0;
    uint8_t i;
    attoHTTPPage_t *page = NULL;
    if (_attoHTTPDefaultPage() || _attoHTTPCheckPage(_attoHTTPDefaultPage)) {
        page = &_attoHTTPDefaultPage;
    } else {
        for (i = 0; i < ATTOHTTP_PAGE_BUFFERS; i++) {
            if (_attoHTTPCheckPage(_attoHTTPPages[i])) {
                page = &_attoHTTPPages[i];
                break;
            }
        }
    }
    if (page != NULL) {
        if (_attoHTTPMethod == GET) {
            _attoHTTP_contenttype = page->type;
            _attoHTTP_contentlength = page->size;
            attoHTTPSendHeaders();
            attoHTTPwrite(page->content, page->size);
            ret = 1;
        } else {
            _attoHTTP_returnCode = UNSUPPORTED;
            ret = -1;
        }
    }

    if (ret == 0) {
        ret = attoHTTPFindAPICallback();
    }
    return ret;
}
/**
 * @brief This gets a character for the URL parsing
 *
 * This character could be in the waiting buffer, or in the URL buffer.  This
 * function chooses the buffer based on the HTTP method.
 *
 * @param c The character buffer to put the character in
 *
 * @return The number of characters retrieved.
 */
uint8_t
attoHTTPParseURLParamChar(char *c)
{
    uint8_t ret = 0;
    if (_attoHTTPMethod == GET) {
        if (_attoHTTP_url_params_start++ < ATTOHTTP_URL_BUFFER_SIZE) {
            *c = *_attoHTTP_url_params;
            _attoHTTP_url_params++;
            ret = 1;
        }
    } else {
        // Take up any space characters in the body.
        do {
            ret = attoHTTPReadC((uint8_t *)c);
        } while ((ret == 1) && (isspace(*c) || (*c == '?')));
    }
    return ret;

}
/***************************************************************************
 * @endcond
 ***************************************************************************/
/**
 * @brief Writes characters out to the client
 *
 * @param buffer The buffer to write out
 * @param len    The length of the buffer
 *
 * @return The number of characters written
 */
uint16_t
attoHTTPwrite(const uint8_t *buffer, uint16_t len)
{
    uint16_t ret = 0;
    uint8_t c;
    while (len-- > 0) {
        c = *buffer++;
        // This makes sure that what we are sending out is UTF-8 compatible.
        ret += attoHTTPWriteC(c);
    }
    return ret;
}

/**
 * @brief Printf like function to write characters out to the client
 *
 * @param format The format string
 * @param ...    Arguments for the format string
 *
 * @return The number of characters written
 */
uint16_t
attoHTTPprintf(const char *format, ...)
{
    char buffer[ATTOHTTP_PRINTF_BUFFER_SIZE];
    uint16_t count;
    va_list ap;
    va_start(ap, format);
    count = vsnprintf(buffer, 128, format, ap);
    va_end(ap);
    return attoHTTPwrite((uint8_t *)buffer, count);
}
/**
 * @brief Writes a buffer out to the client
 *
 * This dynamically determines the length of the string and writes it
 * out to the client
 *
 * @param buffer The buffer to write out
 *
 * @return The number of characters written
 */
uint16_t
attoHTTPprint(const char *buffer)
{
    return attoHTTPwrite((uint8_t *)buffer, strlen(buffer));
}
/**
 * @brief Prints out the first line of the reply
 *
 * This currently supports the following return codes:
 *  - 200 OK
 *  - 202 Accepted
 *  - 400 Bad Request
 *  - 404 Not Fount
 *  - 500 Internal Error
 *  - 501 Not Implemented
 *
 * Anything else returns: 500 Internal Error
 *
 * @param code The return code to use.
 *
 * @return The number of characters printed out
 */
uint16_t
attoHTTPFirstLine(uint16_t code)
{
    char *str = 0;
    uint16_t chars = 0;
    if (_attoHTTP_firstlineSent == 0) {
        _attoHTTP_firstlineSent = 1;
        switch (code) {
            case 200:
                str = "OK";
                break;
            case 202:
                str = "Accepted";
                break;
            case 400:
                str = "Bad Request";
                break;
            case 404:
                str = "Not Found";
                break;
            case 501:
                str = "Not Implemented";
                break;
            default:
                str = "Internal Error";
                code = 500;
                _attoHTTP_returnCode = INTERNAL_ERROR;
                break;
        }
        chars += attoHTTPprintf(HTTP_VERSION " %d %s" HTTPEOL, code, str);
    }
    return chars;
}
/**
 * @brief This retrieves the next parameter in a JSON string
 *
 * @param name      The buffer to put the name into
 * @param name_len  The length of the name buffer
 * @param value     The buffer to put the value into
 * @param value_len The length of the value buffer
 *
 * @return 1 on success, 0 on no
 */
uint8_t
attoHTTPParseJSONParam(char *name, uint8_t name_len, char *value, uint8_t value_len)
{
    char c;
    char *n = name, *v = value;
    uint8_t nl = name_len, vl = value_len;
    uint8_t ret;
    uint8_t sqlevel = 0;
    uint8_t dqlevel = 0;
    uint8_t divider = 0;
    uint8_t level = 0;
    do {
        ret = attoHTTPReadC((uint8_t *)&c);
        if ((ret != 0) && (c != 0)) {
            level = _attoHTTPParseJSONParam_cblevel + _attoHTTPParseJSONParam_sblevel;
            if ((c == ':') && (_attoHTTPParseJSONParam_cblevel == 1) && (_attoHTTPParseJSONParam_sblevel == 0)) {
                name_len = 0;
                divider = c;
            } else if (isspace(c) && (sqlevel == 0) && (dqlevel == 0) && (level <= 1)) {
                // Ignore space if it is not between quote marks
                continue;
            } else if ((c == ',') && (level == 1)) {
                break;
            } else if ((c == '\'') && (level <= 1)) {
                if (sqlevel == 1) {
                    sqlevel = 0;
                } else {
                    sqlevel = 1;
                }
                continue;
            } else if ((c == '"') && (level <= 1)) {
                if (dqlevel == 1) {
                    dqlevel = 0;
                } else {
                    dqlevel = 1;
                }
                continue;
            } else if ((c == '{') && (level == 0)) {
                // Ignore the first one
                _attoHTTPParseJSONParam_cblevel++;
                if (_attoHTTPParseJSONParam_baselevel == 0) {
                    _attoHTTPParseJSONParam_baselevel = c;
                }
                continue;
            } else if ((c == '}') && (_attoHTTPParseJSONParam_cblevel == 1) && (_attoHTTPParseJSONParam_baselevel == '{')) {
                // Ignore the first one
                _attoHTTPParseJSONParam_cblevel--;
                break;
            } else if ((c == '[') && (level == 0)) {
                // Ignore the first one
                _attoHTTPParseJSONParam_sblevel++;
                if (_attoHTTPParseJSONParam_baselevel == 0) {
                    _attoHTTPParseJSONParam_baselevel = c;
                }
                continue;
            } else if ((c == ']') && (_attoHTTPParseJSONParam_sblevel == 1) && (_attoHTTPParseJSONParam_baselevel == '[')) {
                // Ignore the first one
                _attoHTTPParseJSONParam_sblevel--;
                break;
            } else {
                if (name_len > 0) {
                    *name++ = c;
                    name_len--;
                } else {
                    *value++ = c;
                    value_len--;
                }
                if (c == '{') {
                    _attoHTTPParseJSONParam_cblevel++;
                }
                if (c == '}') {
                    _attoHTTPParseJSONParam_cblevel--;
                    if (_attoHTTPParseJSONParam_cblevel == 1) {
                        break;
                    }
                }
                if (c == '[') {
                    _attoHTTPParseJSONParam_sblevel++;
                }
                if (c == ']') {
                    _attoHTTPParseJSONParam_sblevel--;
                    if (_attoHTTPParseJSONParam_sblevel == 1) {
                        break;
                    }
                }
            }
        } else {
            break;
        }
    } while ((value_len > 0) && ret);
    // Make sure there is a trailing \0
    *value= 0;
    *name = 0;
    if ((divider == 0) && (n != name)) {
        strncpy(v, n, vl);
        snprintf(n, nl, "%d", _attoHTTPParseJSONParam_counter++);
    }

    return c == 0;

}
/**
 * @brief This adds a page to the buffer at the given URL
 *
 * @param Callback The callback function to use.
 *
 * @return 1 on success, 0 on failure
 */
uint8_t
attoHTTPDefaultREST(attoHTTPDefAPICallback Callback)
{
    uint8_t ret = 0;
    if (_attoHTTPDefaultCallback == NULL) {
        _attoHTTPDefaultCallback = Callback;
        ret = 1;
    }
    return ret;
}
/**
 * @brief This retrieves the next parameter in a URL string
 *
 * @param name      The buffer to put the name into
 * @param name_len  The length of the name buffer
 * @param value     The buffer to put the value into
 * @param value_len The length of the value buffer
 *
 * @return 1 on success, 0 on no
 */
uint8_t
attoHTTPParseURLParam(char *name, uint8_t name_len, char *value, uint8_t value_len)
{
    char c;
    char decode[3];
    uint8_t ret;
    do {
        ret = attoHTTPParseURLParamChar(&c);
        if ((ret != 0) && (c != 0)) {
            if (c == '=') {
                name_len = 0;
            } else if ((c == '&') || isspace(c)) {
                break;
            } else {
                // This decodes the URL
                if (c == '%') {
                    attoHTTPParseURLParamChar(&decode[0]);
                    attoHTTPParseURLParamChar(&decode[1]);
                    decode[2] = 0;
                    c = strtol(decode, NULL, 16);
                }
                if (name_len > 0) {
                    *name++ = c;
                    name_len--;
                } else {
                    *value++ = c;
                    value_len--;
                }
            }
        } else {
            break;
        }
    } while ((value_len > 0) && ret);
    // Make sure there is a trailing \0
    *value= 0;
    *name = 0;
    return c == 0;

}
/**
 * @brief This retrieves the next parameter.
 *
 * This function will get the next param from a location based on the
 * method.
 *
 * - `GET`    - Reads the params from the url
 * - `POST`   - Reads the params as a url encoded string from the body
 * - `PUT`    - Reads the params as a JSON encoded string from the body
 * - `PATCH`  - Reads the params as a JSON encoded string from the body
 * - `DELETE` - Reads the params as a JSON encoded string from the body
 *
 * @param name      The buffer to put the name into
 * @param name_len  The length of the name buffer
 * @param value     The buffer to put the value into
 * @param value_len The length of the value buffer
 *
 * @return 1 on success, 0 on failure
 */
uint8_t
attoHTTPParseParam(char *name, uint8_t name_len, char *value, uint8_t value_len)
{
    uint8_t ret = 0;
    *name = 0;
    *value = 0;
    switch (_attoHTTPMethod) {
        case GET:
            ret = attoHTTPParseURLParam(name, name_len, value, value_len);
            break;
        case POST:
        case PUT:
        case PATCH:
        case DELETE:
            if (_attoHTTP_contenttype == APPLICATION_XWWWFORMURLENCODED) {
                ret = attoHTTPParseURLParam(name, name_len, value, value_len);
            } else {
                ret = attoHTTPParseJSONParam(name, name_len, value, value_len);
            }
            break;
        default:
            break;
    }
    return ret;
}
/**
 * @brief This adds a page to the buffer at the given URL
 *
 * @param url      The URL string to look for
 * @param page     A pointer to the page data
 * @param page_len The length of the page data
 * @param type     The mimetype to use
 *
 * @return 1 on success, 0 on failure
 */
uint8_t
attoHTTPDefaultPage(const char *url, const uint8_t *page, uint16_t page_len, mimetypes_t type)
{
    uint8_t ret = 0;
    if (_attoHTTPPageEmpty(_attoHTTPDefaultPage)) {
        // Page and page_len should get set first for testing reasons
        _attoHTTPDefaultPage.content = page;
        _attoHTTPDefaultPage.size = page_len;
        _attoHTTPDefaultPage.type = type;
        strncpy(_attoHTTPDefaultPage.url, url, sizeof(_attoHTTPDefaultPage.url));
        ret = 1;
    }
    return ret;
}
/**
 * @brief This adds the default page to the buffer at the given URL
 *
 * @param url      The URL string to look for
 * @param page     A pointer to the page data
 * @param page_len The length of the page data
 * @param type     The mimetype to use
 *
 * @return 1 on success, 0 on failure
 */
uint8_t
attoHTTPAddPage(const char *url, const uint8_t *page, uint16_t page_len, mimetypes_t type)
{
    uint8_t i;
    uint8_t ret = 0;
    for (i = 0; i < ATTOHTTP_PAGE_BUFFERS; i++) {
        if (_attoHTTPPageEmpty(_attoHTTPPages[i])) {
            // Page and page_len should get set first for testing reasons
            _attoHTTPPages[i].content = page;
            _attoHTTPPages[i].size = page_len;
            _attoHTTPPages[i].type = type;
            strncpy((char *)_attoHTTPPages[i].url, (char *)url, sizeof(_attoHTTPPages[i].url));
            ret = 1;
            break;
        }
    }
    return ret;
}
/**
 * @brief This prints out the OK message
 *
 * @return The number of characters printed
 */
uint8_t
attoHTTPSendHeaders(void)
{
    uint16_t chars = 0;
    if (_attoHTTP_firstlineSent == 0) {
        attoHTTPOK();
    }
    if (_attoHTTP_headersSent == 0) {
        chars += attoHTTPprintf("Content-Type: %s" HTTPEOL, _mimetypes[_attoHTTP_contenttype]);
        if (_attoHTTP_contentlength > 0) {
            chars += attoHTTPprintf("Content-Length: %d" HTTPEOL, _attoHTTP_contentlength);
        }
#ifdef ATTOHTTP_GZIP_PAGES
        chars += attoHTTPprint("Content-Encoding: gzip" HTTPEOL);
#endif
        chars += attoHTTPprint(HTTPEOL);
        _attoHTTP_headersSent = 1;
    }
    return chars;
}
/**
 * @brief Sends out the headers for the RESTful API
 *
 * @param code    The HTTP return code
 * @param type    The mime type of return
 * @param headers Extra headers to send.  Each header should end with HTTPEOL
 *
 * @return The number of characters printed
 */
uint16_t
attoHTTPRESTSendHeaders(uint16_t code, char *type, char *headers)
{
    uint16_t chars = 0;
    if (_attoHTTP_firstlineSent == 0) {
        attoHTTPFirstLine(code);
    }
    if (_attoHTTP_headersSent == 0) {
        chars += attoHTTPprintf("Content-Type: %s" HTTPEOL, type);
        if (headers != NULL) {
            chars += attoHTTPprint(headers);
        }
        chars += attoHTTPprint(HTTPEOL);
        _attoHTTP_headersSent = 1;
    }
    return chars;
}
/**
 * @brief Initiialized the variables
 *
 * @return none
 */
void
attoHTTPInit(void)
{
    uint8_t i;
    _attoHTTPDefaultPage.url[0] = 0;
    _attoHTTPDefaultPage.content = NULL;
    _attoHTTPDefaultPage.size = 0;
    _attoHTTPDefaultPage.type = TEXT_HTML;
    _attoHTTPDefaultCallback = NULL;
    for (i = 0; i < ATTOHTTP_PAGE_BUFFERS; i++) {
        _attoHTTPPages[i].url[0] = 0;
        _attoHTTPPages[i].content = NULL;
        _attoHTTPPages[i].size = 0;
        _attoHTTPPages[i].type = TEXT_HTML;
    }
}

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
 * @return The code that was sent out to the client
 *
 * @see returncode_t for details
 */
returncode_t
attoHTTPExecute(void *read, void *write)
{
    int8_t ret;
    _attoHTTP_read = read;
    _attoHTTP_write = write;

    // Init all of the variables.
    attoHTTPInitRun();
    // Parse the first line
    attoHTTPParseMethod();

    if (_attoHTTP_returnCode == OK) {
        attoHTTPParseURI();
        attoHTTPParseVersion();
    }
    if (_attoHTTP_returnCode == OK) {
        attoHTTPParseHeaders();
    }
    if (_attoHTTP_returnCode == OK) {
        _attoHTTP_returnCode = NOT_FOUND;

        ret = attoHTTPFindPage();
        if (ret > 0) {
            _attoHTTP_returnCode = OK;
        } else if (ret == 0) {
            // Not found in the find page, so check the RESTful stuff
        }
    }
    switch (_attoHTTP_returnCode) {
        case OK:
            break;
        case UNSUPPORTED:
            attoHTTPNotImplemented();
            break;
        case NOT_FOUND:
            attoHTTPNotFound();
            break;
        case BADREQUEST:
            attoHTTPBadRequest();
            break;
        default:
            attoHTTPInternalError();
            break;
    }
#ifdef __DEBUG__
    printf("Return Code %d" HTTPEOL, _attoHTTP_returnCode);
#endif

    return _attoHTTP_returnCode;

}
