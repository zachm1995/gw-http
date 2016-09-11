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

- **[MEDIUM]** add messaging logic so that this is also a message server with a REST API for all of its functionality
- **[EASY]** perform routing so that certain paths (e.g. `/gwmsg/`) go to specific plugins
- **[MEDIUM]** add a plugin facility so that different server tasks can be plugged in
- **[EASY]** include more HTTP options and extensions
- **[MEDIUM]** include Rust extensions
- **[TRIVIAL]** RULE THE WORLD

Add your own!
