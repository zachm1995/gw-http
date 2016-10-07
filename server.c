/**
 * MIT License
 *
 * Copyright (c) 2012 Gabriel Parmer
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

#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
// #include <malloc.h>
#include <unistd.h>

/*
 * Create the file descriptor to accept on.  Return -1 otherwise.
 */

int
server_create(short int port)
{
	// Creates a new file descriptor
	int fd;

	// Create new sockaddr_in struct (declared in library)
	struct sockaddr_in server;

	// Check if socket is not established
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Establishing socket");

		// Return error code
		return -1;
	}

	// Set value to default values Address Family(AF) vs Protocol Family (PF)
	server.sin_family      = AF_INET;

	// Set port value to passed in port 
	server.sin_port        = htons(port);

	// Uses inet_aton function (in library) to get host address
	// INADDR_ANY allows unknown host IP
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	// Binds the socket (fd) to the server
	if (bind(fd, (struct sockaddr *)&server, sizeof(server))) {
		perror("binding receive socket");
		return -1;
	}

	// Permits accepting incoming connections, adds a queue limit (10)
	if(listen(fd, 10) < 0) {
		perror("listen call failed");
		return -1;
	}

	// Returns the file descriptor (socket)
	return fd;
}

/*
 * Pass in the accept file descriptor returned from
 * server_create. Return a new file descriptor or -1 on error.
 */

int
server_accept(int fd)
{
	// Create new sockaddr_in struct (declared in library)
	struct sockaddr_in sai;

	// Creates new file descriptor
	int new_fd;

	// Creates a variable for the length of the sai struct
	unsigned int len = sizeof(sai);

	// Sets the local file descriptor to an accepted file descriptor
	new_fd = accept(fd, (struct sockaddr *)&sai, &len);

	// Error handling
	if (-1 == new_fd) {
		perror("accept");
		return -1;
	}

	// Returns accepted file descriptor
	return new_fd;
}
