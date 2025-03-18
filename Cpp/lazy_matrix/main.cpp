#include "lazy_matrix.h"

int main()
{
    ias::lazy_matrix lm1 = ias::lazy_matrix(3, 3);
    lm1(0, 0) = 1.0;

    ias::lazy_matrix lm2 = ias::lazy_matrix(3, 3);
    lm2(0, 0) = 1.0;

    auto a = lm1 + (lm2 - lm1) - lm2;

    lazy_matrix res = a.eval();
    

    return 0;
}