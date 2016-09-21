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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <simple_http.h>

/* 10 MB is max size */
#define MAX_CONTENT_SZ (1024*1024*10)

static char *
error_resp(char *path, int *len)
{
	//const char eresponse[] = "<html><head><title>X-P</title></head><body><font face=\"sans-serif\"><center><h1>X-P</h1><p>Could not find content at <b>%s</b>.</p><p><a href=\"http://giphy.com/gifs/14kdiJUblbWBXy/tile\">HACK THE PLANET!</a></p></center></font></body></html>";
	const char eresponse[] = "<font face=\"sans-serif\"><center><h1>X-P</h1><p>Could not find content at <b>%s</b>.</p><p><a href=\"http://giphy.com/gifs/14kdiJUblbWBXy/tile\">HACK THE PLANET!</a></p></center></font>";
	char *resp;
	int sz = strlen(eresponse) + strlen(path);

	resp = malloc(sz);
	if (!resp) return NULL;
	*len = sprintf(resp, eresponse, path);
	return resp;
}

typedef char *(*content_fn_t)(struct http_req *r, char *path, int *content_len);

#define NMSG 64
#define LINE_MAXSZ 256
struct messages {
	char  *ms[NMSG];
	int    head;
};
static struct messages msgs;
static const char *line_chrome = "<li>%s</li>";
static const char *chrome_pre  = "<ul>";
static const char *chrome_post = "</ul>";
static const size_t max_resp   = sizeof(chrome_pre) + sizeof(chrome_post) + ((LINE_MAXSZ + sizeof(line_chrome)) * NMSG);
static char buff[sizeof(chrome_pre) + sizeof(chrome_post) + ((LINE_MAXSZ + sizeof(line_chrome)) * NMSG)];

static char *
get_shcmsg(struct http_req *r, char *path, int *content_len)
{
	size_t len;
	char  *resp;
	char  *msg, *curs;
	int i;
	(void)path;
	(void)r;

	/* TODO: make the path include msg/channel to select different channels */

	/* add the message if there is one! */
	if (r->val) {
		len = strlen(r->val);
		msg = malloc(len+1);
		strcpy(msg, r->val);
		if (msgs.ms[msgs.head]) free(msgs.ms[msgs.head]);
		msgs.ms[msgs.head] = msg;
		msgs.head = (msgs.head + 1) % NMSG;
	}

	/* construct the response */
	buff[0] = '\0';
	strcat(buff, chrome_pre);
	curs = &buff[sizeof(chrome_pre)];
	for (i = 0 ; i < NMSG ; i++) {
		char *line = msgs.ms[(i + msgs.head) % NMSG];
		int consumed = 0;

		if (!line) continue;
		consumed = snprintf(curs, (max_resp - (curs-buff) - sizeof(chrome_pre)), line_chrome, line);
		curs += consumed;
		assert(curs < buff + max_resp);
	}
	strcat(curs, chrome_post);
	curs += sizeof(chrome_post);

	len  = curs - buff + 1;
	resp = malloc(len);
	assert(resp);
	strcpy(resp, buff);
	*content_len = len;

	return resp;
}

static char *
get_file(struct http_req *r, char *path, int *content_len)
{
	int content_fd, amnt_read = 0;
	struct stat s;
	char *resp;
	(void)r;

	if (stat(path, &s) ||
	    s.st_size > MAX_CONTENT_SZ) goto err;

	content_fd = open(path, O_RDONLY);
	if (content_fd < 0) goto err;

	resp = malloc(s.st_size);
	if (!resp) goto err_close;

	while (amnt_read < s.st_size) {
		int ret = read(content_fd, resp + amnt_read,
			       s.st_size - amnt_read);

		if (ret < 0) goto err_free;
		amnt_read += ret;
	}
	*content_len = s.st_size;

	return resp;
err_free:
	free(resp);
err_close:
	close(content_fd);
err:
	return NULL;
}

struct route_tbl {
	char *path;
	content_fn_t handler;
} routes[] = {
	{.path = "msg/",   .handler = get_shcmsg },
	{.path = "",       .handler = get_file }, /* default path...should match all queries */
	{.path = NULL,     .handler = NULL}
};

static char *
route(struct http_req *r, char *path, int *content_len)
{
	int i;

	for (i = 0 ; routes[i].path ; i++) {
		if (!strstr(path, routes[i].path)) continue;
		assert(routes[i].handler);
		return routes[i].handler(r, path, content_len);
	}

	assert(0); 		/* Should NOT get here...see default case above */
	return NULL;
}

static int
sanity_check(char *path)
{ return (path[0] == '.' || path[0] == '/'); }

char *
content_get(struct http_req *r, char *path, int *content_len)
{
	char *resp;

	/* Bad path?  No file?  Too large? */
	if (sanity_check(path))                    goto err;
	if (!(resp = route(r, path, content_len))) goto err;

	return resp;
err:
	return error_resp(path, content_len);
}
