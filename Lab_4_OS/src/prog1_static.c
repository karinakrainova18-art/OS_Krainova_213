#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include "../include/contract.h"

int gcd(int a, int b);
char* convert(int x);

#define STDOUT_FD 1
#define STDIN_FD 0

static void print_str(const char* s) {
    write(STDOUT_FD, s, strlen(s));
}

static void print_format(const char* format, int num) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), format, num);
    if (len > 0) {
        write(STDOUT_FD, buf, (size_t)len);
    }
}

static int parse_int(const char *str, int *val) {
    if (!str || !*str) return 0;
    long result = 0;
    int sign = 1;
    const char *p = str;
    
    while (*p && isspace(*p)) p++;
    
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    if (!isdigit(*p)) return 0;
    
    while (isdigit(*p)) {
        result = result * 10 + (*p - '0');
        p++;
    }
    
    *val = (int)(result * sign);
    return 1;
}

static void handle_function_1(const char* arg_str) {
    int a, b;
    const char *p = arg_str;
    
    // Пропускаем пробелы
    while (*p && isspace(*p)) p++;
    
    // Парсим первое число
    const char *start = p;
    if (*p == '-' || *p == '+') p++;
    while (*p && isdigit(*p)) p++;
    
    if (p == start) {
        print_str("Ошибка ввода. Для функции 1 требуется два натуральных числа\n");
        return;
    }
    
    char num_str[30];
    size_t len = p - start;
    if (len >= sizeof(num_str)) len = sizeof(num_str) - 1;
    memcpy(num_str, start, len);
    num_str[len] = '\0';
    
    if (!parse_int(num_str, &a)) {
        print_str("Ошибка ввода. Для функции 1 требуется два натуральных числа\n");
        return;
    }
    
    // Пропускаем пробелы между числами
    while (*p && isspace(*p)) p++;
    
    // Парсим второе число
    start = p;
    if (*p == '-' || *p == '+') p++;
    while (*p && isdigit(*p)) p++;
    
    if (p == start) {
        print_str("Ошибка ввода. Для функции 1 требуется два натуральных числа\n");
        return;
    }
    
    len = p - start;
    if (len >= sizeof(num_str)) len = sizeof(num_str) - 1;
    memcpy(num_str, start, len);
    num_str[len] = '\0';
    
    if (!parse_int(num_str, &b)) {
        print_str("Ошибка ввода. Для функции 1 требуется два натуральных числа\n");
        return;
    }
    
    if (a <= 0 || b <= 0) {
        print_str("Ошибка: числа должны быть натуральными (больше 0)\n");
        return;
    }
    
    int result = gcd(a, b);
    print_str("Результат (Евклид): ");
    print_format("%d", result);
    print_str("\n");
}

static void handle_function_2(const char* arg_str) {
    int x;
    if (!parse_int(arg_str, &x)) {
        print_str("Ошибка ввода. Для функции 2 требуется целое число\n");
        return;
    }

    char* res = convert(x);
    if (res) {
        print_str("Результат (двоичный): ");
        print_str(res);
        print_str("\n");
        free(res);
    } else {
        print_str("Ошибка выделения памяти\n");
    }
}

int main() {
    char line[512];
    ssize_t bytes_read;
    
    print_str("Программа 1: Статическая линковка\n");
    print_str("Функция 1: НОД (алгоритм Евклида)\n");
    print_str("Функция 2: Перевод в двоичную систему счисления\n");
    print_str("Введите команду: (1 [число1] [число2] или 2 [число] или 'q' для выхода)\n");
    
    while ((bytes_read = read(STDIN_FD, line, sizeof(line) - 1)) > 0) {
        line[bytes_read] = '\0';
        
        if (line[bytes_read - 1] == '\n') {
            line[bytes_read - 1] = '\0';
        }
        
        if (line[0] == 'q') {
            break;
        } 
        
        if (line[0] == '\0') continue;
        
        int cmd = line[0] - '0';
        char* args = line + 1;
        while (*args == ' ' || *args == '\t') {
            args++;
        }
        
        switch(cmd) {
            case 1:
                handle_function_1(args);
                break;
            case 2:
                handle_function_2(args);
                break;
            default:
                print_str("Недоступная команда, введите 1 или 2\n");
        }
        
        print_str("Введите команду: (1 [число1] [число2] или 2 [число] или 'q' для выхода)\n");
    }
    
    return 0;
}

