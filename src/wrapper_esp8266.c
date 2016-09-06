/**
 * @file    src/wrapper_esp8266.c
 * @author  Scott L. Price <prices@dflytech.com>
 * @note    (C) 2016  Scott L. Price
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
#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "c_types.h"
#include "user_config.h"
#include "user_interface.h"
#include "attohttp.h"

#ifndef  ATTO_MAX_CONN
#define  ATTO_MAX_CONN 1
#endif
/****
 * information on reading byte data in flash:
 *
 * http://richard.burtons.org/2015/07/08/accessing-byte-data-stored-on-flash/
 */


// Listening connection stuff
static struct espconn attoHTTPServer;
static esp_tcp attoHTTPTcp;

typedef struct attoHTTPConnections {
    char *buffer;
    uint16_t length;
    uint16_t ptr;
    uint8_t active;
} attoHTTPConnections_t;

attoHTTPConnections_t esp8266Connections[ATTO_MAX_CONN];

void
attoHTTPClearServerBuffer(attoHTTPConnections_t *conn)
{
    conn->active = 0;
    conn->length = 0;
    conn->ptr = 0;
}

void ICACHE_FLASH_ATTR
attoHTTPDisconnectcb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    if (conn->reverse != NULL) {
        attoHTTPClearServerBuffer(conn->reverse);
    }
    espconn_disconnect(conn);
    conn->reverse = NULL;
}

void ICACHE_FLASH_ATTR
attoHTTPRecvcb(void *arg, char *data, uint16_t len)
{
    struct espconn *conn = (struct espconn *)arg;
    if (conn->reverse == NULL) {
        return;
    }
    attoHTTPConnections_t *cdata = conn->reverse;
    cdata->buffer = data;
    cdata->length = len;
    cdata->ptr = 0;
    attoHTTPExecute((void *)&conn, (void *)&conn);
}

void ICACHE_FLASH_ATTR
attoHTTPConnectcb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    uint16_t i;
    for (i = 0; i < ATTO_MAX_CONN; i++) {
        if (esp8266Connections[i].active == 0) {
            break;
        }
    }
    if (i > ATTO_MAX_CONN) {
        espconn_disconnect(conn);
        conn->reverse = NULL;
    } else {
        //espconn's have a extra flag you can associate extra information with a connection.
        conn->reverse = &esp8266Connections[i];

        //Let's register a few callbacks, for when data is received or a disconnect happens.
        espconn_regist_recvcb(conn, attoHTTPRecvcb);
        espconn_regist_disconcb(conn, attoHTTPDisconnectcb);
        esp8266Connections[i].active = 1;
    }
}

/**
 * @brief The init function for the wrapper
 *
 * This function creates the server socket, and sets everything up
 *
 * @param port The port to open
 *
 * @return None
 */
void ICACHE_FLASH_ATTR
attoHTTPWrapperInit(uint16_t port)
{
    uint16_t i;

    attoHTTPInit();
    attoHTTPServer.type = ESPCONN_TCP;
    attoHTTPServer.state = ESPCONN_NONE;

    attoHTTPServer.proto.tcp = &attoHTTPTcp;
    attoHTTPServer.proto.tcp->local_port = 80;

    espconn_regist_connectcb(&attoHTTPServer, attoHTTPConnectcb);
    espconn_accept(&attoHTTPServer);

    espconn_regist_time(&attoHTTPServer, 15, 0);
    espconn_tcp_set_max_con_allow(&attoHTTPServer, ATTO_MAX_CONN);

    for (i = 0; i < ATTO_MAX_CONN; i++) {
        attoHTTPClearServerBuffer(&esp8266Connections[i]);
    }
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
void ICACHE_FLASH_ATTR
attoHTTPWrapperMain(uint8_t setup)
{

}
/**
 * @brief The end function for the wrapper
 *
 * This function closes all of the open sockets, and closes other stuff down.
 *
 * @return None
 */
void ICACHE_FLASH_ATTR
attoHTTPWrapperEnd(void)
{
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
    struct espconn *conn = (struct espconn *)read;
    if (conn->reverse == NULL) {
        return 0;
    }
    attoHTTPConnections_t *cdata = conn->reverse;
    if (cdata->ptr >= cdata->length) {
        return 0;
    }
    *byte = cdata->buffer[cdata->ptr];
    cdata->ptr++;
    return 1;
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
attoHTTPSetByte(void *write, uint8_t byte)
{
    struct espconn *conn = (struct espconn *)write;
    if (conn->reverse == NULL) {
        return 0;
    }
    return (espconn_send(conn, &byte, 1) == 0);
}
