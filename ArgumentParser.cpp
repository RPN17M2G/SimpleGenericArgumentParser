#include "ArgumentParser.h"


bool ArgumentsParser::extra_s = false;
int ArgumentsParser::SFlagValue = 0;
const std::string csvParser::SCsvPath = "..."; //Replace with csv file path

std::map<std::string, SFlagProperties> csvParser::MFlags;

int main(int argc, char* argv[]) 
{
    ArgumentFunc functionArray[] = { func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, s_func };
    int arrayLength = 17;
    int returnCode = ArgumentsParser::init(argc, argv, functionArray, arrayLength);
    return returnCode;
}

int ArgumentsParser::getSValue()
{
    /*
    A getter for SFlagValue
    Output: int - the S flag value
    */
    return SFlagValue;
}

int ArgumentsParser::init(int argc, char* argv[], ArgumentFunc functionArray[], int size)
{
    /*
    Using the ArgumentParser and the csvParser, replacement for main that is 
    specifically for the parser.
    Output: The return code of the parser
    Input: Main arguments, the function array of the flags
    */
    try {
        csvParser::findFlags(); //Load flags from csv

        ArgumentsParser analyzer(argc, argv, functionArray, size);

        if (analyzer.shouldPrintHelp()) {
            if (argc > NUM_OF_PARAMS_FOR_HELP_IN_ARGC) { 
                throw std::runtime_error("[!] Flag h must be used alone. Please remove any additional flags or arguments.");
            }
            std::cout << "[*] Usage: " << std::endl << "[*] " << argv[0] << " <-flag> <value> <value2> ... <-flag>" << std::endl << "[*] " << argv[0] << csvParser::formatFlagsForPrint() << std::endl << "[*] Options: " << std::endl;
            csvParser::printAllFlags();
            return 0;
        }

        analyzer.finishParser();

        return 0;
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what();
        return -1;
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return -1;
    }
    
}


ArgumentsParser::ArgumentsParser(int argc, char* argv[], ArgumentFunc functionArr[], int size)
{
    
    /*
    Constructor for ArgumentsParser.
    Inserting all tokens from argv into a vector.
    The function excludes the program name.
    Input: Main arguments, the function array of the flags
    */
    for (int i = 0; i < size; i++) {
        this->functionArr.push_back(functionArr[i]);
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { //A flag

            if (MFlagsMap.find(argv[i]) != MFlagsMap.end()) { //Flag is already inserted to the map
                throw std::runtime_error("[!] Duplicate call for flag " + (std::string)(argv[i] + 1));
            }

            if (csvParser::MFlags.find(argv[i]) == csvParser::MFlags.end() && !(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help") || !strcmp(argv[i], "-s"))) { //Flag does not exists
                throw std::runtime_error("[!] Flag " + (std::string)(argv[i] + 1) + " does not exists.");
            }

            if (!(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "-s"))) { //Flag is not a special flag
                try {
                    ArgumentsParser::extra_s = ArgumentsParser::extra_s | csvParser::MFlags.at(argv[i]).extra_s;
                }
                catch (...) {
                    throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
                }
            }


            this->MFlagsMap[argv[i]] = this->extractValue(argv[i], i, argc, argv);
            if (!(strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0)) {
                try {
                    i += csvParser::MFlags.at(argv[i]).numberOfValues;
                }
                catch (...) {
                    throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
                }
            }


        } 
        else {
            throw std::runtime_error("[!] Unexpected value.");
        }
    }
    
}

ArgumentsParser::~ArgumentsParser() noexcept(false)
{
    /*
    deconstructor for ArgumentParser.
    Deleting all strings values from MFlagsMap.
    */
    int index_types = -1;
    for (auto index : MFlagsMap) {
        index_types = -1;
        if (index.first != "-h" && index.first != "--help") {
            try {
                for (auto type : csvParser::MFlags.at(index.first).types) {
                    index_types++;
                    if (type == ETypeValue::STR) {
                        delete index.second.at(index_types).v_str;
                    }
                }
            }
            catch (...) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
        }
    }
}

const inline bool ArgumentsParser::shouldPrintHelp()
{
    /*
    Checking if the tokens are empty or the flag "-h"/"--help" is in
    the tokens vector.
    Output: Boolean - indicates if the program should print the usage documentation and stop.
    */
    return !MFlagsMap.size()
        || MFlagsMap.find("-h") != MFlagsMap.end()
        || MFlagsMap.find("--help") != MFlagsMap.end();
}





std::vector<UValuesUnion> ArgumentsParser::extractValue(const std::string flag,int index ,int argc, char* argv[])
{
    /*
    Extractes flag values from argv,
    sorts the values into their correct types.
    Input: Main arguments, the index of the current flag in argv, the name of the flag
    Output: Vector including the values in an union.
    */
    bool hex_base = false;
    std::vector<UValuesUnion> valuesVector;
    try {
        if (flag != "-h" && flag != "--help") {
            for (int i = 1; i <= csvParser::MFlags.at(flag).numberOfValues; i++) {
                UValuesUnion flagValue;
                if (index + i == argc || argv[index + i][0] == '-') { //The last index or the next index is a flag and not a value
                    throw std::runtime_error((("[!] Missing value for flag " + flag.substr(1)).c_str()));
                }

                switch (csvParser::MFlags.at(flag).types.at(i - 1)) {
                case ETypeValue::INTEGER:
                    hex_base = false;
                    if (strlen(argv[index + i]) >= MIN_LENGTH_FOR_HEX_NUMBER) {
                        if (argv[index + i][0] == '0' && argv[index + i][1] == 'x')
                            hex_base = true;
                    }
                    flagValue.v_integer = csvParser::checkInt(argv[index + i], flag, hex_base);
                    break;

                case ETypeValue::STR:
                    flagValue.v_str = new char[strlen(argv[index + i]) + 1];
                    strcpy_s(flagValue.v_str, strlen(argv[index + i]) + 1, argv[index + i]);
                    break;

                case ETypeValue::BOOLEAN:
                    try {
                        flagValue.v_bool = csvParser::checkBool(argv[index + i], flag);
                    }
                    catch (...) {
                        throw std::runtime_error((("[!] Value is not a boolean for flag " + flag.substr(1)).c_str()));
                    }
                    break;

                case ETypeValue::NONE:
                    break;

                default:
                    break;


                }
                valuesVector.push_back(flagValue);
            }
        }
    }
    catch (std::runtime_error& e) {
        throw e;
    }
    catch (std::exception& e) {
        throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
    }
    
    return valuesVector;
}

void ArgumentsParser::finishParser()
{
    /*
    Passing results to the functions of each flag.
    The function of the flag s is the first function to run.
    */
    if (ArgumentsParser::extra_s && MFlagsMap.find("-s") == MFlagsMap.end()) {
        throw std::runtime_error("[!] The -s flag, which is required for some of your operations, was not provided.");
    }
    else if (MFlagsMap.find("-s") != MFlagsMap.end() && (MFlagsMap.size() == 1 || !ArgumentsParser::extra_s)) { //Used -s alone or when not needed
        throw std::runtime_error("[!] The -s flag was provided when it was not needed for any of your operations.");
    }
    else if (ArgumentsParser::extra_s) {
        if ((*MFlagsMap.find("-s")).second.size() == 1 && ((*MFlagsMap.find("-s")).second.at(0).v_integer >= MIN_VALUE_FOR_S && (*MFlagsMap.find("-s")).second.at(0).v_integer <= MAX_VALUE_FOR_S)) {
            this->SFlagValue = (*MFlagsMap.find("-s")).second.at(0).v_integer;

            try {
                this->functionArr[csvParser::MFlags.at("-s").function_index]((*MFlagsMap.find("-s")).first, (*MFlagsMap.find("-s")).second);
            }
            catch (...) {
                throw std::runtime_error( "[!] Error occurred while accessing flag information - the key does not exist.");
            }

            MFlagsMap.erase("-s"); //Delete the s flag because it's function already ran
        }
        else {
            throw std::runtime_error("[!] Value out of bounds for flag -s");
        }
        
    }

    for (auto index : MFlagsMap) {
        try {
            this->functionArr[csvParser::MFlags.at(index.first).function_index](index.first, index.second);
        }
        catch (...) {
            throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
        }
    }
}

char* csvParser::trim(char* str)
{
    /*
    Trims a char* trailing spaces.
    Input: The char* string to trim
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

void csvParser::findFlags()
{
    /*
    Uploading from the csv file all of the flags and
    their properties.
    */
    std::ifstream readCSV(csvParser::SCsvPath);
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

            part = trim(strtok_s(const_cast<char*>(line.c_str()), ",", &token)); //Cutting the line
            flag = "-";
            flag += part;
            index++;

            MFlags[flag] = SFlagProperties();

            //Does need s flag extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "extra_s");
            try {
                MFlags.at(flag).extra_s = checkBool(part, flag);
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            //Number of values extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "numberOfValues");
            try {
                MFlags.at(flag).numberOfValues = checkInt(part, flag, 0);
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            for (int i = 0; i < MFlags[flag].numberOfValues; i++) {
                //Value type extraction
                part = trim(strtok_s(NULL, ",", &token));
                checkNull(part, flag, "valueType");
                try {
                    MFlags.at(flag).types.push_back(switchValueType(part, flag));
                }
                catch (std::runtime_error& e) {
                    throw e;
                }
                catch (std::exception& e) {
                    throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
                }
                
            }
            
            //Help message extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "helpMessage");
            try {
                MFlags.at(flag).helpMessage = part;
            }
            catch (std::runtime_error& e) {
                throw e;
            }
            catch (std::exception& e) {
                throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
            }
            

            //Function index calculation
            try {
                MFlags.at(flag).function_index = index;
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
        MFlags["-s"] = SFlagProperties();
        MFlags.at("-s").extra_s = false;
        MFlags.at("-s").numberOfValues = 1;
        MFlags.at("-s").types.push_back(INTEGER);
        MFlags.at("-s").helpMessage = "DESC - Choose a number 1 or 2"; //Example operation, change as require
        MFlags.at("-s").function_index = ++index;
    }
    catch (...) {
        throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
    }



}

void csvParser::printAllFlags()
{
    /*
    Printing information about all flags.
    */
    std::cout << "[*] -h / --help: Prints documantiaion of the flags" << std::endl; //Prints help flag
    for (auto index : MFlags) {
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
    Checking if a string is null
    Input: The part to check, the name of the flag and the property for error printing
    */
    if (part == NULL) {
        throw std::runtime_error("[!] Missing flag property " + flag.substr(1) + ", Message: Missing property " + msg);
    }
}

long long csvParser::checkInt(char* part, std::string flag, bool hex)
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

long long csvParser::checkInt(char* part, std::string& flag, int minValue, int maxValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is in between 2 values
    Input: The part to check, the name of the flag for error printing, An indicator of the base of the number, the range of numbers
    Output: The string as int
    */
    long long temp = checkInt(part, flag, hex);
    if (temp < minValue || temp > maxValue) {
        throw std::runtime_error("[!] Value for flag " + flag.substr(1) + " must be greater than " + std::to_string(minValue) + " and less than " + std::to_string(maxValue) + ": " + part);
    }
    return temp;
}

long long csvParser::checkInt(char* part, std::string& flag, int minValue, bool hex)
{
    /*
    Checking if a specific string is an integer and is larger than a value
    Input: The part to check, the name of the flag for error printing, An indicator of the base of the number, the minimum value
    Output: The string as int
    */
    long long temp = checkInt(part, flag, hex);
    if (temp < minValue) {
        throw std::runtime_error("[!] Value for flag " + flag.substr(1) + " must be greater than " + std::to_string(minValue) + ": " + part);
    }
    return temp;
}

bool csvParser::checkBool(char* part, std::string flag)
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

ETypeValue csvParser::switchValueType(std::string part, std::string& flag)
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


