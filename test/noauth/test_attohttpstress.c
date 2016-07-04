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
#include "bigfile.h"

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == STATUS_UNSUPPORTED), "Return was not 'STATUS_UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == STATUS_NOT_FOUND), "Return was not 'STATUS_NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'"); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttpstress)
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
    FCT_TEST_BGN(testGETManyTimes) {
        returncode_t ret;
        uint32_t i;
        returncode_t testCallback(httpmethod_t method, uint16_t accepted, uint8_t **command, uint8_t **id, uint8_t cmdlvl, uint8_t idlvl)
        {
            fct_xchk((method == METHOD_GET), "Method was not 'METHOD_GET'");
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
        char read_buffer[] = "GET /level1/1/level2 HTTP/1.0\r\nAccept: application/json\r\n\r\n";
        attoHTTPDefaultREST(testCallback);
        attoHTTPAddPage("/index.html", default_content, sizeof(default_content), TEXT_HTML);

        for (i = 0; i < 65535; i++) {
            NewConnection();
            ret = attoHTTPExecute(
                (void *)read_buffer,
                (void *)write_buffer
            );
            fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        }
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBigFile) {
        returncode_t ret;
        char buffer[BIGFILE_LEN + 10000];
        char read_buffer[] = "GET /index.html HTTP/1.0\r\nAccept: application/json\r\n\r\n";
        attoHTTPAddPage("/index.html", bigfile, BIGFILE_LEN, TEXT_HTML);

        NewConnection();
        ret = attoHTTPExecute(
            (void *)read_buffer,
            (void *)buffer
        );
        fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'");
        fct_xchk((strlen(buffer) == (BIGFILE_LEN + 84)), "Return was not big enough");

    }
    FCT_TEST_END()

}
FCTMF_FIXTURE_SUITE_END();
