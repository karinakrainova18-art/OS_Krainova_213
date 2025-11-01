#include "lab2.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_string("Usage: "); print_string(argv[0]); print_string(" <array_size>\n");
        return 1;
    }
    int size = 0;
    char *arg = argv[1];
    while (*arg) { size = size * 10 + (*arg - '0'); arg++; }
    if (size <= 0 || (size & (size - 1)) != 0) {
        print_string("Error: array size must be power of two\n");
        return 1;
    }
    
    print_string("Bitonic Sort - Array size: "); print_int(size); print_string("\n");
    
    int thread_counts[] = {1, 4, 8, 10, 12, 16, 64, 128};
    int num_experiments = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    run_experiments(size, thread_counts, num_experiments);
    
    return 0;
}