#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "SFlagProperties.hpp"
#include "ETypeValue.hpp"

class csvParser {

    /*
    Csv file format: flag, extra s, number_of_values, types_of_values, description
    */

public:
    static std::map<std::string, SFlagProperties> s_MFlags;
    static const std::string s_SCsvPath; //Initialized out of the class

    static char* s_trim(char* str);

    static void s_findFlags();

    //Printing for -h flag
    static void s_printAllFlags();
    static std::string s_formatFlagsForPrint();

    //Checking input and converting types
    static void s_checkNull(char* part, std::string& flag, std::string msg);
    static long long s_checkInt(char* part, std::string flag, bool hex);
    static long long s_checkInt(char* part, std::string& flag, int minValue, int maxValue, bool hex);
    static long long s_checkInt(char* part, std::string& flag, int minValue, bool hex);
    static bool s_checkBool(char* part, std::string flag);

    static ETypeValue s_switchValueType(std::string part, std::string& flag);
};

