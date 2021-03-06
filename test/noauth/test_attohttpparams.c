/**
 * @file    test/test_attohttpParams.c
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

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == STATUS_UNSUPPORTED), "Return was not 'STATUS_UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == STATUS_NOT_FOUND), "Return was not 'STATUS_NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'"); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttpParams)
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
    FCT_TEST_BGN(testGETParamsGood) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1?hello=1&goodbye=hereAndThere HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETParamsWithEncodedChars) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("{here And There}", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1?hello=%31&goodbye=%7Bhere%20And%20There%7d HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsGood) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n?hello=1&goodbye=hereAndThere",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsLeading&) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n&hello=1&goodbye=hereAndThere",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsWithEncodedChars) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("{here And There}", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nhello=%31&goodbye=%7Bhere%20And%20There%7d",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsObjectGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n{ hello:1, 'goodbye':\"hereAndThere\" }",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsObjectWithSubArray) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("good bye", name);
            fct_chk_eq_str("[ 'here And', \"There a\" ]", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n { 'hello': 1, \"good bye\": [ 'here And', \"There a\" ] }",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()

    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsObjectWithSubObject) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("good bye", name);
            fct_chk_eq_str("{ 'here': 'And', \"There\": \" a\" }", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n { 'hello': 1, \"good bye\": { 'here': 'And', \"There\": \" a\" } }",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsArrayGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];


            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("0", name);
            fct_chk_eq_str("hello", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("1", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n[ 'hello', \"hereAndThere\" ]",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsStringSQGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("0", name);
            fct_chk_eq_str("hello there", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n'hello there'",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsStringDQGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("0", name);
            fct_chk_eq_str("hello there", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n\"hello there\"",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsIntegerGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("0", name);
            fct_chk_eq_str("123456", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n123456",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsFloatGoodJSON) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("0", name);
            fct_chk_eq_str("1.23456", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\n\r\n1.23456",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTParamsGood) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            char name[40];
            char value[40];

            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("hello", name);
            fct_chk_eq_str("1", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 1), "Return was not 1");
            fct_chk_eq_str("goodbye", name);
            fct_chk_eq_str("hereAndThere", value);
            ret = attoHTTPParseParam(name, 40, value, 40);
            fct_xchk((ret == 0), "Return was not 0");

            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1 HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n\r\n?hello=1&goodbye=hereAndThere\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
    }
    FCT_TEST_END()


}
FCTMF_FIXTURE_SUITE_END();
