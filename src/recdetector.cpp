/**
 * Detector recursion functions from c++ file
 */

#include <cstdio>
#include <memory>
#include <string>
#include <sstream>

#include <map>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/CallGraph.h"
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

#include "FinderASTComsumer.h"
#include "utils.cpp"
 
using namespace clang;
using namespace std;

int main(int argc, char *argv[]) {

    // Get code infos
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

    return 0;
}