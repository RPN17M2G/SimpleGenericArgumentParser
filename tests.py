import unittest
import subprocess
import random
from colorama import Fore

MIN_ROW_LENGTH = 4

def parse_csv(file_path):
    '''
    Parsing csv file into 2d list
    '''
    data = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # If the line is a comment, skip it
            if line.startswith('//'):
                continue
            row = line.strip().split(',')
            data.append(row)
    return data

class TestProgram(unittest.TestCase):
    def setUp(self):
        '''
        Initiating the paramaters for the test
        '''
        self.csv_file = 'flags.csv' 
        #Error messages - not all of the error messages are present because of the use of certin flags in some error messages
        self.error_messages = ["[!] The -s flag was provided when it was not needed for any of your operations.", "[!] The -s flag, which is required for some of your operations, was not provided.", "[!] Missing value for flag s"]  
        self.executable = './ArgumentParser.exe'  # replace with your executable path
        self.expectedOutput = True
        self.which_error = ""

    def generate_value(self, value_type):
        '''
        Generating random value for testing
        '''
        if value_type.lower() == 'integer':
            return str(random.randint(1, 100))
        elif value_type.lower() == 'boolean':
            return str(random.choice(["TRUE", "FALSE","false","true","0","1"]))
        else:
            return 'strtest'

    def test_program(self):
        '''
        Testing the program with expected and unexpected values and flags
        '''
        test_pass, test_failed = 0, 0
        for i in range(1):
            for row in parse_csv(self.csv_file):
                row = list(i.strip() for i in row )
                self.expectedOutput = True
                self.which_error == ""
                if len(row) > MIN_ROW_LENGTH: 
                    test_args = [self.executable]
                    if random.random() > 0.1:  # 1/10 chance to not append -s flag
                        test_args.append('-s')
                        if row[1] == "FALSE":
                            self.expectedOutput = False
                            self.which_error = self.error_messages[0]
                        if random.random() > 0.003:  # 1/300 chance to not append any value
                            test_args.append(str(random.randint(1, 2)))
                        else:
                            self.which_error = self.error_messages[2]
                            self.expectedOutput = False
                    else:
                        self.which_error = self.error_messages[1]
                        self.expectedOutput = False

                    test_args.append('-' + row[0]) #Append flag

                    num_values = int(row[2])
                    types_of_values = row[3:3 + num_values] #Get type of values
                    for value_type in types_of_values:
                        if random.random() < 0.1:  # 1/10 chance to append wrong value type
                            test_args.append('wrong_value')
                            self.expectedOutput = False
                            self.which_error = ""
                        else:
                            test_args.append(self.generate_value(value_type))

                    result = subprocess.run(test_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    outputOut = result.stdout.strip().decode('utf-8')
                    outputErr = result.stderr.strip().decode('utf-8')
                    output = outputOut if outputErr == "" else outputErr #Choose between error message and output
                    output = output.replace("\r\n","") #Remove hidden characters

                    value_args = test_args[1:]

                    #In the test function, output from type boolean shows as a number - 1 for true and 0 for false
                    for i, value in enumerate(value_args):
                        if value == "TRUE" or value == "true":
                            value_args[i] = "1"
                        elif value == "FALSE" or value == "false":
                            value_args[i] = "0"

                    try:
                        #check output
                        self.assertEqual(output, f",{','.join(value_args)}")
                        print(Fore.GREEN + "Test passed for flags: ", test_args, "     Output is: ", output, "\n")
                        test_pass += 1
                    except:
                        if (not self.expectedOutput and (output == self.which_error or self.which_error == "")): #if the input is known to cause error(by purpose) 
                            print(Fore.GREEN + "Test passed for flags: ", test_args, "     Output is: ", output, "\n")
                            test_pass += 1
                        else: #Unexpected mismatch
                            print(Fore.RED + "Test failed for flags: ", test_args, " Output is:", repr(output), " Expected value is: ", repr(f",{','.join(value_args)}"),  "\n")
                            test_failed += 1
        
        green_or_red = Fore.RED if test_pass / (test_failed + test_pass) < 0.5 else Fore.GREEN
        print(f"\n{green_or_red}Test results: {(test_pass / (test_failed + test_pass)) * 100}%\n{Fore.RED}Number of tests failed: {test_failed}, which is {(test_failed / (test_failed + test_pass)) * 100}% out of all tests.\n{Fore.GREEN}Number of tests passed: {test_pass}, which is {(test_pass / (test_failed + test_pass)) * 100}% out of all tests.\n{Fore.WHITE}")


if __name__ == '__main__':
    unittest.main()
