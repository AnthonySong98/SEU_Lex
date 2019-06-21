//
// Created by mac on 2019/5/15.
//

#ifndef LEX_RE_STANDARDIZATION_H
#define LEX_RE_STANDARDIZATION_H


#include <iostream>
#include "helper.h"

#include <stdio.h>
//#include <tchar.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <fstream>

//将不规则的RE转换为规则的RE
//正规的RE只包括 * 连接(点) 或 |
//不正规的RE包括:
//1.分层定义——      (x)
//2.多字符或——      [xyz]和[^xyz]
//3.引号转义——      “xyz”
//4.斜线转义——      \n \t....
//5.任意单个字符——    .
//6.问号和加号——     x? x+    0或一个/大于等于1个

//最后添加连接 点
//

using namespace std;

using std::stack;
using std::vector;
using std::map;
using std::set;
using std::string;
using std::isalnum;
using std::isalpha;
using std::isdigit;
using std::cout;

static void reMap(string& exp, const map<string, string>& reMap);
static void replace_square_brace(string& exp, bool print = 1);
static void replace_question_and_add(string& exp);
static void replace_dot(string &exp);
static void add_dot(string &exp, bool);
void replace_escape(string& exp);

////static void construct_char_set(set<char> &s, const string &content, bool n);

//处理分层,将 alpha/digit/alphanum  按照map转化
static void reMap(string& exp, const map<string, string>& reMap) {
    //找到()里的内容, 如果与reMap匹配更换
    int flag = 1;     //是否继续修改
    while (flag == 1) {
        flag = 0;
        auto first_iter = reMap.begin();
        for (; first_iter != reMap.end(); first_iter++) {
            const string temp = first_iter->first;
            auto front = exp.find(temp);  //第一个下标
            while (front != exp.npos) {
                flag = 1;
                //替换exp内容
                auto replace = first_iter->second;
                int back = temp.size() + front;
                string s1 = exp.substr(0, front - 1);
                string s2 = exp.substr(back + 1);
                exp = s1 + first_iter->second + s2;
                front = exp.find(temp);
                first_iter = reMap.begin();
            }
        }

    }
    //cout << "reMap:  "<<exp << "\n";
    return;
}

//处理[a~z][abc]
static void replace_square_brace(string& exp, bool print) {
    if (exp == "[")
        return;

    auto front = exp.find("[");   //第一个[下标
    //查找front匹配的]下标
    int count = 0;
    if (front != exp.npos)
        count++;
    int back;
    for (back = front + 1; back<exp.size(); back++)
    {
        if (exp[back] == '[')
            count++;
        if (exp[back] == ']')
        {
            count--;
            if (count == 0)
                break;
        }
    }

    set<char>ALLCHAR;
    for (int i = 0; i<ALLSET.size(); i++)
        ALLCHAR.insert(ALLSET[i]);
    while (front != exp.npos) {
        //替换exp内容
        int SIZE = exp.size() - 1;
        bool have_not = false;
        bool first = true;
        std::string replace = "";
        set<char> replace_false;
        int iter = front + 1;
        if (exp[front + 1] == '^') {
            have_not = true;
            iter++;
        }
        for (; iter<back; iter++) {
            if (exp[iter + 1] != '~')
            {
                if (!have_not) {
                    if (first == false)
                        replace += "|";
                    if (exp[iter] == '.') {

                    }
                    replace += exp[iter];
                    first = false;
                }
                else
                    replace_false.insert(exp[iter]);
            }
            else if (exp[iter + 1] == '~') {   //下一个是-
                char left = exp[iter];
                char right = exp[iter + 2];
                iter += 2;        //下一次跳2步
                for (char temp = left; temp <= right; temp++) {
                    if (!have_not) {
                        if (first == false)
                            replace += "|";
                        replace += temp;
                        first = false;
                    }
                    else
                        replace_false.insert(temp);
                }
            }
        }
        string s1 = exp.substr(0, front);
        string s2 = exp.substr(back + 1);
        //没有^
        if (!have_not) {
            exp = s1 + "(" + replace + ")" + s2;
            front = exp.find("[");
            back = exp.find("]");
        }
        else {
            set<char>c;
            set_difference(ALLCHAR.begin(), ALLCHAR.end(), replace_false.begin(), replace_false.end(), inserter(c, c.begin()));    /*取交集运算*/
            string ss = "";
            auto iter = c.begin();


            for (; iter != c.end(); iter++)
            {
                int flag = 0;     //不修改
                if (*iter == '(' || *iter == ')' || *iter == '*' || *iter == '|')
                    flag = 1;
                if (flag)
                    ss = ss + '\\' + *iter;
                else
                    ss += *iter;
            }
            //将字符串变成 a|b
            string ss_new = "";
            for (int i = 0; i<ss.size(); i++) {
                if ((i != ss.size() - 1) && (ss[i] == '\\') && ((ss[i + 1] == '(') || (ss[i + 1] == ')') || (ss[i + 1] == '*') || (ss[i + 1] == '|'))) {
                    ss_new = ss_new + ss[i];
                    continue;
                }
                //\( \) \* \|
                ss_new = ss_new + ss[i];
                if (i != ss.size() - 1)
                    ss_new = ss_new + "|";
            }

            exp = s1 + "(" + ss_new + ")" + s2;
            int offset = exp.size() - SIZE;
            front = exp.find("[", offset);
            //匹配back
            int count = 0;
            if (front != exp.npos) {
                count++;
                for (back = front + 1; back<exp.size(); back++)
                {
                    if (exp[back] == '[')
                        count++;
                    if (exp[back] == ']')
                    {
                        count--;
                        if (count == 0)
                            break;
                    }
                }
                //匹配back}
            }

        }

    }
    if (print == 1) {
        //cout << "squre:  "<<exp << "\n";
    }

    return;
}

//a?    (@|a)       (ab)?   (@|ab)
//a+    a(a)*     (ab)*   (ab)(ab)*
static void replace_question_and_add(string& exp) {
    if (exp == "?" || exp == "+" || exp == "+=")
        return;
    for (int i = 0; i<exp.size(); i++) {
        int SIZE = exp.size();
        if (exp[i] == '?') {
            if (i != 0 && exp[i - 1] == '|')//为全集合的问号
                continue;
            if (i != exp.size() - 1 && exp[i + 1] == '|')
                continue;

            int question = i;
            //替换exp内容
            int left, right;
            std::string replace = "";
            int iter = question - 1;
            if (exp[iter] != ')') {
                //a?
                left = iter - 1;
                right = iter + 2;
                replace = exp[iter];
            }
            else {
                right = iter + 2;
                int count = 1;
                for (left = iter - 1; count != 0; left--) {
                    if (exp[left] == '(') {
                        count--;
                        if (count != 0)
                            replace = exp[left] + replace;
                        continue;
                    }
                    if (exp[left] == ')')
                        count++;
                    replace = exp[left] + replace;
                }
            }
            string s1 = exp.substr(0, left + 1);
            string s2 = exp.substr(right);
            if (left<0)
                s1 = "";
            if (right>exp.size() - 1)
                s2 = "";
            exp = s1 + "(@|" + replace + ")" + s2;
            int offset = exp.size() - SIZE;
            i += offset;
        }
        else if (exp[i] == '+') {
            if (i != 0 && exp[i - 1] == '|')//为+-的加号
                continue;
            if (i != exp.size() - 1 && exp[i + 1] == '|')//为+-的加号
                continue;
            auto add = i;
            //替换exp内容
            int flag = 1;         //表示无括号
            int left, right;
            std::string replace = "";
            int iter = add - 1;
            if (exp[iter] != ')') {
                //a?
                left = iter - 1;
                right = iter + 2;
                replace = exp[iter];
            }
            else {
                flag = 2;
                right = iter + 2;
                int count = 1;
                for (left = iter - 1; count != 0; left--) {
                    if (exp[left] == '(') {
                        count--;
                        if (count != 0)
                            replace = exp[left] + replace;
                        continue;
                    }
                    if (exp[left] == ')')
                        count++;
                    replace = exp[left] + replace;
                }
            }
            string s1 = exp.substr(0, left + 1);
            string s2 = exp.substr(right);
            if (left<0)
                s1 = "";
            //        if(right>exp.size()-1)
            //            s2="";
            if (flag == 2)
                replace = "(" + replace + ")";
            exp = s1 + replace + replace + "*" + s2;
            int offset = exp.size() - SIZE;
            i += offset;
        }
    }
    //cout << "?/+:  "<<exp << "\n";
    return;
}
//将.变成全集
static void replace_dot(string &exp) {
    string s = ALLSET;
    //    replace_square_brace(s,0);
    int count = 0;//表示[ - ]个数
    for (int i = 0; i<exp.size(); i++) {
        int SIZE = exp.size();
        if (exp[i] == '[')
            count++;
        if (exp[i] == ']')
            count--;
        if (exp[i] == '.'&&count != 0) {
            //替换exp内容
            int left, right;
            left = i;
            right = i + 1;
            string s1 = exp.substr(0, left);
            string s2 = exp.substr(right);
            if (left<0)
                s1 = "";
            if (right>exp.size() - 1)
                s2 = "";
            exp = s1 + s + s2;
            int offset = exp.size() - SIZE;
            i += offset;
        }
    }
    //cout << "ALLSET:  "<<exp << "\n";
}


// 最后加点
void  add_dot(string &exp, bool output = 1) {
    if (exp == "||") {
        exp = "|`|";
        //cout << "ESCAPE:" << exp << "\n";
        return;
    }
    string oldExp = exp;
    string dotedExp;
    for (auto oldExpIt = oldExp.cbegin(); oldExpIt != oldExp.cend(); ++oldExpIt) {
        dotedExp += (*oldExpIt);
        if (*oldExpIt == '\\')continue;// 是转义字符，直接跳过
        if ((*oldExpIt == '(' || *oldExpIt == '|') && (oldExpIt == oldExp.cbegin() || *(oldExpIt - 1) != '`'))
            continue;// |或(,且前面不是`
        if ((oldExpIt + 1) == oldExp.cend()) continue; //最后一个字符，不加点
        if (*(oldExpIt + 1) == '|' || *(oldExpIt + 1) == '*' || *(oldExpIt + 1) == ')') continue; //下一个字符已经是操作符或右括号，不加点
        dotedExp += '`';//加点
    }
    exp = dotedExp;
    //if(output)
    //cout << "ADD DOT:" << exp << "\n";
}


//加点后 处理\转义字符
// 先对所有的\变成 \\
// 之后对（）
void replace_escape(string& exp) {
    if (exp == "(" || exp == ")" || exp == "*") {
        exp = "\\" + exp;
        //cout << "ESCAPE:" << exp << "\n";
        return;
    }
    if (exp == "|`|") {
        exp = "\\|`\\|";
        //cout << "ESCAPE:" << exp << "\n";
        return;
    }
    if (exp == "*`=") {
        exp = "\\*`=";
        //cout << "ESCAPE:" << exp << "\n";
        return;
    }
    if (exp == "|`=") {
        exp = "\\|`=";
        //cout << "ESCAPE:" << exp << "\n";
        return;
    }
    for (int i = 0; i<exp.size(); i++) {
        int flag = 1;
        int SIZE = exp.size();
        std::string replace;
        switch (exp[i]) {
            case '\n':
                replace = "\\n";     //双反斜杠
                break;
            case '\t':
                replace = "\\t";
                break;
            case '\v':
                replace = "\\v";
                break;
            case '\f':
                replace = "\\f";
                break;
            case '\'':
                replace = "\\'";
                break;

                //            case '\\':
                //                if(i!=exp.size()-1&&exp[i+1]=='|')
                //                    //将\\| 仍然保留为 \\|
                //                replace="\\\\";
                //                break;
            default: {
                flag = 0;
                break;
            }
        }
        if (flag == 1) {        //如果修改过
            //替换exp内容
            int left, right;
            left = i - 1;
            right = i + 1;
            string s1 = exp.substr(0, left + 1);
            string s2 = exp.substr(right);
            if (left<0)
                s1 = "";
            if (right>exp.size() - 1)
                s2 = "";
            exp = s1 + replace + s2;
            int offset = exp.size() - SIZE;
            i += offset;
        }

    }

    //cout << "ESCAPE:   " << exp << "\n";
}

void re_standardize_1(std::string e, map<string, string>& M) {
    //先处理map里面的
    //    for (auto & p : reMap) {
    //        handle_quote(p.second);
    //        replace_brace(p.second, reMap);
    //    }
    //再处理Vector里面的
    //    handle_quote(e);
    reMap(e, M);
    replace_dot(e);
    replace_square_brace(e);
    replace_question_and_add(e);
    add_dot(e);
    replace_escape(e);
}


//for the most complex RE for block comment
//please refer to http://blog.ostermiller.org/find-comment
//for RE for string
//please refer to https://blog.csdn.net/xfxyy_sxfancy/article/details/45024573
void re_Standardize(vector<Rules>& Vec, map<string, string>& M) {
    //处理Vec里面的
    for (auto & e : Vec) {
        if (e.pattern == "/\\*([^*]|\\n|(\\*+([^*/]|\\n)))*\\*+/") {
            e.pattern = "/`\\*`((\\t|\\n| |!|#|%|&|\\'|\\(|\\)|\"|+|,|-|.|/|0|1|2|3|4|5|6|7|8|9|:|;|<|=|>|?|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|[|]|^|_|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|{|\\||}|~)|\\n|((\\*)`(\\*)*`((\\t|\\n| |!|#|%|&|\\'|\\(|\\)|\"|+|,|-|.|0|1|2|3|4|5|6|7|8|9|:|;|<|=|>|?|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|[|]|^|_|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|{|\\||}|~)|\\n)))*`(\\*)`(\\*)*`/";
            continue;
        }
        if (e.pattern == "[\\n\\t ]+"){
            e.pattern = "(\\n|\\t| )`(\\n|\\t| )*";
            continue;
        }
        if (e.pattern == "++"){
            e.pattern = "+`+";
            continue;
        }
        if (e.pattern == "|="){
            e.pattern = "\\|`=";
            continue;
        }
        if (e.pattern == "|"){
            e.pattern = "\\|";
            continue;
        }
        //cout<<e.pattern<<std::endl;
        reMap(e.pattern, M);
        replace_dot(e.pattern);
        replace_square_brace(e.pattern);
        replace_question_and_add(e.pattern);
        add_dot(e.pattern);
        replace_escape(e.pattern);
        //cout<<std::endl;
    }
}


/*
int main(){
map<string, string> M = {
{"number" ,"{digit}+(.{digit}+)?(E[+-]?{digit}+)?"},
{"id" , "{letter}({letter}|{digit})*"},
{"alpha"   , "[A-Za-z]"},
{"digit"    ,"[0-9]"},
{"alphanum"  ,"[A-Za-z0-9]"}
};
string s="{digit}?";
cout<<"s:  "<<s<<std::endl<<std::endl;
re_standardize_1(s,M);
cout<<"_____________________\n";
s="[a-z]+";
cout<<"s:  "<<s<<std::endl<<std::endl;
re_standardize_1(s,M);
return 0;
}

*/


#endif //LEX_RE_STANDARDIZATION_H
