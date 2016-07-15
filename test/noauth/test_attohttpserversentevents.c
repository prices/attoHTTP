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

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\n\r\n";

#define WRITE_BUFFER_SIZE 1024
#define CheckDefault(ret) fct_xchk((ret == STATUS_SERVERSENTEVENTS), "Return was not 'STATUS_SERVERSENTEVENTS' (%d)", ret); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttpserversentevents)
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
    FCT_TEST_BGN(testServerSentEventHeader) {
        returncode_t ret;
        attoHTTPServerSetEventsURL("/sse");
        ret = attoHTTPExecute(
            (void *)"GET /sse HTTP/1.0\r\nAccept: text/html\r\n\r\n",
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
    FCT_TEST_BGN(testServerSentEventEvent) {
        uint16_t count;
        char *expect = "event:ASDF\n\n";

        count = attoHTTPSendEvent((void *)write_buffer, "ASDF", 4, "", 0);
        fct_xchk((strlen(expect) == count), "Count was %d, expected %d", count, strlen(expect));
        fct_chk_eq_str(expect, write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testServerSentEventData) {
        uint16_t count;
        char *expect = "data:fsda\n\n";
        count = attoHTTPSendEvent((void *)write_buffer, "", 0, "fsda", 4);
        fct_xchk((strlen(expect) == count), "Count was %d, expected %d", count, strlen(expect));
        fct_chk_eq_str(expect, write_buffer);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testServerSentEventBoth) {
        uint16_t count;
        char *expect = "event:ASDF\ndata:fsda\n\n";

        count = attoHTTPSendEvent((void *)write_buffer, "ASDF", 4, "fsda", 4);
        fct_xchk((strlen(expect) == count), "Count was %d, expected %d", count, strlen(expect));
        fct_chk_eq_str(expect, write_buffer);
    }
    FCT_TEST_END()

}
FCTMF_FIXTURE_SUITE_END();
