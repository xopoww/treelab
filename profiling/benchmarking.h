#pragma once


#ifdef _WIN32
#define FILESEP "\\"
#else
#define FILESEP "/"
#endif


#include "../target_interface.h"
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#ifdef _WIN32

    #include <profileapi.h>

    // $timeit is a macro for timing certain operations / groups of operations
    // first argument is the name for a long long variable that will be
    // populated with timer value (in nanosecs); second argument is a block
    // of code to be timed; 
    // this macro declares local variables name __start, __end and __freq, so
    // the use of those names in the block of code being timed should be
    // avoided
    #define $timeit(timer_name, stuff)                          \
    long long timer_name;                                       \
    {                                                           \
        LARGE_INTEGER __start, __end, __freq;                   \
        if (!QueryPerformanceCounter(&__start))                 \
        { throw "Fatal error: unable to get system time."; }    \
        stuff                                                   \
        if (!QueryPerformanceCounter(&__end))                   \
        { throw "Fatal error: unable to get system time."; }    \
        timer_name = __end.QuadPart - __start.QuadPart;         \
        timer_name *= 1000000;                                  \
        if (!QueryPerformanceFrequency(&__freq))                \
        { throw "Fatal error: unable to get system time."; }    \
        timer_name /= __freq.QuadPart;                          \
    }
    
#else

    // TODO: add system api clock for *nix
    #define $timeit(timer_name, stuff) throw "Not implemented!";

#endif


// Conveniece struct holding a pointer to KeyValueTree instance and a name
// of the subject
struct test_subject
{
    KeyValueTree<int, int> * tree;
    std::string name;

    test_subject(std::string _name, KeyValueTree<int, int> * _tree):
        tree(_tree), name(_name)
    {}
    
    test_subject(const char * _name, KeyValueTree<int, int> * _tree):
        tree(_tree), name(_name)
    {}
};

// Convenience struct representing a test case for profiling
// Supports multiple repetitions and averaging for time measuring TCs
struct test_case
{
    using func_t = std::function<std::vector<long long>(KeyValueTree<int, int>* const)>;
    func_t func;
    std::string name;
    std::size_t n_iters;

    std::string output_dir;

    // If _n_iters is 0 or 1, during the performance of TC _func will be
    // run only once; otherwise, it will be run _n_iters times, and the final
    // result will be the element-wise mean of the results of those runs
    test_case(std::string _name, func_t _func, const std::string &_output_dir,
    const std::size_t _n_iters = 0):
        func(_func),
        name(_name),
        n_iters(_n_iters == 0 ? 1 : _n_iters),
        output_dir(_output_dir)
    {}

    // Perform TC on sub and output the results to a file named
    // "{sub.name}_{this->name}.csv" placed in RESULT_FOLDER directory
    void perform(test_subject &sub)
    {
        std::ofstream fout;
        std::stringstream filename;
        filename
            << "." << FILESEP
            << output_dir << FILESEP
            << sub.name << "_" << this->name << ".csv";
        fout.open(filename.str());
        fout << "index,result" << std::endl;

        std::cout << "Permorming test case \"" << this->name << "\""
        << " on test subject \"" << sub.name << "\"";
        if (this->n_iters > 1)
        {
            std::cout << " (results averaged over " << this->n_iters
            << " repetitions)" << std::endl;
            std::cout << "0%\r";
        }
        else
        {
            std::cout << std::endl;
        }

        auto result = this->func(sub.tree);
        for (std::size_t iter = 1; iter < n_iters; iter++)
        {
            std::cout << iter * 100 / n_iters << "%" << "\r";

            auto res = this->func(sub.tree);
            for (std::size_t i = 0; i < res.size(); i++)
            {
                result[i] += res[i];
            }
        }

        for (auto& r : result)
        {
            r /= n_iters;
        }

        for (std::size_t i = 0; i < result.size(); i++)
        {
            fout << i + 1 << "," << result[i] << std::endl;
        }

        fout.close();

        std::cout << "Done. " << result.size() << " records written to "
        << filename.str() << std::endl << std::endl;
    }

};