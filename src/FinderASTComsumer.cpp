/**
* Comsumer for detecting recursion functions
*/
 
#include <map>
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
void FinderASTComsumer::detectRec()
{
    for (auto &f : functionisrec) {
        f.second = false;
        for (CallGraphNode::const_iterator CI = f.first->begin(), CE = f.first->end(); CI != CE; ++CI) {
            if (*CI == f.first){
                (*CI)->print(llvm::errs());
                //llvm::errs() << " is recursion function!\n";
                f.second = true;
                break;
            }
        }
    }
}