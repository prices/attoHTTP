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

uint8_t attoHTTPParseJSONParam(char *name, uint8_t name_len, char *value, uint8_t value_len);

static const uint8_t default_content[] = "Default";
static const char default_return[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 8\r\n\r\nDefault";

#define WRITE_BUFFER_SIZE 1024
#define CheckUnsupported(ret) fct_xchk((ret == STATUS_UNSUPPORTED), "Return was not 'STATUS_UNSUPPORTED'"); fct_chk_eq_str("HTTP/1.0 501 Not Implemented\r\n", write_buffer)
#define CheckNotFound(ret) fct_xchk((ret == STATUS_NOT_FOUND), "Return was not 'STATUS_NOT_FOUND'"); fct_chk_eq_str("HTTP/1.0 404 Not Found\r\n", write_buffer)
#define CheckDefault(ret) fct_xchk((ret == STATUS_OK), "Return was not 'STATUS_OK'"); fct_chk_eq_str(default_return, write_buffer)


char write_buffer[WRITE_BUFFER_SIZE];

FCTMF_FIXTURE_SUITE_BGN(test_attohttpjson)
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
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_Simple) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"World\" }";
        while (ret) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("World", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()

    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_ColonInValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"World:I am here\" }";
        while (ret) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("World:I am here", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_SingleQuoteInValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"I'm here\" }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("I'm here", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_DoubleQuoteInValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": 'I\"m here' }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("I\"m here", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_DoubleQuoteInDoubleQuoteInValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"I\"m here\" }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("Imhere ", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_IntValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": 1234 }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("1234", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_BadIntValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": 1a2b3c4d }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("1a2b3c4d", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_NameTooBig) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello012345678901234567890123456789A\": \"There\" }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello01234567890123", name);
                fct_chk_eq_str("There", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_ValueTooBig) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"There012345678901234567890123456789A\" }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("There01234567890123", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_NegativeIntValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": -1234 }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("-1234", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_FloatValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": 1.234 }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("1.234", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_NegativeFloatValue) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": -1.234 }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                fct_chk_eq_str("Hello", name);
                fct_chk_eq_str("-1.234", value);
                found++;
            }
        }
        fct_xchk((1 == found), "Found: %d != 1", found);
    }
    FCT_TEST_END()
    /**
     * @brief This tests JSONArray* functions
     *
     * @return void
     */
    FCT_TEST_BGN(testPowerPort_JSONParse_object) {
        char name[20];
        char value[20];
        uint8_t ret = 1;
        uint8_t found = 0;
        TestReadString = (uint8_t *)"{ \"Hello\": \"World\", \"I am\": \"here\" }";
        while (ret == 1) {
            ret = attoHTTPParseJSONParam(name, sizeof(name), value, sizeof(value));
            if (ret == 1) {
                if (found == 0) {
                    fct_chk_eq_str("Hello", name);
                    fct_chk_eq_str("World", value);
                } else if (found == 1) {
                    fct_chk_eq_str("I am", name);
                    fct_chk_eq_str("here", value);
                }
                found++;
            }
        }
        fct_xchk((2 == found), "Found: %d != 2", found);
    }
    FCT_TEST_END()
}
FCTMF_FIXTURE_SUITE_END();
