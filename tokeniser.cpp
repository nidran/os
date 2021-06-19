
using namespace std;
const char delim[] = " \t\r\n\v\f";
void getToken(string fileName, vector <string> &tokens, vector<int> &tknln, vector<int> &tknoff){
    tokens.clear();
    ifstream file(fileName);
    int ln = 1;
    int localOffset;
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
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
            tokens.push_back(tokenStr);
            tknoff.push_back(localOffset);
            tknln.push_back(ln);
            token = strtok(NULL, delim);
        }
        ln++;
    }
//    cout << " end get token" << endl;
}