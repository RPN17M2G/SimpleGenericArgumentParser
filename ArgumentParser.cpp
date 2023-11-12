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
    */
    return SFlagValue;
}

int ArgumentsParser::init(int argc, char* argv[], ArgumentFunc functionArray[], int size)
{
    /*
    Using the ArgumentParser and the csvParser, replacment for main that is 
    specificly for the parser.
    */
    try {
        csvParser::findFlags(); //Load flags from csv

        ArgumentsParser analyzer(argc, argv, functionArray, size);

        if (analyzer.shouldPrintHelp()) {
            std::cout << "[*]Usage: " << std::endl << "[*]" << argv[0] << " <-flag> <value> <value2> ... <-flag>" << std::endl << "[*]" << argv[0] << csvParser::formatFlagsForPrint() << std::endl << std::endl << "[*]Options: " << std::endl;
            csvParser::printAllFlags();
            return 0;
        }

        analyzer.finishParser();

        return 0;
    }
    catch (ReturnCodeException& e) {
        return e.getCode();
    }
}


ArgumentsParser::ArgumentsParser(int argc, char* argv[], ArgumentFunc functionArr[], int size)
{
    
    /*
    Constructor for ArgumentsParser.
    Inserting all tokens from argv to a vector
    without the program name.
    */
    for (int i = 0; i < size; i++) {
        this->functionArr.push_back(functionArr[i]);
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { //A flag

            if (MFlagsMap.find(argv[i]) != MFlagsMap.end()) { //Flag already inserted to the map
                std::cerr << "[!]Duplicate call for flag " << (std::string)(argv[i] + 1) << std::endl;
                throw ReturnCodeException(1);
            }

            if (csvParser::MFlags.find(argv[i]) == csvParser::MFlags.end() && !(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help") || !strcmp(argv[i], "-s"))) { //Flag does not exists
                std::cerr << "[!]Flag " << (std::string)(argv[i] + 1) << " does not exists." << std::endl;
                throw ReturnCodeException(1);
            }

            if (!(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "-s"))) { //Not special flag
                ArgumentsParser::extra_s = ArgumentsParser::extra_s | csvParser::MFlags[argv[i]].extra_s;
            }


            this->MFlagsMap[argv[i]] = this->extractValue(argv[i], i, argc, argv);

            i += csvParser::MFlags[argv[i]].numberOfValues;

        } 
        else {
            std::cerr << "[!]Unexpected value." << std::endl;
            throw ReturnCodeException(1);
        }
    }
    
}

ArgumentsParser::~ArgumentsParser()
{
    /*
    deconstructor for ArgumentParser.
    deleting all strings from MFlagsMap.
    */
    int index_types = -1;
    for (auto index : MFlagsMap) {
        index_types = -1;
        for (auto type : csvParser::MFlags[index.first].types) {
            index_types++;
            if (type == ETypeValue::STR) {
                delete index.second.at(index_types).v_str;
            }
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
    bool hex_base = false;
    std::vector<UValuesUnion> valuesVector;
    for (int i = 1; i <= csvParser::MFlags[flag].numberOfValues; i++) {
        UValuesUnion flagValue;
        if (index + i == argc || argv[index + i][0] == '-') { //The last index or next index is a flag and not a value
            std::cerr << (("[!]Missing value for flag " + flag.substr(1)).c_str()) << std::endl;
            throw ReturnCodeException(1);
        }

        switch (csvParser::MFlags[flag].types.at(i - 1)) {
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
                    std::cerr << (("[!]Value is not an integer or out of range for flag " + flag.substr(1)).c_str()) << std::endl;
                    throw ReturnCodeException(1);
                }
                break;

            case ETypeValue::NONE:
                break;

            default:
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
    if (ArgumentsParser::extra_s && MFlagsMap.find("-s") == MFlagsMap.end()) {
        std::cerr << "[!]Didn't provided -s flag which is needed for some of your operations." << std::endl;
        throw ReturnCodeException(1);
    }
    else if (MFlagsMap.find("-s") != MFlagsMap.end() && (MFlagsMap.size() == 1 || !ArgumentsParser::extra_s)) { //Used -s alone or when not needed
        std::cerr << "[!]Provided -s flag when not needed for any of your operations" << std::endl;
        throw ReturnCodeException(1);
    }
    else if (ArgumentsParser::extra_s) {
        if ((*MFlagsMap.find("-s")).second.size() == 1 && ((*MFlagsMap.find("-s")).second.at(0).v_integer >= MIN_VALUE_FOR_S && (*MFlagsMap.find("-s")).second.at(0).v_integer <= MAX_VALUE_FOR_S)) {
            this->SFlagValue = (*MFlagsMap.find("-s")).second.at(0).v_integer;
            this->functionArr[csvParser::MFlags["-s"].function_index]((*MFlagsMap.find("-s")).first,(*MFlagsMap.find("-s")).second);
            MFlagsMap.erase("-s"); //Delete the s flag because it's function already ran
        }
        else {
            std::cerr << "[!]Value out of bounds for flag -s" << std::endl;
            throw ReturnCodeException(1);
        }
        
    }

    for (auto index : MFlagsMap) {
        this->functionArr[csvParser::MFlags[index.first].function_index](index.first,index.second);
    }
}

char* csvParser::trim(char* str)
{
    size_t start = strspn(str, " ");

    // Remove trailing spaces
    size_t end = strlen(str);
    while (end > 0 && str[end - 1] == ' ') {
        end--;
    }

    // Terminate the string
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

    while (std::getline(readCSV, line)) {
        if (line.length() > 2 && line.at(0) != '/' && line.at(1) != '/') { //Not a comment
            part = trim(strtok_s(const_cast<char*>(line.c_str()), ",", &token)); //Cutting the line
            flag = "-";
            flag += part;
            index++;

            //Does need s flag extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "extra_s");
            MFlags[flag].extra_s = checkBool(part, flag);

            //Number of values extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "numberOfValues");
            MFlags[flag].numberOfValues = checkInt(part, flag, 0);

            for (int i = 0; i < MFlags[flag].numberOfValues; i++) {
                //Value type extraction
                part = trim(strtok_s(NULL, ",", &token));
                checkNull(part, flag, "valueType");
                MFlags[flag].types.push_back(switchValueType(part, flag));
            }
            
            //Help message extraction
            part = trim(strtok_s(NULL, ",", &token));
            checkNull(part, flag, "helpMessage");
            MFlags[flag].helpMessage = part;

            //Function index
            MFlags[flag].function_index = index;
        }
        
        
    }

    //Adding s flag manullly
    MFlags["-s"].extra_s = false;
    MFlags["-s"].numberOfValues = 1;
    MFlags["-s"].types.push_back(INTEGER);
    MFlags["-s"].helpMessage = "DESC - Choose a number 1 or 2"; //Example operation, change as require
    MFlags["-s"].function_index = ++index;



}

void csvParser::printAllFlags()
{
    /*
    Printing information about all flags extracted from csv
    */
    std::cout << "[*]-h / --help: Prints documantiaion of the flags" << std::endl; //Prints help flag
    for (auto index : MFlags) {
        if (index.first != "-h") { //Dont print help flag again
            std::cout << "[*]" << index.first << ": " << index.second.helpMessage
                << ", Function index: " << index.second.function_index  << ", Number of values: " << index.second.numberOfValues;
            for (int i = 0; i < index.second.numberOfValues; i++) {
                std::cout << ", Type for value number " << i + 1 << ": " << MTypeValueForPrint[index.second.types.at(i)];
            }
            std::cout << std::endl << std::endl;
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
    Checking if a part is null and printing missing flag property
    input: char* part, std::string& flag, std::string msg
    output: none
    */
    if (part == NULL) {
        std::cerr << (("[!]Missing flag property " + flag.substr(1)).c_str()) << ", Message: Missing property " << msg << std::endl;
        throw ReturnCodeException(1);
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
        if (temp == 0 && part[0] != '0') {
            throw std::exception();
        }
    }
    catch (...) {
        std::cerr << (("[!]Value is not an integer for flag " + flag.substr(1)).c_str()) << std::endl;
        throw ReturnCodeException(1);
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
        std::cerr << "[!]Value for flag " << flag.substr(1) << " must be greater than " << minValue << " and be smaller than " << maxValue << ": " << part << std::endl;
        throw ReturnCodeException(1);
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
        std::cerr << "[!]Value for flag " << flag.substr(1) << " must be greater than " << minValue << ": " << part << std::endl;
        throw ReturnCodeException(1);
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
    bool result = !(!strcmp(part, "FALSE") || !strcmp(part, "false"));
    if (strcmp(part, "true") && strcmp(part, "false") && strcmp(part, "TRUE") && strcmp(part, "FALSE") && !(strlen(part) == 1 && (*part == '0' || *part == '1'))) { //Input is not TRUE and is not FALSE
        std::cerr << "[!]Value must be TRUE or FALSE or true or false or 0 or 1 for flag " << flag.substr(1) << std::endl;
        throw ReturnCodeException(1);
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
        std::cerr << "[!]CSV file error: " << std::endl << (("Value must be BOOLEAN or INTEGER or STR for flag " + flag.substr(1)).c_str()) << std::endl;
        throw ReturnCodeException(1);
    }
    return returnValue;
}


