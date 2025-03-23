#include "lazy.h"
#include <iomanip>

int main()
{
    constexpr size_t N = 1000;
    constexpr size_t ARR_SIZE = 1000; 

    vector<int> v1(N);
    containers::fill(v1, 1);
    vector<int> v2(N);
    containers::fill(v2, 2);

    auto v_dur1 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { volatile auto v3 = (v1 + v2 * v1) - v2; })
                      .count();

    lazy_vector<int> lv1(N);
    containers::fill(lv1, 1);
    lazy_vector<int> lv2(N);
    containers::fill(lv2, 2);

    auto v_dur2 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { 
                                                            auto lv3 = (lv1 + lv2 * lv1) - lv2; 
                                                            volatile auto res = lv3.eval(); })
                      .count();

    array<int, ARR_SIZE> a1;
    containers::fill(a1, 1);
    array<int, ARR_SIZE> a2;
    containers::fill(a2, 2);

    auto a_dur1 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { volatile auto a3 = (a1 + a2 * a1) - a2; })
                      .count();

    lazy_array<int, ARR_SIZE> la1;
    containers::fill(la1, 1);
    lazy_array<int, ARR_SIZE> la2;
    containers::fill(la2, 2);

    auto a_dur2 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { 
                                                            auto la3 = (la1 + la2 * la1) - la2; 
                                                            volatile auto res = la3.eval(); })
                      .count();

    matrix<int> m1(N, N);
    containers::fill(m1, 1);
    matrix<int> m2(N, N);
    containers::fill(m2, 2);

    auto m_dur1 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { volatile auto m3 = (m1 + m2 * m1) - m2; })
                      .count();

    lazy_matrix<int> lm1(N, N);
    containers::fill(lm1, 1);
    lazy_matrix<int> lm2(N, N);
    containers::fill(lm2, 2);

    auto m_dur2 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                           { 
                                                            auto lm3 = (lm1 + lm2 * lm1) - lm2; 
                                                            volatile auto res = lm3.eval(); })
                      .count();

    array2d<int, ARR_SIZE, ARR_SIZE> a2d1;
    containers::fill(a2d1, 1);
    array2d<int, ARR_SIZE, ARR_SIZE> a2d2;
    containers::fill(a2d2, 2);

    auto a2d_dur1 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                             { volatile auto a2d3 = (a2d1 + a2d2 * a2d1) - a2d2; })
                        .count();

    lazy_array2d<int, ARR_SIZE, ARR_SIZE> la2d1;
    containers::fill(la2d1, 1);
    lazy_array2d<int, ARR_SIZE, ARR_SIZE> la2d2;
    containers::fill(la2d2, 2);

    auto a2d_dur2 = utils::time_it<std::chrono::nanoseconds>(15, []() {}, [&]()
                                                             { 
                                                              auto la2d3 = (la2d1 + la2d2 * la2d1) - la2d2; 
                                                              volatile auto res = la2d3.eval(); })
                        .count();

    std::cout << std::left << std::setw(20) << "Container Type"
              << std::setw(20) << "Non-Lazy (ns)"
              << std::setw(20) << "Lazy (ns)"
              << std::setw(20) << "Size" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    std::cout << std::setw(20) << "Vector"
              << std::setw(20) << v_dur1
              << std::setw(20) << v_dur2
              << std::setw(20) << N << std::endl;

    std::cout << std::setw(20) << "Array"
              << std::setw(20) << a_dur1
              << std::setw(20) << a_dur2
              << std::setw(20) << ARR_SIZE << std::endl;

    std::cout << std::setw(20) << "Matrix"
              << std::setw(20) << m_dur1
              << std::setw(20) << m_dur2
              << std::setw(20) << (N * N) << std::endl;

    std::cout << std::setw(20) << "Array2d"
              << std::setw(20) << a2d_dur1
              << std::setw(20) << a2d_dur2
              << std::setw(20) << (ARR_SIZE * ARR_SIZE) << std::endl;

    // lazy_array2d<int, ARR_SIZE, ARR_SIZE> a1d2;
    // containers::fill(a1d2, 1);

    // lazy_array2d<int, ARR_SIZE, ARR_SIZE> a2d2;
    // containers::fill(a2d2, 2);

    // auto dur = utils::time_it(15, [](){}, [&]()
    // {
    //     auto a3d2 = (a1d2 + a2d2 * a1d2) - a2d2;
    //     volatile auto res = a3d2.eval();
    // }).count();
    


    return 0;
}