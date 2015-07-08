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

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == UNSUPPORTED), "Return was not 'UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == NOT_FOUND), "Return was not 'NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == OK), "Return was not 'OK'"); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttpAPI)
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
    FCT_TEST_BGN(testGETDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == GET), "Return was not 'GET'");
            fct_xchk((cmdlvl == 2), "Command level was not 2");
            fct_xchk((idlvl == 1), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_xchk((id[1] == NULL), "id[1] != NULL");
            fct_xchk((command[2] == NULL), "command[2] != NULL");
            fct_xchk((id[2] == NULL), "id[2] != NULL");
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1/1/level2 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPOSTDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == POST), "Return was not 'POST'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPUTDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == PUT), "Return was not 'PUT'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PUT /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPATCHDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == PATCH), "Return was not 'PATCH'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PATCH /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testDELETEDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == DELETE), "Return was not 'DELETE'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"DELETE /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultRESTExtraSlash) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == GET), "Return was not 'GET'");
            fct_xchk((cmdlvl == 1), "Command level was not 1");
            fct_xchk((idlvl == 0), "Id level was not 0");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_xchk((id[0] == NULL), "id[0] != NULL");
            fct_xchk((command[1] == NULL), "command[1] != NULL");
            fct_xchk((id[1] == NULL), "id[1] != NULL");
            fct_xchk((command[2] == NULL), "command[2] != NULL");
            fct_xchk((id[2] == NULL), "id[2] != NULL");
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1/ HTTP/1.0\r\nAccept: application/json\r\n\r\n",
            (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
    }
    FCT_TEST_END()

    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultRESTSendHeadersBadCode) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            attoHTTPRESTSendHeaders(12345, "application/json", NULL);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
        fct_chk_eq_str("HTTP/1.0 500 Internal Error\r\nContent-Type: application/json\r\n\r\n", write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultRESTSendHeadersOKCode) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            attoHTTPRESTSendHeaders(200, "application/json", NULL);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: application/json\r\n\r\n", write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultRESTSendHeadersExtraHeaders) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            attoHTTPRESTSendHeaders(200, "application/json", "Content-Encoding: gzip" HTTPEOL);
            return OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == OK), "Return was not 'OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nContent-Encoding: gzip\r\n\r\n", write_buffer);
    }
    FCT_TEST_END()



}
FCTMF_FIXTURE_SUITE_END();
