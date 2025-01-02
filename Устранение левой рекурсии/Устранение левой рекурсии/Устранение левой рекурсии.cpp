﻿#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <set>
#include <algorithm>

using namespace std;
map<string, vector<string> > Grammar;
vector<string> Variables = {};
vector<string> Terminals = {
"+", "-", "^", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
"x", "t", "/", "ln(", "(", ")", "c", "s", "tg(", "os(", "qrt(", 
"in(", "g(", "exp("};
vector<string> NotTerminals = {
"A", "B", "C", "D", "E", "F", "G", "H", "I",
"J", "K", "L", "M", "N", "P", "Q", "T" };
string StartNotTerminal = "E";
map<string, set<string>>FIRST;
map<string, set<string>>FOLLOW;

set<string> fromTokensFIRST(vector<string>& tokens)
{
    set<string> first;
    bool haveEpsilon = true;
    for (int i = 0; i < tokens.size() && haveEpsilon; i++)
    {
        haveEpsilon = false;
        for (auto s : FIRST[tokens[i]])
        {
            haveEpsilon = haveEpsilon || s.empty();
            if (!s.empty())
                first.insert(s);
        }
    }
    if (haveEpsilon)
        first.insert("");
    return first;
}

void printFIRST()
{
    for (auto p : FIRST)
    {
        cout << p.first << ": {";
        if (p.second.size() > 0)
        {
            for (set<string> ::iterator it = p.second.begin(); it != --p.second.end(); it++)
            {
                if ((*it).empty())
                    cout << "eps";
                else
                    cout << *it;
                cout << ", ";
            }
            if ((*(--p.second.end())).empty())
                cout << "eps";
            else
                cout << *(--p.second.end());
        }
        cout << "}\n";
    }
}

void printFOLLOW()
{
    for (auto p : FOLLOW)
    {
        cout << p.first << ": {";
        if (p.second.size() > 0)
        {
            for (set<string> ::iterator it = p.second.begin(); it != --p.second.end(); it++)
            {
                if ((*it).empty())
                    cout << "eps";
                else
                    cout << *it;
                cout << ", ";
            }
            if ((*(--p.second.end())).empty())
                cout << "eps";
            else
                cout << *(--p.second.end());
        }
        cout << "}\n";
    }
}

set<string> intersectionOfSets(set<string> a, set<string> b)
{
    set<string>intersectionSet;
    while (!a.empty() && !b.empty())
    {
        if (*a.begin() < *b.begin())
            a.erase(a.begin());
        else if (*a.begin() > *b.begin())
            b.erase(b.begin());
        else
        {
            intersectionSet.insert(*a.begin());
            a.erase(a.begin());
            b.erase(b.begin());
        }
    }
    return intersectionSet;
}

map<string, vector<vector<string>>> tokensFromGrammar(map<string, vector<string>>& grammar,
    vector<string>& terminals, vector<string>& notTerminals)
{
    map<string, vector<vector<string>>>tokensGrammar;
    for (auto notTerminal : notTerminals)
    {
        for (auto prod : grammar[notTerminal])
        {
            vector<string>tokens;
            int inx = 0;
            while (inx < prod.size())
            {
                string choosenTerminal = "";
                string choosenNotTerminal = "";
                for (auto terminal : terminals)
                {
                    if (inx + terminal.size() <= prod.size()
                        && prod.substr(inx, terminal.size()) == terminal
                        && terminal.size() > choosenTerminal.size())
                        choosenTerminal = terminal;
                }
                if (!choosenTerminal.empty())
                {
                    tokens.push_back(choosenTerminal);
                    inx += choosenTerminal.size();
                }
                for (auto notTerminal : notTerminals)
                {
                    if (inx + notTerminal.size() <= prod.size()
                        && prod.substr(inx, notTerminal.size()) == notTerminal)
                        choosenNotTerminal = notTerminal;
                }
                if (!choosenNotTerminal.empty())
                {
                    tokens.push_back(choosenNotTerminal);
                    inx += choosenNotTerminal.size();
                }
            }
            tokensGrammar[notTerminal].push_back(tokens);
        }
    }
    return tokensGrammar;
}

bool checkLL(map<string, vector<string>>& grammar, vector<string>& terminals,
    vector<string>& notTerminals)
{
    map<string, vector<vector<string>>>tokensGrammar = 
        tokensFromGrammar(grammar, terminals, notTerminals);
    bool isLL = true;
    for (auto p : tokensGrammar)
    {
        for (int i = 0; i < p.second.size(); i++)
        {
            set<string>alphaFirst = fromTokensFIRST(p.second[i]);
            for (int j = i + 1; j < p.second.size(); j++)
            {
                set<string>bettaFirst = fromTokensFIRST(p.second[j]);
                set<string>intersectionResult = intersectionOfSets(alphaFirst, bettaFirst);
                /* if (!intersectionResult.empty())
                {
                    cout << p.first << "\n";
                    for (auto s : p.second[j])
                        cout << s << " ";
                    cout << "\n";
                    for (auto s : p.second[i])
                        cout << s << " ";
                    cout << "\n---------------------\n";
                }*/
                isLL = isLL && intersectionResult.empty();
                if (alphaFirst.find("") != alphaFirst.end())
                {
                    intersectionResult = intersectionOfSets(bettaFirst, FOLLOW[p.first]);
                    /* if (!intersectionResult.empty())
                    {
                        cout << "******" << p.first << "\n";
                        for (auto s : bettaFirst)
                            cout << s << " ";
                        cout << "\n";
                        for (auto s : FOLLOW[p.first])
                            cout << s << " ";
                        cout << "\n---------------------\n";
                    }*/
                    isLL = isLL && intersectionResult.empty();
                }
            }
        }
    }
    return isLL;
}

void fillFOLLOW(map<string, vector<string>>& grammar, vector<string>& terminals,
    vector<string>& notTerminals, string& startNotTerminal)
{
    FOLLOW[startNotTerminal].insert("$");
    map<string, vector<vector<string>>>tokensGrammar =
        tokensFromGrammar(grammar, terminals, notTerminals);
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto notTerminal : notTerminals)
        {
            for (auto tokens : tokensGrammar[notTerminal])
            {
                vector<string>afterTokens = tokens;
                reverse(afterTokens.begin(), afterTokens.end());
                for (int i = 0; i < tokens.size(); i++)
                {
                    afterTokens.pop_back();
                    if (tokensGrammar.find(tokens[i]) != tokensGrammar.end())
                    {
                        reverse(afterTokens.begin(), afterTokens.end());
                        set<string> gettedFirst = fromTokensFIRST(afterTokens);
                        reverse(afterTokens.begin(), afterTokens.end());
                        int startCount = FOLLOW[tokens[i]].size();
                        for (auto s : gettedFirst)
                        {
                            if (!s.empty())
                                FOLLOW[tokens[i]].insert(s);
                        }
                        if (gettedFirst.find("") != gettedFirst.end())
                        {
                            for (auto s : FOLLOW[notTerminal])
                                FOLLOW[tokens[i]].insert(s);
                        }
                        flag = flag || startCount < FOLLOW[tokens[i]].size();
                    }
                }
            }
        }
    }
}

void fillFIRST(map<string, vector<string>>& grammar, 
    vector<string>& terminals, vector<string>& notTerminals)
{
    for (auto s : terminals)
        FIRST[s].insert(s);
    map<string, vector<vector<string>>>tokensGrammar = 
        tokensFromGrammar(grammar, terminals, notTerminals);
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto notTerminal : notTerminals)
        {
            int startCount = FIRST[notTerminal].size();
            for (auto tokens : tokensGrammar[notTerminal])
            {
                bool haveEpsilon;
                for (int i = 0; i < tokens.size(); i++)
                {
                    haveEpsilon = true;
                    for (int j = 0; j < i && haveEpsilon; j++)
                        haveEpsilon = FIRST[tokens[j]].find("") != FIRST[tokens[j]].end();
                    if (haveEpsilon)
                    {
                        for (auto a : FIRST[tokens[i]])
                            FIRST[notTerminal].insert(a);
                    }
                }
                haveEpsilon = true;
                for (int i=0; i<tokens.size() && haveEpsilon; i++)
                    haveEpsilon = FIRST[tokens[i]].find("") != FIRST[tokens[i]].end();
                if (haveEpsilon)
                    FIRST[notTerminal].insert("");
            }
            flag = flag || startCount < FIRST[notTerminal].size();
        }
    }
}

void initGrammar()
{
    Grammar["F"] = { "C", "N", "(E)", "-F", "sqrt(E)",
        "sin(E)", "cos(E)", "tg(E)", "ctg(E)", "-F", "ln(E)", "exp(E)"};
    Grammar["T"] = { "F", "TF", "T/F" };
    Grammar["E"] = { "T", "E+T", "E-T", "E^T" };
    Grammar["C"] = { "NC", "x", "t"};
    Grammar["N"] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    for (char i = '0'; i <= '9'; i++)
    {
        string s = "N";
        s += i;
        Grammar["N"].push_back(s);
    }
}

void printGrammar(map<string, vector<string>>& grammar)
{
    for (auto elem : grammar)
    {
        cout << elem.first << "-->";
        for (int i=0; i<elem.second.size() - 1; i++)
            cout << elem.second[i] << "|";
        cout << elem.second.back() << "\n";
    }
}

void initVariables()
{
    for (char i = 'A'; i <= 'Z'; i++)
    {
        if (i == 'O') continue;
        string s = "";
        s += i;
        Variables.push_back(s);
    }
    for (char i = 'A'; i <= 'Z'; i++)
    {
        if (i == 'O') continue;
        string s = "1";
        s = i + s;
        Variables.push_back(s);
    }
}

void deleteDirectRecursion(string recursionVariable, map<string, vector<string>>& grammar)
{
    vector<string>afterVar(0);
    vector<string>beforeVar(0);
    for (auto elem : grammar[recursionVariable])
    {
        if (elem.empty())
        {
            beforeVar.push_back(elem);
            continue;
        }
        bool flag = true;
        for (int inx = 0; inx < min(elem.size(), recursionVariable.size()) && flag; inx++)
            flag = elem[inx] == recursionVariable[inx];
        if (!flag) beforeVar.push_back(elem);
        else afterVar.push_back(elem.substr(min(elem.size(), recursionVariable.size())));
    }
    if (afterVar.empty()) 
        return;
    for (auto var : Variables)
    {
        if (grammar.find(var) == grammar.end())
        {
            for (int i = 0; i < beforeVar.size(); i++)
                beforeVar[i] += var;
            for (int i = 0; i < afterVar.size(); i++)
                afterVar[i] = afterVar[i] + var;
            afterVar.push_back("");
            if (beforeVar.empty())
                beforeVar.push_back(var);
            grammar[recursionVariable] = beforeVar;
            grammar[var] = afterVar;
            return;
        }
    }
}

void clearLeftRecursion(map<string, vector<string>>& grammar)
{
    vector<string> variables(0);
    for (auto p : grammar) variables.push_back(p.first);
    for (int i=0; i<variables.size(); i++)
    {
        for (int j = 0; j < i; j++) 
        {
            int count = 0;
            vector<string>newGrammarVector(0);
            for (int k=0; k < grammar[variables[i]].size(); k++)
            {
                string elem = grammar[variables[i]][k];
                if (elem.empty())
                {
                    newGrammarVector.push_back(grammar[variables[i]][k]);
                    continue;
                }
                bool flag = true;
                for (int inx = 0; inx < min(elem.size(), variables[j].size()) && flag; inx++)
                    flag = elem[inx] == variables[j][inx];
                if (flag)
                {
                    string rightPart = elem.substr(min(elem.size(), variables[j].size()));
                    for (auto q : grammar[variables[j]])
                    {
                        string s = q + rightPart;
                        newGrammarVector.push_back(s);
                    }
                }
                else
                    newGrammarVector.push_back(grammar[variables[i]][k]);
            }
            grammar[variables[i]] = newGrammarVector;
        }
        deleteDirectRecursion(variables[i], grammar);
    }
}

void replaceProduction(string& variable, map<string, vector<string>>& grammar, string& prefix)
{
    string newVar;
    for (auto var : Variables)
    {
        if (grammar.find(var) == grammar.end())
        {
            newVar = var;
            break;
        }
    }
    vector<string>newGrammarVector(0);
    vector<string>newVarGrammarVector(0);
    for (int i = 0; i < grammar[variable].size(); i++)
    {
        bool flag = grammar[variable][i].size() >= prefix.size();
        for (int j = 0; j < prefix.size() && flag; j++)
            flag = prefix[j] == grammar[variable][i][j];
        if (flag)
            newVarGrammarVector.push_back(grammar[variable][i].substr(prefix.size()));
        else
            newGrammarVector.push_back(grammar[variable][i]);
    }
    for (auto p : grammar)
    {
        if (p.second == newVarGrammarVector)
        {
            newGrammarVector.push_back(prefix + p.first);
            grammar[variable] = newGrammarVector;
            return;
        }
    }
    newGrammarVector.push_back(prefix + newVar);
    grammar[newVar] = newVarGrammarVector;
    grammar[variable] = newGrammarVector;
}

void leftFactorisation(map<string, vector<string>>& grammar)
{
    for (map<string, vector<string>> ::iterator it = grammar.begin(); it != grammar.end(); it++)
    {
        int count = (int)1e9;
        while (count > 1)
        {
            count = 1;
            string prefix = "";
            map<string, int>prefixCountMap;
            for (int i = 0; i < it->second.size(); i++)
            {
                string s = "";
                for (int j = 0; j < it->second[i].size(); j++)
                {
                    s += it->second[i][j];
                    prefixCountMap[s]++;
                }
            }
            for (auto p : prefixCountMap)
            {
                if (p.second > 1 && p.first.size() > prefix.size())
                {
                    count = p.second;
                    prefix = p.first;
                }
            }
            if (count > 1)
            {
                string currentVariable = it->first;
                replaceProduction(currentVariable, grammar, prefix);
            }
        }
    }
}

void getLLFromMyGrammar()
{
    clearLeftRecursion(Grammar);
    leftFactorisation(Grammar);
    Grammar["G"].pop_back();
    Grammar["L"].erase(Grammar["L"].begin());
    clearLeftRecursion(Grammar);
    leftFactorisation(Grammar);
    fillFIRST(Grammar, Terminals, NotTerminals);
    fillFOLLOW(Grammar, Terminals, NotTerminals, StartNotTerminal);
    if (checkLL(Grammar, Terminals, NotTerminals))
        cout << "Success!\n";
    else
        cout << "Fail!\n";
}

int main()
{
    initGrammar();
    initVariables();
    map<string, vector<string>>tmpGrammar;
    // tmpGrammar["E"] = { "TA" };
    // tmpGrammar["A"] = { "+TA", "" };
    // tmpGrammar["T"] = { "FB" };
    // tmpGrammar["B"] = { "*FB", "" };
    // tmpGrammar["F"] = { "(E)", "id" };
    // tmpGrammar["A"] = { "(A)A", "" };
    // tmpGrammar["B"] = { "BB", "(B)", "" };
    vector<string>notTerminals;
    vector<string>terminals;
    notTerminals = {"E", "A", "T", "B", "F"};
    terminals = { "+", "*", "(", ")", "id" };
    string startNotTerminal = "E";
    getLLFromMyGrammar();
    // printGrammar(Grammar);
}