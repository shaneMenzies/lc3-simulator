/**
 * @file native_test.cpp
 * @author Shane Menzies
 * @brief Native test for performance comparison
 * @date 05/11/22
 *
 *
 */

#include <iostream>

// 28 terms
const int num_terms = 0x1c;

int fibonacci(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return (fibonacci(n - 1) + fibonacci(n - 2));
    }
}

int main() {

    int results[num_terms];

    std::cout << "Starting native test...\n";

    for (int i = 0; i <= num_terms; i++) {
        results[i] = fibonacci(i);
    }

    std::cout << "Native Test Complete.\n";

    return 0;
}