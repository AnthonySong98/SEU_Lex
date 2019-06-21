//
// Created by mac on 2019/5/15.
//

#ifndef LEX_RE_EXTRACTION_H
#define LEX_RE_EXTRACTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include "helper.h"


using namespace std;

// read the whole lex.l file into a string for later use
void ReadLecFile2String(const string file_address,string& resString){
    ifstream inFile;
    inFile.open(file_address); //open the input file

    stringstream strStream;
    strStream << inFile.rdbuf(); //read the file

    resString = strStream.str(); //str holds the content of the file

    //cout << resString << endl; //you can do anything with the string!!!

    inFile.close();
}

// divide lex file into three parts and extract RE
/*
 * for more information please refer to https://www.geeksforgeeks.org/regex-regular-expression-in-c/
 */
void ExtractThreeParts(string wholeLex,vector<Rules>& lex_rules,map<string, string>& M){

    // extract comment in lex.l
    regex commentInLexRE("\\%\\{([^}]+)\\%\\}");
    smatch mCommentInLex;
    regex_search(wholeLex,mCommentInLex,commentInLexRE);
    string comment = mCommentInLex[1];
    //cout<<a;

    //extract definitions

    int next1 = wholeLex.find(comment)+comment.length()+2;
    int i = next1;
    int flag = 1;
    while(i<wholeLex.length()&&flag){
        if(wholeLex[i]=='%'&& wholeLex[i+1]=='%'){flag=0;break;}
        i++;
    }
    string definitions = wholeLex.substr(next1,i-next1-1);

    //extract rules

    int next2 = i+2;
    i = next2;
    flag = 1;
    while(i<wholeLex.length()&&flag){
        if(wholeLex[i]=='%'&&wholeLex[i+1]=='%'){flag=0;
            break;}
        i++;
    }
    string rules = wholeLex.substr(next2,i-next2);

    //extract user subroutines

    string subroutines = wholeLex.substr(i+2,wholeLex.length()-i-2);


    /*
    //extract definition in lex.l
    regex definitionsRE("\\%\\}([^%]+)\\%\\%");
    smatch mDefinitions;
    regex_search(wholeLex,mDefinitions,definitionsRE);
    string b = mDefinitions[1];
    cout<<b<<endl;

    //extract rules in lex.l
    regex rulesRE("\\%\\%((.)+)\\%\\%");
    smatch mRules;
    regex_search(wholeLex,mRules,rulesRE);
    string c = mRules[1];
    cout<<c<<endl;

     */
    /*for (auto x : mCommentInLex)
        cout << x << " "<<endl;
        cout<<endl;
        */


    i = 0;

    unordered_map<string,string> definitions_map;
    while(i<definitions.length()){
        while(i<definitions.length()&& definitions[i] == '\n'){i++;}
        if(i<definitions.length()){
            int def_start = i ;
            while(i<definitions.length()&& (definitions[i] != '\t' && definitions[i] != ' ') ){ i++ ;}
            string ReName = definitions.substr(def_start,i-def_start);
            //cout<<ReName<<"  ";
            while(i<definitions.length()&& (definitions[i] == '\t' || definitions[i] == ' ')){i++;}
            int re_start = i;
            while(i<definitions.length()&& definitions[i] != ';'){i++;}
            string ReExpr = definitions.substr(re_start,i-re_start);
            //cout<<ReExpr<<endl;
            i++;

            //substitute inside definitions first

            //cout<<endl<<"substitute"<<endl;
            unsigned long first_index = definitions.find(ReName);
            unsigned long next_index = definitions.find(ReName,first_index+ReName.length());
            while(next_index != string::npos){
                definitions.replace(next_index-1,ReName.length()+2,ReExpr);
                next_index = definitions.find(ReName,next_index+ReName.length());

            }

            // store the definitions in order to make substitution for rules easier
            definitions_map.insert(pair<string,string>(ReName,ReExpr));
            M.insert(pair<string,string>(ReName,ReExpr));
            //cout<<definitions;
            //cout<<definitions.find(ReName,first_index+ReName.length())<<" ";
        }

    }


    //cout<<definitions;

    //extract Rules
    int j = 0;

    while(j<rules.length()){
        while (j<rules.length()&&rules[j] == '\n'||rules[j] == '\t'||rules[j] == ' '){j++;}
        if(j<rules.length()){
            int rule_start = j;
            while(j<rules.length() && (rules[j] != '\t' /*&& rules[j] != ' '*/)) {j++;}
            string RuleName  = rules.substr(rule_start,j-rule_start);
            //substitute some rules according to the definitions
            int k;
            for(auto& v:definitions_map){
                k = 0;
                unsigned long next_index = RuleName.find(v.first);
                while(next_index != string::npos){
                    if(RuleName[next_index])
                        RuleName.replace(next_index-1,v.first.length()+2,v.second);
                    next_index = RuleName.find(v.first,next_index+v.first.length());
                }
                //cout<<rules;
            }

            //cout<<RuleName<<"    ";

            while(j<rules.length() && (rules[j] == '\t' || rules[j] == ' ')) {j++;}
            int action_start = j;
            while(j<rules.length() && rules[j] != '\n'){j++;}
            string ReAction = rules.substr(action_start,j-action_start);
            //cout<<ReAction<<endl;

            regex e("\"(.*)\"");
            smatch m;
            regex_search(ReAction, m, e);
            ReAction = m[1];

            Rules newRule;
            newRule.pattern = RuleName;
            newRule.actions = ReAction;
            lex_rules.push_back(newRule);
            j++;
        }
    }

    //substitute some rules according to the definitions
    /*
    for(auto& v:definitions_map){
        j = 0;
        unsigned long next_index = rules.find(v.first);
        while(next_index != string::npos){
            if(rules[next_index])
            rules.replace(next_index,v.first.length(),v.second);
            next_index = rules.find(v.first,next_index+v.first.length());
        }
        cout<<rules;
    }
     */

}


#endif //LEX_RE_EXTRACTION_H
