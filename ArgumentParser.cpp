#include "ArgumentParser.h"

int main(int argc, char* argv[])
{
    csvParser::findFlags(); //Load flags from csv

    ArgumentsParser analyzer(argc, argv);
    
    if (analyzer.shouldPrintHelp()) {
        std::cout << "Usage: " << std::endl << "ArgumentParser <-flag> <value> <value2> ... <-flag>" << std:: endl << "ArgumentParser " << csvParser::formatFlagsForPrint() << std::endl << std::endl << "Options: " << std::endl;
        csvParser::printAllFlags();
        return 0;
    }

    analyzer.finishParser();
}

ArgumentsParser::ArgumentsParser(int argc, char* argv[])
{
    
    /*
    Constructor for ArgumentsParser.
    Inserting all tokens from argv to a vector
    without the program name.
    */
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { //A flag

            if (MFlagsMap.find(argv[i]) != MFlagsMap.end()) { //Flag already inserted to the map
                std::cerr << "Duplicate call for flag " << (std::string)(argv[i] + 1) << std::endl << "Exiting program.";
                exit(1);
            }

            if (csvParser::MFlags.find(argv[i]) == csvParser::MFlags.end() && !(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help"))) { //Flag does not exists
                std::cerr << "Flag " << (std::string)(argv[i] + 1) << " does not exists." << std::endl << "Exiting program.";
                exit(1);
            }

            this->MFlagsMap[argv[i]] = this->extractValue(argv[i], i, argc, argv);

            i += csvParser::MFlags[argv[i]].numberOfValues;
        } 
        else {
            std::cerr << "Unexpected value." << std::endl << "Exiting program.";
            exit(1);
        }
    }
    
}

ArgumentsParser::~ArgumentsParser()
{
    /*
    deconstructor for ArgumentParser.
    deleting all strings from MFlagsMap.
    */
    for (auto index : MFlagsMap) {
        if (csvParser::MFlags[index.first].type == ETypeValue::STR) {
            for (auto i : index.second) {
                delete(i.v_str);
            }
        }
    }
}

const void ArgumentsParser::printMap()
{
    /*
    Printing all tokens in the vector
    *Mainly for test purposes.
    */
    for (auto index : MFlagsMap) {
        if (csvParser::MFlags[index.first].numberOfValues > 0) {
            std::cout << index.first << std::endl;
            for (auto i : index.second) {
                std::cout << "Value: ";

                switch (csvParser::MFlags[index.first].type) {
                    case ETypeValue::INTEGER:
                        std::cout << "Integer - " << i.v_integer;
                        break;

                    case ETypeValue::STR:
                        std::cout << "String - " << i.v_str << std::endl;
                        break;

                    case ETypeValue::BOOLEAN:
                        std::cout << "Hex - 0x" << std::hex << i.v_integer << std::endl;
                        break;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;

        }
    }
}

const inline bool ArgumentsParser::shouldPrintHelp()
{
    /*
    Checking if the tokens are empty or the flag "-h"/"--help" is in
    the tokens vector.
    returning a bool which indicates if the program should print the usage and stop.
    */
    return !MFlagsMap.size()
        || MFlagsMap.find("-h") != MFlagsMap.end()
        || MFlagsMap.find("--help") != MFlagsMap.end();
}



std::vector<UValuesUnion> ArgumentsParser::extractValue(const std::string flag,int index ,int argc, char* argv[])
{
    /*
    Extractes Command values from known information.
    Finds if a flag needs value and what this value is.
    Returns the values for the object
    */
    UValuesUnion temp;
    bool hex_base = false;
    std::vector<UValuesUnion> valuesVector;
    for (int i = 1; i <= csvParser::MFlags[flag].numberOfValues; i++) {
        UValuesUnion flagValue;
        if (index + i == argc || argv[index + i][0] == '-') { //The last index or next index is a flag and not a value
            std::cerr << (("Missing value for flag " + flag.substr(1)).c_str()) << std::endl << "Exiting program.";
            exit(1);
        }

        switch (csvParser::MFlags[flag].type) {
            case ETypeValue::INTEGER:
                hex_base = false;
                if (strlen(argv[index + i]) >= MIN_LENGTH_FOR_HEX_NUMBER) {
                    if (argv[index + i][0] == '0' && argv[index + i][1] == 'x')
                        hex_base = true;
                }
                flagValue.v_integer = csvParser::checkInt(argv[index + i], flag, hex_base);
                break;

            case ETypeValue::STR:
                flagValue.v_str = argv[index + i];
                break;

            case ETypeValue::BOOLEAN:
                try {
                    flagValue.v_bool = csvParser::checkBool(argv[index + i], flag);
                }
                catch (...) {
                    std::cerr << (("Value is not an integer or out of range for flag " + flag.substr(1)).c_str()) << std::endl << "Exiting program.";
                    exit(1);
                }
                break;

                
        }
        valuesVector.push_back(flagValue);
    }
    
    return valuesVector;
}

void ArgumentsParser::finishParser()
{
    /*
    Passing results to the functions of each flag
    */
    for (auto index : MFlagsMap) {
        functionArray[csvParser::MFlags[index.first].function_index](index.second.size(), index.second);
    }
}

void csvParser::findFlags()
{
    /*
    Uploading from the csv file all of the flags and
    their properties.

    
    
    CSV file format:
    nameOfFlag(must start with '-'),mustHaveValue(must be "TRUE" or "FALSE" or those values in number(0/1)),type(Number between 0 to the length of typeValue enum)
    ,numberOfValues(must be greater than 0), helpMessage(messeage to print), typeOfOperation(Number between 0 to the length of typeOfOperation enum)
    
    
    */
    std::ifstream readCSV(csvParser::SCsvPath);
    std::string line;
    char* part;
    char* token;
    std::string flag = "-";

    while (std::getline(readCSV, line)) {
        part = strtok_s(const_cast<char*>(line.c_str()), ",", &token); //Cutting the line
        flag = "-";
        flag += part;
        
        //Number of values extraction
        part = strtok_s(NULL, ",", &token);
        checkNull(part, flag, "numberOfValues");
        MFlags[flag].numberOfValues = checkInt(part, flag, 0);

        //Value type extraction
        part = strtok_s(NULL, ",", &token);
        checkNull(part, flag, "valueType");
        MFlags[flag].type = switchValueType(part, flag);

        //Help message extraction
        part = strtok_s(NULL, ",", &token);
        checkNull(part, flag, "helpMessage");
        MFlags[flag].helpMessage = part;

        //Function index extraction
        part = strtok_s(NULL, ",", &token);
        checkNull(part, flag, "functionIndex");
        MFlags[flag].function_index = checkInt(part, flag, 0, 8);
        
    }
}

void csvParser::printAllFlags()
{
    /*
    Printing information about all flags extracted from csv
    */
    std::cout << "-h / --help" << std::endl << "    Prints documantiaion of the flags" << std::endl; //Prints help flag
    for (auto index : MFlags) {
        std::cout << index.first << std::endl << "    " << index.second.helpMessage << std::endl << "    Type: " << MTypeValueForPrint[index.second.type] << 
            std::endl << "    Number of values: " << index.second.numberOfValues << std::endl 
            << "    Function index: " << index.second.function_index << std::endl;
    }
}

std::string csvParser::formatFlagsForPrint()
{
    /*
    Formating the list of all the flags for printing
    Output: the list of all flags formated
    */
    std::string output = "";
    for (auto index : MFlags) {
        output += " [" + index.first + "]";
    }
    return output;
}

void csvParser::checkNull(char* part, std::string& flag, std::string msg)
{
    /*
    Checking if a part is null and printing missing flag property
    input: char* part, std::string& flag, std::string msg
    output: none
    */
    if (part == NULL) {
        std::cerr << (("Missing flag property " + flag.substr(1)).c_str()) << ", Message: Missing property " << msg << std::endl << "Exiting program.";
        exit(1);
    }
}

long long csvParser::checkInt(char* part, std::string flag, bool hex)
{
    /*
    Checking if a specific string is an integer and printing a message if not
    input: char* part, std::string flag
    Output: the string as int
    */
    long long temp;
    int base = hex ? BASE_HEX : BASE_DECIMAL; 

    try {
        temp = std::strtol(part, NULL, base);
    }
    catch (...) {
        std::cerr << (("Value is not an integer for flag " + flag.substr(1)).c_str()) << std::endl << "Exiting program.";
        exit(1);
    }
    return temp;
}

long long csvParser::checkInt(char* part, std::string& flag, int minValue, int maxValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is in between 2 values
    Input: char* part, std::string flag, int minValue, int maxValue
    Output: the string as int
    */
    long long temp = checkInt(part, flag, hex);
    if (temp < minValue || temp > maxValue) {
        std::cerr << "Value for flag " << flag.substr(1) << " must be greater than " << minValue << " and be smaller than " << maxValue << ": " << part << std::endl << "Exiting program.";
        exit(1);
    }
    return temp;
}

long long csvParser::checkInt(char* part, std::string& flag, int minValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is larger than a value
    Input: char* part, std::string flag, int minValue
    Output: the string as int
    */
    long long temp = checkInt(part, flag, hex);
    if (temp < minValue) {
        std::cerr << "Value for flag " << flag.substr(1) << " must be greater than " << minValue << ": " << part << std::endl << "Exiting program.";
        exit(1);
    }
    return temp;
}

bool csvParser::checkBool(char* part, std::string flag)
{
    /*
    Checking if a specific string is a boolean and converting to bool.
    Input: char* part, std::string flag
    Output: the string as bool
    */
    bool result = strcmp(part, "FALSE");
    if (strcmp(part, "TRUE") && strcmp(part, "FALSE") && !(strlen(part) == 1 && (*part == '0' || *part == '1'))) { //Input is not TRUE and is not FALSE
        std::cerr << "Value must be TRUE or FALSE or 0 or 1 for flag " << flag.substr(1) << std::endl << "Exiting program.";
        exit(1);
    }

    if (*part == '0') {
        result = false;
    }
    else if (*part == '1') {
        result = true;
    }

    return result;
}

ETypeValue csvParser::switchValueType(std::string part, std::string& flag)
{
    /*
    Switching value type from string to the corresponding int from the csv
    input: std::string part, std::string& flag
    output: ETypeValue
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
        std::cerr << "CSV file error: " << std::endl << (("Value must be BOOLEAN or INTEGER or STR for flag " + flag.substr(1)).c_str()) << std::endl << "Exiting program.";
        exit(1);
    }
    return returnValue;
}


