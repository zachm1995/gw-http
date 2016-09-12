# gw-http

HTTP web and networking server by the GWU SHC group!

## Use

```
$ make
$ ./gw-http 8080
...point your browser to localhost:8080/shc.html...
$ make clean
```

## Project ideas

Expand this code to

- **[EASY]** add an awesome 404 page with tons of awesome tech
- **[EASY]** add other error pages
- **[MEDIUM]** trivial message board
 - add a [HTTP PUT](http://www.restapitutorial.com/lessons/httpmethods.html) handler (10 lines of code)
 - add a linked-list or otherwise data-structure to track the messages that have been put
 - add a webpage that displays these messages, with a prompt at the bottom that makes the next PUT request
 - add auto-refresh logic to the webpage
 - now you have a trivial message board!
- **[HARD]** Add [fastcgi](https://en.wikipedia.org/wiki/FastCGI) support
 - Also integrate with [memcache](https://memcached.org/) to cache output because performance!
- **[MEDIUM]** add messaging logic so that this is also a message server with a REST API for all of its functionality
- **[EASY]** perform routing so that certain paths (e.g. `/gwmsg/`) go to specific plugins
- **[MEDIUM]** add a plugin facility so that different server tasks can be plugged in (e.g. messaging)
- **[EASY]** include more HTTP options and extensions
- **[MEDIUM]** include Rust extensions
- **[HARD]** add isolation of plugins using 1. processes, then 2. `chroot` or container equivalents
- **[HARD]** integrate with **slack** as a plugin
- **[MEDIUM]** add Travis CI integration on github
- **[TRIVIAL]** RULE THE WORLD

Add your own!
