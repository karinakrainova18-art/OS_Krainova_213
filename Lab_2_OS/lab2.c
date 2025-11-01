#include "lab2.h"

int int_to_str(int value, char* buffer, int buffer_size) {
    if (value == 0) {
        if (buffer_size >= 2) { buffer[0] = '0'; buffer[1] = '\0'; return 1; }
        return 0;
    }
    int is_negative = value < 0;
    if (is_negative) value = -value;
    int length = 0, temp = value;
    while (temp > 0) { temp /= 10; length++; }
    if (is_negative) length++;
    if (length >= buffer_size) return 0;
    int index = length;
    buffer[index] = '\0'; index--;
    do { buffer[index] = '0' + (value % 10); value /= 10; index--; } while (value > 0);
    if (is_negative) buffer[0] = '-';
    return length;
}

int double_to_str(double value, char* buffer, int buffer_size, int decimals) {
    if (buffer_size < 1) return 0;
    int int_part = (int)value;
    int length = int_to_str(int_part, buffer, buffer_size);
    if (length == 0 || decimals <= 0) return length;
    if (length + 1 + decimals >= buffer_size) return length;
    buffer[length] = '.'; length++;
    double fractional = value - int_part;
    if (fractional < 0) fractional = -fractional;
    for (int i = 0; i < decimals; i++) {
        fractional *= 10;
        int digit = (int)fractional;
        buffer[length + i] = '0' + digit;
        fractional -= digit;
    }
    buffer[length + decimals] = '\0';
    return length + decimals;
}

void print_string(const char* str) {
    int length = 0;
    while (str[length] != '\0') length++;
    write(STDOUT_FILENO, str, length);
}

void print_int(int value) {
    char buffer[32];
    int length = int_to_str(value, buffer, sizeof(buffer));
    if (length > 0) write(STDOUT_FILENO, buffer, length);
}

void print_double(double value, int decimals) {
    char buffer[32];
    int length = double_to_str(value, buffer, sizeof(buffer), decimals);
    if (length > 0) write(STDOUT_FILENO, buffer, length);
}

void print_array(int *arr, int n, const char* label) {
    print_string(label); print_string(": [");
    if (n <= 20) {
        for (int i = 0; i < n; i++) { print_int(arr[i]); if (i < n - 1) print_string(", "); }
    } else {
        for (int i = 0; i < 5; i++) { print_int(arr[i]); print_string(", "); }
        print_string("..., ");
        for (int i = n - 5; i < n; i++) { print_int(arr[i]); if (i < n - 1) print_string(", "); }
    }
    print_string("]\n");
}

void compare_and_swap(int *a, int *b, int dir) {
    if ((dir && *a > *b) || (!dir && *a < *b)) {
        int temp = *a; *a = *b; *b = temp;
    }
}

void bitonic_merge(int *arr, int left, int count, int dir) {
    if (count <= 1) return;
    int k = count / 2;
    for (int i = left; i < left + k; i++) compare_and_swap(&arr[i], &arr[i + k], dir);
    bitonic_merge(arr, left, k, dir);
    bitonic_merge(arr, left + k, k, dir);
}

void sequential_bitonic_sort(int *arr, int left, int count, int dir) {
    if (count <= 1) return;
    int k = count / 2;
    sequential_bitonic_sort(arr, left, k, 1);
    sequential_bitonic_sort(arr, left + k, k, 0);
    bitonic_merge(arr, left, count, dir);
}

void create_thread(pthread_t *thread, SortParams *params) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);
    pthread_create(thread, &attr, parallel_bitonic_sort, params);
    pthread_attr_destroy(&attr);
}

void* parallel_bitonic_sort(void *params) {
    SortParams *p = (SortParams*)params;
    int count = p->right - p->left + 1;
    if (count <= 1) return NULL;
    int k = count / 2;
    if (p->available_threads <= 1) {
        sequential_bitonic_sort(p->arr, p->left, count, p->dir);
        return NULL;
    }
    pthread_t thread;
    SortParams left_params = {.arr=p->arr, .left=p->left, .right=p->left+k-1, .dir=1, .available_threads=p->available_threads/2};
    SortParams right_params = {.arr=p->arr, .left=p->left+k, .right=p->right, .dir=0, .available_threads=p->available_threads-left_params.available_threads};
    create_thread(&thread, &left_params);
    parallel_bitonic_sort(&right_params);
    pthread_join(thread, NULL);
    bitonic_merge(p->arr, p->left, count, p->dir);
    return NULL;
}

int *generate_array(int size) {
    int *arr = malloc(size * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < size; i++) arr[i] = (rand() % 10000) - 5000;
    return arr;
}

int is_sorted(int *arr, int n, int dir) {
    for (int i = 0; i < n - 1; i++) {
        if ((dir && arr[i] > arr[i + 1]) || (!dir && arr[i] < arr[i + 1])) return 0;
    }
    return 1;
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void run_experiments(int size, int thread_counts[], int num_experiments) {
    int *original_arr = generate_array(size);
    print_array(original_arr, size, "Original array");
    int *arr_seq = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) arr_seq[i] = original_arr[i];
    double start_time = get_time();
    sequential_bitonic_sort(arr_seq, 0, size, 1);
    double seq_time = (get_time() - start_time) * 1000;
    print_string("\nSequential: "); print_double(seq_time, 2); print_string(" ms, Sorted: "); 
    print_string(is_sorted(arr_seq, size, 1) ? "yes" : "no"); print_string("\n");
    print_array(arr_seq, size, "Sequential result");
    
    print_string("\nParallel experiments:\n");
    print_string("Threads | Time (ms) | Speedup | Efficiency\n");
    print_string("------------------------------------------\n");
    
    for (int i = 0; i < num_experiments; i++) {
        int threads = thread_counts[i];
        int *arr_par = malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) arr_par[j] = original_arr[j];
        start_time = get_time();
        SortParams params = {.arr=arr_par, .left=0, .right=size-1, .dir=1, .available_threads=threads};
        parallel_bitonic_sort(&params);
        double par_time = (get_time() - start_time) * 1000;
        double speedup = seq_time / par_time;
        double efficiency = (speedup / threads) * 100;
        
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%7d | %9.2f | %7.2f | %9.2f%%\n", threads, par_time, speedup, efficiency);
        print_string(buffer);
        
        if (i == 0) {
            print_array(arr_par, size, "Parallel result (first)");
            print_string("Check: "); print_string(is_sorted(arr_par, size, 1) ? "passed" : "failed"); print_string("\n");
        }
        free(arr_par);
    }
    free(original_arr);
    free(arr_seq);
}