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
#include "CodeInsert.h"
#include "CodeChangeVisitor.h"

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

map<FunctionDecl*, CodeInsert> funInsertMap;

string getNewName(char* name)
{
    string name_(name);
    return name_.insert(name_.find_first_of('.'), "_goto");
}

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


    // 2. Generate codes to be inserted
     cout << endl << "[Genhead]" << endl;
    for (auto &f : TheBasic.info_visitor.funInfoMap)
    {
      funInsertMap[f.first] = CodeInsert(f.first, f.second);
      funInsertMap[f.first].DebugOutput();
    }

    // 3. Get recursion functions
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


    // 4. Generate new code
    cout << endl << "[Gencode]" << endl;
    CompilerInstance WriteCompInst;
    GenCompInst(argv[1], WriteCompInst);
    Rewriter TheRewriter;
    TheRewriter.setSourceMgr(WriteCompInst.getSourceManager(), WriteCompInst.getLangOpts());

    MyASTConsumer TheConsumer(TheRewriter);
    TheConsumer.setInfomation(finderASTComsumer.functionrectime, funInsertMap);
    ParseAST(WriteCompInst.getPreprocessor(), &TheConsumer, WriteCompInst.getASTContext());

    std::error_code OutErrorInfo;
    string newfilename = getNewName(argv[1]);
    llvm::raw_fd_ostream outFile(llvm::StringRef(newfilename), OutErrorInfo, llvm::sys::fs::F_None);

    const RewriteBuffer *RewriteBuf =
    TheRewriter.getRewriteBufferFor(WriteCompInst.getSourceManager().getMainFileID());
    outFile << "#include <stack>\n" << std::string(RewriteBuf->begin(), RewriteBuf->end());
    outFile.close();

    return 0;
  }
