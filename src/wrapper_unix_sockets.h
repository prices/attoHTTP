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
#include <sys/time.h>
#include <time.h>

#include "attohttp.h"

#ifdef __ATTOHTTP_H_DONE__
// Done include this bit until the attohttp.h file has been included

/** This is our unix socket */
int attoHTTPUnixSock;

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
#ifdef _DEBUG_
    printf("Disconnected from socket %d\r\n", attoHTTPUnixSock);
#endif
    attoHTTPUnixSock = -1;
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
static inline void
attoHTTPWrapperInit(uint16_t port)
{
    attoHTTPInit();
    struct sockaddr_in server;
    int t;
    int ret = -1;
    attoHTTPUnixSock = -1;
    if ((attoHTTPUnixSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    printf("Trying to create network socket on port %d...\r\n", port);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    errno = 0;
    t = time(NULL);
    while ((ret != 0) && (t > (time(NULL) - 60))) {
        ret = bind(attoHTTPUnixSock, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
        if (ret != 0) {
            sleep(1);
        }
    }
    if (ret == 0) {
        if (listen(attoHTTPUnixSock, 1) < 0) {
            perror("listen");
            attoHTTPWrapperEnd();
            exit(EXIT_FAILURE);
        }
#ifdef _DEBUG_
        printf("Connected on port %d on socket %d.\r\n", port, attoHTTPUnixSock);
#endif
    } else {
        perror("bind");
        attoHTTPWrapperEnd();
        exit(EXIT_FAILURE);
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
static inline void
attoHTTPWrapperMain(uint8_t setup)
{
    socklen_t size;
    struct sockaddr_in clientname;
    fd_set active;
    int16_t newSock;
    int ret;
    if (attoHTTPUnixSock > 0) {
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
#ifdef _DEBUG_
            printf("New connection on socket %d\r\n", newSock);
#endif
            attoHTTPExecute((void *)&newSock, (void *)&newSock);
#ifdef _DEBUG_
            printf("Closing connection on socket %d\r\n", newSock);
#endif
            close(newSock);
        }
    }

}
#endif //#ifdef __ATTOHTTP_H_DONE__

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
static inline int8_t
attoHTTPGetByte(void *read, uint8_t *byte) {
    int16_t sock = *(int16_t *)read;
    int16_t ret = 0;
    struct timeval timeout = {1, 0};

    fd_set active;
    if (sock > 0) {
        FD_ZERO(&active);
        FD_SET(sock, &active);
        do {
            ret = select(FD_SETSIZE, &active, NULL, NULL, &timeout);
            if (ret < 0) {
                if (errno != EINTR) {
                    perror("select");
                    close(sock);
                    exit(errno);
                }
            } else if ((ret > 0) && FD_ISSET(sock, &active)) {
                ret = recv(sock, byte, 1, 0);
            }
        } while (ret < 0);
#ifdef __DEBUG__
        if (ret > 0) {
            if (*byte < 32) {
                printf("[%d]", *byte);
            }
            if (*byte == 10) {
                printf("\r");
            }
            if (*byte != 13) {
                printf("%c", *byte);
            }
        }
#endif
        
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
static inline int16_t
attoHTTPSetByte(void *write, uint8_t byte) {
    int16_t ret = 0;
    int16_t sock = *(int16_t *)write;
    while (ret <= 0) {
        ret = send(sock, &byte, 1, MSG_NOSIGNAL);
        if (ret < 0) {
            if ((errno != EINTR) && (errno != EAGAIN)) {
#ifdef __DEBUG__
                perror("Send");
#endif
                break;
            }
        }
    }
    return ret;
}


#endif // #ifndef __ATTOHTTP_H__
