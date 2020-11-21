// A simple test of kVTree implementation (without benchmarking).

#define _DEBUG 0 // this macro controls output to the console (see "common.h")

// Edit these 2 lines to change the type of the tree being tested

#include "simple_tree.h"    // <<<<<
#define TreeT SimpleTree    // <<<<<


#include <iostream>
#include <string>

const char * crepr(const int num)
{
    switch (num)
    {
        case 0:
        return "zero";
        case 1:
        return "one";
        case 2:
        return "two";
        case 3:
        return "three";
        case 4:
        return "four";
        case 5:
        return "five";
        case 6:
        return "six";
        case 7:
        return "seven";
        case 8:
        return "eight";
        case 9:
        return "nine";
        default:
        return "unsupported";
    }
}

std::string repr(const int num)
{
    return std::string(crepr(num));
}

std::vector<int> nums{5, 1, 4, 7, 9, 6, 2};

template <typename kT, typename vT>
void print(TreeT<kT, vT> &tree)
{
#if defined _DEBUG && _DEBUG > 0
    tree.print();
#else
    for (auto n : nums)
    {
        std::cout << n << " : " << tree[n] << std::endl;
    }
#endif
}

int main()
{
    TreeT<int, std::string> tree;

    for (auto n : nums)
    {
        std::string nr = repr(n);
        tree.insert(n, nr);
    }

    print(tree);

    std::cout << std::endl;

    std::cout << "4: " << (tree.contains(4) ? "true" : "false") << std::endl;
    std::cout << "3: " << (tree.contains(3) ? "true" : "false") << std::endl;

    std::cout << std::endl;

    tree[1] = std::string("ONE");

    print(tree);

    std::cout << std::endl;

    tree[3] = std::string("tree");
    std::cout << "3: " << (tree.contains(3) ? "true" : "false") << std::endl;

    std::cout << std::endl;

    tree.erase(1);
    std::cout << "1: " << (tree.contains(1) ? "true" : "false") << std::endl;

    std::cout << std::endl;

    std::cout << "1: " << tree[1] << std::endl;
    std::cout << "1: " << (tree.contains(1) ? "true" : "false") << std::endl;

    std::cout << std::endl;

    std::string buf;
    tree.find(5, buf);
    std::cout << buf << std::endl;

    std::cout << std::endl;
    
    print(tree);

    std::cout << std::endl;

    TreeT<int, std::string> other(tree);
    TreeT<int, std::string> another(std::move(tree));
    std::cout << 
        "tree: " << tree.size() << std::endl <<
        "other: " << other.size() << std::endl <<
        "another: " << another.size() << std::endl;

}

// Expected output (with _DEBUG == 0):

/*
5 : five
1 : one
4 : four
7 : seven
9 : nine
6 : six
2 : two

4: true
3: false

5 : five
1 : ONE
4 : four
7 : seven
9 : nine
6 : six
2 : two

3: true

1: false

1:
1: true

five

5 : five
1 :
4 : four
7 : seven
9 : nine
6 : six
2 : two

tree: 0
other: 8
another: 8

*/