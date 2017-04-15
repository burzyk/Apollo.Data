/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
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
//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_NETWORK_H_
#define SRC_UTILS_NETWORK_H_

#include <stddef.h>

typedef int sdb_socket_t;

#define SDB_INVALID_SOCKET  ((sdb_socket_t)-1)

sdb_socket_t sdb_socket_listen(int port, int backlog);
sdb_socket_t sdb_socket_connect(const char *hostname, int port);
int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size);
int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size);
void sdb_socket_close(sdb_socket_t socket);
sdb_socket_t sdb_socket_accept(sdb_socket_t socket);

#endif  // SRC_UTILS_NETWORK_H_
