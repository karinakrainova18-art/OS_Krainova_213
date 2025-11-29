#include "../include/contract.h"

// Реализация №1: Алгоритм Евклида
int gcd(int a, int b) {
    if (a <= 0 || b <= 0) {
        return 0;
    }
    
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    
    return a;
}

