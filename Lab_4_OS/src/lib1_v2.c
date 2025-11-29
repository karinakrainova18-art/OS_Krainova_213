#include "../include/contract.h"

// Реализация №2: Наивный алгоритм
int gcd(int a, int b) {
    if (a <= 0 || b <= 0) {
        return 0;
    }
    
    int min_val = (a < b) ? a : b;
    int result = 1;
    
    for (int i = 2; i <= min_val; i++) {
        if (a % i == 0 && b % i == 0) {
            result = i;
        }
    }
    
    return result;
}

