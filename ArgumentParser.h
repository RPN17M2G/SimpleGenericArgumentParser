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

union UValuesUnion {
    long long v_integer;
    char* v_str;
    bool v_bool;
};

// ---------------------------- Test functions -------------------------------------
void func0(int count, std::vector<UValuesUnion> values) {
    bool first = values.at(0).v_bool;
    bool second = values.at(1).v_bool; //For example, no input tests were made

    std::cout << "Bools for func0 are: " << std::boolalpha << first << " " << second << std::endl;
}

void func1(int count, std::vector<UValuesUnion> values) {
    int first = values.at(0).v_integer;

    std::cout << "Hex Number for func1 is: 0x" << std::hex << first << std::endl;

}

void func2(int count, std::vector<UValuesUnion> values) {
    int first = values.at(0).v_integer;
    int second = values.at(1).v_integer;
    int third = values.at(2).v_integer;


    std::cout << "Hex Numbers for func2 are: 0x" << std::hex << first << " 0x" << second << " 0x" << third << std::endl;

}

void func3(int count, std::vector<UValuesUnion> values) {
    std::string first = values.at(0).v_str;
    std::string second = values.at(1).v_str;
    std::string third = values.at(2).v_str;
    std::string forth = values.at(3).v_str;


    std::cout << "strings for func3 are: " << first << " " << second << " " << third << " " << forth << std::endl;
}

void func4(int count, std::vector<UValuesUnion> values) {
    int first = values.at(0).v_integer;

    std::cout << "Hex Number for func4 is: 0x" << std::hex << first << std::endl;
}

void func5(int count, std::vector<UValuesUnion> values) {
    int first = values.at(0).v_integer;
    int second = values.at(1).v_integer;


    std::cout << "Hex Numbers for func5 are: 0x" << std::hex << first << " 0x" << second << std::endl;
}

void func6(int count, std::vector<UValuesUnion> values) {
    int first = values.at(0).v_integer;

    std::cout << "Hex Number for func6 is: 0x" << std::hex << first << std::endl;
}
//---------------------------- Test functions -------------------------------------

#define EMPTY_ARGS 2
#define BASE_HEX 16
#define BASE_DECIMAL 10
#define MIN_LENGTH_FOR_HEX_NUMBER 3

enum ETypeValue { NONE, INTEGER, STR, BOOLEAN, NUMBER_OF_ELEMENTS_IN_ENUM  };
std::map<ETypeValue, std::string> MTypeValueForPrint{
    {NONE, "No value"},
    {INTEGER, "Int"},
    {STR, "String"},
    {BOOLEAN, "Boolean"},
    {NUMBER_OF_ELEMENTS_IN_ENUM, "Hex number"}
};



struct SFlagProperties {
    ETypeValue type;
    int numberOfValues;
    std::string helpMessage;
    int function_index;
};

typedef void (*func)(int, std::vector<UValuesUnion>);
func functionArray[] = {func0, func1, func2, func3, func4, func5, func6};



class csvParser {

    /*
    CSV file format:
    nameOfFlag(must start with '-'),mustHaveValue(must be "TRUE" or "FALSE" or those values in number(0/1)),type(Number between 0 to the length of typeValue enum)
    ,numberOfValues(must be greater than 0), helpMessage(messeage to print), typeOfOperation(Number between 0 to the length of typeOfOperation enum)
    */

    public:
        static std::map<std::string, SFlagProperties> MFlags;
        static const std::string SCsvPath; //Initialized out of the class

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
const std::string csvParser::SCsvPath = "...";
std::map<std::string, SFlagProperties> csvParser::MFlags;

class ArgumentsParser {
    private:
        std::map<std::string, std::vector<UValuesUnion>> MFlagsMap;

    public:
        ArgumentsParser(int argc, char* argv[]);
        ~ArgumentsParser();
    
        //Printing
        const void printMap();
        const inline bool shouldPrintHelp();

        //Extracting values for a specific token(flag)
        static std::vector<UValuesUnion> extractValue(const std::string flag, int index, int argc ,char* argv[]);

        void finishParser();
};



