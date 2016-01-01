/**
 * Comsumer for detecting recursion functions
 */

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
    void detectRec();
};