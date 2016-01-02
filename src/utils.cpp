/**
 * Some utils functions
 */
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

// Some utils functions to be used
void GenCompInst(char* filename, CompilerInstance& CompInst) {
    CompInst.createDiagnostics();
    LangOptions &lo = CompInst.getLangOpts();
    lo.CPlusPlus = 1;

    auto TO = std::make_shared<TargetOptions>();
    TO->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(CompInst.getDiagnostics(), TO);
    CompInst.setTarget(TI);

    CompInst.createFileManager();
    FileManager &FileMgr = CompInst.getFileManager();
    CompInst.createSourceManager(FileMgr);
    SourceManager &SourceMgr = CompInst.getSourceManager();
    CompInst.createPreprocessor(TU_Module);
    CompInst.createASTContext();

    // Set the main file handled by the source manager to the input file.
    const FileEntry *FileIn = FileMgr.getFile(filename);
    SourceMgr.setMainFileID(SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
    CompInst.getDiagnosticClient().BeginSourceFile(
        CompInst.getLangOpts(), &CompInst.getPreprocessor());
}
