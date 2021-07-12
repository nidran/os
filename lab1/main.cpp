#include<stdio.h>
#include <iostream>
#include <string.h>
#include<string>
#include <fstream>
#include<iterator>
#include <vector>
#include <map>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
using namespace std;
//#include<bits/stdc++.h>


template<typename T>
inline void remove(vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}
struct warning1{
    int module;
    string token;
    int size;
    int maxsize;
};
class MemModInfo {
public:
    string symbol;
    int moduleNo;
    int used;
    MemModInfo(string i, int m);

//    A(string n, string l) { b = n ;c = l; }
//    string Getb(){ return b; }
//    string Getc(){ return c; }
    bool operator==(const MemModInfo & obj2)
    {
        return (this->symbol.compare(obj2.symbol)==0 && (this->moduleNo == (obj2.moduleNo)));
    }
};
MemModInfo:: MemModInfo(string i, int m)
{
    symbol=i;
    moduleNo = m;
}

int nextOff;
string nextToken;
int nextLn;

vector <MemModInfo> rule7Info;
typedef vector< pair<int, string> > moduleInfo;
vector<string> allUseList;
vector<string> curUseList;
map<string, bool> curUseListNU;
vector<string> tokens;
vector<int> tknln;
int ln;
//void printRule7(MemModInfo mem);
vector<int> tknoff;
map<string, int> symbolTable;
vector<string> isSymUsed;
vector<string > symbolsDefUsed;
vector<string> seenSym;
vector<string> modSymDefNtUsed;
map<string, int> symDefNotUsed;
vector<string> duplicateSym;
map<string, int> stringCase;
void parserError(int errCode, int ln , int off);
class MemoryMap {
public:
    string err;
    int loc;
    int addr;
    MemoryMap( int n, int m, string i);

//    A(string n, string l) { b = n ;c = l; }
//    string Getb(){ return b; }
//    string Getc(){ return c; }
//    bool operator==(const MemModInfo & obj2)
//    {
//        return (this->symbol.compare(obj2.symbol)==0 && (this->moduleNo == (obj2.moduleNo)));
//    }
};
MemoryMap:: MemoryMap( int n, int m, string i)
{
    err=i;
    loc = m;
    addr = n;
}
vector<MemoryMap> memory;
map<int, int> memoryMap;
vector<int> memoryMapModule;
map<int, string> memoryMapErr;
vector<string> warnings;
vector<warning1> warning1;
int ac=0;
//vector<warning2> warning2;
const char delim[] = " \t\r\n\v\f";
void checkGlobalOffset(int idx);
bool isNum(string tokn);
bool isValidAddrMode(string token);
bool isString(string tokn);
bool isSymLen(string tokn);

void getToken(string fileName, vector <string> &tokens, vector<int> &tknln, vector<int> &tknoff);

void passOne();

void passTwo();

int memMap = 0;
int pass2uc = 0;
int moduleAddr = 0;
void printRule5();
void printMemoryMap();
vector <pair<string, int> > rule4stack;
//void updateRule4();
void printRule4();
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
    printRule4();

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
//            cout<<"module addr"<<moduleAddr<<" "<<moduleAddr+ac;
            moduleAddr+=ac;
            idx = lclidx;
            modules++;
            printRule5();
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
//        cout<<"in warning";
//        cout<<it->second <<" "<<moduleAddr ;

        if (it->second > (moduleAddr )+ ac){
//            cout<<it->second <<" "<<moduleAddr ;
            struct warning1 war;
            war.token =it->first;
//            cout<<it->second;

            war.maxsize = count -1;
            war.module = modules+1;
//            cout<<"in warning";
            if (it== symbolTable.begin()){
                war.size =it->second ;
                it->second = 0;

            }
            else {
                war.size =it->second - moduleAddr;
                it->second = moduleAddr;
            }
            warning1.push_back(war);


        }
    }

}

int updateDefList(vector<string> curTokens, int start) {
    int idx = start;
    int dc;
//    cout<<"def list"<<endl;
    if (!isNum(curTokens.at(idx))) {

        parserError(1, idx);
    }
    dc = stoi(curTokens.at(idx));
    if (dc > 16){
        parserError(5, idx);
    }

    int end = start + 2*dc;
    idx++;
    int lc = 1;
    if (idx >= curTokens.size()){
        parserError(2, idx-1);
        exit(0);
    }
    while (  idx < end) {
//        cout<<"idx"<<idx<<"end"<<end;
        if (!isString(curTokens.at(idx))) {
            parserError(2, idx);
        }

        if (!isSymLen(curTokens.at(idx))) {
            parserError(4, idx);
        }
        if (!isNum(curTokens.at(idx + 1))) {
//            cout<<"here"<<curTokens.at(idx + 1);
            parserError(1, idx+1);
        }

        string curToken = curTokens.at(idx);
        int curTokenLoc = stoi(curTokens.at(idx + 1));

        if (find(seenSym.begin(), seenSym.end(), curToken) == seenSym.end()) {

            seenSym.push_back(curToken); // to be used to compare with use list later
            modSymDefNtUsed.push_back(curToken);
            symDefNotUsed.insert( pair<string,int> (curToken, modules));
            symbolTable.insert(pair<string, int>(curToken, globalOffset + curTokenLoc));
        } else {
            duplicateSym.push_back(curToken);
        }
//        cout<<curTokens.at(idx)<<curTokens.at(idx+1);
        idx += 2;
    }
//    cout<<idx;
    return idx;

}

int updateUseList(vector<string> curTokens, int start) {

//    cout<<" in use list"<<endl;
    int idx = start;
    int uc;
//    cout<<curTokens.at(idx);
    if (!isNum(curTokens.at(idx))){
//        cout<<"errr";
        parserError(1, idx);
    }

    uc = stoi(curTokens.at(idx));

    if (uc > 16)
        parserError(6, idx);
    if ( idx + uc > curTokens.size()-1)
        parserError(2, nextLn, nextOff);
    idx++;



    int lc = 1;
//    cout<<uc<<" "<<idx+uc<<" "<<idx;
    int end = idx+ uc;
//    cout<<uc<<lc<<idx<<end;
    while (idx <end ) {
//        cout<< idx<<" "<<end<<endl;
//        cout<<"while use"<<idx<<" "<<endl;
//        cout<<curTokens.at(idx);
        if (!isString(curTokens.at(idx)))
            parserError(2, idx);
        if (!isSymLen(curTokens.at(idx)))
            parserError(4, idx);


//        cout<<curTokens.at(idx);
        idx++;
    }

    return idx ;
}

int updateInstList(vector<string> curTokens, int start) {
//    cout<<" in ins";
    int idx = start;

    if (!isNum(curTokens.at(idx)))
        parserError(1, idx);
    ac = stoi(curTokens.at(idx));
//    cout<<ac;
    if (ac > 512)
        parserError(5, idx);

    idx++;
    globalOffset += ac;

    int lc = 1;
    if (globalOffset > 512) {
        parserError(7, idx);
    }
//    cout<<idx<<curTokens.size();
    while (lc <= ac) {
//         cout<<"here";
//         cout<<curTokens.at(idx);
//         if ( idx == curTokens.size()-1)
//         {
//
//         }
        if ( idx == curTokens.size()){
            parserError(3, idx);
        }
        if (!isValidAddrMode(curTokens.at(idx)) || idx == curTokens.size())
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

bool isString(string tokn) {
    if ( !isdigit(tokn[0])){

        return true;
    }

    return false;
}

bool isSymLen(string tokn) {
    if ( tokn.length() > 16) {
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
    if (symbolTable.size()>0) {


        for (it = symbolTable.begin(); it != symbolTable.end(); it++) {
            if (find(duplicateSym.begin(), duplicateSym.end(), it->first) != duplicateSym.end()) {
                cout << it->first << "=" << it->second << " "
                     << " Error: This variable is multiple times defined; first value used" << endl;
            } else {
                cout << it->first << '='
                     << it->second
                     << endl;
            }
        }
    }
    else {
        cout<<endl;
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
            MemModInfo info(tkn, modules);
//            info.symbol = tkn;
//            info.moduleNo= modules;
            info.used = 0;
            rule7Info.push_back(info);

            curUseListNU.insert(make_pair(tkn, false));
            // found, should remove
            remove(modSymDefNtUsed, tkn);

//            vector<int>::iterator pos = std::find(modSymDefNtUsed.begin(), modSymDefNtUsed.end(), tkn);
//            if (pos != modSymDefNtUsed.end()) // == myVector.end() means the element was not found
//                modSymDefNtUsed.erase(pos);
        } else {
            parserError(4, idx);
        }

        idx++;
    }
//    cout<<"use list"<<allUseList.size()<<endl;
//    for ( auto it :allUseList){
//        cout<<it;
//    }
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
        //storing memory map module no
        memoryMapModule.push_back(modules);
        if (opCode > 9 && addMode=="R")  {
            memory.push_back(MemoryMap(memMap, 9999," Error: Illegal opcode; treated as 9999"));
            memoryMap.insert(make_pair(memMap, 9999));
            memoryMapErr.insert(make_pair(memMap, " Error: Illegal opcode; treated as 9999"));

        }
        else if (opCode > 9 && addMode == "I")  {

            memory.push_back(MemoryMap(memMap, 9999,"  Error: Illegal immediate value; treated as 9999"));
            memoryMap.insert(make_pair(memMap, 9999));
            memoryMapErr.insert(make_pair(memMap, " Error: Illegal immediate value; treated as 9999"));

        }
        else {
            switch (stringCase.at(addMode)) {
                case 1 :
                    if (operand > (addC - 1)) {
                        memory.push_back(MemoryMap(memMap, opCode * 1000 + moduleAddr," Error: Relative address exceeds module size; zero used"));
                        memoryMap.insert(make_pair(memMap, opCode * 1000 + moduleAddr));
                        memoryMapErr.insert(
                                make_pair(memMap, " Error: Relative address exceeds module size; zero used"));
                    } else {
                        memory.push_back(MemoryMap(memMap, value + moduleAddr,""));
                        memoryMap.insert(make_pair(memMap, value + moduleAddr));
                    }
                    break;
                case 2 :
                    if (value > 9999) {
                        memory.push_back(MemoryMap(memMap, 9999," Error: Illegal immediate value; treated as 9999"));
                        memoryMap.insert(make_pair(memMap, 9999));
                        memoryMapErr.insert(make_pair(memMap, " Error: Illegal immediate value; treated as 9999"));
                    } else if (opCode > 9) {
                        memory.push_back(MemoryMap(memMap, 9999," Error: Illegal opcode; treated as 9999"));
                        memoryMap.insert(make_pair(memMap, 9999));
                        memoryMapErr.insert(make_pair(memMap, " Error: Illegal opcode; treated as 9999"));

                    } else {
                        memory.push_back(MemoryMap(memMap, value ,""));
                        memoryMap.insert(make_pair(memMap, value));

                    }
                    break;
                case 3 :
//                    cout << "In E" << endl;
//                    cout << operand << " " << curUseList.size();

                    if (operand > curUseList.size()) {
                        memory.push_back(MemoryMap(memMap, value," Error: External address exceeds length of uselist; treated as immediate"));
                        memoryMap.insert(make_pair(memMap, value));
                        memoryMapErr.insert(make_pair(memMap,
                                                      " Error: External address exceeds length of uselist; treated as immediate"));

                    } else {
//
                        if (symbolTable.find(allUseList[operand]) == symbolTable.end()) {
//                            cout<<"looking at "<<allUseList[operand];
                            memory.push_back(MemoryMap(memMap, opCode * 1000," Error: " + allUseList[operand] + " is not defined; zero used"));
                            memoryMap.insert(make_pair(memMap, opCode * 1000));
                            memoryMapErr.insert(
                                    make_pair(memMap, " Error: " + allUseList[operand] + " is not defined; zero used"));

                            auto it = find(rule7Info.begin(), rule7Info.end(), MemModInfo(curUseList[operand], modules));
//
                            if (it != rule7Info.end()) {
                                it->used = 1;

                            }

                        } else {
                            //  found
                            map<string, int>::iterator itr = symbolTable.find(curUseList[operand]);
                            memory.push_back(MemoryMap(memMap, opCode * 1000 + (itr->second) ,""));
                            memoryMap.insert(make_pair(memMap, opCode * 1000 + (itr->second)));

                            symbolsDefUsed.push_back(allUseList[operand]);
//                            cout<<"here";
//                            cout<<curUseList[operand]<<" ";
                            auto it = find(rule7Info.begin(), rule7Info.end(), MemModInfo(curUseList[operand], modules));
//                           int i =0;

                            if (it != rule7Info.end()) {
//
                                it->used = 1;
//
                            }
//

                        }

                        auto defItr = symDefNotUsed.find(allUseList[operand]);
                        if(defItr != symDefNotUsed.end()) {
                            symDefNotUsed.erase(defItr);
                        }
                    }
//                    find_if(v.rbegin(), v.rend(),
//                                 [](int i) { return i == 42; });
////                    cout<<"rule 7 info"<<endl;
//                    for ( auto it : rule7Info){
//                        cout<<it.symbol<<" "<<it.used<<" "<<it.moduleNo<<endl;
//                    }

                    break;
                case 4 :
//                                    cout << "in A" << endl;
                    if (operand >= 512) {
                        memory.push_back(MemoryMap(memMap, opCode * 1000  ," Error: Absolute address exceeds machine size; zero used"));
                        memoryMap.insert(make_pair(memMap, opCode * 1000));
                        memoryMapErr.insert(
                                make_pair(memMap," Error: Absolute address exceeds machine size; zero used") );

                    } else {
                        memory.push_back(MemoryMap(memMap, value  ,""));
                        memoryMap.insert(make_pair(memMap, value));
//
                    }

                    break;
                default:
                    cout << "";
            }
        }

        i += 1;
        idx += 2;
        memMap++;

    }

//    cout<<moduleAddr;
    moduleAddr = moduleAddr + addC;
    return idx;

}

void passTwo() {

    moduleAddr=0;
    modules=1;
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

            lclidx = updateIns2Use(tokens, idx);
//            cout<<seenSym.size()<<endl;
//            cout<<modSymDefNtUsed.size()<<endl;
//cout<<modules<<endl;

            idx = lclidx;
//            updateRule4();
            modules++;

            curUseList.clear();
//            cout<<modSymDefNtUsed.size()<<endl;
//            cout<<rule4stack.size()<<endl;
        }

//        modSymDefNtUsed.clear();
        type++;
    }


}



void printRule4(){
    for (auto x: symDefNotUsed) {
        cout << "Warning: Module " << x.second+1 <<": "<< x.first<<" was defined but never used\n";

    }
}
//void printRule7(MemModInfo mem) {
//    map<string, bool>::iterator it;
////
//    for (it = curUseListNU.begin(); it != curUseListNU.end(); it++)
//    {
//        if (it->second == false){
//            cout << "Warning: Module " << modules <<": "<< it->first<<" appeared in the uselist but was not actually used\n";
//        }
//    }
//
//}

void printRule5(){
    int i =0;
    while ( i < warning1.size()){
        struct warning1 war = warning1.at(i);
        cout<<"Warning: Module "<<war.module << ": " <<war.token << " too big " <<war.size <<" (max="<<war.maxsize<<") assume zero relative\n";
        i++;
    }

}
void printMemoryMap() {

    cout <<endl << "Memory Map" << endl;

//    cout<<rule7Info.size();
//    for ( auto it : rule7Info){
//        cout<<it.symbol<<"  "<<it.used<<" "<<it.moduleNo<<" " <<endl;
//    }
    map<int, int>::iterator it;
    int cm = 0;
    int idx=0;
    int j =0;
    int size = 0;
    int mm = memory.size();
    for ( int k =0; k <memory.size(); k++){
//        cout<<" in for"<<endl;
//        cout<<memoryMapModule.size();
//        cout<<idx<<k<<mm<<endl;

        MemoryMap m = memory.at(k);
//        cout<<"at k"<<k;
        cout << setw(3) << setfill('0') << m.addr<< ": " << setw(4) << setfill('0') << m.loc
             << m.err <<endl;
        if ( idx < mm && memoryMapModule.at(idx)!=cm && rule7Info.size()>0 ){
//            cout<<" in if";
            while( j < rule7Info.size()  && rule7Info.at(j).moduleNo == cm){
                if (rule7Info.at(j).used == 0){
//                  cout<<rule7Info.at(j).symbol<<" used "<<rule7Info.at(j).used<<" "<<" "<<rule7Info.at(j).moduleNo<<endl;
                    cout << "Warning: Module " << rule7Info.at(j).moduleNo <<": "<< rule7Info.at(j).symbol<<" appeared in the uselist but was not actually used\n";
                }
//
                j++;
//
            }
            cm++;
        }
        else if (rule7Info.size()>0 && rule7Info.at(j).moduleNo == cm && (j == rule7Info.size()-1 ) && ( size == mm -1 )) {
//            cout<<" in else";
            cout << "Warning: Module " << rule7Info.at(j).moduleNo <<": "<< rule7Info.at(j).symbol<<" appeared in the uselist but was not actually used\n";
        }

        idx++;
        size++;
    }




}

void parserError(int errCode, int idx) {
    if (idx == tknln.size()){
        cout << "Parse Error line " << tknln.at(idx-1) << " offset " << tknoff.at(idx-1) << ": " << parseErrorMsg[errCode];
        exit(0);
    }
    cout << "Parse Error line " << tknln.at(idx) << " offset " << tknoff.at(idx) << ": " << parseErrorMsg[errCode];
    exit(0);

}

void parserError(int errCode, int ln , int off) {

    cout << "Parse Error line " << ln << " offset " << off << ": " << parseErrorMsg[errCode];
    exit(0);

}

void initialiseErrMsg() {
    parseErrorMsg.insert(make_pair(1, "NUM_EXPECTED"));
    parseErrorMsg.insert(make_pair(2, "SYM_EXPECTED"));
    parseErrorMsg.insert(make_pair(3, "ADDR_EXPECTED"));
    parseErrorMsg.insert(make_pair(4, "SYM_TOO_LONG"));
    parseErrorMsg.insert(make_pair(5, "TOO_MANY_DEF_IN_MODULE"));
    parseErrorMsg.insert(make_pair(6, "TOO_MANY_USE_IN_MODULE"));
    parseErrorMsg.insert(make_pair(7, "TOO_MANY_INSTR"));


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

void getToken(string fileName, vector <string> &tokens, vector<int> &tknln, vector<int> &tknoff){
    tokens.clear();
    ifstream file(fileName);
    int ln = 1;
    int localOffset=1;
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            int lele = tknln.size();
            tknln[lele -1]= ln;
            nextToken="";
            nextLn=ln;
            nextOff = localOffset;
            ln++;
            continue;

        }
        char *token;
        char char_array[line.length() + 1];
        localOffset = 1;
        strcpy(char_array, line.c_str());
        token = strtok(char_array, delim);
        while (token != NULL) {
            string tokenStr(token);
            localOffset = line.find(tokenStr);
            localOffset++;
            tknoff.push_back(localOffset);
            tokens.push_back(tokenStr);
            tknln.push_back(ln);
            nextLn = ln;
            nextOff= localOffset+tokenStr.size();
            nextToken = tokenStr;
            token = strtok(NULL, delim);
        }

        ln++;

    }
//    cout<<nextOffset;

}

