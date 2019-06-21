//
// Created by mac on 2019/5/8.
//

#ifndef LEX_RE2NFA_H
#define LEX_RE2NFA_H

#include <iostream>
#include "helper.h"

#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;


int NFAStateNumber = 0;

// for details please refer to https://www.geeksforgeeks.org/stack-set-2-infix-to-postfix/
string StandardForm2SuffixForm(string standardForm) {
    stack<char> opStack;
    opStack.push('#');
    string res = "";

    unordered_map<char, int> precedenceMap{ { '\\',4 },{ '*',3 },{ '`',2 },{ '|',1 } };
    int i = 0;
    while (i != standardForm.size()) {
        if (standardForm[i] != '\\' && standardForm[i] != '(' && standardForm[i] != ')' && standardForm[i] != '*'
            && standardForm[i] != '`' && standardForm[i] != '|') {
            res = res + standardForm[i];
        }
        else if (i + 1 != standardForm.size() && standardForm[i] == '\\') {
            res = res + '\\' + standardForm[i + 1];
            i++;
        }
        else if (standardForm[i] == '(') {
            opStack.push('(');
        }
        else if (standardForm[i] == ')') {
            while (opStack.top() != '#' && opStack.top() != '(') {
                char c = opStack.top();
                opStack.pop();
                res = res + c;
            }
            if (opStack.top() == '(') {
                char c = opStack.top();
                opStack.pop();
            }
        }
        else {
            while (opStack.top() != '#' && precedenceMap[standardForm[i]] <= precedenceMap[opStack.top()]) {
                char c = opStack.top();
                opStack.pop();
                res = res + c;
            }
            opStack.push(standardForm[i]);
        }

        i++;
    }

    while (opStack.top() != '#') {
        char c = opStack.top();
        opStack.pop();
        res = res + c;
    }

    return res;
}


// for debugging
void PrintNFA(NFA & _NFA) {
    cout << "Start State Number: " << _NFA.startState << endl;
    cout << "End State Number: ";
    for (auto & v : _NFA.endStatesMap) {
        cout << v.first << " " << v.second << " ";
    }
    cout << endl;
    cout << "For each state:" << endl;
    for (auto & v : _NFA.statesMap) {
        cout << "For state" << v.first << ": " << endl;
        for (auto& u : v.second.exEdgesMultiMap) {
            cout << v.second.number << " -> " << u.first << " -> " << u.second << endl;
        }
    }
    cout << endl;
}


// concatenation for two NFA
NFA ConcatenationNFA(NFA & NFA1, NFA & NFA2) {
    // find max used number so that we can use the min unused number to add more states
    map<int, int>::const_iterator it1 = NFA1.endStatesMap.end();
    it1--;
    map<int, int>::const_iterator it2 = NFA2.endStatesMap.end();
    it2--;
    int initialStateNumber = max(it1->first, it2->first) + 1;

    NFA resNFA;
    resNFA.startState = initialStateNumber;

    unordered_map<int, NFAstate> resStatesMap;

    for (auto& u : NFA1.statesMap) {
        resStatesMap.insert(u);
    }


    return resNFA;
}


int stateNumber = 1;

// convert each RE to its mini NFA according to its suffixForm
/*
* pay attention : each NFA has only one accepting state.
*/
NFA SuffixForm2NFA(string suffixForm, int tokenType) {
    stack<NFA> helper;
    int i = 0;
    //cout<<endl;

    while (i < suffixForm.size()) {

        //cout<<suffixForm[i];
        switch (suffixForm[i]) {
            case '|': {

                NFA NFA1 = helper.top();
                helper.pop();

                NFA NFA2 = helper.top();
                helper.pop();

                //start node
                NFAstate nfAstate1;
                nfAstate1.number = stateNumber++;
                unordered_multimap<char, int> exEdgesMultiMap1{ { '@',NFA1.startState },{ '@',NFA2.startState } };
                nfAstate1.exEdgesMultiMap = exEdgesMultiMap1;

                NFA1.statesMap.insert(pair<int, NFAstate>(nfAstate1.number, nfAstate1));
                NFA1.startState = nfAstate1.number;

                NFA1.statesMap.insert(NFA2.statesMap.begin(), NFA2.statesMap.end());

                //PrintNFA(NFA1);
                //cout<<"~~~~~~~~~~~~~~~~~~\n";

                //end node
                NFAstate nfAstate2;
                nfAstate2.number = stateNumber++;
                int end1, end2;
                if (NFA1.endStatesMap.size() == 1 && NFA2.endStatesMap.size() == 1) {
                    end1 = (--NFA1.endStatesMap.end())->first;
                    end2 = (--NFA2.endStatesMap.end())->first;

                    //cout<<end1<<" "<<end2<<endl;
                    unordered_multimap<char, int> exEdgesMultiMap1{ { '@',nfAstate2.number } };

                    //NFA1.statesMap.find(end1)->second = ;
                    NFA1.statesMap[end1].exEdgesMultiMap = exEdgesMultiMap1;
                    NFA1.statesMap[end2].exEdgesMultiMap = exEdgesMultiMap1;
                }


                //unordered_multimap<char,int> exEdgesMultiMap2;

                NFA1.statesMap.insert(pair<int, NFAstate>(nfAstate2.number, nfAstate2));
                NFA1.endStatesMap = map<int, int>{ { nfAstate2.number,999 } };

                //PrintNFA(NFA1);
                helper.push(NFA1);

                break;
            }
            case '*': {
                NFA topNFA = helper.top();
                helper.pop();


                int originstart = topNFA.startState;
                //start node
                NFAstate nfAstate1;
                nfAstate1.number = stateNumber++;
                unordered_multimap<char, int> exEdgesMultiMap1{ { '@',topNFA.startState },{ '@',stateNumber } };
                nfAstate1.exEdgesMultiMap = exEdgesMultiMap1;

                topNFA.statesMap.insert(pair<int, NFAstate>(nfAstate1.number, nfAstate1));
                topNFA.startState = nfAstate1.number;

                //PrintNFA(topNFA);
                //NFA1.statesMap.insert(NFA2.statesMap.begin(),NFA2.statesMap.end());

                //end node
                NFAstate nfAstate2;
                nfAstate2.number = stateNumber++;
                int end1;
                if (topNFA.endStatesMap.size() == 1) {
                    end1 = (--topNFA.endStatesMap.end())->first;
                    unordered_multimap<char, int> exEdgesMultiMap1{ { '@',nfAstate2.number },{ '@',originstart } };
                    topNFA.statesMap[end1].exEdgesMultiMap = exEdgesMultiMap1;
                }


                //unordered_multimap<char,int> exEdgesMultiMap2;
                topNFA.statesMap.insert(pair<int, NFAstate>(nfAstate2.number, nfAstate2));
                topNFA.endStatesMap = map<int, int>{ { nfAstate2.number,999 } };

                //PrintNFA(topNFA);
                helper.push(topNFA);

                break;
            }
            case '`': {

                NFA NFA2 = helper.top();
                helper.pop();

                NFA NFA1 = helper.top();
                helper.pop();


                int end1 = (--NFA1.endStatesMap.end())->first;
                unordered_multimap<char, int> exEdgesMultiMap1{ { '@',NFA2.startState } };
                NFA1.statesMap[end1].exEdgesMultiMap = exEdgesMultiMap1;

                NFA1.statesMap.insert(NFA2.statesMap.begin(), NFA2.statesMap.end());

                NFA1.endStatesMap = NFA2.endStatesMap;

                //PrintNFA(NFA1);

                helper.push(NFA1);
                break;
            }
            case  '\\': {

                NFAstate nfAstate1;
                nfAstate1.number = stateNumber++;

                char c;
                switch (suffixForm[i + 1]) {
                    case 't': {c = '\t';
                        break;
                    }
                    case 'n': {
                        c = '\n';
                        break;
                    }
                    case 'r': {
                        c = '\r';
                        break;
                    }
                    case 'b': {
                        c = '\b';
                        break;
                    }
                    case '|': {
                        c = '|';
                        break;
                    }
                    case '(': {
                        c = '(';
                        break;
                    }
                    case ')': {
                        c = ')';
                        break;
                    }
                    case '*': {
                        c = '*';
                        break;
                    }
                    default: {
                        c = suffixForm[i + 1];
                        break;
                    }
                }


                unordered_multimap<char, int> exEdgesMultiMap1{ { c,stateNumber++ } };
                nfAstate1.exEdgesMultiMap = exEdgesMultiMap1;

                NFAstate nfAstate2;
                nfAstate2.number = stateNumber - 1;

                NFA newNFA;
                newNFA.startState = stateNumber - 2;
                newNFA.statesMap.insert(pair<int, NFAstate>(nfAstate1.number, nfAstate1));
                newNFA.statesMap.insert(pair<int, NFAstate>(nfAstate2.number, nfAstate2));

                newNFA.endStatesMap = map<int, int>{ { nfAstate2.number,99 } };

                //PrintNFA(newNFA);
                helper.push(newNFA);

                i++;
                break;
            }
            default: {
                NFAstate nfAstate1;
                nfAstate1.number = stateNumber++;
                unordered_multimap<char, int> exEdgesMultiMap1{ { suffixForm[i],stateNumber++ } };
                nfAstate1.exEdgesMultiMap = exEdgesMultiMap1;

                NFAstate nfAstate2;
                nfAstate2.number = stateNumber - 1;

                NFA newNFA;
                newNFA.startState = stateNumber - 2;
                newNFA.statesMap.insert(pair<int, NFAstate>(nfAstate1.number, nfAstate1));
                newNFA.statesMap.insert(pair<int, NFAstate>(nfAstate2.number, nfAstate2));

                newNFA.endStatesMap = map<int, int>{ { nfAstate2.number,99 } };

                //PrintNFA(newNFA);

                helper.push(newNFA);
            }
        }


        //cout<<"current: "<<i<<endl;
        //cout<<"------------------"<<endl;

        i++;
    }

    NFA resNFA = helper.top();
    resNFA.endStatesMap[stateNumber - 1] = tokenType;

    helper.pop();
    return resNFA;
    //cout<<helper.size()<<endl;


}

// Conbine all mini NFAs according to their priority
NFA ConbineNFA(vector<NFA> allMiniNFA) {
    int i;
    NFA resNFA;

    if (allMiniNFA.size() == 0) return resNFA;

    resNFA = allMiniNFA[0];

    for (i = 1; i<allMiniNFA.size(); i++) {
        //resNFA.startState = stateNumber ++;
        NFAstate newNFAstate;
        newNFAstate.number = stateNumber++;

        int start1 = resNFA.startState;
        int start2 = allMiniNFA[i].startState;

        newNFAstate.exEdgesMultiMap.insert(pair<char, int>('@', start1));
        newNFAstate.exEdgesMultiMap.insert(pair<char, int>('@', start2));

        resNFA.startState = newNFAstate.number;
        resNFA.statesMap.insert(pair<int, NFAstate>(newNFAstate.number, newNFAstate));

        resNFA.statesMap.insert(allMiniNFA[i].statesMap.begin(), allMiniNFA[i].statesMap.end());

        resNFA.endStatesMap.insert(allMiniNFA[i].endStatesMap.begin(), allMiniNFA[i].endStatesMap.end());
    }

    return resNFA;
}

//Combine all mini NFAs using the method mentioned in dragon book which is more reasonable and efficient
NFA CombineNFA(vector<NFA> allMiniNFA) {
    int numMiniNFA = allMiniNFA.size();
    NFA resNFA;
    if (numMiniNFA == 0) return  resNFA;
    if (numMiniNFA == 1) return allMiniNFA[0];

    int i;
    //resNFA = allMiniNFA[0];
    NFAstate newNFAstate;
    newNFAstate.number = stateNumber++;

    resNFA.startState = newNFAstate.number;


    int currentStartState;
    for (i = 0; i<numMiniNFA; i++) {
        currentStartState = allMiniNFA[i].startState;
        newNFAstate.exEdgesMultiMap.insert(pair<char, int>('@', currentStartState));

        resNFA.statesMap.insert(allMiniNFA[i].statesMap.begin(), allMiniNFA[i].statesMap.end());
        resNFA.endStatesMap.insert(allMiniNFA[i].endStatesMap.begin(), allMiniNFA[i].endStatesMap.end());
    }

    resNFA.statesMap.insert(pair<int, NFAstate>(newNFAstate.number, newNFAstate));

    return resNFA;

}

// generate the whole NFA from given REset
/*
* vector<pair<string,int> > means that
* string refers to the pattern (standard form) of the token
* and int refers to the code number to the token
*/
NFA GenerateNFA(vector<pair<string, int> >& REset) {

    vector<NFA> allMiniNFA;

    for (auto& u : REset) {
        allMiniNFA.push_back(SuffixForm2NFA(StandardForm2SuffixForm(u.first), u.second));
    }

    /*for(auto& v:allMiniNFA){
    PrintNFA(v);
    }
    */
    NFA wholeNFA;
    wholeNFA = CombineNFA(allMiniNFA);

    //cout<<"whole NFA :"<<endl;
    //PrintNFA(wholeNFA);

    return wholeNFA;
}


#endif //LEX_RE2NFA_H
