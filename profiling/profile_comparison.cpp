//#pragma once

#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

#include <fstream>
ostream *output;

class LogDuration {
public:
  explicit LogDuration(const string& msg = "")
    : message(msg + ": ")
    , start(steady_clock::now())
  {
  }

  ~LogDuration() {
    auto finish = steady_clock::now();
    auto dur = finish - start;
    *output //<< message
       << duration_cast<microseconds>(dur).count();
       //<< " microseconds" << endl;
  }
private:
  string message;
  steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
LogDuration UNIQ_ID(__LINE__){message};

#include <vector>

std::vector<int> xs;

void foo()
{
    for (auto& x : xs)
    {
        x++;
    }
}

void with_class()
{
    LOG_DURATION()
    foo();
}

void without_class()
{
    auto start = std::chrono::steady_clock::now();
    foo();
    auto finish = std::chrono::steady_clock::now();
    *output << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
}

#define N_ITERS 100000

int main()
{
    std::ofstream fout;
    fout.open("profile_comparison.csv", std::ios::out | std::ios::trunc);
    fout << "class,no_class" << std::endl;
    output = &fout;

    for (std::size_t i = 0; i < N_ITERS; i++)
    {
        if (i % (N_ITERS / 100) == 1)
        {
            std::cout << (i - 1) / (N_ITERS / 100) << "%\r";
        }
        xs = std::vector<int>(10000, 0);
        with_class();
        fout << ",";
        xs = std::vector<int>(10000, 0);
        without_class();
        fout << std::endl;
    }

    std::cout << "Done." << std::endl;
}