#include <iostream>
#include <fstream>
#include "RE_Extraction.h"
#include "Re2NFA.h"
#include "RE_Standardization.h"
#include "DFAMinimization.h"
#include "NFAToDFA.h"
#include "GenerateCCode.h"
#include "helper.h"
using namespace std;

NFA wholeNFA;
DFA wholeDFA;
vector<Rules> lex_rules;
map<string, string> M;
string res;

int main(int arg,char** argv) {
    if (arg != 2) {
        cout << "Format: ./SeuLex *.l" << endl;
        return 0;
    }
    string ins=argv[1];
    //string ins = "/Users/mac/Desktop/编译原理课程设计/Seu_Lex/lex.l";
    cout << "\n----------------Reading Lex.l..." << endl;
    ReadLecFile2String(ins, res);
    cout << "Completion" << endl;

    cout << "\n----------------Extracting Three Parts and Standardizing RE..." << endl;
    ExtractThreeParts(res, lex_rules, M);

//    for(int i = 0 ; i < lex_rules.size() ; i ++ ){
//        cout<<lex_rules[i].pattern<<"---"<<lex_rules[i].actions<<endl;
//    }

    re_Standardize(lex_rules, M);
    vector<pair<string, int> >REset;
    int k = 0;
    for (k = 0; k < lex_rules.size(); k++) {
        REset.push_back(pair<string, int>(lex_rules[k].pattern, k));
    }
    cout << "Completion" << endl;

//    for(int i = 0 ; i < lex_rules.size() ; i ++ ){
//        cout<<lex_rules[i].pattern<<"---"<<lex_rules[i].actions<<endl;
//    }
//

    cout << "\n----------------RE to NFA..." << endl;
    wholeNFA = GenerateNFA(REset);
    cout << "Completion" << endl;


    cout << "\n----------------NFA to DFA..." << endl;
    NFAToDFA(wholeNFA, wholeDFA);
    cout << "Completion" << endl;

    cout << "\n----------------DFAMinimization..." << endl;
    DFAMinimization(wholeDFA);
    cout << "Completion" << endl;

    cout << "\n----------------Generate Lex.c..." << endl;
    generateCCode(wholeDFA, lex_rules);
    cout << "Completion" << endl;


}

//#include <iostream>
//
//int main() {
//    std::cout << "Hello, World!" << std::endl;
//    return 0;
//}