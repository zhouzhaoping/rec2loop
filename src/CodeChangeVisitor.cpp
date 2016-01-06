#include "CodeChangeVisitor.h"
#include <string>
#include <iostream>

bool MyASTVisitor::VisitBinaryOperator(BinaryOperator *b) {

//cout << "in binaryopt" << endl;
    return true;
}

bool MyASTVisitor::VisitVarDecl(VarDecl* v) {

//cout << "in vardecl" << endl;
    return true;
}

bool MyASTVisitor::VisitStmt(Stmt *s) {

//cout << "in stmt" << endl;
    return true;
}

bool MyASTVisitor::VisitFunctionDecl(FunctionDecl *f) {

//cout << "in functiondecl" << endl;
    if (f->hasBody()) {

        CodeInsert codeInsert;
        for (auto &ff : funInsertMap)
        {
            if (ff.first->getNameInfo().getName().getAsString() == f->getNameInfo().getName().getAsString())
            {            
                codeInsert = ff.second;
                break;
            }
        }

        TheRewriter.InsertText(f->getSourceRange().getBegin(), codeInsert.stct, true, true);

        SourceLocation s = f->getNameInfo().getBeginLoc();
        //TheRewriter.InsertText(s, "loop", true, false);

        Stmt *FuncBody = f->getBody();

        SourceLocation ST = FuncBody->getLocStart().getLocWithOffset(2);

        TheRewriter.InsertText(ST, codeInsert.header, true, true);

        string footer;
        CodeInsert::IndentToString(footer, codeInsert.indent, "}\n}\n");
        if (codeInsert.funcInfo->fun_type != "void")
            CodeInsert::IndentToString(footer, codeInsert.indent, "return retVal;\n");
        footer += "}\n";
        TheRewriter.InsertText(FuncBody->getLocEnd().getLocWithOffset(1), footer, true, true);
    }

    return true;
}