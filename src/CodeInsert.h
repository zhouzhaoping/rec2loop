#ifndef CodeInsert_H
#define CodeInsert_H

#include <string>
#include "clang/AST/RecursiveASTVisitor.h"

#include "Funcinfo.h"
// Variables about the code inserted
class CodeInsert
{
public:

  FunctionDecl* funcDecl;
  Funcinfo* funcInfo;
  CodeInsert(){}
  CodeInsert(FunctionDecl* funcDecl_, Funcinfo* funcInfo_);

string header, stct;
int indent = 1, cur_stage = 2;
static void IndentToString(string& base, int indent, string aim);

void GenStruct();

void GenHeader(int n);
void DebugOutput();


    map<SourceLocation,int> mmap;

};

#endif