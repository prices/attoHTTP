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

#define _attoHTTPCheckPage(page)  (!_attoHTTPPageEmpty(page) && (0 == strncmp((char *)_attoHTTP_url, (char *)page.url, sizeof(page.url))))
#define _attoHTTPDefaultPage() (!_attoHTTPPageEmpty(_attoHTTPDefaultPage) && (strncmp((char *)_attoHTTP_url, "/", sizeof(_attoHTTP_url)) == 0) && (_attoHTTP_url_len == 1))
#define _attoHTTPPushC(char) _attoHTTP_extra_c = char
#define _attoHTTPPageEmpty(page) (page.content == NULL)

unsigned char favicon_ico[] = {
  0x1f, 0x8b, 0x08, 0x08, 0xbf, 0x58, 0xcd, 0x55, 0x00, 0x03, 0x66, 0x61,
  0x76, 0x69, 0x63, 0x6f, 0x6e, 0x2e, 0x70, 0x6e, 0x67, 0x00, 0xeb, 0x0c,
  0xf0, 0x73, 0xe7, 0xe5, 0x92, 0xe2, 0x62, 0x60, 0x60, 0xe0, 0xf5, 0xf4,
  0x70, 0x09, 0x02, 0xd2, 0x02, 0x20, 0xcc, 0xc1, 0x06, 0x24, 0xe5, 0x3f,
  0xff, 0x4f, 0x04, 0x52, 0x6c, 0x49, 0xde, 0xee, 0x2e, 0x0c, 0xff, 0x41,
  0x70, 0xc1, 0xde, 0xe5, 0x93, 0x81, 0x22, 0xb7, 0x3c, 0x5d, 0x1c, 0x43,
  0x2c, 0x4e, 0x5f, 0xbd, 0x6c, 0xc8, 0xd5, 0x6c, 0x20, 0xc2, 0xf6, 0x60,
  0x3f, 0xab, 0xac, 0x92, 0xc7, 0x89, 0x77, 0xb2, 0x73, 0x6c, 0x9b, 0x9b,
  0x9b, 0x9a, 0x5f, 0x3c, 0x79, 0xf4, 0xce, 0xd2, 0xa6, 0xf5, 0xb9, 0x40,
  0xfe, 0x02, 0xf7, 0x82, 0x02, 0x51, 0xa3, 0x9d, 0x16, 0x2c, 0xff, 0xfc,
  0x64, 0xa3, 0x94, 0xc5, 0xbc, 0xda, 0xff, 0x7d, 0xd8, 0xf6, 0xe1, 0x63,
  0xdc, 0xa4, 0x72, 0x65, 0xcf, 0x9a, 0x89, 0xd6, 0xfb, 0xce, 0xe4, 0x37,
  0xec, 0x7b, 0x2f, 0x78, 0xdc, 0x70, 0xc9, 0x14, 0x01, 0x27, 0x85, 0x2e,
  0x25, 0x9e, 0xc3, 0x09, 0xe2, 0x19, 0x9c, 0x0d, 0xce, 0x96, 0x7d, 0x87,
  0x0b, 0x65, 0xfa, 0xd3, 0x73, 0x1d, 0xa6, 0xa5, 0xce, 0x59, 0xd6, 0xf9,
  0x7b, 0x5f, 0xf1, 0xbc, 0xf4, 0x65, 0x99, 0x5e, 0x0c, 0x67, 0x3e, 0xff,
  0x31, 0x7e, 0x60, 0xfb, 0x7a, 0x97, 0x10, 0x43, 0xb3, 0xc2, 0x0e, 0xb6,
  0x1e, 0xc3, 0x9c, 0xdb, 0xd3, 0x12, 0x25, 0x82, 0x38, 0x3e, 0x1a, 0xb5,
  0xbd, 0xde, 0x75, 0xdd, 0xe1, 0xb2, 0xc9, 0x35, 0xeb, 0x8a, 0x6b, 0xaa,
  0x0b, 0xa6, 0x28, 0x31, 0x94, 0xc5, 0xba, 0xaf, 0x35, 0xed, 0x35, 0xbc,
  0x12, 0x35, 0x9f, 0xa9, 0xfc, 0xf4, 0x6a, 0x06, 0x86, 0x9a, 0xfb, 0xed,
  0xcf, 0x2b, 0x23, 0xa6, 0xb1, 0x96, 0xc6, 0xce, 0xaf, 0x6f, 0x29, 0xdf,
  0xa8, 0xd1, 0xe5, 0x7c, 0xdb, 0x73, 0x52, 0x8d, 0x5d, 0xd3, 0xb5, 0x59,
  0xa9, 0x91, 0x41, 0x9a, 0xae, 0x9a, 0xaa, 0xab, 0x57, 0x3d, 0xda, 0x96,
  0xbb, 0xfc, 0xc0, 0xdb, 0xff, 0xd3, 0x5f, 0x32, 0x9e, 0x2d, 0x9f, 0xca,
  0x56, 0xa3, 0xbd, 0xaf, 0x0e, 0xe8, 0x4b, 0x06, 0x4f, 0x57, 0x3f, 0x97,
  0x75, 0x4e, 0x09, 0x4d, 0x00, 0x91, 0x99, 0x0f, 0x4f, 0x25, 0x01, 0x00,
  0x00
};
unsigned int favicon_ico_len = 325;


/***************************************************************************
 *                              Private Parameters
 * @cond dev
 ***************************************************************************/
/** @var The HTTP method from the client */
httpmethod_t _attoHTTPMethod;
/** @var The HTTP version from the client */
httpversion_t _attoHTTPVersion;
/** @var Our URL buffer */
uint8_t _attoHTTP_url[ATTOHTTP_URL_BUFFER_SIZE];
/** @var The last character read that wasn't used by what read it
 *
 * This needs to be bigger than a character to save a character + sign information
 */
int16_t _attoHTTP_extra_c;
/** @var Pointer to the start of the parameters in the URL */
uint8_t *_attoHTTP_url_params;
/** @var The length into the string where the params start */
uint16_t _attoHTTP_url_params_start;
/** @var The length of the URL */
uint16_t _attoHTTP_url_len;
/** @var Pointer to our read parameter */
void *_attoHTTP_read;
/** @var Pointer to our write parameter */
void *_attoHTTP_write;
/** @var Flag to say that we are done receiving headers */
uint8_t _attoHTTP_headersDone;
/** @var Flag to say that our headers are sent */
uint8_t _attoHTTP_headersSent;
/** @var Flag to say the first line of our return has been sent */
uint8_t _attoHTTP_firstlineSent;
/** @var The return code to send the client */
returncode_t _attoHTTP_returnCode;
/** @var These are what the client said they wanted for mime-type in the return */
uint16_t _attoHTTP_accept;
/** @var Incoming content type */
mimetypes_t _attoHTTP_contenttype;
/** @var Incoming content length */
uint32_t _attoHTTP_contentlength;
/** @var Our different pages are stored here */
attoHTTPPage_t _attoHTTPPages[ATTOHTTP_PAGE_BUFFERS];
/** @var The default HTTP page is stored here */
attoHTTPPage_t _attoHTTPDefaultPage;
/** @var The default API callback function is stored here */
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
    [APPLICATION_XWWWFORMURLENCODED] = (uint8_t *)"application/x-www-form-urlencoded",
    [IMAGE_PNG] = (uint8_t *)"image/png"
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
_attoHTTPInitRun(void)
{
    _attoHTTPMethod = NOTSUPPORTED;
    _attoHTTPVersion = VUNKNOWN;
    _attoHTTP_url_len = 0;
    _attoHTTP_headersDone = 0;
    _attoHTTP_headersSent = 0;
    _attoHTTP_firstlineSent = 0;
    _attoHTTP_returnCode = RUNKNOWN;
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
_attoHTTPReadC(uint8_t *c)
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
_attoHTTPWriteC(uint8_t c)
{
    return attoHTTPSetByte(_attoHTTP_write, c);
}
/**
 * @brief Read characters until a non-space character is encountered.
 *
 * @return 1 if there is more to read, 0 if done reading
 */
int8_t
_attoHTTPParseSpace(void)
{
    int8_t ret;
    uint8_t c;
    do {
        ret = _attoHTTPReadC(&c);
    } while (isblank(c) && (ret > 0));
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
_attoHTTPParseEOL(void)
{
    uint8_t ret = 1;
    uint8_t c;
    int8_t eolCount = 0;
    // Remove any extra space
    do {
        ret = _attoHTTPReadC(&c);
        if (c == '\n') {
            eolCount++;
        }
    } while ((isspace(c) || (eolCount == 0)) && (ret > 0) && (eolCount < 2));
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
static inline int8_t
_attoHTTPParseMethod(void)
{
    int8_t ret;
    uint8_t buffer[10];
    uint16_t ptr;
    // Remove any extra space
    ret = _attoHTTPParseSpace();
    if (ret > 0) {
        ptr = 0;
        do {
            ret = _attoHTTPReadC(&buffer[ptr]);
            if (isblank(buffer[ptr])) {
                break;
            } else {
                ptr++;
            }
        } while ((ret > 0) && (ptr < (sizeof(buffer) - 1)));
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
    } else {
        _attoHTTP_returnCode = INTERNAL_ERROR;
    }

    return ret;

}
/**
 * @brief Finds the url and stores it for later usage
 *
 * @return 1 if there is more to read, 0 if done reading
 */
static inline int8_t
_attoHTTPParseURL(void)
{
    int8_t ret;
    uint8_t c;
    // Remove any extra space
    ret = _attoHTTPParseSpace();

    if (ret > 0) {
        _attoHTTP_url_len = 0;
        do {
            ret = _attoHTTPReadC(&_attoHTTP_url[_attoHTTP_url_len]);
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
            ret = _attoHTTPReadC(&c);
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
static inline int8_t
_attoHTTPParseVersion()
{
    int8_t ret;
    uint8_t buffer[10];
    uint16_t ptr;
    // Remove any extra space
    ret = _attoHTTPParseSpace();

    if (ret > 0) {
        ptr = 0;
        do {
            ret = _attoHTTPReadC(&buffer[ptr]);
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

    ret = _attoHTTPParseEOL();

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
static inline int8_t
_attoHTTPParseHeader(uint8_t *name, uint16_t namesize, uint8_t *value, uint16_t valuesize)
{
    int8_t ret;
    namesize--; // Account for the termination character
    do {
        ret = _attoHTTPReadC(name);
        if (*name == ':') {
            break;
        } else {
            name++;
            namesize--;
        }
    } while ((ret > 0) && (namesize > 0));
    *name = 0; // Terminate the string

    ret = _attoHTTPParseSpace();

    valuesize--; // Account for the termination character
    do {
        ret = _attoHTTPReadC(value);
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
    ret = _attoHTTPParseEOL();
    return ret;
}
/**
 * @brief Parses headers and saves inforamtion it needs out of them.
 *
 * @return 1 if there is more to read, 0 if done reading
 */
static inline int8_t
_attoHTTPParseHeaders(void)
{
    int8_t ret = 1;
    uint8_t i;
    uint8_t name[ATTOHTTP_HEADER_NAME_SIZE];
    uint8_t value[ATTOHTTP_HEADER_VALUE_SIZE];

    while ((_attoHTTP_headersDone == 0) && (ret > 0)) {
        ret = _attoHTTPParseHeader(name, sizeof(name), value, sizeof(value));
        if (strncasecmp((char *)name, "accept", sizeof(name)) == 0) {
            for (i = 0; i < ATTOHTTP_MIME_TYPES; i++) {
                if (strncasecmp((char *)value, (char *)_mimetypes[i], sizeof(value)) == 0) {
                    _attoHTTP_accept = (1<<i);
                }
            }
        } else if (strncasecmp((char *)name, "content-type", sizeof(name)) == 0) {
            for (i = 0; i < ATTOHTTP_MIME_TYPES; i++) {
                if (strstr((char *)value, (char *)_mimetypes[i]) != NULL) {
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
static inline int8_t
_attoHTTPFindAPICallback(void)
{
    int8_t ret = 0;
    uint8_t *command[ATTOHTTP_API_LEVELS];
    uint8_t *id[ATTOHTTP_API_LEVELS];
    uint8_t i;
    uint8_t *url_ptr = _attoHTTP_url;
    uint16_t ctr = _attoHTTP_url_len;
    uint8_t cmdlvl = 0;
    uint8_t idlvl = 0;
    // Find the Callback
    if (_attoHTTPDefaultCallback != NULL) {
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
        if (cmdlvl > 0) {
            _attoHTTP_returnCode = _attoHTTPDefaultCallback(_attoHTTPMethod, _attoHTTP_accept, command, id, cmdlvl, idlvl);
        } else {
            _attoHTTP_returnCode = INTERNAL_ERROR;
        }
    }
    return ret;
}
/**
 * @brief Finds the page associated with the URL.
 *
 * If no pages are found, it will try to run an API callback function using
 * _attoHTTPFindAPICallback()
 *
 * @return 1 if a page was found (or function called), 0 otherwise.
 */
static inline int8_t
_attoHTTPFindPage(void)
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
            _attoHTTP_returnCode = OK;
            _attoHTTP_contenttype = page->type;
            _attoHTTP_contentlength = page->size;
            attoHTTPSendHeaders();
            attoHTTPwrite(page->content, page->size);
            ret = 1;
        } else {
#ifdef __DEBUG__
        printf("Wrong method on page: %d\r\n", _attoHTTPMethod);
#endif
            
            _attoHTTP_returnCode = UNSUPPORTED;
            ret = -1;
        }
    }

    if (ret == 0) {
        ret = _attoHTTPFindAPICallback();
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
_attoHTTPParseURLParamChar(char *c)
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
            ret = _attoHTTPReadC((uint8_t *)c);
        } while ((ret == 1) && (isspace((uint8_t)*c) || (*c == '?')));
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
        ret += _attoHTTPWriteC(c);
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
    // This makes sure it is always zero terminated.
    if (count == ATTOHTTP_PRINTF_BUFFER_SIZE) {
        buffer[ATTOHTTP_PRINTF_BUFFER_SIZE - 1] = 0;
        count--;
    }
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
            case 401:
                str = "Unauthorized";
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
    int8_t ret;
    uint8_t sqlevel = 0;
    uint8_t dqlevel = 0;
    uint8_t divider = 0;
    uint8_t level = 0;
    do {
        ret = _attoHTTPReadC((uint8_t *)&c);
        if ((ret > 0) && (c != 0)) {
            level = _attoHTTPParseJSONParam_cblevel + _attoHTTPParseJSONParam_sblevel;
            if ((c == ':') && (_attoHTTPParseJSONParam_cblevel == 1) && (_attoHTTPParseJSONParam_sblevel == 0)) {
                name_len = 0;
                divider = c;
            } else if (isspace((uint8_t)c) && (sqlevel == 0) && (dqlevel == 0) && (level <= 1)) {
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
        name_len = 0;
    }

    return name_len == 0;

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
    int8_t ret;
    uint16_t count = 0;
    do {
        ret = _attoHTTPParseURLParamChar(&c);
        if ((ret > 0) && (c != 0)) {
            if (c == '=') {
                name_len = 0;
            } else if ((c == '&') || isspace((uint8_t)c)) {
                if (count > 0) {
                    break;
                }
            } else {
                // This decodes the URL
                if (c == '%') {
                    _attoHTTPParseURLParamChar(&decode[0]);
                    _attoHTTPParseURLParamChar(&decode[1]);
                    decode[2] = 0;
                    c = strtol(decode, NULL, 16);
                }
                if (name_len > 0) {
                    count++;
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
    return name_len == 0;

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
        attoHTTPFirstLine(_attoHTTP_returnCode);
    }
    if (_attoHTTP_headersSent == 0) {
        chars += attoHTTPprintf("Content-Type: %s; charset=utf-8" HTTPEOL, _mimetypes[_attoHTTP_contenttype]);
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
        chars += attoHTTPprintf("Content-Type: %s; charset=utf-8" HTTPEOL, type);
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
 * This function should be called once when the code using attoHTTP is being
 * set up.  This makes sure that everything is in a known state when it starts
 * running.
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
    attoHTTPAddPage("/favicon.ico", favicon_ico, favicon_ico_len, IMAGE_PNG);
}

/**
 * @brief Main function that runs everything
 *
 * This function serves a single TCP connection.
 *
 * This will process one connection, start to finish, when it is run.
 * It should only be called if there is a connection to deal with.
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
    _attoHTTPInitRun();
    // Parse the first line
    _attoHTTPParseMethod();

    if (_attoHTTP_returnCode == RUNKNOWN) {
        _attoHTTPParseURL();
        _attoHTTPParseVersion();
    }
    if (_attoHTTP_returnCode == RUNKNOWN) {
        _attoHTTPParseHeaders();
    }
    if (_attoHTTP_returnCode == RUNKNOWN) {
        _attoHTTP_returnCode = NOT_FOUND;

        ret = _attoHTTPFindPage();
        if (ret > 0) {
            _attoHTTP_returnCode = OK;
        } else if (ret == 0) {
            // Not found in the find page, so check the RESTful stuff
        }
    }
    if (_attoHTTP_returnCode == RUNKNOWN) {
        _attoHTTP_returnCode = INTERNAL_ERROR;
    }
    attoHTTPFirstLine(_attoHTTP_returnCode);
#ifdef __DEBUG__
    printf("Return Code %d" HTTPEOL, _attoHTTP_returnCode);
#endif

    return _attoHTTP_returnCode;

}
