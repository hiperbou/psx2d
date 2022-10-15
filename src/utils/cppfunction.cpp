#include "cppfunction.h"

class Calculator {
public:
    int sum(int a, int b){
        return a + b;
    }
};

Calculator calculator;

int someCppFunction(int a, int b) {
    return calculator.sum(a, b);
}