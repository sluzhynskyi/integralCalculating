#include <iostream>
#include <cmath>
#include "../include/func_to_integrate.h"
#include "../include/configuration_t.h"
//
// Created by sluzh on 3/1/2020.
//
using std::exp;
using std::pow;
using std::cos;
using std::cout;


double func_to_integrate(const double &x, const double &y, const configuration_t &conf) {
    double res = 0;
    double temp;
    double temp_eq;
    for (int i = 0; i < conf.m; ++i) {
        temp_eq = pow((x - conf.a1[i]), 2) + pow((y - conf.a2[i]), 2);
        temp = conf.c[i] * exp((-1 / M_PI) * temp_eq) * cos(M_PI * temp_eq);
        res = res + temp;
    }
    return -res;
};




