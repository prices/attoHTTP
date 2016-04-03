/**
 * @file    src/attohttpAPI.c
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
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == STATUS_UNSUPPORTED), "Return was not 'STATUS_UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == STATUS_NOT_FOUND), "Return was not 'STATUS_NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'"); fct_chk_eq_str(default_return, write_buffer)


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
            fct_xchk((method == METHOD_GET), "Return was not 'METHOD_GET'");
            fct_xchk((cmdlvl == 2), "Command level was not 2");
            fct_xchk((idlvl == 1), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_xchk((id[1] == NULL), "id[1] != NULL");
            fct_xchk((command[2] == NULL), "command[2] != NULL");
            fct_xchk((id[2] == NULL), "id[2] != NULL");
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1/1/level2 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testPOSTDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_POST), "Return was not 'METHOD_POST'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"POST /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testPUTDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_PUT), "Return was not 'METHOD_PUT'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PUT /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testPATCHDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_PATCH), "Return was not 'METHOD_PATCH'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"PATCH /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testDELETEDefaultREST) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_DELETE), "Return was not 'METHOD_DELETE'");
            fct_xchk((cmdlvl == 3), "Command level was not 2");
            fct_xchk((idlvl == 3), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_chk_eq_str("level2", (char *)command[1]);
            fct_chk_eq_str("2", (char *)id[1]);
            fct_chk_eq_str("level3", (char *)command[2]);
            fct_chk_eq_str("3", (char *)id[2]);
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"DELETE /level1/1/level2/2/level3/3 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testGETDefaultRESTExtraSlashCommand) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_GET), "Return was not 'METHOD_GET'");
            fct_xchk((cmdlvl == 1), "Command level was not 1");
            fct_xchk((idlvl == 0), "Id level was not 0");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_xchk((id[0] == NULL), "id[0] != NULL");
            fct_xchk((command[1] == NULL), "command[1] != NULL");
            fct_xchk((id[1] == NULL), "id[1] != NULL");
            fct_xchk((command[2] == NULL), "command[2] != NULL");
            fct_xchk((id[2] == NULL), "id[2] != NULL");
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1/ HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testGETDefaultRESTExtraSlashId) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_DELETE), "Return was not 'METHOD_DELETE'");
            fct_xchk((cmdlvl == 1), "Command level was not 1");
            fct_xchk((idlvl == 1), "Id level was not 1");
            fct_xchk((accepted == (1<<APPLICATION_JSON)), "Accepted was not correct");
            fct_chk_eq_str("level1", (char *)command[0]);
            fct_chk_eq_str("1", (char *)id[0]);
            fct_xchk((command[1] == NULL), "command[1] != NULL");
            fct_xchk((id[1] == NULL), "id[1] != NULL");
            fct_xchk((command[2] == NULL), "command[2] != NULL");
            fct_xchk((id[2] == NULL), "id[2] != NULL");
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"DELETE /level1/1/ HTTP/1.0\r\nAccept: application/json\r\n\r\n",
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
    FCT_TEST_BGN(testGETDefaultRESTSendHeadersBadCode) {
        returncode_t ret;

        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            attoHTTPRESTSendHeaders(12345, "application/json", NULL);
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        fct_chk_eq_str("HTTP/1.0 500 Internal Error\r\nContent-Type: application/json; charset=utf-8\r\n\r\n", write_buffer);
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
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n\r\n", write_buffer);
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
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Encoding: gzip\r\n\r\n", write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testGETDefaultRESTPrintfBufferOverflow) {
        returncode_t ret;
        
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            attoHTTPRESTSendHeaders(200, "application/json", "Content-Encoding: gzip" HTTPEOL);
            attoHTTPprintf("%s", "0123456789ABCDEF1123456789ABCDEF2123456789ABCDEF3123456789ABCDEF4123456789ABCDEF5123456789ABCDEF6123456789ABCDEF7123456789ABCDEF8123456789ABCDEF9123456789ABCDEF");
            return STATUS_OK;
        }

        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);
        ret = attoHTTPExecute(
            (void *)"GET /level1 HTTP/1.0\r\nAccept: application/json\r\n\r\n",
                              (void *)write_buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        fct_chk_eq_str("HTTP/1.0 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nContent-Encoding: gzip\r\n\r\n0123456789ABCDEF1123456789ABCDEF2123456789ABCDEF3123456789ABCDEF4123456789ABCDEF5123456789ABCDEF6123456789ABCDEF7123456789ABCDE", write_buffer);
    }
    FCT_TEST_END()



}
FCTMF_FIXTURE_SUITE_END();
