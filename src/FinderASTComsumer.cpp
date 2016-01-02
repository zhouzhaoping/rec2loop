/**
* Comsumer for detecting recursion functions
*/
 
#include <vector>
#include <map>
#include <stack>
#include "FinderASTComsumer.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Analysis/CallGraph.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace std;

bool FinderASTComsumer::HandleTopLevelDecl(DeclGroupRef DR) {

    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
        callgraph.TraverseDecl(*b);

    return true;
}
void FinderASTComsumer::initRec()
{
    CallGraphNode *root = callgraph.getRoot();
    for (CallGraphNode::const_iterator CI = root->begin(), CE = root->end(); CI != CE; ++CI) {
        functionisrec[*CI] = false;
    }
}
void FinderASTComsumer::detectLinearRec()
{
    for (auto &f : functionisrec) {
        f.second = false;
        for (CallGraphNode::const_iterator CI = f.first->begin(), CE = f.first->end(); CI != CE; ++CI) {
            if (*CI == f.first){
                //(*CI)->print(llvm::errs());
                //llvm::errs() << " is recursion function!\n";
                f.second = true;
                break;
            }
        }
    }
}


void FinderASTComsumer::detectCycleRec()
{
    count = 0;
    functionVisited.clear();
    for (auto &f : functionisrec) {
        functionVisited[f.first] = false;
    }
    trace.clear();
    
    // detect f is recursion or not and output circle
    for (auto &f : functionisrec) {
        flag = false;
        trace.push_back(f.first);
        functionVisited[f.first] = true;
        CycleDFSfinder(f.first, f.first);
        functionVisited[f.first] = false;
        trace.pop_back();
        if (flag)
            functionVisited[f.first] = true;//delete source
    }
}

void FinderASTComsumer::CycleDFSfinder(CallGraphNode* source, CallGraphNode* curnode)
{
    for (CallGraphNode::const_iterator CI = curnode->begin(), CE = curnode->end(); CI != CE; ++CI) {//TODO:iter same calls
        if (*CI == source){
            llvm::errs() << "recursion circle" << ++count << ": ";
            for (auto &n : trace)
            {
                functionisrec[n] = true;
                n->print(llvm::errs());
                llvm::errs() << "->";
            }
            source->print(llvm::errs());
            llvm::errs() << "\n";
            flag = true;
        }
        else if (functionVisited[*CI] == false)
        {
            trace.push_back(curnode);
            functionVisited[*CI] = true;
            CycleDFSfinder(source, *CI);
            functionVisited[*CI] = false;
            trace.pop_back();
        }
    }
}

void FinderASTComsumer::printRecFunction()
{
    for (auto &f : functionisrec) {
        if (f.second)
        {
            f.first->print(llvm::errs());
            llvm::errs() << " is recursion function!\n";
        }
    }
}