/*******************************************************************************
 * Name        : death_assembler.cpp
 * Author      : Alexander Jansiewicz & Clare O'Brien
 * Version     : 1.0
 * Date        : 12/5/2022
 * Description : Converts an .asm file into image files to be used by the Death Machine.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

string convert_immediate(string &input) {
    string str = "";
    if (input[0] == '-') {
        str += input[0];
        for (size_t i = 1; i < input.size() - 1; i++) {
            if (i >= 5) {
                if ((input[i] == ']') || (input[i] == '\n') || (input[i] == '\t')) {
                    break;
                } else {
                    return "ERROR";
                }
            } else {
                if (isdigit(input[i])) {
                    str += input[i];
                } else if ((input[i] == ']') || (input[i] == '\n') || (input[i] == '\t')) {
                    break;
                } else {
                    return "ERROR";
                }
            }
        }
    } else {
        for (size_t i = 0; i < input.size() - 1; i++) {
            if (i >= 4) {
                if (input[i] == ']') {
                    break;
                } else {
                    return "ERROR";
                }
            } else {
                if (isdigit(input[i])) {
                    str += input[i];
                } else if (input[i] == ']') {
                    break;
                } else {
                    return "ERROR";
                }
            }
        }
    }

    int num = stoi(str);
    if ((num < -2048) && (num > 2047)) {
        return "ERROR";
    }
    str = bitset<12>(num).to_string();
    return str;
}

string convert_register(string &register_data){
	if (register_data == "0"){
		return "000";
	} else if (register_data == "1"){
		return "001";
	} else if (register_data == "2"){
		return "010";
	} else if (register_data == "3"){
		return "011";
	} else if (register_data == "4"){
		return "100";
	} else if (register_data == "5"){
		return "101";
	} else if (register_data == "6"){
		return "110";
	} else if (register_data == "7"){
		return "111";
	} else {
		return "ERROR";
	}
}

// GENERATES MACHINE CODE FOR ADD OR SUB INSTRUCTION
string generateADDorSUB(string &line_text){
	string opcode;
	if (line_text[0] == 'a'){
		opcode = "000";
	} else {
		opcode = "001";
	}

	string targetstr = line_text.substr(5,1);
	string oprnd1str = line_text.substr(9,1);
	string oprnd2str = line_text.substr(13,1);

	string target = convert_register(targetstr);
	string oprnd1 = convert_register(oprnd1str);
	string imm12 = "000000000000";
	string oprnd2 = convert_register(oprnd2str);
	if ((target == "ERROR") || (oprnd1 == "ERROR") || (oprnd2 == "ERROR")) {
		return "ERROR";
	}

	return opcode + target + oprnd1 + imm12 + oprnd2;
}

// GENERATES MACHINE CODE FOR BL INSTRUCTION
string generateBL(string &line_text) {
	string opcode = "100";
	string target = "110";
	string oprnd1 = "000";
	string immediate = line_text.substr(3);
    string imm12 = convert_immediate(immediate);
	string oprnd2 = "000";
    if ((target == "ERROR") || (oprnd1 == "ERROR") || (oprnd2 == "ERROR") || (imm12 == "ERROR")) {
		return "ERROR";
	}
	return opcode + target + oprnd1 + imm12 + oprnd2;
}

// GENERATES MACHINE CODE FOR CBZ INSTRUCTION
string generateCBZ(string &line_text) {
	string opcode = "110";

	string targetstr = line_text.substr(5,1);
    string immediate = line_text.substr(8);

	string target = convert_register(targetstr);
	string oprnd1 = "000";
    string imm12 = convert_immediate(immediate);			// ignore whitespace??? figure this out
	string oprnd2 = "000";
	if ((target == "ERROR") || (oprnd1 == "ERROR") || (oprnd2 == "ERROR") || (imm12 == "ERROR")) {
		return "ERROR";
	}
	return opcode + target + oprnd1 + imm12 + oprnd2;
}

// GENERATES MACHINE CODE FOR CBNZ INSTRUCTION
string generateCBNZ(string &line_text) {
	string opcode = "111";

	string targetstr = line_text.substr(6,1);
    string immediate = line_text.substr(9);

	string target = convert_register(targetstr);
	string oprnd1 = "000";
	string imm12 = convert_immediate(immediate);				// ignore whitespace??? figure this out
	string oprnd2 = "000";

	if ((target == "ERROR") || (oprnd1 == "ERROR") || (oprnd2 == "ERROR") || (imm12 == "ERROR")) {
		return "ERROR";
	}
	return opcode + target + oprnd1 + imm12 + oprnd2;
}

// GENERATES MACHINE CODE FOR LDR OR STR INSTRUCTION
string generateLDRorSTR(string &line_text) {
	string opcode;
	if (line_text[0] == 'l'){
		opcode = "010";
	} else {
		opcode = "011";
	}

	string targetstr = line_text.substr(5,1);
	string oprnd1str = line_text.substr(10,1);
    string immediate = line_text.substr(13);

	string target = convert_register(targetstr);
	string oprnd1 = convert_register(oprnd1str);
	string imm12 = convert_immediate(immediate);

	string oprnd2 = "000";
	if ((target == "ERROR") || (oprnd1 == "ERROR") || (oprnd2 == "ERROR") || (imm12 == "ERROR")) {
		return "ERROR";
	}
	return opcode + target + oprnd1 + imm12 + oprnd2;
}

// GENERATES MACHINE CODE FOR RET INSTRUCTION
string generateRET(string &line_text) {
	return "101110000000000000000000";
}

int binary_to_hex(string &input) {
    return stoi(input, 0, 2);
}

int main(int argc, char *argv[]) {
    // TODO: Parse arguments through
    // ./death_assembler file_name
    // Dummy instruction after program completion: ADD XZR, XZR, XZR 
    ifstream reading;
    istringstream iss;
    ofstream output1("instruction.txt"); // For .text
    ofstream output2("data.txt"); // For .data
    string line_text;
    vector<string> instructions;
    vector<string> data;

    if (argc != 1) {
        cout << "Usage: ./death_assembler   |   Only works if \"death_assembly.txt\" is present in the same directory." << endl;
        return 1;
    }
    
    reading.open("death_assembly.txt", ios::in);

    if (!reading) {
        cerr << "Error: \"death_assembly.txt\" not found. Did you misspell the file name?" << endl;
        return 1;
    }

    cout << "File inputted successfully. Reading..." << endl;

    while (getline (reading, line_text)) {
        // READ THE LINE CHARACTER BY CHARACTER, CONVERT TO LOWERCASE, AND THEN GENERATE MACHINE CODE
        if (line_text.length() == 1) {
            continue;
        }

        if ((line_text.find(".data:") >= 0) && (line_text.find(".data:") < line_text.size())) {
            cout << "Interpreting .data section..." << endl;
            while (getline (reading, line_text)) {
                string hex = "";
                for (int i = 0; i < 4; i++) {
                    if (isdigit(line_text[i]) || 
                            (line_text[i] == 'a') ||
                            (line_text[i] == 'b') ||
                            (line_text[i] == 'c') ||
                            (line_text[i] == 'd') ||
                            (line_text[i] == 'e') ||
                            (line_text[i] == 'f')) {
                        hex += line_text[i];
                    } else {
                        cerr << "Error: Invalid .data number entered." << endl;
                        return 1;
                    }
                }
                data.push_back(hex);
            }
            break;
        }
        
        string machine_code;
        cout << "Interpreting: " << line_text << endl;
        transform(line_text.begin(), line_text.end(), line_text.begin(), ::tolower);

        // CHECK FOR ADD INSTRUCTION
        if (line_text[0] == 'a'){
        	if ((line_text[1] == 'd') && (line_text[2] == 'd')){
        		machine_code = generateADDorSUB(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
        	} else {
        		cerr << "Error: Invalid Instruction Detected." << endl;
        		return 1;
        	}
        }

        // CHECK FOR BL INSTRUCTION
        if (line_text[0] == 'b'){
            if (line_text[1] == 'l'){
            	machine_code = generateBL(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
            } else {
                cerr << "Error: Invalid Instruction Detected." << endl;
                return 1;
            }
        }

        // CHECK FOR CBZ OR CBNZ INSTRUCTION
        if (line_text[0] == 'c'){
            if (line_text[1] == 'b'){
            	if (line_text[2] == 'z') {
            		machine_code = generateCBZ(line_text);
					if (machine_code == "ERROR") {
						cerr << "Error: Invalid Instruction Detected." << endl;
        				return 1;
					}
            	} else if ((line_text[2] == 'n') && (line_text[3] == 'z')) {
            		machine_code = generateCBNZ(line_text);
					if (machine_code == "ERROR") {
						cerr << "Error: Invalid Instruction Detected." << endl;
        				return 1;
					}
            	} else {
            		cerr << "Error: Invalid Instruction Detected." << endl;
            		return 1;
            	}
            } else {
            	cerr << "Error: Invalid Instruction Detected." << endl;
            	return 1;
            }
        }

        // CHECK FOR LDR INSTRUCTION
        if (line_text[0] == 'l'){
        	if ((line_text[1] == 'd') && (line_text[2] == 'r')){
        		machine_code = generateLDRorSTR(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
            } else {
                cerr << "Error: Invalid Instruction Detected." << endl;
                return 1;
            }
        }

        // CHECK FOR RET INSTRUCTION
        if (line_text[0] == 'r'){
        	if ((line_text[1] == 'e') && (line_text[2] == 't')){
        		machine_code = generateRET(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
        	} else {
        		cerr << "Error: Invalid Instruction Detected." << endl;
                return 1;
        	}
        }

        // CHECK FOR STR OR SUB INSTRUCTION
        if (line_text[0] == 's'){
        	if ((line_text[1] == 't') && (line_text[2] == 'r')){
        		machine_code = generateLDRorSTR(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
            } else if ((line_text[1] == 'u') && (line_text[2] == 'b')) {
            	machine_code = generateADDorSUB(line_text);
				if (machine_code == "ERROR") {
					cerr << "Error: Invalid Instruction Detected." << endl;
        			return 1;
				}
            } else {
            	cerr << "Error: Invalid Instruction Detected." << endl;
            	return 1;
            }
        }
        cout << "     Interpreted as: " << machine_code << endl;
        instructions.push_back(machine_code);
    }
    
    // Finished with reading input file
    reading.close();

    // HANDLING .text
    output1 << "v3.0 hex words addressed" << endl;
    int limit = 0;
    size_t size = 0;
    while (limit <= 65520) {
        output1 << setw(4) << setfill('0') << hex << limit << ":";
        for (int i = 0; i < 8; i++) {
            if (size < instructions.size()) {
                output1 << " " << setw(6) << setfill('0') << hex << binary_to_hex(instructions[size]);
                size++;
            } else {
                output1 << " 1f8007";
            }
        }
        output1 << endl;
        limit += 8;
    }
    output1 << endl;
    output1.close();

    // HANDLING .data    
    output2 << "v3.0 hex words addressed" << endl;
    limit = 0;
    size = 0;
    while (limit <= 65520) {
        output2 << setw(4) << setfill('0') << hex << limit << ":";
        for (int i = 0; i < 16; i++) {
            if (size < data.size()) {
                output2 << " " << data[size];
                size++;
            } else {
                output2 << " 0000";
            }
        }
        output2 << endl;
        limit += 16;
    }
    output2 << endl;
    output2.close();
    cout << "Memory files successfully outputted!" << endl;
    return 0;
}