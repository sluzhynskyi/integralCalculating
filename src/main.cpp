#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>
#include <atomic>
#include <exception>
#include <string>
#include <cmath>
#include <mutex>
#include "../include/configuration_t.h"
#include "../include/func_to_integrate.h"

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    static_assert(std::chrono::steady_clock::is_steady, "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}


// For printing or writing to file
template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

template<typename func_T>
void integrate(func_T func, double x1, double x2, double y1, double y2, const configuration_t &conf, size_t steps,
          double &data) {
    const double x1_o = x1;
    double res = 0;
    double delta_x = (x2 - x1) / steps;
    double delta_y = (y2 - y1) / steps;
    while (y1 < y2) {
        x1 = x1_o;
        while (x1 < x2) {
            res += func(x1, y1, conf);
            x1 += delta_x;
        }
        y1 += delta_y;
    }
    data = res * delta_x * delta_y;
}

int main(int argc, char *argv[]) {
    std::string filename("conf.txt");
    if (argc == 2)
        filename = argv[1];
    if (argc > 2) {
        std::cerr << "Too many arguments. Usage: \n"
                     "<program>\n"
                     "or\n"
                     "<program> <config-filename>\n" << std::endl;
        return 1;
    }
    std::ifstream config_stream(filename);
    if (!config_stream.is_open()) {
        std::cerr << "Failed to open configuration file " << filename << std::endl;
        return 2;
    }
    configuration_t config;
    try {
        config = read_conf(config_stream);
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 3;
    }
    std::mutex m;
    double cur_res = 0;
    auto before = get_current_time_fenced();

    size_t steps = config.initial_steps;
//    cur_res = integrate(func_to_integrate, config, steps);
    if (config.threads == 1) {
        integrate(func_to_integrate, config.x1, config.x2, config.y1, config.y2, config, steps, cur_res);
    } else {
        std::vector<std::thread> threads{};
        std::vector<double> res_local(config.threads);
        double l, y1, y2;
        size_t st;
        st = (size_t) (steps / config.threads);
        for (int i = 0; i < config.threads; ++i) {
            l = (double) (config.y2 - config.y1) / config.threads;
            y1 = config.y1 + (i * l);
            y2 = config.y1 + (i + 1) * l;
            threads.emplace_back(integrate<double (*)(const double &, const double &, const configuration_t &)>,
                                 func_to_integrate, config.x1, config.x2, y1, y2, config,
                                 (size_t) (steps / config.threads), std::ref(res_local[i]));
        }
        for (auto &t: threads) {
            t.join();
        }

        for (const auto &x: res_local) {
            cur_res += x;
        }
    }
    double prev_res = cur_res;
    bool to_continue = true;
    double abs_err = -1; // Just guard value
    double rel_err = -1; // Just guard value
// #define PRINT_INTERMEDIATE_STEPS
    while (to_continue) {
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << cur_res << " : " << steps << " steps" << endl;
#endif
        prev_res = cur_res;
        steps *= 2;
        if (config.threads == 1) {
            integrate(func_to_integrate, config.x1, config.x2, config.y1, config.y2, config, steps, cur_res);
        } else {
            std::vector<std::thread> threads{};
            std::vector<double> res_local(config.threads);

            double l, y1, y2;
            size_t st;
            st = (size_t) (steps / config.threads);
            for (int i = 0; i < config.threads; ++i) {
                l = (double) (config.y2 - config.y1) / config.threads;
                y1 = config.y1 + (i * l);
                y2 = config.y1 + (i + 1) * l;
                threads.emplace_back(integrate<double (*)(const double &, const double &, const configuration_t &)>,
                                     func_to_integrate, config.x1, config.x2, y1, y2,
                                     config, st , std::ref(res_local[i]));
            }
            for (auto &t: threads) {
                t.join();
            }
            cur_res = 0;
            for (const auto &x: res_local) {
                cur_res += x;
            }
        }
        abs_err = std::fabs(cur_res - prev_res);
        rel_err = std::fabs((cur_res - prev_res) / cur_res);
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << '\t' << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
#endif

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue && (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;

    std::cout << "Result: " << cur_res << std::endl;
    std::cout << "Abs err : Rel err" << " -- " << abs_err << " : " << rel_err << std::endl;
    std::cout << "Time: " << to_us(time_to_calculate) << std::endl;

    return 0;

}