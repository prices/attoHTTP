/**
 * @file    test/test_attohttp.c
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

int8_t attoHTTPWrapperCheckAuth(uint8_t auth, int8_t *cred)
{
    return 0;
}

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == UNSUPPORTED), "Return was not 'UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckUnauthorized(ret) fct_xchk((ret == UNAUTHORIZED), "Return was not 'UNAUTHORIZED'"); fct_chk_eq_str("HTTP/1.0 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"attoHTTP Server\"\r\n\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == NOT_FOUND), "Return was not 'NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == OK), "Return was not 'OK'"); fct_chk_eq_str(default_return, write_buffer)
#define CheckRet(expect, value) fct_xchk((expect == value), "Expected %d got %d", expect, value)
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
    FCT_TEST_BGN(testBase64EncodePerfectSized) {
        uint16_t ret;
        int8_t input[] = "input:string";
        int8_t expect[] = "aW5wdXQ6c3RyaW5n";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Encode1CharShort) {
        uint16_t ret;
        int8_t input[] = "input:string1";
        int8_t expect[] = "aW5wdXQ6c3RyaW5nMQ==";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Encode2CharShort) {
        uint16_t ret;
        int8_t input[] = "input:string12";
        int8_t expect[] = "aW5wdXQ6c3RyaW5nMTI=";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64EncodeOutputBufferTooSmall) {
        uint16_t ret;
        int8_t input[] = "input:string";
        int8_t expect[] = "aW5wdXQ6c";
        int8_t output[10];
        ret = attoHTTPBase64Encode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64DecodePerfectSize) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5n";
        int8_t expect[] = "input:string";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Decode2CharShortPaddingCorrect) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMQ==";
        int8_t expect[] = "input:string1";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Decode1CharShortPaddingCorrect) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMTI=";
        int8_t expect[] = "input:string12";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief 1 character too small in input
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64DecodeInput1CharTooSmallNoPadding) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMQ=";
        int8_t expect[] = "input:string1";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief 2 characters too small in input
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64DecodeInput2CharTooSmallNoPadding) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMQ";
        int8_t expect[] = "input:string1";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief 2 characters too small in input
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64DecodeInput3CharTooSmallNoPadding) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nM";
        int8_t expect[] = "input:string";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief 2 characters too small in input
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64DecodeOutputBufferTooSmall) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMQ";
        int8_t expect[] = "input:str";
        int8_t output[10];
        ret = attoHTTPBase64Decode(input, strlen((char *)input), output, sizeof(output));
        CheckRet(strlen((char *)expect), ret);
        fct_chk_eq_str((char *)expect, (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageNoAuthHeader) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnauthorized(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageBadHeaderAuthName) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\nAuthorization: BadAuth asdfasdfasdfasdfasdf\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnauthorized(ret);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETPageBadAuth) {
        returncode_t ret;
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /index.html HTTP/1.0\r\nAccept: text/html\r\nAuthorization: Basic asdfasdfasdfasdfasdf\r\n\r\n",
            (void *)write_buffer
        );
        CheckUnauthorized(ret);
    }
    FCT_TEST_END()

}
FCTMF_FIXTURE_SUITE_END();
