#include "CodeInsert.h"
#include <iostream>
using namespace std;

 string int2str( int num)
 {
    if (num == 0 )
       return " 0 " ;                                                                                                                                      
   
    string str = "" ;
    int num_ = num > 0 ? num : - 1 * num;

    while (num_)
    {
       str = ( char )(num_ % 10 + 48 ) + str;
       num_ /= 10 ;
    } 
 
    if (num < 0 )
       str = " - " + str;

    return str;
}

CodeInsert::CodeInsert(FunctionDecl* funcDecl_, Funcinfo* funcInfo_)
  {
      funcDecl = funcDecl_;
      funcInfo = funcInfo_;
      stuctname = funcInfo->fun_name + "_SnapShotStruct";
      GenStruct();
      //GenHeader(3);
  }

void CodeInsert::IndentToString(string& base, int indent, string aim) {
    string whitespace("");
    for (int i = 0; i < indent; ++i)
    whitespace += '\t';
    base += whitespace + aim;   
}

void CodeInsert::GenStruct() {
    string aim("struct " + stuctname + " {\n");
    IndentToString(stct, indent, aim);
    for (auto it = funcInfo->loc_vars.begin(), ie = funcInfo->loc_vars.end(); it != ie; ++it)
    {
        if (it->second.first == 0)
            IndentToString(stct, indent, it->second.second+" "+it->first+";\n");
        else
            IndentToString(stct, indent, it->second.second+" arg"+to_string(it->second.first)+";\n");
    }
    IndentToString(stct, indent, "int label;\n");
    indent--;
    IndentToString(stct, indent, "};\n\n");
}

void CodeInsert::GenHeader(int n) {

    if (funcInfo->fun_type != "void")
        IndentToString(header, indent, funcInfo->fun_type+" retVal;\n\n");

    IndentToString(header, indent, "std::stack<" + stuctname + "> snapshotStack;\n");
    IndentToString(header, indent, stuctname + " newSnapshot;\n\n");
    IndentToString(header, indent, stuctname + " currentSnapshot;\n\n");
    IndentToString(header, indent, "currentSnapshot.label = 1;\n");
    for (auto it = funcInfo->fun_prms.begin(), ie = funcInfo->fun_prms.end(); it != ie; ++it)
        IndentToString(header, indent, "currentSnapshot.arg"+to_string(it->second.first)+"="+it->first+";\n");

    IndentToString(header, indent, "snapshotStack.push(currentSnapshot);\n");
    IndentToString(header, indent, "label0:\n");
    IndentToString(header, indent++, "if (!snapshotStack.empty()) {\n");
    IndentToString(header, indent, "currentSnapshot=snapshotStack.top();\n");
    IndentToString(header, indent, "snapshotStack.pop();\n");
    IndentToString(header, indent++, "switch(currentSnapshot.label) {\n");
    for (int i = 1; i <= n + 1; ++i)
    {
      IndentToString(header, indent, "case "+int2str(i)+":goto label" + int2str(i) + ";\n");
    }
    IndentToString(header, --indent, "}\n");
    IndentToString(header, --indent, "}\n"); 
    IndentToString(header, indent++, "else\n");
    IndentToString(header, indent--, "return retVal;\n");
    IndentToString(header, indent++, "label1:\n");
}
void CodeInsert::DebugOutput()
{
  //cout << "header:" << endl << header << endl;
  cout << "stct:" << endl << stct << endl;
}