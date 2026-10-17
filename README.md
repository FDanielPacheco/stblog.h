# stblog.h

A small single-header logging library for C and C++. By default, it logs nicely formatted messages to the console (stderr), but custom handlers and output streams can easily be provided.

## Default example

```
[example1][INFO ] (src/mixlink.c:1048) :block block2 [t_tick] success init ... 
[example1][INFO ] (src/mixlink.c:1048) :block block3 [t_tick] success init ... 
[example1][INFO ] (src/mixlink.c:1048) :block block1 [t_tick] success init ... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1]
00000000  54 68 69 73 20 69 73 20  61 20 74 72 61 6e 73 66  |This is a transf|
00000010  65 72                                             |er|
00000012
[example1]
00000000  54 68 69 73 20 69 73 20  61 20 74 72 61 6e 73 66  |This is a transf|
00000010  65 72                                             |er|
00000012
[example1]
00000000  54 68 69 73 20 69 73 20  61 20 74 72 61 6e 73 66  |This is a transf|
00000010  65 72                                             |er|
00000012
[example1][WARN ] (src/mixlink.c:694) :Child exited not handled 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
[example1][INFO ] (src/mixlink.c:1018) :Notified systemd... 
^\[example1][INFO ] (src/mixlink.c:1210) :sigquit pressed... 
[example1][INFO ] (src/mixlink.c:1060) :block block2 [t_tick] success deinit ... 
[example1][INFO ] (src/mixlink.c:1060) :block block3 [t_tick] success deinit ... 
[example1][INFO ] (src/mixlink.c:1060) :block block1 [t_tick] success deinit ... 
[example1][INFO ] (src/mixlink.c:1335) :end gracefully 
```

## Usage Examples

To use `stblog.h`, define `STBLOG_IMPLEMENTATION` in one C or C++ source file before including the header. In all other source files, simply include the header normally.

### Default Logger 
This example demonstrates using the default console logger to report a serial port opening failure.

```c
#define STBLOG_IMPLEMENTATION
#include "stblog.h"
// ...
#include <xcserial.h>
#define PORT "/dev/ttyUSB0"
// ...

int main() {
        // ...
        int err = serial_open(&serial, PORT);
        if (0 > err) { // this function from xcserial.h returns -errno code
                LOG_ERRNO(
                        0,                       // use default logger (stdout)
                        "example-app-name",      // program name that is reporting log
                        -err,                    // errno based code 
                        LOG_ERROR,               // type of message
                        "%s not available", PORT // custom message
                );
                // ...
        }
        // ...
}
```

### Default Hexdump
This example shows how to log incoming raw data buffers in a pretty hexadecimal format using the default logger.

```c
#define STBLOG_IMPLEMENTATION
#include "stblog.h"
#define APP_NAME "example-app-name"
// ...
#include <xcserial.h>
// ...

int main() {
        // ...
        int errr = 0; 
        uint8_t rx_buf[16];
        ssize_t bytes_read = serial_read(rx_buf, 1, sizeof(rx_buf), 0, &serial, &err);

        if (0 > err) { // this function from xcserial.h returns -errno code
                LOG_ERRNO(0, APP_NAME,-err, LOG_ERROR, "failed to read");
                // ...
        }
        LOG_HEXDUMP(0, APP_NAME, LOG_INFO, rx_buf, bytes_read);
        // ...
}
```

### Custom logger
This example employs a custom logger handler to output logs with a customized layout.

```c
//...
void handler(struct stblog_report repo, const char *msg) {
        fprintf( 
                stderr, "[%s] (%s:%d) %s\n",
                report.label, report.filename, report.line, msg
        );
}

int main(){
        struct stblog_logger logger = {.handler=handler, .enable=1};
        int err = serial_open(&serial, PORT);
        if (0 > err) { // this function from xcserial.h returns -errno code
                LOG_ERRNO(&logger, "example", -err, LOG_ERROR, "%s not available", PORT);
                // ...
        }
        // ...
}
```

## Interface

Verbosity levels are defined by `enum stblog_level`: `LOG_DEBUG`, `LOG_INFO`, `LOG_WARN`, `LOG_ERROR` and `LOG_FATAL` an application that desires to define a verbosity level, can leverage this logging levels to chose what to show.
Log entries pass around an agnostic context defined by `struct stblog_report`:

```c
struct stblog_report {
        const char       *label;
        const char       *filename;
        int               line;
        int               err_code;
        enum stblog_level prio;
};
```
If you define `#define DISABLE_LOGGING` macro before including `stblog.h` either in the source code or within the compilation flags, then no matter the level of verbosity no message will appear. 

## Author

Fábio D. Pacheco
(pacheco.castro.fabio@gmail.com)

## License

`stblog.h` is released under the [MIT](https://opensource.org/license/MIT).