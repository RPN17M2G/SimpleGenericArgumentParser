#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstring>
#include <functional>
#include <cstdarg>
#include<array> 
#include <exception>

#define EMPTY_ARGS 2
#define BASE_HEX 16
#define BASE_DECIMAL 10
#define MIN_LENGTH_FOR_HEX_NUMBER 3
#define MIN_VALUE_FOR_S 1
#define MAX_VALUE_FOR_S 2
#define NUM_OF_PARAMS_FOR_HELP_IN_ARGC 2

union UValuesUnion {
    long long v_integer;
    char* v_str;
    bool v_bool;
};


typedef void (*ArgumentFunc)(std::string, std::vector<UValuesUnion>);

class ArgumentsParser {
    private:
        std::map<std::string, std::vector<UValuesUnion>> MFlagsMap;
        std::vector<ArgumentFunc> functionArr;
        static bool extra_s;
        static int SFlagValue;

        //Pass flag values into each flag's corresponding function
        void finishParser();

        //Printing
        const inline bool shouldPrintHelp();

        //Extracting values for a specific token(flag)
        static std::vector<UValuesUnion> extractValue(const std::string flag, int index, int argc, char* argv[]);

    public:
        ArgumentsParser(int argc, char* argv[], ArgumentFunc functionArr[], int size);
        ~ArgumentsParser() noexcept(false);

        static int getSValue();

        static int init(int argc, char* argv[], ArgumentFunc functionArray[], int size);
};
