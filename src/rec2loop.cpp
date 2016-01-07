
/**
 * Recursion To Loop
 */

#include <cstdio>
#include <memory>
#include <string>
#include <sstream>

#include <map>
#include <iostream>

#include "utils.cpp"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"                                                                                                             
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace std;

// Variables about the program infos
string fun_name, fun_type;
map<string,pair<int,string>>  fun_prms;
map<string,pair<int,string>>  loc_vars;

// Variables about the code inserted
string header, stct;
int indent = 1, cur_stage = 0;

void IndentToString(string& base, int indent, string aim) {
    string whitespace("");
    for (int i = 0; i < indent; ++i)
    whitespace += '\t';
    base += whitespace + aim;   
}

void GenStruct() {
    string aim("struct SnapShotStruct {\n");
    IndentToString(stct, indent, aim);
    for (auto it = loc_vars.begin(), ie = loc_vars.end(); it != ie; ++it)
    {
        if (it->second.first == 0)
            IndentToString(stct, indent, it->second.second+" "+it->first+";\n");
        else
            IndentToString(stct, indent, it->second.second+" arg"+to_string(it->second.first)+";\n");
    }
    IndentToString(stct, indent, "int stage;\n");
    indent--;
    IndentToString(stct, indent, "};\n\n");
}

void GenHeader() {

    if (fun_type != "void")
        IndentToString(header, indent, fun_type+" retVal;\n\n");

    IndentToString(header, indent, "stack<SnapShotStruct> snapshotStack;\n");
    IndentToString(header, indent, "SnapShotStruct currentSnapshot;\n\n");
    IndentToString(header, indent, "currentSnapshot.stage = 0;\n");
    for (auto it = fun_prms.begin(), ie = fun_prms.end(); it != ie; ++it)
        IndentToString(header, indent, "currentSnapshot.arg"+to_string(it->second.first)+"="+it->first+";\n");

    IndentToString(header, indent, "snapshotStack.push(currentSnapshot);\n");
    IndentToString(header, indent++, "while(!snapshotStack.empty()) {\n");
    IndentToString(header, indent, "currentSnapshot=snapshotStack.top();\n");
    IndentToString(header, indent, "snapshotStack.pop();\n");
    IndentToString(header, indent, "switch(currentSnapshot.stage) {\n");
    IndentToString(header, indent++, "case "+to_string(cur_stage++)+":{ \n");
}


// GetInfoVisitor: get infos about function and variables
class GetInfoVisitor : public RecursiveASTVisitor<GetInfoVisitor> 
{

public:

    bool VisitVarDecl(VarDecl* v)
    {
        QualType t = v->getType();
        string n = v->getNameAsString();
        loc_vars[n] = pair<int,string>(0, t.getAsString());
        return true;
    }

    bool VisitFunctionDecl(FunctionDecl* f)
    {
        if (f->hasBody())
        {
            fun_name = f->getNameInfo().getName().getAsString();
            fun_type  = f->getReturnType().getAsString();

            for (int i = 0, j = f->getNumParams(); i < j; ++i) {
                ParmVarDecl* pvd = f->parameters()[i];
                fun_prms[pvd->getNameAsString()] = pair<int,string>(i+1, pvd->getType().getAsString());
            }
        }
        return true;
    }

};

class BasicASTComsumer : public ASTConsumer {

public:
    GetInfoVisitor info_visitor;

    virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
        
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
            info_visitor.TraverseDecl(*b);

        // Change parameters to be special
        for (auto it = loc_vars.begin(), ie = loc_vars.end(); it != ie; ++it)
            if (fun_prms.count(it->first))
                it->second.first = fun_prms[it->first].first;

        return true;
    }

};

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    MyASTVisitor(Rewriter &R) : TheRewriter(R) {}

    void AddBrace(Stmt *s) {
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

    map<SourceLocation,int> mmap;
    void ChangeCall(CallExpr* c, SourceLocation p) {

        string nextStage;

        IndentToString(nextStage, indent, "SnapShotStruct newSnapshot;\n");
        IndentToString(nextStage, indent, "newSnapshot.stage = 0;\n");

        // Parameters called to assign
        unsigned int number = c->getNumArgs(), i;
        for (i = 1; i <= number; ++i) {

            ImplicitCastExpr* ic = cast<ImplicitCastExpr>(c->getArgs()[i-1]);
            Expr* e = ic->getSubExpr();

            // Promise to be declared reference expression*
            DeclRefExpr* d = dyn_cast<DeclRefExpr>(e);
            mmap[d->getLocStart()] = 1;
            
            string n = "currentSnapshot." + d->getNameInfo().getName().getAsString();
            
            IndentToString(nextStage, indent, "newSnapshot.arg"+to_string(i)+"="+n+";\n");
        }

        IndentToString(nextStage, indent, "currentSnapshot.stage++;\n");
        IndentToString(nextStage, indent, "snapshotStack.push(currentSnapshot);\n");
        IndentToString(nextStage, indent, "snapshotStack.push(newSnapshot);\n");
        IndentToString(nextStage, indent, "break;}\n");
        IndentToString(nextStage, indent, "case "+to_string(cur_stage++)+":{ \n");

        TheRewriter.InsertTextBefore(p, nextStage);
    }

    bool VisitBinaryOperator(BinaryOperator *b) {
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

    bool VisitVarDecl(VarDecl* v) {
        if (!isa<ParmVarDecl>(v)) {
            SourceRange sr = v->getSourceRange();
            TheRewriter.RemoveText(sr);
        }
        return true;
    }

    bool VisitStmt(Stmt *s) {
        
        // Call-Expression --> stack operations
        if (isa<Expr>(s) && isa<CallExpr>(s) && fun_type == "void") {
            
            CallExpr* c = cast<CallExpr>(s);
            ChangeCall(c, c->getLocStart());

        }

        if (isa<ReturnStmt>(s)) {

            SourceLocation se = Lexer::findLocationAfterToken(s->getLocEnd(),
                                        tok::semi,
                                        TheRewriter.getSourceMgr(),
                                        TheRewriter.getLangOpts(), true);
            TheRewriter.InsertText(se, "continue;\n", true, true);

            SourceLocation st = s->getLocStart();
            TheRewriter.ReplaceText(st, 6, "retVal=");
        }

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

        if (isa<DeclRefExpr>(s)) {
            DeclRefExpr* expr = cast<DeclRefExpr>(s);
            if (mmap.count(expr->getLocStart())) return true;
            string name = expr->getNameInfo().getName().getAsString();
            if (loc_vars.count(name)) {
                if (loc_vars[name].first == 0)
                    TheRewriter.InsertText(expr->getLocStart(), "currentSnapshot.", true, true);
                else {
                    SourceRange sr(expr->getLocStart(), expr->getLocEnd());
                    TheRewriter.ReplaceText(sr, "currentSnapshot.arg"+to_string(loc_vars[name].first));
                }
            }
        }

        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *f) {

        if (f->hasBody()) {

            TheRewriter.InsertText(f->getSourceRange().getBegin(), stct, true, true);

            SourceLocation s = f->getNameInfo().getBeginLoc();
            TheRewriter.InsertText(s, "loop", true, false);

            Stmt *FuncBody = f->getBody();

            SourceLocation ST = FuncBody->getLocStart().getLocWithOffset(2);

            TheRewriter.InsertText(ST, header, true, true);

            string footer;
            IndentToString(footer, indent, "}\n}\n");
            if (fun_type != "void")
            IndentToString(footer, indent, "return retVal;\n");
            footer += "}\n";
            TheRewriter.InsertText(FuncBody->getLocEnd().getLocWithOffset(1), footer, true, true);
        }

        return true;
    }

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

private:
    MyASTVisitor Visitor;
};


int main(int argc, char *argv[]) {

    // Get code infos
    CompilerInstance BasicCompInst;
    GenCompInst(argv[1], BasicCompInst);
    BasicASTComsumer TheBasic;
    ParseAST(BasicCompInst.getPreprocessor(), &TheBasic, BasicCompInst.getASTContext());

    // Generate codes to be inserted
    GenStruct();
    GenHeader();

    // Generate new code
    CompilerInstance WriteCompInst;
    GenCompInst(argv[1], WriteCompInst);
    Rewriter TheRewriter;
    TheRewriter.setSourceMgr(WriteCompInst.getSourceManager(), WriteCompInst.getLangOpts());

    MyASTConsumer TheConsumer(TheRewriter);
    ParseAST(WriteCompInst.getPreprocessor(), &TheConsumer, WriteCompInst.getASTContext());

    std::error_code OutErrorInfo;
    llvm::raw_fd_ostream outFile(llvm::StringRef(fun_name+"loop.cpp"), OutErrorInfo, llvm::sys::fs::F_None);

    const RewriteBuffer *RewriteBuf =
    TheRewriter.getRewriteBufferFor(WriteCompInst.getSourceManager().getMainFileID());
    outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
    outFile.close();
    
    return 0;
}
