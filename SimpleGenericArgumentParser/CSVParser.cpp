#include "CSVParser.hpp"

#define BASE_HEX 16
#define BASE_DECIMAL 10

const std::string csvParser::s_SCsvPath = "..."; //Replace with csv file path

char* csvParser::s_trim(char* str)
{
    /*
    Trims a char* trailing spaces.
    Input: The char* string to s_trim
    Output: The trimmed string
    */
    if (str == NULL) { //The line ended, but a value was expected.
        throw std::runtime_error("[!] CSV file error: The line ended, but a value was expected.");
    }
    size_t start = strspn(str, " ");

    // Remove trailing spaces
    size_t end = strlen(str);
    while (end > 0 && str[end - 1] == ' ') {
        end--;
    }

    str[end] = '\0';

    return str + start;
}

void csvParser::s_findFlags()
{
    /*
    Uploading from the csv file all of the flags and
    their properties.
    */
    std::ifstream readCSV(csvParser::s_SCsvPath);
    std::string line;
    char* part;
    char* token;
    std::string flag = "-";
    int index = -1;

    if (!readCSV.is_open()) {
        throw std::runtime_error("[!] Error opening csv file - aborting program.");
    }

    while (std::getline(readCSV, line)) {
        if (line.length() > 2 && line.at(0) != '/' && line.at(1) != '/') { //Not a comment

            part = s_trim(strtok_s(const_cast<char*>(line.c_str()), ",", &token)); //Cutting the line
            flag = "-";
            flag += part;
            index++;

            s_MFlags[flag] = SFlagProperties();

            //Does need s flag extraction
            part = s_trim(strtok_s(NULL, ",", &token));
            s_checkNull(part, flag, "extra_s");
            try {
                s_MFlags.at(flag).extra_s = s_checkBool(part, flag);
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            
            //Number of values extraction
            part = s_trim(strtok_s(NULL, ",", &token));
            s_checkNull(part, flag, "numberOfValues");
            try {
                s_MFlags.at(flag).numberOfValues = s_checkInt(part, flag, 0);
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            for (int i = 0; i < s_MFlags[flag].numberOfValues; i++) {
                //Value type extraction
                part = s_trim(strtok_s(NULL, ",", &token));
                s_checkNull(part, flag, "valueType");
                try {
                    s_MFlags.at(flag).types.push_back(s_switchValueType(part, flag));
                }
                catch (std::runtime_error& e) {
                    throw e;
                }
                catch (std::exception& e) {
                    throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
                }
                
            }
            
            //Help message extraction
            part = s_trim(strtok_s(NULL, ",", &token));
            s_checkNull(part, flag, "helpMessage");
            try {
                s_MFlags.at(flag).helpMessage = part;
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            //Function index calculation
            try {
                s_MFlags.at(flag).function_index = index;
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            if (strtok_s(NULL, ",", &token) != NULL) { //CSV line length does not follow the format
                throw std::runtime_error("[!] CSV file error: CSV line length does not follow the format.");
            }
        }
        
        
    }

    //Adding s flag manullly
    try {
        s_MFlags["-s"] = SFlagProperties();
        s_MFlags.at("-s").extra_s = false;
        s_MFlags.at("-s").numberOfValues = 1;
        s_MFlags.at("-s").types.push_back(INTEGER);
        s_MFlags.at("-s").helpMessage = "DESC - Choose a number 1 or 2"; //Example operation, change as require
        s_MFlags.at("-s").function_index = ++index;
    }
    catch (...) {
        throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
    }
}

void csvParser::s_printAllFlags()
{
    /*
    Printing information about all flags.
    */
    std::cout << "[*] -h / --help: Prints documantiaion of the flags" << std::endl; //Prints help flag
    for (auto index : s_MFlags) {
        if (index.first != "-h") { //Don't print help flag again
            std::cout << "[*] " << index.first << ": " << index.second.helpMessage
                << ", Function index: " << index.second.function_index  << ", Number of values: " << index.second.numberOfValues;
            for (int i = 0; i < index.second.numberOfValues; i++) {
                try {
                    std::cout << ", Type for value number " << i + 1 << ": " << MTypeValueForPrint[index.second.types.at(i)];
                }
                catch (...) {
                    throw std::runtime_error("[!] Error occurred while accessing the types of " + index.first + " flag.");
                }
            }
            std::cout << std::endl;
        }
    }
}

std::string csvParser::s_formatFlagsForPrint()
{
    /*
    Formating the list of all the flags for printing
    Output: the list of all flags formated
    */
    std::string output = "";
    for (auto index : s_MFlags) {
        output += " [" + index.first + "]";
    }
    return output;
}

void csvParser::s_checkNull(char* part, std::string& flag, std::string msg)
{
    /*
    Checking if a string is null
    Input: The part to check, the name of the flag and the property for error printing
    */
    if (part == NULL) {
        throw std::runtime_error("[!] Missing flag property " + flag.substr(1) + ", Message: Missing property " + msg);
    }
}

long long csvParser::s_checkInt(char* part, std::string flag, bool hex)
{
    /*
    Checking if a specific string is an integer
    Input: The part to check, the name of the flag for error printing and an indicator of the base of the number
    Output: The string as int
    */
    long long temp;
    int base = hex ? BASE_HEX : BASE_DECIMAL; 

    try {
        temp = std::strtol(part, NULL, base);
        if (temp == 0 && part[0] != '0') {
            throw std::runtime_error("[!] Value is not an integer for flag " + flag.substr(1));
        }
    }
    catch (...) {
        throw std::runtime_error("[!] Value is not an integer for flag " + flag.substr(1));
    }
    return temp;
}

long long csvParser::s_checkInt(char* part, std::string& flag, int minValue, int maxValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is in between 2 values
    Input: The part to check, the name of the flag for error printing, An indicator of the base of the number, the range of numbers
    Output: The string as int
    */
    long long temp = s_checkInt(part, flag, hex);
    if (temp < minValue || temp > maxValue) {
        throw std::runtime_error("[!] Value for flag " + flag.substr(1) + " must be greater than " + std::to_string(minValue) + " and less than " + std::to_string(maxValue) + ": " + part);
    }
    return temp;
}

long long csvParser::s_checkInt(char* part, std::string& flag, int minValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is larger than a value
    Input: The part to check, the name of the flag for error printing, An indicator of the base of the number, the minimum value
    Output: The string as int
    */
    long long temp = s_checkInt(part, flag, hex);
    if (temp < minValue) {
        throw std::runtime_error("[!] Value for flag " + flag.substr(1) + " must be greater than " + std::to_string(minValue) + ": " + part);
    }
    return temp;
}

bool csvParser::s_checkBool(char* part, std::string flag)
{
    /*
    Checking if a specific string is a boolean and converting to bool.
    Input: The part to check, the name of the flag for error printing
    Output: The string as bool
    */
    bool result = !(!strcmp(part, "FALSE") || !strcmp(part, "false"));
    if (strcmp(part, "true") && strcmp(part, "false") && strcmp(part, "TRUE") && strcmp(part, "FALSE") && !(strlen(part) == 1 && (*part == '0' || *part == '1'))) { //Input is not TRUE and is not FALSE
        throw std::runtime_error("[!] The value must be either TRUE, FALSE, true, false, 0, or 1 for flag " + flag.substr(1));
    }

    if (*part == '0') {
        result = false;
    }
    else if (*part == '1') {
        result = true;
    }

    return result;
}

ETypeValue csvParser::s_switchValueType(std::string part, std::string& flag)
{
    /*
    Switching value type from string to the corresponding enum value
    Input: The part to switch and the flag for error printing
    Output: ETypeValue
    */
    ETypeValue returnValue = NONE;
    if (part == "BOOLEAN") {
        returnValue = BOOLEAN;
    }
    else if (part == "INTEGER") {
        returnValue = INTEGER;
    }
    else if (part == "STR") {
        returnValue = STR;
    }
    else {
        throw std::runtime_error("[!] CSV file error: The value must be either BOOLEAN, INTEGER, or STR for flag " + flag.substr(1));
    }
    return returnValue;
}


