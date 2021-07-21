#include "utils/scope_exit.hpp"
#include <string>
#include <fstream>
#include <iostream>

#define DEBUG_PRINT 1
#define TEST_MERGE 1

static const std::string basic_file_path{"../resources/chr0716.txt"};

int main()
{
    std::string basic_str{};
    {
        std::ifstream basic_ifs{basic_file_path, std::ios::in};
        SCOPE_GUARD
        {
            basic_ifs.close();
        };
        getline(basic_ifs, basic_str);
    }
#if DEBUG_PRINT
    std::cout << "basic_str: " << basic_str << '\n';
#endif
    
    return 0;
}