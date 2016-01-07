/**
 * Comsumer for detecting recursion functions
 */

#ifndef FinderASTComsumer_H
#define FinderASTComsumer_H

#include "clang/AST/ASTConsumer.h"
#include "clang/Analysis/CallGraph.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace std;


class FinderASTComsumer : public ASTConsumer {

public:
    CallGraph callgraph;

    virtual bool HandleTopLevelDecl(DeclGroupRef DR);

    map<CallGraphNode*,bool> functionisrec;
    void initRec();
    //void detectRec();
    // simple implementation for detecting linear recursion function
    void detectLinearRec();
    map<CallGraphNode*,int> functionrectime;


    map<CallGraphNode*,bool> functionVisited;
    vector<CallGraphNode*> trace;
    int count = 0;
    bool flag = false;
    void CycleDFSfinder(CallGraphNode* source, CallGraphNode* curnode);
    // using graph algorithm for detecting cycle base-on callgraph
    void detectCycleRec();

    void printRecFunction();
};

#endif