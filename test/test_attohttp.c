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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "attohttp.h"
#include "test.h"

static const char default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == UNSUPPORTED), "Return was not 'UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == NOT_FOUND), "Return was not 'NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == OK), "Return was not 'OK'"); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttp)
{
    /**
    * @brief This sets up this suite
    *
    * @return 0 success, otherwise failure
    */
    FCT_SETUP_BGN() {
        TestInit();
        memset(write_buffer, 0, WRITE_BUFFER_SIZE);
        attoHTTPInit();
    }
    FCT_SETUP_END();
    /**
    * @brief This tears down this suite
    *
    * @return 0 success, otherwise failure
    */
    FCT_TEARDOWN_BGN() {
    } FCT_TEARDOWN_END();
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testMethodUnsupportedMethod) {
        returncode_t ret;

        ret = attoHTTPExecute(
            (void *)"BADMETHOD /index.html HTTP/1.0\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnsupported(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPage) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPUTPage) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PUT /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnsupported(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTPage) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnsupported(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testDELETEPage) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"DELETE /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnsupported(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPATCHPage) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PATCH /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnsupported(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testMethodExtraSpaces) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET         /index.html HTTP/1.0\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testURLExtraSpaces) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html          HTTP/1.0\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testVersionExtraSpaces) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0    \r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testHTTP1.1) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.1\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testHTTP0.9) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/0.9\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageURLParams) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html?test=1&hello=2 HTTP/1.0\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testAddPageBufferOverrun) {
        returncode_t ret;
        attoHTTPAddPage("012345678901234567890123456789012345679801234567890123456789", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET 0123456789012345678901234567890123456789 HTTP/1.0    \r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageURLBufferOverrun) {
        returncode_t ret;
        attoHTTPAddPage("012345678901234567890123456789012345679801234567890123456789", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 HTTP/1.0\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultPage) {
        returncode_t ret;
        attoHTTPDefaultPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET / HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETIndexSetDefaultPage) {
        returncode_t ret;
        attoHTTPDefaultPage("/index.html", (char *)default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckDefault(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageSetManyPages) {
        returncode_t ret;
        char index[] = "Index1";
        char index2[] = "Index2";
        char index3[] = "Index3";
        char index4[] = "Index4";
        char index5[] = "Index5";
        char index6[] = "Index6";
        char index7[] = "Index7";
        attoHTTPDefaultPage("/index.html", (char *)index, sizeof(index), TEXT_HTML);
        attoHTTPAddPage("/index2.html", (char *)index2, sizeof(index2), TEXT_HTML);
        attoHTTPAddPage("/index3.html", (char *)index3, sizeof(index3), TEXT_HTML);
        attoHTTPAddPage("/index4.html", (char *)index4, sizeof(index4), TEXT_HTML);
        attoHTTPAddPage("/index5.html", (char *)index5, sizeof(index5), TEXT_HTML);
        attoHTTPAddPage("/index6.html", (char *)index6, sizeof(index6), TEXT_HTML);
        attoHTTPAddPage("/index7.html", (char *)index7, sizeof(index7), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index6.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: 7\r\n\r\nIndex6", write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageTextPlain) {
        returncode_t ret;
        char index[] = "Index61";
        attoHTTPDefaultPage("/index.html", (char *)index, sizeof(index), TEXT_PLAIN);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/plain\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 8\r\n\r\nIndex61", write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageNoPages) {
        returncode_t ret;
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
                              (void *)write_buffer
        );
        CheckNotFound(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultPageNoPages) {
        returncode_t ret;
        ret = attoHTTPExecute(
            (void *)"GET / HTTP/1.0\r\nAccept: text/html\r\n\r\n",
                              (void *)write_buffer
        );
        CheckNotFound(ret);
    }
    FCT_TEST_END()
}
FCTMF_FIXTURE_SUITE_END();
