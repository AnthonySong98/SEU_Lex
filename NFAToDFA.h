//
// Created by mac on 2019/5/17.
//

#ifndef LEX_NFATODFA_H
#define LEX_NFATODFA_H

#include "helper.h"
#include <iostream>
#include <fstream>

using namespace std;

static int numOfNFAState;
static int counter;  //DFA计数器
//DFA转换为DFA
void NFAToDFA(const NFA&, DFA&);
//通过子集构造法求解子集
void subset_construction(const unordered_set<int>&, unordered_set<int>&, const char, const unordered_map<int, NFAstate>&);
//求epsilon闭包
void epsilon_closure(unordered_set<int>&, const unordered_map<int, NFAstate>&);
//选择Action
bool chooseAction(const DFAstate&, const map<int, int>&, int&);
//输出DFA
void printDFA(DFA&dfa,string);

void NFAToDFA(const NFA& nfa, DFA& dfa) {
    queue<int> queueDFA;		//  存放未处理DFA状态的编号
    const string edgeSet(ALLSET);//设置边的全集
    numOfNFAState = nfa.statesMap.size();
    counter = 0;
    //加入DFA开始状态
    DFAstate state;
    state.number = counter++;
    state.identitySet.insert(nfa.startState);
    epsilon_closure(state.identitySet, nfa.statesMap);
    dfa.endStatesMap.clear();
    dfa.startState = state.number;
    dfa.statesVec.push_back(state);
    unordered_set<int> tempSet;

    queueDFA.push(state.number);

    while (!queueDFA.empty()) {
        //cout << "generating" << endl;
        int number = queueDFA.front();
        queueDFA.pop();
        for (const char& ch : edgeSet) {//遍历字符集
            tempSet.clear();
            //cout << "Subset construction" << endl;
            subset_construction(dfa.statesVec[number].identitySet, tempSet, ch, nfa.statesMap);
            if (tempSet.size() == 0)continue;
            //cout << "epsilon_closure construction" << endl;
            epsilon_closure(tempSet, nfa.statesMap);
            bool have = false;
            int ext_number = -1;
            for (auto& state : dfa.statesVec)  //确认子集是否已经存在
                if (state.identitySet == tempSet) {
                    have = true;
                    ext_number = state.number;
                    break;
                }
            if (have) {
                dfa.statesVec[number].exEdgesMap[ch] = ext_number;
                continue;
            }
            //若为新的子集则增加新的DFA状态
            state.number = counter++;
            state.identitySet = tempSet;
            queueDFA.push(state.number);
            dfa.statesVec.push_back(state);
            //该条出边连接到新的子集
            dfa.statesVec[number].exEdgesMap[ch] = state.number;
        }
    }

    //cout << "confirm endState" << endl;
    //寻找DFA终态并确定action
    for (auto dfa_i : dfa.statesVec) {
        int action;
        if (chooseAction(dfa_i, nfa.endStatesMap, action))
            dfa.endStatesMap.insert(std::make_pair(dfa_i.number, action));
    }

    //cout << "NFA To DFA complete!" << endl;
}


void subset_construction(const unordered_set<int> &initialStates, unordered_set<int>&targetStates
        , const char ch, const unordered_map<int, NFAstate>& NFAStatesMap) {
    for (auto& state : initialStates) {
        auto range_it = NFAStatesMap.find(state)->second.exEdgesMultiMap.equal_range(ch);
        auto Itbe = range_it.first, Ited = range_it.second;
        if (Itbe == Ited) continue;		//该NFA状态没有对应ch的出边
        while (Itbe != Ited) {			//加入所有转移状态
            targetStates.insert(Itbe->second);
            ++Itbe;
        }
    }
}


void epsilon_closure(unordered_set<int>&initialSet, const unordered_map<int, NFAstate>&NFAStatesMap) {
    queue<int> nfaStateSet;
    vector<bool> ifhandled(numOfNFAState + 1);
    for (auto nfa_i : initialSet) {
        nfaStateSet.push(nfa_i);
        ifhandled[nfa_i] = true;
    }
    while (!nfaStateSet.empty()) {
        int nfa_i = nfaStateSet.front();
        nfaStateSet.pop();
        auto range_it = NFAStatesMap.find(nfa_i)->second.exEdgesMultiMap.equal_range('@');
        auto Itbe = range_it.first, Ited = range_it.second;
        while (Itbe != Ited) {			//加入所有新的状态
            //当前epsilon闭包中没有该状态，新加入队列
            int unhandled = Itbe->second;
            //cout << unhandled << endl;
            if (!ifhandled[unhandled]) {
                nfaStateSet.push(unhandled);
                initialSet.insert(unhandled);
                ifhandled[unhandled] = true;
            }
            ++Itbe;
        }
    }
}

bool chooseAction(const DFAstate& state, const map<int, int>& nfaEndStatesMap, int& action) {
    const int maxinum = 2147483647;
    int mininum = maxinum;	//保存最小NFA状态编号(优先级最高 )
    pair<int, int> state_action(0, 0);
    for (auto nfa_i : state.identitySet) {
        auto k_v = nfaEndStatesMap.find(nfa_i);
        if (k_v == nfaEndStatesMap.end())continue;
        if (mininum > k_v->first) {
            mininum = k_v->first;
            state_action = std::make_pair(k_v->first, k_v->second);
        }
    }
    if (maxinum == mininum)return false;
    //否则该DFA状态为终态
    action = state_action.second;
    return true;
}

void printDFA(DFA&dfa,string s) {
    ofstream fout(s);
    //number of end state
    fout << dfa.endStatesMap.size() << endl;
    //list of end states and their action
    for (auto& s : dfa.endStatesMap)
        fout << s.first << " " << s.second << endl;
    //startstate
    fout << dfa.startState << endl;
    //list of all DFA states and their number & NFA states set & edge
    for (auto& s : dfa.statesVec) {
        fout << s.number << endl;
        for (auto& i : s.identitySet)
            fout << " "<< i;
        fout << endl;
        //char int
        for (auto& i : s.exEdgesMap)
            fout << i.first << "  " << i.second << endl;
        fout << endl;
    }

}



#endif //LEX_NFATODFA_H
