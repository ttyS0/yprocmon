/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file debug.h
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief debug 调试辅助函数
 */

#include <cstdio>

#define console_print(fmt, ...) \
            do { fprintf(stdout, fmt, ##__VA_ARGS__); } while (0)

#define console_error(fmt, ...) \
            do { fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define debug_print(fmt, ...) \
            do { if (DEBUG_TEST) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define debug_log(fmt, ...) \
        do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
