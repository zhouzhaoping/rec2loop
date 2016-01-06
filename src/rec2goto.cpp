/**
* Recursion To Loop and Goto
*/

#include <cstdio>
#include <memory>
#include <string>
#include <sstream>

#include <map>
#include <iostream>

#include "utils.cpp"
#include "FinderASTComsumer.h"
#include "Funcinfo.h"

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

/*
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
*/


int main(int argc, char *argv[]) {
  if (argc != 2) {
    llvm::errs() << "Usage: recursionsample <filename>\n";
    return 1;
  }


// 1. Get code infos
  cout << endl << "[Travel]" << endl;
  CompilerInstance BasicCompInst;
  GenCompInst(argv[1], BasicCompInst);
  BasicASTComsumer TheBasic;
  ParseAST(BasicCompInst.getPreprocessor(), &TheBasic, BasicCompInst.getASTContext());


// Change parameters to be special from local infomation
  cout << endl << "[Change]" << endl;
  for (auto &f : TheBasic.info_visitor.funInfoMap)
  {
    for (auto it = f.second->loc_vars.begin(), ie = f.second->loc_vars.end(); it != ie; ++it)
      if (f.second->fun_prms.count(it->first))
      {
        it->second.first = f.second->fun_prms[it->first].first;
        cout << "change " << it->first << endl;
      } 

    }

    // debug output
    cout << endl << "[Debug]" << endl;
    for (auto &f : TheBasic.info_visitor.funInfoMap)
    {
      f.second->DebugOutput();
    }

    // 2. Get recursion functions
    cout << endl << "[findRecs]" << endl;
    CompilerInstance finderCompInst;
    GenCompInst(argv[1], finderCompInst);
    FinderASTComsumer finderASTComsumer;
    ParseAST(finderCompInst.getPreprocessor(), &finderASTComsumer, finderCompInst.getASTContext());

    finderASTComsumer.callgraph.dump();
    finderASTComsumer.initRec();

#ifndef DETECT_CYCLE
    finderASTComsumer.detectLinearRec();
#else
    finderASTComsumer.detectCycleRec();
#endif
    finderASTComsumer.printRecFunction();

    // Generate codes to be inserted
    //GenStruct();
    //GenHeader();

    return 0;
  }
