/**
 * @file    src/attohttp.h
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
#ifndef __WRAPPER_UNIX_SOCKETS_H__
#define __WRAPPER_UNIX_SOCKETS_H__

#include <stdint.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "attohttp.h"

#ifdef __ATTOHTTP_H_DONE__
// Done include this bit until the attohttp.h file has been included

/** @var This is our unix socket */
int attoHTTPUnixSock;

/**
 * @brief The init function for the wrapper
 *
 * This function creates the server socket, and sets everything up
 *
 * @param port The port to open
 *
 * @return None
 */
static inline void
attoHTTPWrapperInit(uint16_t port, uint16_t flags)
{
    attoHTTPInit(flags);
    struct sockaddr_in server;
    attoHTTPUnixSock = -1;
    if ((attoHTTPUnixSock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    printf("Trying to create network socket on port %d...\n", port);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    errno = 0;

    if (bind(attoHTTPUnixSock, (struct sockaddr *) &server, sizeof(struct sockaddr_in))) {
        perror("binding stream socket");
        exit(EXIT_FAILURE);
    }
    if (listen(attoHTTPUnixSock, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Connected on port %d on socket %d.\n", port, attoHTTPUnixSock);
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
static inline void
attoHTTPWrapperMain(uint8_t setup)
{
    socklen_t size;
    struct sockaddr_in clientname;
    fd_set active;
    int16_t newSock;
    int ret;
    FD_ZERO(&active);
    FD_SET(attoHTTPUnixSock, &active);
    if ((ret = select(FD_SETSIZE, &active, NULL, NULL, NULL)) < 0) {
        if (errno != EINTR) {
            perror("select");
            exit(EXIT_FAILURE);
        }
    }

    if ((ret > 0) && FD_ISSET(attoHTTPUnixSock, &active)) {
        // Connection request on original socket.
        size = sizeof(clientname);
        newSock = accept(attoHTTPUnixSock, (struct sockaddr *) &clientname, &size);
        if (newSock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New connection on socket %d\r\n", newSock);
        attoHTTPExecute((void *)&newSock, (void *)&newSock);
        printf("Closing connection on socket %d\r\n", newSock);
        close(newSock);
    }


}
/**
 * @brief The end function for the wrapper
 *
 * This function closes all of the open sockets, and closes other stuff down.
 *
 * @return None
 */
static inline void
attoHTTPWrapperEnd(void)
{
    close(attoHTTPUnixSock);
    printf("Disconnected from socket %d\n", attoHTTPUnixSock);
}
#endif //#ifdef __ATTOHTTP_H_DONE__

/**
 * @brief User function to get a byte
 *
 * This function must be defined by the user.  It will allow this software to
 * get bytes from any source.
 *
 * @param read This is whatever it needs to be.  Could be a socket, or an object,
 *              or something totally different.  It will be called with whatever
 *              extra argument was given to the execute routine.
 * @param byte  A pointer to the byte we need to put the next character in.
 *
 * @return 1 if a character was read, 0 otherwise.
 */
static inline uint16_t
attoHTTPGetByte(void *read, char *byte) {
    int16_t sock = *(int16_t *)read;
    uint16_t ret = 0;
    fd_set active;
    if (sock > 0) {
        FD_ZERO(&active);
        FD_SET(sock, &active);
        if ((ret = select(FD_SETSIZE, &active, NULL, NULL, NULL)) < 0) {
            if (errno != EINTR) {
                perror("select");
                exit(errno);
            }
        }
        if ((ret > 0) && FD_ISSET(sock, &active)) {
            ret = recv(sock, byte, 1, 0);
#ifdef __DEBUG
            if (ret) {
                if (*byte < 32) {
                    printf("(%d)", *byte);
                } else {
                    printf("%c", *byte);
                }
            }
#endif
        }
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
static inline uint16_t
attoHTTPSetByte(void *write, char byte) {
    int16_t sock = *(int16_t *)write;
    return send(sock, &byte, 1, 0);
}


#endif // #ifndef __ATTOHTTP_H__
