
#include <iostream>

#ifndef TESTS
#define TESTS

#include "ArgumentParser.h"

// ---------------------------- Test functions -------------------------------------
void func_test(std::string flag, std::vector<UValuesUnion> values) {
    //Function for automatic tests
    int index = 0;
    std::cout << "," << flag;
    for (auto type : csvParser::MFlags.find(flag)->second.types) {
        switch (type) {
        case INTEGER:
            std::cout << "," << values.at(index).v_integer;
            break;
        case BOOLEAN:
            std::cout << "," << values.at(index).v_bool;
            break;
        case STR:
            std::cout << "," << values.at(index).v_str;
            break;

        case NONE:
            break;

        default:
            break;
        }
        index++;
    }
}

void s_func(std::string flag, std::vector<UValuesUnion> values) {
    //Function for automatic tests for the s flag
    int first = values.at(0).v_integer;

    std::cout << ",-s," << first << std::endl;
}
//---------------------------- Test functions -------------------------------------

#endif
