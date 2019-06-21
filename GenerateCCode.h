//
// Created by mac on 2019/6/1.
//

#ifndef SEU_LEX_GENERATECCODE_H
#define SEU_LEX_GENERATECCODE_H

#include "helper.h"
#include <fstream>
using namespace std;

void generateCCode(DFA&dfa, vector<Rules>& lex_rules) {
    string s = "Lex.yy.cpp";
    int number1 = dfa.endStatesMap.size();
    int number2 = ALLSET.size();
    int number3 = dfa.statesVec.size();
    bool cnt;
    string outs;
    ofstream out(s.c_str());


    out << "#include<iostream>" << endl;
    out << "#include<string>" << endl;
    out << "#include<fstream>" << endl;
    out << "#include<vector>" << endl;
    out << "#include<stack>" << endl;
    out << "#include<set>" << endl;
    out << "#include<map>" << endl;
    out << "using namespace std;"<<endl;
    out << "const int numberOfEndState ="<< number1 <<";"<< endl;
    out << "const int numberOfChar=" << number2	<< ";"<<endl;
    out << "const int numberOfState=" << number3	<< ";"<<endl;
    out << endl;
    out << "pair<int,int> actions[numberOfEndState]={" << endl;
    cnt = false;
    for (auto&x : dfa.endStatesMap) {
        if (cnt)out << ","; else cnt = true;
        out << "{" << x.first << "," << x.second << "}";
    }
    out	<<"};" << endl;
    out << "int startState="<<dfa.startState<<";" << endl;
    out << "int edges[numberOfState][numberOfChar]={" << endl;
    for (int i = 0; i < number3; i++) {
        out << "{";
        for (int j = 0; j < number2; j++) {
            int p = dfa.statesVec[i].exEdgesMap.count(ALLSET[j]);
            if (!p)p = -1; else p = dfa.statesVec[i].exEdgesMap[ALLSET[j]];
            out <<p;
            if (j + 1 != number2)out << ",";
        }
        out << "}";
        if (i + 1 != number3)out << ",\n";
    }
    out << "};" << endl;
    out << endl;
    out << "const string ALLSET=\"";
    for (size_t i = 0; i < number2; i++)
        switch (ALLSET[i]) {
            case ' ':out << " "; break;
            case '\"':out << "\\" << "\""; break;
            case '\r':out << "\\" << "r"; break;
            case '\n':out << "\\" << "n"; break;
            case '\t':out << "\\" << "t"; break;
            default:out << ALLSET[i];
        }
    out<<"\";" << endl;
    out << "string rules[" << lex_rules.size() << "]={";
    for (size_t i = 0; i < lex_rules.size();i++) {
        out <<"\"";
        int le=lex_rules[i].actions.length();
        for (size_t j = 0; j < le; j++)
            if (lex_rules[i].actions[j] == '\"')out << "\\\"";
            else out << lex_rules[i].actions[j];
        out<< "\"";
        if (i != lex_rules.size() - 1)out << ",";
    }
    out << "};" << endl;
    out << endl;
    out << endl;
    out << "map<int,int> endState;" << endl;
    out << "const string un = \"Unkown\";" << endl;
    out << "struct Unit {" << endl;
    out << "	string type;" << endl;
    out << "	string content;" << endl;
    out << "	int id;" << endl;
    out << "};" << endl;
    out << "vector<Unit> lexicalUnit;" << endl;
    out << "vector<string> word;" << endl;
    out << "set<string> sset;" << endl;
    out << "map<string, int> idmap;" << endl;
    out << "" << endl;
    out << "ifstream input;" << endl;
    out << "ofstream output;" << endl;
    out << "string tkstring;" << endl;
    out << "char c;" << endl;
    out << "int reserve_tag, known_tag;" << endl;
    out << "bool flag;" << endl;
    out << endl;
    out << endl;
    out << "bool NextToken(int &state);" << endl;
    out << "void add(string);" << endl;
    out << "string token2(string);" << endl;
    out << "" << endl;
    out << "int main(int arg,char**argv){" << endl;
    out << "	if(arg!=2){" << endl;
    out << "		cout<<\"Format: . / getToken *.cpp\"<<endl;" << endl;
    out << "		return 0;" << endl;
    out << "	}" << endl;
    out << endl;
    out << "	string s=argv[1];" << endl;
    out << "	input.open(s);" << endl;
    out << "	output.open(\"Lex_Tokens.txt\");" << endl;
    out << "	for(int i=0;i<numberOfEndState ;i++)" << endl;
    out << "		endState.insert(actions[i]);" << endl;
    out << endl;
    out << "	c = input.get();" << endl;
    out << "	tkstring = \"\";" << endl;
    out << "	while (!input.eof()) {" << endl;
    out << "		flag = false;" << endl;
    out << "		int state;" << endl;
    out << "		state = startState;" << endl;
    out << "		flag = NextToken(state);" << endl;
    out << endl;
    out << "		if (!flag) {" << endl;
    out << "			//Unkown" << endl;
    out << "			if (sset.find(tkstring) == sset.end()) add(tkstring);" << endl;
    out << "			lexicalUnit.push_back(Unit{ un,tkstring,idmap[tkstring] });" << endl;
    out << "			c = input.get();" << endl;
    out << "		}" << endl;
    out << "	}" << endl;
    out << "	for (size_t i = 0; i < lexicalUnit.size(); i++) {" << endl;
    out << "		int length = lexicalUnit[i].content.size();" << endl;
    out << endl;
    out << "		for (size_t j = 0; j < length; j++) {" << endl;
    out << "			switch (lexicalUnit[i].content[j]) {" << endl;
    out << "			case ' ': break;" << endl;
    out << "			case '\\r':break;" << endl;
    out << "			case '\\n': break;" << endl;
    out << "			case '\\t':break;" << endl;
    out << "			default:output << lexicalUnit[i].content[j];" << endl;
    out << "			}" << endl;
    out << "		}" << endl;
    out << "        if(lexicalUnit[i].content.size()==1&&(lexicalUnit[i].content[0]==' '||lexicalUnit[i].content[0]=='\\r'||lexicalUnit[i].content[0]=='\\t'||lexicalUnit[i].content[0]=='\\n')){}" << endl;
    out << "		else{output << '\t' << lexicalUnit[i].type << '\t' << lexicalUnit[i].id  << endl;}" << endl;
    out << "	}" << endl;
    out << "	output.close();" << endl;
    out << "	input.close();" << endl;
    out << "	return 0;" << endl;
    out << "	" << endl;
    out << "	" << endl;
    out << "} " << endl;
    out << "" << endl;
    out << "void add(string str) {" << endl;
    out << "	word.push_back(str);" << endl;
    out << "	sset.insert(str);" << endl;
    out << "	idmap[str] = word.size();" << endl;
    out << "}" << endl;
    out << "" << endl;
    out << "int check(char c){" << endl;
    out << "	int le=ALLSET.length();" << endl;
    out << "	for(int i=0;i<le;i++)" << endl;
    out << "		if(c==ALLSET[i])return i;" << endl;
    out << "	return -1;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool NextToken(int &state) {" << endl;
    out << "	stack<int>stk; " << endl;
    out << "	int cnt,x;" << endl;
    out << "	tkstring = \"\";" << endl;
    out << "	tkstring += c;" << endl;
    out << "	x=check(c);" << endl;
    out << "	if (x==-1 || edges[state][x] == -1)" << endl;
    out << "		return false;" << endl;
    out << "	stk.push(state);" << endl;
    out << "	state = edges[state][x];" << endl;
    out << "	c = input.get();" << endl;
    out << "	x=check(c);" << endl;
    out << "	while (x!=-1&&edges[state][x]!=-1 ) {" << endl;
    out << "		stk.push(state);" << endl;
    out << "		tkstring += c;" << endl;
    out << "		state = edges[state][x];" << endl;
    out << "		c = input.get();" << endl;
    out << "		x=check(c);" << endl;
    out << "	}" << endl;
    out << "	stk.push(state);" << endl;
    out << "	input.seekg(-1, ios_base::cur);" << endl;
    out << "	while (!stk.empty()) {" << endl;
    out << "		state=stk.top();" << endl;
    out << "		if (endState.count(state))" << endl;
    out << "			break;" << endl;
    out << "		stk.pop();" << endl;
    out << "		input.seekg(-1, ios_base::cur);" << endl;
    out << "	}" << endl;
    out << endl;
    out << "	c = input.get(); " << endl;
    out << "	if(stk.empty()){" << endl;
    out << endl;
    out << "		tkstring = \"\";" << endl;
    out << "		tkstring += c;" << endl;
    out << "		return false;" << endl;
    out << "	}" << endl;
    out << "	if (sset.find(tkstring) == sset.end()) add(tkstring);" << endl;
    out << "	lexicalUnit.push_back(Unit{ rules[endState[state]],tkstring,idmap[tkstring] });" << endl;
    out << endl;
    out << "	return true;" << endl;
    out << "}" << endl;
    out << "" << endl;

}

#endif //SEU_LEX_GENERATECCODE_H
