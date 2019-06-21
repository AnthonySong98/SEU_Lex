//
// Created by mac on 2019/5/17.
//

#ifndef LEX_DFAMINIMIZATION_H
#define LEX_DFAMINIMIZATION_H

#include "helper.h"
#include <iostream>

using namespace std;

void DFAMinimization(DFA&);
void splitDFASets(vector<unordered_set<int> > &, DFA&);
static unordered_map<int, int> statesSetsMap;	//当前子集在哪一个集合中
static vector<unordered_set<int> > statesSetsVec;	//DFA状态集分解结果
static int counter2;			//最小化DFA数

void DFAMinimization(DFA&dfa) {
    queue<unordered_set<int> > statesQueue;	//需进一步分解的DFA集合队列
    queue<bool> stateTryCount;
    unordered_set<int> states;
    int cntEndState;			//终态数目
    counter2 = 0;
    statesSetsMap.clear();
    statesSetsVec.clear();
    //所有DFA终态分开，从0开始编号
    for (auto& dfaEndState : dfa.endStatesMap)
        statesSetsMap[dfaEndState.first] = counter2++;		//所有DFA终态为独立的状态
    cntEndState = counter2; //前面的状态均为终态
    //所有非终态DFA状态先放入一个集合中
    for (auto& dfaState : dfa.statesVec)
        if (statesSetsMap.count(dfaState.number) == 0) {
            statesSetsMap.insert(std::make_pair(dfaState.number, counter2));
            states.insert(dfaState.number);
        }
    counter2++;
    statesQueue.push(states);
    stateTryCount.push(true);  //两次尝试,保证可以“回头看”

    //直到不会产生新的DFA状态集合的划分为止
    while (!statesQueue.empty()) {
        unordered_set<int> states = statesQueue.front();
        statesQueue.pop();
        vector<unordered_set<int> > split_set;
        split_set.push_back(states);
        splitDFASets(split_set, dfa);

        if (split_set.size() > 1) {//可以进一步分解DFA集合
            for (auto& sets : split_set)
                if (sets.size()>1) {
                    statesQueue.push(sets);
                    stateTryCount.push(true);
                }
        }
        else {//无法进一步分解，尝试保留，等待“回头看”
            if (stateTryCount.front()) {
                statesQueue.push(states);
                stateTryCount.push(false);
            }
        }
        stateTryCount.pop();
    }


    //根据DFA集合划分情况建立新的DFA
    vector<vector<int> > newDFAStates;
    vector<int> newStates;
    newStates.clear();
    for (int i = 0; i < counter2; i++)
        newDFAStates.push_back(newStates);
    //根据对每一个DFA状态划分的标号归入新DFA状态
    for (auto& dfaState : dfa.statesVec)
        newDFAStates[statesSetsMap[dfaState.number]].push_back(dfaState.number);
    //新建DFA
    DFA newdfa; //每个终态划分中仅包含一个原DFA终态
    newdfa.startState = statesSetsMap[dfa.startState];
    for (int i = 0; i < counter2; i++) {
        DFAstate newState;
        //记录DFA0终态
        if (i < cntEndState)
            newdfa.endStatesMap[i] = dfa.endStatesMap[newDFAStates[i][0]];
        newState.number = i;
        for (auto& dfa_i : newDFAStates[i]) { //所包含的NFA状态set合并
            newState.identitySet.insert(dfa.statesVec[dfa_i].identitySet.begin(), dfa.statesVec[dfa_i].identitySet.end());
            for (auto& edge : dfa.statesVec[dfa_i].exEdgesMap)
                if (newState.exEdgesMap.count(edge.first) == 0) { //新边指向新DFA划分编号
                    newState.exEdgesMap[edge.first] = statesSetsMap[edge.second];
                }
        }
        newdfa.statesVec.push_back(newState);
    }

    //替代原来的DFA
    dfa.endStatesMap = newdfa.endStatesMap;
    dfa.startState = newdfa.startState;
    dfa.statesVec.clear();
    for (auto& state : newdfa.statesVec)
        dfa.statesVec.push_back(state);
}


void splitDFASets(vector<unordered_set<int> > &split_set, DFA& dfa) {
    bool fa = false, f;			//表示是否可以进一步划分/是否在已有划分中可匹配到相应集合
    vector<int> mapSetNumber;	//对应下标映射到的DFA集合编号
    int mapNumber, sz;			//当前DFA集合中DFA状态出边指向的DFA集合编号
    unordered_set<int> states;	//用来保存经过修改的split_set[0]
    const string edgeSet(ALLSET);//设置边的全集
    states = split_set[0];

    for (const char& ch : edgeSet) {
        if (fa)break;  //产生新的划分后便退出，每次新产生的划分数有限
        mapSetNumber.clear();
        for (auto& dfa_i : states) {
            //cout << "----" << dfa_i << endl;
            if (dfa.statesVec[dfa_i].exEdgesMap.count(ch) == 0)mapNumber = -1;//代表该出边不存在
            else mapNumber = dfa.statesVec[dfa_i].exEdgesMap[ch];
            if (mapSetNumber.empty())mapSetNumber.push_back(mapNumber);
            if (mapNumber != mapSetNumber[0]) {//存在DFA状态出边ch到达的DFA状态集合编号不一致,会产生新的划分
                split_set[0].erase(dfa_i);		//剔除不一致的DFA状态
                fa = true;
                f = false;
                sz = mapSetNumber.size();
                for (int i = 1; i < sz; i++)
                    if (mapNumber == mapSetNumber[i]) {
                        statesSetsMap[dfa_i] = counter2 - (sz - i);
                        split_set[i].insert(dfa_i);
                        f = true; break;
                    }
                if (!f) {//未找到匹配的划分
                    unordered_set<int> newStateSet;//新建划分
                    newStateSet.insert(dfa_i);
                    split_set.push_back(newStateSet);
                    mapSetNumber.push_back(counter2);
                    //更新DFA集合计数器
                    statesSetsMap[dfa_i] = counter2++;
                }
            }
            /*
            cout << endl;
            for (auto& dfa_ii : split_set[0])
            cout << " " << dfa_ii; cout << endl;
            for (auto& dfa_ii : states)
            cout << " " << dfa_ii; cout << endl;
            cout << "--" << dfa_i<<endl;

            */
        }
    }

}

#endif //LEX_DFAMINIMIZATION_H
