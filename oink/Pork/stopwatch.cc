// Stopwatch: given a list of methods, injects instrumentation code into them

#include "piglet.h"
#include "patcher.h"

// #include "cc_ast_aux.h"         // LoweredASTVisitor

#include <set>
#include <sstream>
#include <string>
#include <fstream>

static std::set<std::string> methods;

class Stopwatch : public ASTVisitor {
private:
// LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  Patcher &patcher;
  std::set<char const *> files;

public:
  Stopwatch(Patcher &patcher)
//     : loweredVisitor(this)
    : patcher(patcher)
  {}

  virtual bool visitFunction(Function *func) {
    Variable *v = func->nameAndParams->var;
    if (v->isImplicitMemberFunc())
      return false;

    std::string fullname = v->fullyQualifiedName0().c_str();
    if (methods.find(fullname) != methods.end()) {
      return false;
    }
    S_compound *body = func->body->asS_compound();
    // punt on headers
    char const *file = sourceLocManager->getFile(body->loc);
    if (!strstr(file, ".cpp"))
      return false;
    
    CPPSourceLoc csl(body->loc);

    if (!csl.hasExactPosition())
      return false;
  
    std::string indent = "\n";

    if (body->stmts.isEmpty())
      return false;

    Statement *firstStatement = body->stmts.first();

    // functions that begin with a return usually aren't interesting
    if (firstStatement->isS_return())
      return false;

    CPPSourceLoc endcsl(firstStatement->loc);
    if (!endcsl.hasExactPosition()) {
      if (endcsl.macroExpansion)
        endcsl.overrideLoc(endcsl.macroExpansion->preStartLoc);
      indent = patcher.getRange(PairLoc(csl, endcsl));
      //get rid of {
      if (indent.size())
        indent.erase(0,1);
      std::string::size_type endline = indent.rfind("\n");
      if (endline != std::string::npos) {
        indent.erase(0, endline);
      }
    }

    std::stringstream ss;
    ss << indent << "InstrumentBlock __ib(\"" << fullname
       << "\");";
    patcher.insertBefore(csl, ss.str(), 1);
    ensureInclude(file);
    return false;
  }
  
  void ensureInclude(char const *file) {
    if (files.find(file) != files.end())
      return;
    files.insert(file);
    std::ifstream f(file);
    std::string linestr;
    int line = 0;
    while (getline(f, linestr)) {
      ++line;
      if (!strncmp("#include", linestr.c_str(), 8)) {
        UnboxedLoc loc(line, 1);
        patcher.insertBefore(file, loc, "#include \"instrument.h\"\n");
        break;
      }
    }
    f.close();
  }
};

int main(int argc, char **argv) {
  int curarg = 1;
  if (curarg + 1 < argc && !strcmp("--methodlist", argv[curarg])) {
    std::ifstream f(argv[++curarg]);
    std::string line;
    while (getline(f, line)) {
      methods.insert(line.c_str());
    }
    f.close();
    ++curarg;
  }

  PigletParser parser;
  Patcher p;
  Stopwatch visitor(p);
  for (int i = curarg;i< argc;i++) {
    TranslationUnit *unit = parser.getASTNoExc(argv[i]);
    unit->traverse(visitor);
  }

  return 0;
}
