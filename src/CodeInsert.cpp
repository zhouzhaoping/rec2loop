#include "CodeInsert.h"
#include <iostream>
using namespace std;

CodeInsert::CodeInsert(FunctionDecl* funcDecl_, Funcinfo* funcInfo_)
  {
      funcDecl = funcDecl_;
      funcInfo = funcInfo_;
      GenStruct();
      GenHeader();
  }

void CodeInsert::IndentToString(string& base, int indent, string aim) {
    string whitespace("");
    for (int i = 0; i < indent; ++i)
    whitespace += '\t';
    base += whitespace + aim;   
}

void CodeInsert::GenStruct() {
    string aim("struct SnapShotStruct {\n");
    IndentToString(stct, indent, aim);
    for (auto it = funcInfo->loc_vars.begin(), ie = funcInfo->loc_vars.end(); it != ie; ++it)
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

void CodeInsert::GenHeader() {

    if (funcInfo->fun_type != "void")
        IndentToString(header, indent, funcInfo->fun_type+" retVal;\n\n");

    IndentToString(header, indent, "stack<SnapShotStruct> snapshotStack;\n");
    IndentToString(header, indent, "SnapShotStruct currentSnapshot;\n\n");
    IndentToString(header, indent, "currentSnapshot.stage = 0;\n");
    for (auto it = funcInfo->fun_prms.begin(), ie = funcInfo->fun_prms.end(); it != ie; ++it)
        IndentToString(header, indent, "currentSnapshot.arg"+to_string(it->second.first)+"="+it->first+";\n");

    IndentToString(header, indent, "snapshotStack.push(currentSnapshot);\n");
    IndentToString(header, indent++, "while(!snapshotStack.empty()) {\n");
    IndentToString(header, indent, "currentSnapshot=snapshotStack.top();\n");
    IndentToString(header, indent, "snapshotStack.pop();\n");
    IndentToString(header, indent, "switch(currentSnapshot.stage) {\n");
    IndentToString(header, indent++, "case "+to_string(cur_stage++)+":{ \n");
}
void CodeInsert::DebugOutput()
{
  cout << "header:" << endl << header << endl;
  cout << "stct:" << endl << stct << endl;
}