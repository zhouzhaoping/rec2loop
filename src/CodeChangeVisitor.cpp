#include "CodeChangeVisitor.h"
#include <string>
#include <iostream>

#include "clang/Lex/Preprocessor.h"

    void MyASTVisitor::AddBrace(Stmt *s) {
        if (!isa<CompoundStmt>(s)) {
            SourceLocation st = s->getLocStart();
            SourceLocation se = s->getLocEnd();
            
            TheRewriter.InsertText(st, "{\n", true, true);

            int offset = Lexer::MeasureTokenLength(se,
                            TheRewriter.getSourceMgr(),
                            TheRewriter.getLangOpts()) + 1;
            TheRewriter.InsertText(se.getLocWithOffset(offset), "\n}", true, true);
        }
    }

    void MyASTVisitor::ChangeCall(CallExpr* c, SourceLocation p) {

        string nextStage;

        //CodeInsert::IndentToString(nextStage, curCodeInsert.indent, curCodeInsert.stuctname + " newSnapshot;\n");
        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "newSnapshot.label = 1;\n");

        // Parameters called to assign
        unsigned int number = c->getNumArgs(), i;
        for (i = 1; i <= number; ++i) {

            ImplicitCastExpr* ic = cast<ImplicitCastExpr>(c->getArgs()[i-1]);
            Expr* e = ic->getSubExpr();

            // Promise to be declared reference expression*
            DeclRefExpr* d = dyn_cast<DeclRefExpr>(e);
            curCodeInsert.mmap[d->getLocStart()] = 1;
            
            string n = "currentSnapshot." + d->getNameInfo().getName().getAsString();
            
            CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "newSnapshot.arg"+to_string(i)+"="+n+";\n");
        }

        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "currentSnapshot.label++;\n");
        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "snapshotStack.push(currentSnapshot);\n");
        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "snapshotStack.push(newSnapshot);\n");
        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "goto label0;\n");
        CodeInsert::IndentToString(nextStage, curCodeInsert.indent, "label"+to_string(curCodeInsert.cur_stage++)+":\n");

        TheRewriter.InsertTextBefore(p, nextStage);
    }

bool MyASTVisitor::VisitBinaryOperator(BinaryOperator *b) {

    if (functionNeedChange == false) return true;
    
    cout << "in binaryopt" << endl;
    b->dump();

     if (b->isAssignmentOp() && isa<CallExpr>(b->getRHS())) {
            
            CallExpr* c = cast<CallExpr>(b->getRHS());
            SourceLocation l = b->getLHS()->getLocStart();

            SourceLocation r = c->getRParenLoc();
            SourceRange* srcRange = new SourceRange(c->getLocStart(), r);
            TheRewriter.ReplaceText(*srcRange, "retVal");

            ChangeCall(c, l);
            
        }
    return true;
}

bool MyASTVisitor::VisitVarDecl(VarDecl* v) {

    if (functionNeedChange == false) return true;

    cout << "in vardecl" << endl;
    v->dump();
    if (!isa<ParmVarDecl>(v)) {
            SourceRange sr = v->getSourceRange();
            TheRewriter.RemoveText(sr);
        }
    return true;
}

bool MyASTVisitor::VisitStmt(Stmt *s) {

      if (functionNeedChange == false) return true;
      
        cout << "in stmt:" << endl;
        s->dump();
    // Call-Expression --> stack operations
        /*
        if (isa<Expr>(s) && isa<CallExpr>(s) && curCodeInsert.funcInfo->fun_type == "void") {
            Expr *e = cast<Expr>(s);
            CallExpr* c = cast<CallExpr>(e);
            ChangeCall(c, c->getLocStart());

        }*/

        // add brace
        if (isa<IfStmt>(s)) {
            IfStmt *IfStatement = cast<IfStmt>(s);
            Stmt *Then = IfStatement->getThen();

            AddBrace(Then);

            Stmt *Else = IfStatement->getElse();
            if (Else) AddBrace(Else);
        } else if (isa<WhileStmt>(s)) {
            WhileStmt* w = cast<WhileStmt>(s);
            AddBrace(w->getBody());
        } else if (isa<ForStmt>(s)) {
            ForStmt* f = cast<ForStmt>(s);
            AddBrace(f->getBody());
        }

        // change return stmt
        if (isa<ReturnStmt>(s)) {

            
            //TheRewriter.InsertText(s->getLocEnd(), "\ngoto label0;\n", true, true);
            
            SourceLocation st = s->getLocStart();
            if (curCodeInsert.funcInfo->fun_type != "void")
                TheRewriter.ReplaceText(st, 6, "retVal=");
            else
                TheRewriter.ReplaceText(st, 6, "");


            SourceLocation se = Lexer::findLocationAfterToken(s->getLocEnd(),
                                        tok::semi,
                                        TheRewriter.getSourceMgr(),
                                        TheRewriter.getLangOpts(), true);
            TheRewriter.InsertText(se, "\ngoto label0;\n", true, true);
        }

        // change currentSnapshot.var
        if (isa<DeclRefExpr>(s)) {
            DeclRefExpr* expr = cast<DeclRefExpr>(s);
            if (curCodeInsert.mmap.count(expr->getLocStart())) return true;
            string name = expr->getNameInfo().getName().getAsString();
            if (curCodeInsert.funcInfo->loc_vars.count(name)) {
                if (curCodeInsert.funcInfo->loc_vars[name].first == 0)
                    TheRewriter.InsertText(expr->getLocStart(), "currentSnapshot.", true, true);
                else {
                    SourceRange sr(expr->getLocStart(), expr->getLocEnd());
                    TheRewriter.ReplaceText(sr, "currentSnapshot.arg"+to_string(curCodeInsert.funcInfo->loc_vars[name].first));
                }
            }
        }

        return true;
}

bool MyASTVisitor::VisitFunctionDecl(FunctionDecl *f) {

    cout << "in functiondecl" << endl;
    f->dump();
    if (f->hasBody()) {

        functionNeedChange = false;

        // detect linear recurtion
        int calltimes = 0;
        for (auto &ff : functionrectime)
        {
            if (ToString(ff.first) == f->getNameInfo().getName().getAsString())
            {   
                calltimes = ff.second;
                break;             
            }
        }
        if (calltimes == 0)return true;

        // find insertCode
        int flag = 0;
        for (auto &ff : funInsertMap)
        {
            if (ff.first->getNameInfo().getName().getAsString() == f->getNameInfo().getName().getAsString())
            {            
                curCodeInsert = ff.second;                                                                                                                                                                                                                                                                                                                                                                                                                                                             
                flag = 1;
                break;
            }
        }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
        if (flag == 0)return true;

        functionNeedChange = true;
        curCodeInsert.GenHeader(calltimes);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             

        TheRewriter.InsertText(f->getSourceRange().getBegin(), curCodeInsert.stct, true, true);

//SourceLocation s = f->getNameInfo().getBeginLoc();
//TheRewriter.InsertText(s, "loop", true, false);

        Stmt *FuncBody = f->getBody();

        SourceLocation ST = FuncBody->getLocStart().getLocWithOffset(2);

        //TheRewriter.InsertText(ST, curCodeInsert.stct, true, true);
        TheRewriter.InsertText(ST, curCodeInsert.header, true, true);

/*
string footer;
CodeInsert::IndentToString(footer, codeInsert.indent, "}\n}\n");
if (codeInsert.funcInfo->fun_type != "void")
CodeInsert::IndentToString(footer, codeInsert.indent, "return retVal;\n");
footer += "}\n";
TheRewriter.InsertText(FuncBody->getLocEnd().getLocWithOffset(1), footer, true, true);
*/
}

return true;
}