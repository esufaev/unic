#include "lazy_matrix.h"

int main()
{
    lazy_matrix lm1 = lazy_matrix(3, 3);
    lm1(0, 0) = 1.0;

    lazy_matrix lm2 = lazy_matrix(3, 3);
    lm2(0, 0) = 1.0;
    lm2(2, 2) = 1.0;

    lazy_matrix lm3 = lazy_matrix(3, 3);
    lm3(0, 0) = 4.0;
    lm3(2, 2) = 1.0;

    lazy_matrix lm4 = lazy_matrix(3, 3);
    lm4(0, 0) = 5.0;
    lm4(2, 2) = 1.0;

    auto a = lm4 * ((lm1 + lm2) - lm3);
    lazy_matrix res = a.eval();
    
    res.print();

    return 0;
}