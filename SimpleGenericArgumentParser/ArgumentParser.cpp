#include "ArgumentParser.hpp"

bool ArgumentsParser::extra_s = false;
int ArgumentsParser::SFlagValue = 0;

std::map<std::string, SFlagProperties> csvParser::s_MFlags;

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
        csvParser::s_findFlags(); //Load flags from csv

        ArgumentsParser analyzer(argc, argv, functionArray, size);

        if (analyzer.shouldPrintHelp()) {
            if (argc > NUM_OF_PARAMS_FOR_HELP_IN_ARGC) { 
                throw std::runtime_error("[!] Flag h must be used alone. Please remove any additional flags or arguments.");
            }
            std::cout << "[*] Usage: " << std::endl << "[*] " << argv[0] << " <-flag> <value> <value2> ... <-flag>" << std::endl << "[*] " << argv[0] << csvParser::s_formatFlagsForPrint() << std::endl << "[*] Options: " << std::endl;
            csvParser::s_printAllFlags();
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

            if (csvParser::s_MFlags.find(argv[i]) == csvParser::s_MFlags.end() && !(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help") || !strcmp(argv[i], "-s"))) { //Flag does not exists
                throw std::runtime_error("[!] Flag " + (std::string)(argv[i] + 1) + " does not exists.");
            }

            if (!(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "-s"))) { //Flag is not a special flag
                try {
                    ArgumentsParser::extra_s = ArgumentsParser::extra_s | csvParser::s_MFlags.at(argv[i]).extra_s;
                }
                catch (...) {
                    throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
                }
            }


            this->MFlagsMap[argv[i]] = this->extractValue(argv[i], i, argc, argv);
            if (!(strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0)) {
                try {
                    i += csvParser::s_MFlags.at(argv[i]).numberOfValues;
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
                for (auto type : csvParser::s_MFlags.at(index.first).types) {
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
            for (int i = 1; i <= csvParser::s_MFlags.at(flag).numberOfValues; i++) {
                UValuesUnion flagValue;
                if (index + i == argc || argv[index + i][0] == '-') { //The last index or the next index is a flag and not a value
                    throw std::runtime_error((("[!] Missing value for flag " + flag.substr(1)).c_str()));
                }

                switch (csvParser::s_MFlags.at(flag).types.at(i - 1)) {
                case ETypeValue::INTEGER:
                    hex_base = false;
                    if (strlen(argv[index + i]) >= MIN_LENGTH_FOR_HEX_NUMBER) {
                        if (argv[index + i][0] == '0' && argv[index + i][1] == 'x')
                            hex_base = true;
                    }
                    flagValue.v_integer = csvParser::s_checkInt(argv[index + i], flag, hex_base);
                    break;

                case ETypeValue::STR:
                    flagValue.v_str = new char[strlen(argv[index + i]) + 1];
                    strcpy_s(flagValue.v_str, strlen(argv[index + i]) + 1, argv[index + i]);
                    break;

                case ETypeValue::BOOLEAN:
                    try {
                        flagValue.v_bool = csvParser::s_checkBool(argv[index + i], flag);
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
                this->functionArr[csvParser::s_MFlags.at("-s").function_index]((*MFlagsMap.find("-s")).first, (*MFlagsMap.find("-s")).second);
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
            this->functionArr[csvParser::s_MFlags.at(index.first).function_index](index.first, index.second);
        }
        catch (...) {
            throw std::runtime_error("[!] Error occurred while accessing flag information - the key does not exist.");
        }
    }
}
