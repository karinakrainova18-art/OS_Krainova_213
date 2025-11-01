#ifndef LAB2_H
#define LAB2_H

#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    int *arr;
    int left;
    int right;
    int dir;
    int available_threads;
} SortParams;

int int_to_str(int value, char* buffer, int buffer_size);
int double_to_str(double value, char* buffer, int buffer_size, int decimals);
void print_string(const char* str);
void print_int(int value);
void print_double(double value, int decimals);
void print_array(int *arr, int n, const char* label);

void compare_and_swap(int *a, int *b, int dir);
void bitonic_merge(int *arr, int left, int count, int dir);

void sequential_bitonic_sort(int *arr, int left, int count, int dir);
void create_thread(pthread_t *thread, SortParams *params);
void* parallel_bitonic_sort(void *params);

int *generate_array(int size);
int is_sorted(int *arr, int n, int dir);
double get_time();
void run_experiments(int size, int thread_counts[], int num_experiments);

#endif