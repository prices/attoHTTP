/**
 * @file    src/wrapper_unix_sockets.h
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

#include "application.h"
#include "attohttp.h"

// Done include this bit until the attohttp.h file has been included

/** This is our unix socket */
TCPServer *w_server;

/**
 * @brief The init function for the wrapper
 *
 * This function creates the server socket, and sets everything up
 *
 * @param port The port to open
 *
 * @return None
 */
void
attoHTTPWrapperInit(uint16_t port)
{
    attoHTTPInit();
    w_server = new TCPServer(port);
    w_server->begin();
}
/**
 * @brief The main function for the wrapper
 *
 * This runs everything.  It returns after servicing one socket (or none if
 * there are no requests).  It must be called in a loop
 *
 * @param setup This may or may not be used in the future.
 *
 * @return None
 */
void
attoHTTPWrapperMain(uint8_t setup)
{
    TCPClient client = w_server->available();
    if (client) {
        attoHTTPExecute((void *)&client, (void *)&client);
        client.flush();
    }
    client.stop();

}
/**
 * @brief The end function for the wrapper
 *
 * This function closes all of the open sockets, and closes other stuff down.
 *
 * @return None
 */
void
attoHTTPWrapperEnd(void)
{
    delete w_server;
}

/**
 * @brief User function to get a byte
 *
 * This function must be defined by the user.  It will allow this software to
 * get bytes from any source.
 * 
 * This function should only return when it has something (ret == 1), when it
 * timed out waiting for something (ret == 0), or when there was an error (ret == -1)
 *
 * @param read This is whatever it needs to be.  Could be a socket, or an object,
 *              or something totally different.  It will be called with whatever
 *              extra argument was given to the execute routine.
 * @param byte  A pointer to the byte we need to put the next character in.
 *
 * @return 1 if a character was read, 0 otherwise.
 */
int16_t
attoHTTPGetByte(void *read, uint8_t *byte) {
    int16_t ret = 0;
    TCPClient *client = (TCPClient *)read;
    uint32_t timeout = millis() + 1000;
    int c;
    if (client->connected()) {
        do {
            if (client->available() > 0) {
                c = client->read();
                if (c >= 0) {
                    *byte = (c & 0xFF);
                    ret = 1;
                }
            }
        } while ((ret == 0) && (timeout > millis()));
    } else {
        ret = -1;
    }
    return ret;
}
/**
 * @brief User function to set a byte
 *
 * This function must be defined by the user.  It will allow this software to
 * set bytes to any destination.
 *
 * @param write This is whatever it needs to be.  Could be a socket, or an object,
 *              or something totally different.  It will be called with whatever
 *              extra argument was given to the execute routine.
 * @param byte  A pointer to the byte we need to put the next character in.
 *
 * @return 1 if a character was read, 0 otherwise.
 */
uint16_t
attoHTTPSetByte(void *write, uint8_t byte) {
    TCPClient *client = (TCPClient *)write;
    return client->write((const uint8_t *)&byte, 1);
}
