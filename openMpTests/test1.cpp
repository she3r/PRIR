
#include <iostream>
#include <ostream>
#include <vector>

class Evaluator {
public:
    Evaluator();
    ~Evaluator();
    double evaluate(double x) {
        double result = 0;
        for (int i = 1; i <= 10; i++) {
            result += x * i;
        }
        return result;
    }

    double* evaluate_array(int len) {
        double* result = new double[len];
        for(int i = 0; i < len; i++) {
            result[i] = i;
        }
#pragma omp parallel for collapse(2)
        for (int j = 0; j < len; j++) {
            for (int i = 1; i <= 10; i++) {
                double res = 0;
                res += result[j] * i;
            }
            result[j] = 0;
        }
        return result;
    }
};

int main() {
    std::vector<int> testVector2(10,2);
    std::vector<int> testVector1(10,1);
    testVector2.data() = testVector1.data();
}