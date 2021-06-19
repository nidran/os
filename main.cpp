#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <unistd.h>
#include "tokeniser.cpp"
using namespace std;

struct warning1{
    int module;
    string token;
    int size;
    int maxsize;
};
struct warning2{
    int module;
    string token;

};
vector<string> allUseList;
vector<string> curUseList;
vector<string> tokens;
vector<int> tknln;
int ln;
vector<int> tknoff;
map<string, int> symbolTable;
vector<string> isSymUsed;
map<string, bool> isSymDef;
vector<string> seenSym;
vector<string> duplicateSym;
map<string, int> stringCase;
map<int, int> memoryMap;
map<int, string> memoryMapErr;
vector<string> warnings;
vector<warning1> warning1;
vector<warning2> warning2;
void checkGlobalOffset(int idx);
bool isNum(string tokn);
bool isValidAddrMode(string token);

void getToken(string fileName, vector <string> &tokens, vector<int> &tknln, vector<int> &tknoff);

void passOne();

void passTwo();

int memMap = 0;
int pass2uc = 0;
int moduleAddr = 0;
void printWarning1();
void printMemoryMap();

void parserError(int errCode, int idx);

int updateDefList(vector<string> curTokens, int start);

int updateUseList(vector<string> curTokens, int start);

int updateInstList(vector<string> curTokens, int start);

void initialiseErrMsg();

void reset();
void checkIfSymbOutofMod(int count);

void printSymbolTable(map<string, int> symbolTable, vector<string> duplicateSym);

int globalOffset;
int modules;
map<int, string> parseErrorMsg;
void sanityCheck(int argc,  string argv);

void printWarning2();

int main(int argc, const char *argv[]) {
    sanityCheck(argc, argv[1]);
    initialiseErrMsg();
    reset();
    getToken(argv[1], tokens, tknln, tknoff);
    passOne();
    printSymbolTable(symbolTable, duplicateSym);
    reset();
    getToken(argv[1], tokens, tknln, tknoff);
    passTwo();
    printMemoryMap();
    printWarning2();
}

void passOne() {
    int type = 1;
    int idx;
    int lclidx = 0;
    modules = 0;
    ln = 0;
    idx = 0;

    while (idx < tokens.size()) {
        if (type % 3 == 1) {
            lclidx = updateDefList(tokens, idx);
            idx = lclidx;
        } else if (type % 3 == 2) {
            lclidx = updateUseList(tokens, idx);
            idx = lclidx;
        } else {
            lclidx = updateInstList(tokens, idx);
            checkIfSymbOutofMod((lclidx-idx)/2);
            checkGlobalOffset( idx);
            idx = lclidx;
            modules++;
            printWarning1();
            warning1.clear();
            warnings.clear();
        }
        type++;
    }
}

void checkGlobalOffset(int idx){
    if (globalOffset > 512){
        parserError(6,idx)  ;
    }

}
void checkIfSymbOutofMod(int count) {
    map<string, int>::iterator it;
    for ( it = symbolTable.begin(); it!= symbolTable.end(); it++){
        if (it->second > (moduleAddr + count)){
            struct warning1 war;
            war.token =it->first;
            war.size =it->second - moduleAddr;
            war.maxsize = count -1;
            war.module = modules;

            warning1.push_back(war);


        }
    }

}

int updateDefList(vector<string> curTokens, int start) {
    int idx = start;
    int dc;
    int end = start + 2*dc;
    if (!isNum(curTokens.at(idx)))
        parserError(1, idx);
    dc = stoi(curTokens.at(idx));
    end+=start;
    if (dc > 16)
        parserError(5, idx);
    idx++;
    int lc = 1;
    if (idx >= curTokens.size()){
        parserError(2, idx-1);
        exit(0);
    }
    while (lc < 2 * dc + 1 && idx < end) {
        if (isNum(curTokens.at(idx))) {
            parserError(2, idx);
        }

        if (!isNum(curTokens.at(idx + 1))) {
            parserError(1, idx+1);
        }
        string curToken = curTokens.at(idx);
        int curTokenLoc = stoi(curTokens.at(idx + 1));

        if (find(seenSym.begin(), seenSym.end(), curToken) == seenSym.end()) {
            seenSym.push_back(curToken);
            isSymDef.insert(make_pair(curToken, false));
            symbolTable.insert(pair<string, int>(curToken, globalOffset + curTokenLoc));
        } else {
            duplicateSym.push_back(curToken);
        }
        lc += 2;
        idx += 2;
    }

    return idx;

}

int updateUseList(vector<string> curTokens, int start) {

    int idx = start;
    int uc;
    if (!isNum(curTokens.at(idx)))
        parserError(1, idx);
    uc = stoi(curTokens.at(idx));
    int end = start + uc;
    if (uc > 16)
        parserError(5, idx);
    idx++;
    int lc = 1;
    while (lc < uc + 1 && idx < end) {
        if (!isNum(curTokens.at(idx))) {
            lc++;

        } else {
            parserError(5, idx);
        }
    }

    return idx + uc;
}

int updateInstList(vector<string> curTokens, int start) {

    int idx = start;
    int ac;
    if (!isNum(curTokens.at(idx)))
        parserError(1, idx);
    ac = stoi(curTokens.at(idx));
    if (ac > 16)
        parserError(1, idx);
    ac = ac;
    idx++;
    globalOffset += ac;
    moduleAddr+=ac;
    int lc = 1;
    if (globalOffset > 512) {
        parserError(6, idx);
    }
    while (lc <= ac) {
        if (!isValidAddrMode(curTokens.at(idx)))
            parserError(3, idx);

        if (!isNum(curTokens.at(idx + 1)))
            parserError(1, idx);

        lc++;
        idx += 2;

    }
    return idx;
}

bool isNum(string tokn) {
    for (int i = 0; i < tokn.size(); i++) {
        if (!isdigit(tokn[i]))
            return false;
    }
    return true;
}

bool isValidAddrMode(string token) {
    if (token == "R" || token == "A" || token == "I" || token == "E")
        return true;
    return false;
}

void printSymbolTable(map<string, int> symbolTable, vector<string> duplicateSym) {
    cout << "Symbol Table" << endl;
    map<string, int>::iterator it;

    for (it = symbolTable.begin(); it != symbolTable.end(); it++) {
        if (find(duplicateSym.begin(), duplicateSym.end(), it->first) != duplicateSym.end()) {
            cout << it->first << "=" << it->second << " "
                 << "Error: This variable is multiple times defined; first value used" << endl;
        } else {
            cout << it->first << '='
                 << it->second
                 << endl;
        }
    }

}

int updatePass2Use(vector<string> curTokens, int start) {
//    pass2uc = 0;
    int idx = start;
    int uc;
    if (!isNum(curTokens.at(idx))) {
        exit(0);
    }
    uc = stoi(curTokens.at(idx));
    if (uc > 16) {
        exit(0);
    }
//    pass2uc= uc;
    idx++;
    curUseList.clear();
    int lc = 1;
    while (lc < uc + 1) {
        if (!isNum(curTokens.at(idx))) {
            lc++;
            string tkn = curTokens.at(idx);
            allUseList.push_back(tkn);
            curUseList.push_back(tkn);
        } else {
            parserError(4, idx);
        }

        idx++;
    }
    cout<<"use liset"<<curUseList.size()<<endl;
    return idx;
}

int updateIns2Use(vector<string> curTokens, int idx) {
    int addC = stoi(curTokens.at(idx));
    int i = 1;
    idx++;
    while (i <= addC) {
        string addMode = curTokens.at(idx);
        int value = stoi(curTokens.at(idx + 1));
        int opCode = value / 1000;
        int operand = value % 1000;
        stringCase.insert(make_pair("R", 1));
        stringCase.insert(make_pair("I", 2));
        stringCase.insert(make_pair("E", 3));
        stringCase.insert(make_pair("A", 4));

        switch (stringCase.at(addMode)) {
            case 1 :
                if (operand > (addC - 1)) {
                    memoryMap.insert(make_pair(memMap, opCode * 1000 + moduleAddr));
                    memoryMapErr.insert(make_pair(memMap, "Error: Relative address exceeds module size; zero used"));
                } else {
                    memoryMap.insert(make_pair(memMap, value + moduleAddr));
                }
                break;
            case 2 :
                if (value > 9999) {
                    memoryMap.insert(make_pair(memMap, 9999));
                    memoryMapErr.insert(make_pair(memMap, " Error: Illegal immediate value; treated as 9999"));
                } else if (opCode > 9) {
                    memoryMap.insert(make_pair(memMap, 9999));
                    memoryMapErr.insert(make_pair(memMap, "Error: Illegal opcode; treated as 9999"));

                } else {
                    memoryMap.insert(make_pair(memMap, value));

                }
                break;
            case 3 :
                            cout<<"In E"<<endl;
                            cout<<operand<<" "<<curUseList.size();

                if (operand > curUseList.size()) {
                    memoryMap.insert(make_pair(memMap, value));
                    memoryMapErr.insert(make_pair(memMap, " Error: External address exceeds length of uselist; treated as immediate"));

                } else {
//
                    if (symbolTable.find(allUseList[operand]) == symbolTable.end()) {
                        memoryMap.insert(make_pair(memMap, opCode * 1000));
                        memoryMapErr.insert(make_pair(memMap, " Error: " +allUseList[operand]+" is not defined; zero used"));


                    } else {
                        map<string, int>::iterator itr = symbolTable.find(curUseList[operand]);
                        memoryMap.insert(make_pair(memMap, opCode * 1000 + (itr->second)));
//                        isSymUsed.erase(curUseList[operand]);
//                        isSymUsed.erase(find(isSymUsed.begin(),isSymUsed.end(),curUseList[operand]));
//                        map<string>::iterator ptr = isSymUsed.find(curUseList[operand]);
//                        isSymUsed.erase(ptr);
//                        ptr->second = true;
//
//
                    }
                }

                break;
            case 4 :
//                                    cout << "in A" << endl;
                if (operand >= 512) {
                    memoryMap.insert(make_pair(memMap, opCode * 1000));
                    memoryMapErr.insert(make_pair(memMap, "Error: Absolute address exceeds machine size; zero used"));

                } else {
                    memoryMap.insert(make_pair(memMap, value));
//
                }

                break;
            default:
                cout << "none";
        }
        i += 1;
        idx += 2;
        memMap++;

    }
//    cout<<moduleAddr;
//    moduleAddr = moduleAddr + addC;
    return idx;

}

void passTwo() {
    moduleAddr=0;
    int type = 1;
    int idx = 0;
    int lclidx = 0;
    idx = 0;
    while (idx < tokens.size()) {
        if (type % 3 == 1) {
            lclidx = updateDefList(tokens, idx);
            idx = lclidx;

        }
        else if (type % 3 == 2) {
            lclidx = updatePass2Use(tokens, idx);
            idx = lclidx;
        }
        else {
//            pass2uc = curUseList.size();
            cout<<curUseList.size();
            lclidx = updateIns2Use(tokens, idx);
            idx = lclidx;
            modules++;
            curUseList.clear();
        }
        type++;
    }

}

void printWarning2() {
    map<string, bool>::iterator ptr ;
    ptr->second = true;

//    for ( ptr = symbolUsed.begin(); ptr!= symbolUsed.end(); ptr++){
//        struct warning2 war;
//        war.token = ptr->first;
//        war.module = ptr->second;
//        warning2.push_back(war);
//    }
//    int i =0;
//    while ( i < warning2.size()){
//        struct warning2 war = warning2.at(i);
//        cout << "Warning: Module " << war.module+1 <<": "<< war.token<<" was defined but never used\n";
//        i++;
//    }
}

void printWarning1(){
    int i =0;
    while ( i < warning1.size()){
        struct warning1 war = warning1.at(i);
        cout<<"Warning: Module "<<war.module << ": " <<war.token << " too big " <<war.size <<" (max="<<war.maxsize<<") assume zero relative\n";
        i++;
    }

}
void printMemoryMap() {

    cout << "Memory Map" << endl;
    map<int, int>::iterator it;
    for (it = memoryMap.begin(); it != memoryMap.end(); it++) {
        if (memoryMapErr.find(it->first)== memoryMapErr.end()) {
            cout << setw(3) << setfill('0') << it->first << ": " << setw(4) << setfill('0') << it->second
                 << endl;
        }
        else {
//            cout<<"herh"<<it->first<<" "<<it->second;
            cout << setw(3) << setfill('0') << it->first << ": " << it->second <<" "
                 << memoryMapErr[it->first]<<endl;
        }


    }
}

void parserError(int errCode, int idx) {

    cout << "Parse Error line " << tknln.at(idx) << " offset " << tknoff.at(idx) << ": " << parseErrorMsg[errCode];
    exit(0);

}

void initialiseErrMsg() {
    parseErrorMsg.insert(make_pair(1, "NUM_EXPECTED"));
    parseErrorMsg.insert(make_pair(2, "SYM_EXPECTED"));
    parseErrorMsg.insert(make_pair(3, "ADDR_EXPECTED"));
    parseErrorMsg.insert(make_pair(4, "SYM_TOO_LONG"));
    parseErrorMsg.insert(make_pair(5, "TOO_MANY_DEF_IN_MODULE"));
    parseErrorMsg.insert(make_pair(6, "TOO_MANY_USE_IN_MODULE"));
    parseErrorMsg.insert(make_pair(6, "TOO_MANY_INSTR"));


}

void reset() {
    tokens.clear();
    warnings.clear();
}
void sanityCheck(int argc, string file){
    if (argc != 2)
        exit(0);
    string filename = file;
    if (filename.length() == 0)
        exit(0);
    if (access(filename.c_str(), F_OK) == -1)
        exit(0);
}
