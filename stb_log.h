/**
 * @file      stb_log.h
 * 
 * @version   0.2.0
 *
 * @date      16-03-2026
 * @date      06-09-2026
 * @date      18-10-2026
 *  
 * @author    Fábio D. Pacheco, 
 * @email     fabio.d.pacheco@inesctec.pt or pacheco.castro.fabio@gmail.com
 *
 * @note
 * stb_log - simple logging system 
 * Copyright (C) 2026 Fábio D. Pacheco 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the “Software”), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */

#ifndef STB_LOG_H
#define STB_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifndef STBIDEF
        #define STBIDEF
#endif

enum stblog_level {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL
};

struct stblog_report {
        const char        *label;     
        const char        *filename;
        int                line;
        int                err_code;
        enum stblog_level  prio;    
};

struct stblog_logger {
        void (*handler)(struct stblog_report, const char *);
        int enable;
};

STBIDEF void
stblog_default(
        struct stblog_report  report,
        const char           *msg
);

STBIDEF void 
stblog_emit(
        struct stblog_logger *lg,
        struct stblog_report  report,
        const char           *fmt,
        ...
);

STBIDEF void
stblog_hexdump(
        struct stblog_logger *lg,
        struct stblog_report  report,
        const void           *buf,
        size_t                nelem 
);

#ifndef DISABLE_LOGGING
        #define LOG_ERRNO(logger, label, err, prio, fmt, ...)                          \
                stblog_emit(                                                           \
                        (logger),                                                      \
                        (struct stblog_report){label, __FILE__, __LINE__, err, prio},  \
                        fmt, ##__VA_ARGS__                                             \
                )
        #define LOG_HEXDUMP(logger, label, prio, buf, bytes)                           \
                stblog_hexdump(                                                        \
                        (logger),                                                      \
                        (struct stblog_report){label, __FILE__, __LINE__, 0, prio},    \
                        (buf), (bytes)                                                 \
                )
#else
        #define LOG_ERRNO(logger, label, err, prio, fmt, ...) \
                ((void)0)
        #define LOG_HEXDUMP(logger, label, prio, buf, bytes)  \
                ((void)0)
#endif // DISABLE_LOGGING

#endif // STB_LOG_H

#ifdef STB_LOG_IMPLEMENTATION

/** 
@brief Console print
 */
STBIDEF void
stblog_default(
        struct stblog_report  report,
        const char           *msg
){
        const char *log_level[] = {"DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};
        const char *log_color[] = {
            "\x1b[90m", /* DEBUG  - gray */
            "\x1b[32m", /* INFO   - green */
            "\x1b[33m", /* WARN   - yellow */
            "\x1b[31m", /* ERROR  - red */
            "\x1b[35;1m"/* FATAL  - bold magenta */
        };
        const char *reset_color = "\x1b[0m";

        if ( report.prio < 0 || report.prio > LOG_FATAL ) {
                report.prio = LOG_DEBUG;
        }
        const char *errmsg = !report.err_code ? "" : strerror(report.err_code);
        fprintf(
                stderr,
                "%s[%s][%s] (%s:%d) %s:%s %s\n",
                log_color[report.prio],
                report.label,
                log_level[report.prio],
                report.filename,
                report.line,
                errmsg,
                msg,
                reset_color
        );
}

/** 
@brief Call either external logging system or internal logging 
 */
STBIDEF void
stblog_emit(
        struct stblog_logger *lg,
        struct stblog_report  report,
        const char           *fmt,
        ...
){
        char buf[256];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);

        if (lg && lg->enable) {
                lg->handler(report, buf);
        }
        else {
                stblog_default(report, buf);
        }
}

/** 
@brief External call to hexdump to dump information in pretty hexadecimal 
 */
STBIDEF void
stblog_hexdump(
        struct stblog_logger *lg,
        struct stblog_report  report,
        const void           *buf,
        size_t                bytes 
) {
        FILE *fp = popen("hexdump -C", "w");
        if (!fp) {
                LOG_ERRNO(lg, report.label, -errno, report.prio, "popen|hexdump");
                return;
        }
        printf("[%s]\n", report.label);
        (void) fwrite(buf, 1, bytes, fp);
        (void) pclose(fp);        
}

#endif // STB_LOG_IMPLEMENTATION
