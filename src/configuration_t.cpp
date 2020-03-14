#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "../include/configuration_t.h"

configuration_t read_conf(std::istream &cFile) {
    std::ios::fmtflags flags(cFile.flags()); // Save stream state
//    cFile.exceptions(std::ifstream::failbit); // Enable exception on fail
    configuration_t res;
    std::string line;
    int n;
    try {
        while (getline(cFile, line)) {
            if (line[0] == '#' || line.empty())
                continue;
            std::vector<std::string> line_lst;
            boost::split(line_lst, line, boost::is_any_of("="));
            std::string name = boost::algorithm::trim_copy(line_lst[0]);
            std::string value = boost::algorithm::trim_copy(line_lst[1]);
            if (name == "relative_error") {
                res.rel_err = std::stod(value);
            } else if (name == "absolute_error") {
                res.abs_err = std::stod(value);
            } else if(name == "threads") {
                res.threads = std::stoi(value);
            } else if (name == "x1") {
                res.x1 = std::stoi(value);
            } else if (name == "x2") {
                res.x2 = std::stoi(value);
            } else if (name == "y1") {
                res.y1 = std::stoi(value);
            } else if (name =="y2" ){
                res.y2 = std::stoi(value);
            } else if (name == "m" ){
                res.m = std::stoi(value);
            } else if(name == ("a1") ){
                std::stringstream ss(value);
                while(ss >> n) {
                    res.a1.push_back(n);
                }
            } else if(name == "a2"){
                std::stringstream ss(value);
                while(ss >> n) {
                    res.a2.push_back(n);
                }
            } else if(name == "c"){
                std::stringstream ss(value);
                while(ss >> n) {
                    res.c.push_back(n);
                }
            } else if( name == "initial_steps") {
                res.initial_steps = std::stoi(value);
            } else if(name  == "max_steps") {
                res.max_steps = std::stoi(value);
            }
        }
    } catch (std::ios_base::failure &fail) // Life without RAII is hard...
    {
        cFile.flags(flags);
        throw; // re-throw exception
    }
    cFile.flags(flags);
    if (res.x2 <= res.x1) {
        throw std::runtime_error("x1 should be <= x2");
    }
    if (res.y2 <= res.y1) {
        throw std::runtime_error("y1 should be <= y2");
    }
    if ((res.a1.size() != res.m) || (res.a2.size() != res.m) || (res.c.size() != res.m)) {
        std::cout << res.a1.size() << ' ' << res.a2.size()<< ' ' <<res.c.size()  << std::endl;
        throw std::runtime_error("Arrays should be the same length as a m");
    }
    if (res.initial_steps < 10) {
        throw std::runtime_error("Too few initial_steps");
    }
    if (res.max_steps < 10) {
        std::cout<<res.max_steps<<std::endl;
        throw std::runtime_error("Too small max_steps");
    }
    if (res.abs_err <= 0 || res.rel_err <= 0) {
        throw std::runtime_error("Errors should be positive");
    }
    return res;
}