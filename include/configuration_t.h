//
// Created by sluzh on 3/6/2020.
//

#ifndef INTEGRALCALCULATING_CONFIGURATION_T_H
#define INTEGRALCALCULATING_CONFIGURATION_T_H
#include <vector>
struct configuration_t {
    double rel_err, abs_err;
    double x1, x2, y1, y2;
    int m, threads;

    std::vector<int> a1;
    std::vector<int> a2;
    std::vector<int> c;

    size_t initial_steps;
    size_t max_steps;
};
configuration_t read_conf(std::istream &cFile);
#endif //INTEGRALCALCULATING_CONFIGURATION_T_H
