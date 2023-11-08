#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstring>
#include <functional>
#include <cstdarg>

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
    bool does_need_s_flag;
};


class csvParser {

    /*
    Csv file format: flag, extra s, number_of_values, types_of_values, description
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
const std::string csvParser::SCsvPath = "..."; //Replace with csv file path
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
        const bool s_flag(int argc, char* argv[]);

        //Extracting values for a specific token(flag)
        static std::vector<UValuesUnion> extractValue(const std::string flag, int index, int argc ,char* argv[]);

        void finishParser();

        static bool extra_s;
};

bool ArgumentsParser::extra_s = false;



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

typedef void (*func)(std::string flag, std::vector<UValuesUnion>);
func functionArray[] = { func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, s_func };
