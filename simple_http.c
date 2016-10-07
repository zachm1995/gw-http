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

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <simple_http.h>

struct http_req *
shttp_alloc_req(int fd, char *request)
{
	// Create a new http_req[est] struct (defined in simple_http.h)
	struct http_req *r;

	// Asserts that the request exists
	assert(request);

	// Allocates memory for the local struct
	r = malloc(sizeof(struct http_req));

	// Error handling for malloc
	if (!r) return NULL;

	// Fills the struct with 0s
	memset(r, 0, sizeof(struct http_req));

	// Initializes the struct attribute to passed request
	r->request = request;

	// Initializes the struct attribute to length of request
	r->req_len = strlen(request);

	// Initializes the struct attribute to default value of NULL
	r->path = NULL;

	// Initializes the struct attribute to passed file descriptor
	r->fd = fd;

	// Returns a http_req struct with all the attributes initialized to values
	return r;
}

void
shttp_free_req(struct http_req *r)
{
	r->path = NULL;
	if (r->request)   free(r->request);
	if (r->response)  free(r->response);
	if (r->resp_head) free(r->resp_head);
	close(r->fd);
	free(r);
}

static inline char *
find_whitespace(char *s)
{
	// Asserts that the passed in char* exists
	assert(s);

	// Iterates through the char* and counts number of whitespaces
	while (*s != ' ' && *s != '\0') s++;

	// Returns the number of whitespaces in char*
	return s;
}

/*
 * Pass in the request.  Set the ->path field in r to point to the
 * path that is being requested.
 */

int
shttp_get_path(struct http_req *r)
{
	// Create char* for current position, end, and path
	char *curr, *end, *path;

	// Assert that r exists
	assert(r);

	// Assert that the request attribute in r exists
	assert(r->request);

	// Checks if request is a properly formatted HTTP GET request
	if (strncmp(r->request, "GET ", strlen("GET "))) return -1;

	// Sets the path variable to curr variable to the request attribute in r
	path = curr = r->request + sizeof("GET ")-1;

	// Sets end to the number of whitespaces in char*
	end = find_whitespace(curr);


	if (*end == '\0') return -1;
	*end = '\0';

	if (*path == '/') path++;
	r->path = path;

	return 0;
}

static const char success_head[] =
	"HTTP/1.1 200 OK\r\n"
	"Date: Sat, 14 Feb 2008 14:59:00 GMT\r\n"
	"Connection: close\r\n"
	"Content-Length: ";
#define MAX_DIGITS 128

/*
 * Creates the ->response field in r that includes "answer", but also
 * includes other data.
 */
int
shttp_alloc_response_head(struct http_req *r, char *data, int dlen)
{
	int  head_sz, pre_sz, len_sz;
	char len_str[MAX_DIGITS];
	char *resp_hd;

	r->response = data;
	r->resp_len = dlen;

	len_sz  = snprintf(len_str, MAX_DIGITS, "%d\r\n\r\n", r->resp_len);
	if (len_sz < 1) return -1;

	pre_sz  = strlen(success_head);
	assert(pre_sz > 0);
	head_sz = pre_sz + len_sz;
	assert(head_sz > 0);
	resp_hd = malloc(head_sz);
	if (!resp_hd)   return -1;

	r->resp_head   = resp_hd;
	r->resp_hd_len = head_sz;

	memcpy(resp_hd, success_head, pre_sz);
	memcpy(resp_hd + pre_sz, len_str, len_sz);

	return 0;
}
