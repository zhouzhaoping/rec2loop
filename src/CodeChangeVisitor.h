#ifndef CodeChangeVisitor_H
#define CodeChangeVisitor_H

#include "CodeInsert.h"
#include <map>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

using namespace clang;
using namespace std;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:

    std::map<FunctionDecl*, CodeInsert> funInsertMap;

    MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

    bool VisitBinaryOperator(BinaryOperator *b);

    bool VisitVarDecl(VarDecl* v);

    bool VisitStmt(Stmt *s);

    bool VisitFunctionDecl(FunctionDecl *f);

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

    void addInfomation(map<FunctionDecl*, CodeInsert> funInsertMap_)
    {
        Visitor.funInsertMap = funInsertMap_;
    }

private:
    MyASTVisitor Visitor;
};

#endif