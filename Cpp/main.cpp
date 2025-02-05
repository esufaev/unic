#include <iostream>

int main() {
    double a = 1.0e-10;
    double b = 1.0;
    
    if ((a + b) == (b + a))
        std::cout << "Равны" << std::endl;
    else
        std::cout << "Не равны" << std::endl;

    return 0;
}
