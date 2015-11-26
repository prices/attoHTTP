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

#define WRITE_BUFFER_SIZE 1024

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
    FCT_TEST_BGN(testBase64Encode_001) {
        uint16_t ret;
        int8_t input[] = "input:string";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(16, ret);
        fct_chk_eq_str("aW5wdXQ6c3RyaW5n", (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Encode_002) {
        uint16_t ret;
        int8_t input[] = "input:string1";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(20, ret);
        fct_chk_eq_str("aW5wdXQ6c3RyaW5nMQ==", (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Encode_003) {
        uint16_t ret;
        int8_t input[] = "input:string12";
        int8_t output[64];
        ret = attoHTTPBase64Encode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(20, ret);
        fct_chk_eq_str("aW5wdXQ6c3RyaW5nMTI=", (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Decode_001) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5n";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(12, ret);
        fct_chk_eq_str("input:string", (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Decode_002) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMQ==";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(14, ret);
        fct_chk_eq_str("input:string1", (char *)output);
    }
    FCT_TEST_END()
    /**
     * @brief This tests the empty queue functions
     *
     * @return void
     */
    FCT_TEST_BGN(testBase64Decode_003) {
        uint16_t ret;
        int8_t input[] = "aW5wdXQ6c3RyaW5nMTI=";
        int8_t output[64];
        ret = attoHTTPBase64Decode(input, sizeof(input) - 1, output, sizeof(output));
        CheckRet(15, ret);
        fct_chk_eq_str("input:string12", (char *)output);
    }
    FCT_TEST_END()

}
FCTMF_FIXTURE_SUITE_END();
