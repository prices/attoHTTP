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
#include <stdint.h>
#include <inttypes.h>
#include "test.h"

uint8_t *TestWriteString, *TestReadString;
uint16_t TestWriteCount, TestReadCount;

FCT_BGN()
{
    FCTMF_SUITE_CALL(test_attohttp);
}
FCT_END();


void
TestInit(void)
{
    TestWriteString = NULL;
    TestReadString = NULL;
    TestWriteCount = 0;
    TestReadCount = 0;
}


uint16_t
attoHTTPGetByte(void *extra, uint8_t *byte)
{
    if (TestReadString == NULL) {
        TestReadString = (uint8_t *)extra;
    }
    if ((TestReadString != NULL) && (byte != NULL)) {
        *byte = TestReadString[TestReadCount];
        if (*byte > 0) {
            // If we get the end of string, just keep returning it.
            TestReadCount++;
        }
    } else {
        return 0;
    }
    return (*byte == 0) ? 0 : 1;
}

uint16_t
attoHTTPSetByte(void *extra, uint8_t byte)
{
    if (TestWriteString == NULL) {
        TestWriteString = (uint8_t *)extra;
    }
    if (TestWriteString != NULL) {
        TestWriteString[TestWriteCount] = byte;
        TestWriteCount++;
    }
    return 1;
}

