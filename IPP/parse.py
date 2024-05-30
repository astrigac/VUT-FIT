# IPP 2023/24
# Project 1 - Parser of IPPcode24
# Author: Aurel Strigac <xstrig00>

import sys
import xml.etree.ElementTree as ET  # For building and manipulating XML trees
import xml.dom.minidom              # For pretty-printing XML strings
import re                           # For regular expressions

# Function used for displaying the help message
def print_help():
    help_message = """
//////////////////////////////////////////////////////////////////////////////////////

IPP Project 2024 - Part 1: parse.py
    
//////////////////////////////////////////////////////////////////////////////////////

DESCRIPTION:
   Filtring script that reads the source code in IPP-code24 from the standard input, 
   checks the lexical and syntactical correctness of the code, and prints out the XML 
   representation of the program according to the specification.

USAGE:
   Command to run the script:   python3.10 parse.py [--help]

OPTIONS:
   --help      Display this help message.

ERROR CODES:
    21 - Incorrect or missing header in the source code written in IPPcode24.
    22 - Unknown or invalid opcode in the source code written in IPPcode24.
    23 - Other lexical or syntactic errors in the source code written in IPPcode24.

For more information, visit the official documentation (readme1.pdf).
"""
    print(help_message)

#//////////////////////////////////////////////     PARSING    //////////////////////////////////////////////

header = ".IPPCODE24"

# Global regular expressions used for argument parsing
label_regex = re.compile(r'^([a-zA-Z]|_|-|\$|&|%|\*|!|\?)([a-zA-Z0-9]|_|-|\$|&|%|\*|!|\?)*$')  
var_regex = re.compile(r'^(GF|TF|LF)@([a-zA-Z]|_|-|\$|&|%|\*|!|\?)([a-zA-Z0-9]|_|-|\$|&|%|\*|!|\?)*$')
int_hex_regex = re.compile(r'^0[xX][0-9a-fA-F]+(_[0-9a-fA-F]+)*$')
int_oct_regex = re.compile(r'^0[oO]?[0-7]+(_[0-7]+)*$')
escape_seq_regex = re.compile(r'\\[0-9]{3}')
    
@staticmethod
def check_symb_syn(token):
    """
    Method used for checking if the symbol given is syntactically correct.

    :param token: Token to be checked.
    """
    parts = token.split("@", 1)

    if len(parts) < 2:
        sys.stderr.write(f"ERROR: Invalid symbol format {token}.\n")
        sys.exit(23)

    type_, val = parts

    # First, check if the symbol is not a variable
    if not var_regex.match(token):
        # Check for booleans
        if type_ == "bool" and val in ("true", "false"):
            return
        # Check for nil type
        elif type_ == "nil" and val == "nil":
            return
        # Check for integers
        elif type_ == "int":
            if val.isdigit() or int_hex_regex.match(val) or int_oct_regex.match(val) or val:
                return
            else:
                sys.stderr.write(f"ERROR: Invalid integer symbol {token}.\n")
                sys.exit(23)
        # Check for strings
        elif type_ == "string":
            if "\\" in val:
                num_of_backslashes = val.count("\\")
                regex_matches = len(escape_seq_regex.findall(val))

                if num_of_backslashes != regex_matches:
                    sys.stderr.write(f"ERROR: Invalid escape sequence in {token}.\n")
                    sys.exit(23)
            return

        else:
            sys.stderr.write(f"ERROR: Invalid symbol type or value {token}.\n")
            sys.exit(23)
    else:
        # Symbol is a variable, considered correct
        return
    
@staticmethod
def check_length(instruction, number, expected):
    """
    Method used for comparing the expected number of argument with the actual number according to IPPcode24

    :param number: Actual number of arguments.
    :param expected: Expected number of arguments.
    """
    if number != expected:
        sys.stderr.write(f"ERROR: Wrong number of arguments for the {instruction} instruction.\n")
        sys.stderr.write(f"    EXPECTED: {expected}\n")
        sys.stderr.write(f"    ACTUAL: {number}\n")
        sys.exit(23)
  
@staticmethod    
def check_type_syn(token):
    """
    Method used for checking if the type given is correct according to IPPcode24.

    :param token: Token to be checked.
    """
    if token not in ["int", "bool", "string", "nil"]:
        sys.stderr.write(f"ERROR: Invalid type {token}!\n")
        sys.exit(23)
     
@staticmethod    
def check_var_syn(token):
    """
    Method used for checking if the variable given is syntactically correct according to IPPcode24.

    :param token: Token to be checked.
    """
    if not var_regex.match(token):
        sys.stderr.write(f"ERROR: Invalid variable {token}!\n")
        sys.exit(23)
    
@staticmethod    
def check_label_syn(token):
    """
    Method used for checking if the label given is syntactically correct according to IPPcode24.

    :param token: Token to be checked.
    """
    if not label_regex.match(token):
        sys.stderr.write(f"ERROR: Invalid label {token}!\n")
        sys.exit(23)
        

#//////////////////////////////////////////////     XML GENERATING    //////////////////////////////////////////////   
    
@staticmethod
def XML_initialize():
    """
    Method used for initialization of the XML tree.
    
    Returns an XML ElementTree with root element set for IPPcode24.
    """
    
    root = ET.Element("program")
    root.set('language', 'IPPcode24')
    return ET.ElementTree(root), root

    
@staticmethod
def XML_write_arg(root, num, token): 
    """
    Method used for insertion of the XML representation of an argument into the XML tree.

    :param root: The parent element to attach this argument to.
    :param num: The number of the token in operation arguments.
    :param token: The token to be written.
    """
        
    # Determine type and value based on the token
    if "@" in token:
        parts = token.split("@")
        type_ = parts[0]
        if type_ in ["GF", "TF", "LF"]:
            type_ = "var"
            value = token
        elif type_ in ["int", "bool", "string", "nil"]:
            value = parts[1]
    else:
        if token in ["int", "bool", "string", "nil"]:
            type_ = "type"
        else:
            type_ = "label"
        value = token
    
    # Create and append the argument element to the XML
    arg_element = ET.SubElement(root, f"arg{num}")
    arg_element.set("type", type_)
    arg_element.text = value

@staticmethod
def XML_body(lines, xml_tree, root):
    """
    Method used for processing IPPcode24 instructions from lines, creating corresponding 'instruction' elements in the XML tree. 
    Each instruction is analyzed for its syntax and arguments. Appropriate checks are performed based on the instruction type, 
    including validation for variables, labels, symbols, and types while utilizing specific checking methods. 
    Arguments are appended as child elements to their respective instruction elements, and each instruction is sequentially numbered.

    :param lines: A list of strings, each representing a line of IPPcode24 code. The first line (header) is expected to be already verified.
    :param xml_tree: An ElementTree object that represents the entire XML document.
    :param root: The root element of the XML document to which instruction elements will be appended. This should be an Element object that is the root of `xml_tree`.
    """
    # Variable used for numbering the instructions in xml file
    instruction_order = 1

    # Iterate through the instructions while skipping the header
    for line in lines[1:]: 
        # Create a new 'instruction' element
        instruction_element = ET.SubElement(root, 'instruction')
    
        # Set the 'order' attribute of the 'instruction' element
        instruction_element.set('order', str(instruction_order))
    
        tokens = line.split()  # Splits the line into tokens
        instruction = tokens[0].upper()  # Converts the first token (command, most likely) to uppercase
        instruction_element.set('opcode', instruction)

        # Watch out for multiple headers 
        if instruction == header:
            sys.stderr.write("ERROR: Multiple headers.\n")
            sys.exit(23)

        # Check the syntax of the instruction
        elif instruction in ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]:
            # These instructions don't need any arguments
            # No need for any other check than the number of tokens
            check_length(instruction, len(tokens), 1)
            pass

        elif instruction in ["DEFVAR", "POPS"]:
            # These instructions require 1 argument, a variable
            check_length(instruction, len(tokens), 2)
            check_var_syn(tokens[1])
            pass
    
        elif instruction in ["CALL", "LABEL", "JUMP"]:
            # These instructions require 1 argument, a label
            check_length(instruction, len(tokens), 2)
            check_label_syn(tokens[1])
            pass
    
        elif instruction in ["PUSHS", "WRITE", "EXIT", "DPRINT"]:
            # These instructions require 1 argument, a symbol
            check_length(instruction, len(tokens), 2)
            check_symb_syn(tokens[1])
            pass

        elif instruction in ["MOVE", "NOT", "INT2CHAR", "STRLEN", "TYPE"]:
            # These instructions require 2 arguments, a variable followed by a symbol
            check_length(instruction, len(tokens), 3)
            check_var_syn(tokens[1])
            check_symb_syn(tokens[2])
            pass
    
        elif instruction in ["READ"]:
            # This instruction requires 2 arguments, a variable followed by a type
            check_length(instruction, len(tokens), 3)
            check_var_syn(tokens[1])
            check_type_syn(tokens[2])
            pass

        elif instruction in ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR"]:
            # These instructions require 3 arguments, a variable followed by 2 symbols
            check_length(instruction, len(tokens), 4)
            check_var_syn(tokens[1])
        
            for token in tokens[2:4]:  # Include tokens[2] and tokens[3]
                check_symb_syn(token)
            pass
    
        elif instruction in ["JUMPIFEQ", "JUMPIFNEQ"]:
            # These instructions require 3 arguments, a label followed by 2 symbols
            check_length(instruction, len(tokens), 4)
            check_label_syn(tokens[1])
        
            for token in tokens[2:4]:  # Include tokens[2] and tokens[3]
                check_symb_syn(token)
            pass

        else:
            # No instruction found
            sys.stderr.write(f"ERROR: Invalid instruction {instruction}.\n")
            sys.exit(22)
    
        # Write the instruction arguments into the xml tree (if needed)
        if len(tokens) > 1:
            for i, token in enumerate(tokens[1:], start=1):  # Start enumerating from 1 to match arg numbering and skip instruction
                XML_write_arg(instruction_element, i, token)
        
        # Increment the instruction order for the next instruction
        instruction_order += 1

    
@staticmethod
def XML_finish(xml_tree):
    """
    Method used for finishing up the XML and writing it to stdout with XML declaration including encoding.
    
    :param xml_tree: An ElementTree object representing the root of the XML tree to be processed and printed.
    """
        
    # Convert the XML tree to a byte string without pretty printing
    xml_str = ET.tostring(xml_tree.getroot(), encoding='unicode', xml_declaration=True)
    
    # Use xml.dom.minidom to pretty-print the XML string with tab indentation
    pretty_xml_as_string = xml.dom.minidom.parseString(xml_str).toprettyxml(indent="\t")

    # Prepare the final pretty-printed XML string with a correct XML declaration
    my_declaration = '<?xml version="1.0" encoding="UTF-8"?>\n'
    pretty_xml_without_declaration = '\n'.join(pretty_xml_as_string.split('\n')[1:])
    final_pretty_xml= my_declaration + pretty_xml_without_declaration

    print(final_pretty_xml)

#//////////////////////////////////////////////     MAIN    //////////////////////////////////////////////

if __name__ == "__main__":
    # Check if '--help' is in the list of command-line arguments
    if "--help" in sys.argv[1:]:
        print_help()
        sys.exit(0)
    else:
        # If there are arguments, but '--help' is not one of them
        if len(sys.argv) > 1:
            print("ERROR: Unsupported argument(s).", file=sys.stderr)
            sys.exit(10)

# Read the input from stdin
input = sys.stdin.read()

if not input.strip():
    sys.stderr.write("ERROR: Empty input.\n")
    sys.exit(11)

# Split the input into lines  
lines = input.splitlines()

# Initialization of an XML file
xml_tree, root = XML_initialize()

# Clean up each line from multiple spaces and comments
lines = [
    re.sub(' +', ' ', re.sub('#.*', '', line)).strip() 
    for line in lines if line.strip() and not re.match('^#', line.strip())
]

# Remove empty lines
lines = [line for line in lines if line]

if not lines or lines[0].upper() != header:
    sys.stderr.write("ERROR: Invalid or missing header.\n")
    sys.exit(21)

#Construct the already initialized XML tree
XML_body(lines, xml_tree, root)

# Print the XML file to stdout  
XML_finish(xml_tree)  
