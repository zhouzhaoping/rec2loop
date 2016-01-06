#include <map>
#include <string>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace std;

class Funcinfo
{
public:
// Variables about the program infos
  string fun_name, fun_type;
  map<string,pair<int,string>>  fun_prms;
  map<string,pair<int,string>>  loc_vars;
// var:<name,<rank,type>>

// Variables about the code inserted
  string header, stct;
  int indent = 1, cur_stage = 0;
  void DebugOutput();
};

// GetInfoVisitor: get infos about function and variables
class GetInfoVisitor : public RecursiveASTVisitor<GetInfoVisitor> 
{

public:

  Funcinfo *curFuncinfo;
  // Collection of FuncDecl and Funcinfomation
  map<FunctionDecl*, Funcinfo*> funInfoMap;

  bool VisitVarDecl(VarDecl* v);

  bool VisitFunctionDecl(FunctionDecl* f);

};

class BasicASTComsumer : public ASTConsumer {

public:
  GetInfoVisitor info_visitor;

  virtual bool HandleTopLevelDecl(DeclGroupRef DR) {

    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
      info_visitor.TraverseDecl(*b);

    return true;
  }

};