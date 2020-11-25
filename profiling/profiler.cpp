#include "../simple_tree.h"
#include "../rb_tree.h"
#include "../avltree.h"

#include "benchmarking.h"
#include "flags.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

std::vector<long long> test_fill_tree(KeyValueTree<int, int> *tree, const std::vector<int> &input, const bool test_depth)
{
    tree->clear();
    std::vector<long long> result;
    result.reserve(input.size());

    for (auto number : input)
    {
        if (test_depth)
        {
            tree->insert(number, 0);
            result.push_back(tree->depth());
        }
        else
        {
            $timeit(timer,
            tree->insert(number, 0);
            )
            result.push_back(timer);
        }
    }

    return result;
}


int main(int argc, const char * argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
    {
        std::cout << "Profiler accepts following parameters (use --{name}={value} syntax):" << std::endl
            << "\t--size - the size of input arrays, default=10000" << std::endl
            << "\t--iters - number of repetitions over which time measurements are averaged, default=50" << std::endl
            << "\t--output - output directory (this directory must exist in \".\" exist before profiler is run)" << std::endl;
        return 0;
    }

    // the size of input arrays
    unsigned int N_ITEMS = parse_flag(argc, argv, "size", 10000);
    // number of repetitions over which time measurements are averaged
    unsigned int N_ITERS = parse_flag(argc, argv, "iters", 50);
    // output directory
    std::string OUTPUT = parse_flag(argc, argv, "output", "results");

    std::vector<test_subject> subjects
    {
        {"simple", new SimpleTree<int, int>},
        {"red-black", new RBTree<int, int>},
        {"avl", new AVLTree<int, int>}
    };

    // sorted input array
    std::vector<int> sorted;
    sorted.reserve(N_ITEMS);
    for (int i = 1; i <= N_ITEMS; i++)
    {
        sorted.push_back(i);
    }

    // unsorted input array is obtained by shuffling sorted one;
    // for reproducibility default_random_engine with predefined seed (1234)
    // is used
    std::vector<int> unsorted = sorted;
    std::shuffle(unsorted.begin(), unsorted.end(),
        std::default_random_engine(1234));

    std::vector<test_case> cases
    {
        {
            "insertion",
            [unsorted](KeyValueTree<int, int>* const tree)
            -> std::vector<long long> { return test_fill_tree(tree, unsorted, false); },
            OUTPUT,
            N_ITERS
        },
        {
            "insertion_sorted",
            [sorted](KeyValueTree<int, int>* const tree)
            -> std::vector<long long> { return test_fill_tree(tree, sorted, false); },
            OUTPUT,
            N_ITERS
        },
        {
            "depth",
            [unsorted](KeyValueTree<int, int>* const tree)
            -> std::vector<long long> { return test_fill_tree(tree, unsorted, true); },
            OUTPUT
        },
        {
            "depth_sorted",
            [sorted](KeyValueTree<int, int>* const tree)
            -> std::vector<long long> { return test_fill_tree(tree, sorted, true); },
            OUTPUT
        },
    };

    std::cout << "Starting profiling. Number of test subjects: " << subjects.size()
        << ", number of test cases: " << cases.size() << "." << std::endl
        << "Using following parameters: " << std::endl
        << "\t- iters=" << N_ITERS << std::endl
        << "\t- size=" << N_ITEMS << std::endl
        << "Output will be written to ./" << OUTPUT << "/" << std::endl
        << std::endl << "Go take a coffee." << std::endl << std::endl;

    auto start_time = std::chrono::steady_clock::now();

    for (auto &tc : cases)
    {
        for (auto &sub : subjects)
        {
            tc.perform(sub);
        }
    }

    long long total_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start_time
    ).count();

    std::cout << "All test cases are performed, total time elapsed: "
        << total_time / 60 << " m " << total_time % 60 << " s." << std::endl;

    for (auto &sub : subjects)
    {
        delete sub.tree;
    }
}