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


#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER

#define EMPTY_ARGS 2
#define BASE_HEX 16
#define BASE_DECIMAL 10
#define MIN_LENGTH_FOR_HEX_NUMBER 3
#define MIN_VALUE_FOR_S 1
#define MAX_VALUE_FOR_S 2


union UValuesUnion {
    long long v_integer;
    char* v_str;
    bool v_bool;
};

enum ETypeValue { NONE, INTEGER, STR, BOOLEAN };
std::map<ETypeValue, std::string> MTypeValueForPrint{
    {NONE, "No value"},
    {INTEGER, "Int"},
    {STR, "String"},
    {BOOLEAN, "Boolean"}
};

struct SFlagProperties {
    std::vector<ETypeValue> types;
    int numberOfValues;
    std::string helpMessage;
    int function_index;
    bool extra_s;
};

class ReturnCodeException : public std::exception {
    //Exception for returning to main a different code than 0
    public:
        ReturnCodeException(int code) : code(code), codeStr(std::to_string(code)) { }

        const char* what() const throw () override {
            return codeStr.c_str();
        }

        int getCode() {
            return this->code;
        }

    private:
        int code;
        std::string codeStr;
};

class csvParser {

    /*
    Csv file format: flag, extra s, number_of_values, types_of_values, description
    */

    public:
        static std::map<std::string, SFlagProperties> MFlags;
        static const std::string SCsvPath; //Initialized out of the class

        static char* trim(char* str);

        static void findFlags();

        //Printing for -h flag
        static void printAllFlags();
        static std::string formatFlagsForPrint();

        //Checking input and converting types
        static void checkNull(char* part, std::string& flag, std::string msg);
        static long long checkInt(char* part, std::string flag, bool hex);
        static long long checkInt(char* part, std::string& flag, int minValue, int maxValue, bool hex);
        static long long checkInt(char* part, std::string& flag, int minValue, bool hex);
        static bool checkBool(char* part, std::string flag);

        static ETypeValue switchValueType(std::string part, std::string& flag);


};


typedef void (*ArgumentFunc)(std::string, std::vector<UValuesUnion>);

class ArgumentsParser {
    private:
        std::map<std::string, std::vector<UValuesUnion>> MFlagsMap;
        std::vector<ArgumentFunc> functionArr;
        static bool extra_s;
        static int SFlagValue;

        void finishParser();

        //Printing
        const inline bool shouldPrintHelp();

        //Extracting values for a specific token(flag)
        static std::vector<UValuesUnion> extractValue(const std::string flag, int index, int argc, char* argv[]);

    public:
        ArgumentsParser(int argc, char* argv[], ArgumentFunc functionArr[], int size);
        ~ArgumentsParser();

        static int getSValue();

        static int init(int argc, char* argv[], ArgumentFunc functionArray[], int size);
};


#include "tests.h"

#endif



