// Sample 3: Keywords Stress & High Duplicate Rejection Ratio
#include <iostream>

int calculate(int a, int b) {
    if (a > b) {
        return a;
    } else if (a == b) {
        return 0;
    } else {
        return b;
    }
}

int main() {
    const int maxIterations = 10;
    for (int i = 0; i < maxIterations; ++i) {
        for (int j = 0; j < maxIterations; ++j) {
            int result = calculate(i, j);
            if (result > 5) {
                continue;
            } else {
                break;
            }
        }
    }
    return 0;
}
