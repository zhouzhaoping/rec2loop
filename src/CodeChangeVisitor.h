#ifndef CodeChangeVisitor_H
#define CodeChangeVisitor_H

#include "CodeInsert.h"
#include <map>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Analysis/CallGraph.h"

using namespace llvm;
using namespace clang;
using namespace std;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:

    std::map<CallGraphNode*,int> functionrectime;
    std::map<FunctionDecl*, CodeInsert> funInsertMap;

    MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

    CodeInsert curCodeInsert;

    bool VisitBinaryOperator(BinaryOperator *b);

    bool VisitVarDecl(VarDecl* v);

    bool VisitStmt(Stmt *s);

    bool VisitFunctionDecl(FunctionDecl *f);

    // Helper method for converting LLVM entities that support the print() method
    // to a string.
    template <typename T> static std::string ToString(const T *Obj) {
      std::string S;
      raw_string_ostream OS(S);
      Obj->print(OS);
      return OS.str();
    }
    bool functionNeedChange;
    void AddBrace(Stmt *s);
    void ChangeCall(CallExpr* c, SourceLocation p);

private:
    Rewriter &TheRewriter;
};

class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(Rewriter &R) : Visitor(R) {}

    virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
            Visitor.TraverseDecl(*b);

        return true;
    }

    void setInfomation(map<CallGraphNode*,int> functionrectime_, map<FunctionDecl*, CodeInsert> funInsertMap_)
    {
        Visitor.functionrectime = functionrectime_;
        Visitor.funInsertMap = funInsertMap_;
    }

private:
    MyASTVisitor Visitor;
};

#endif