#include "Tests.hpp"
#include "ArgumentParser.hpp"

int main(int argc, char* argv[]) 
{
    ArgumentFunc functionArray[] = { func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, func_test, s_func };
    int arrayLength = 17;
    int returnCode = ArgumentsParser::init(argc, argv, functionArray, arrayLength);
    return returnCode;
}
