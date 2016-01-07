#include "Funcinfo.h"
#include <iostream>

 void Funcinfo::DebugOutput()
  {
    cout << "Function: " << fun_type << " " << fun_name << endl;
    cout << "parm: " << endl;
    for (auto &p : fun_prms)
      cout << p.first << " " << p.second.first << " " << p.second.second << endl;
    cout << "locvar: " << endl;
    for (auto &p : loc_vars)
      cout << p.first << " " << p.second.first << " " << p.second.second << endl;
  }


  bool GetInfoVisitor::VisitVarDecl(VarDecl* v)
  {

    QualType t = v->getType();
    string n = v->getNameAsString();
    if (curFuncinfo)
    {
      curFuncinfo->loc_vars[n] = pair<int,string>(0, t.getAsString());
      cout << "in vardecl: " << n << " " << curFuncinfo->loc_vars[n].first << " " << curFuncinfo->loc_vars[n].second << endl;
    }
    else
      cout << "find global vardecl: " << n << " " << t.getAsString() << endl;
    return true;
  }

  bool GetInfoVisitor::VisitFunctionDecl(FunctionDecl* f)
  {
    if (f->hasBody())
    {
      cout << "in function: " << f->getNameInfo().getName().getAsString() << endl;

      Funcinfo *funcinfo;
      funcinfo = new Funcinfo();

      curFuncinfo = funcinfo;

      funcinfo->fun_name = f->getNameInfo().getName().getAsString();
      funcinfo->fun_type  = f->getReturnType().getAsString();

      for (int i = 0, j = f->getNumParams(); i < j; ++i) {
        ParmVarDecl* pvd = f->parameters()[i];
        funcinfo->fun_prms[pvd->getNameAsString()] = pair<int,string>(i+1, pvd->getType().getAsString());
      }

      funInfoMap[f] = funcinfo;
    }
    return true;
  }