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
    std::string fullname = v->fullyQualifiedName0().c_str();
    S_compound *body = func->body->asS_compound();
    char const *file = sourceLocManager->getFile(body->loc);
    CPPSourceLoc csl(body->loc);

    // skip the methods the user specified to skip on the
    // command-line
    if (methods.find(fullname) != methods.end()) return false;

    // skip implicit members, such as the ctors you get if you don't
    // write one yourself
    if (v->isImplicitMemberFunc()) return false;

    // skip bodies where I can't get the exact position (?)
    if (!csl.hasExactPosition()) return false;

    // skip functions defined in header files; FIX: don't use .cpp
    if (!strstr(file, ".cpp")) return false;

    // skip functions having empty bodies
    if (body->stmts.isEmpty()) return false;
    Statement *firstStatement = body->stmts.first();
    CPPSourceLoc endcsl(firstStatement->loc);
  
    // skip functions that amount to a single expression
    if (firstStatement->isS_return()) return false;

    // FIX: what does this do?  is this all just to compute the
    // indentation?
    std::string indent = "\n";
    if (!endcsl.hasExactPosition()) {
      if (endcsl.macroExpansion) {
        endcsl.overrideLoc(endcsl.macroExpansion->preStartLoc);
      }
      indent = patcher.getRange(PairLoc(csl, endcsl));
      //get rid of {
      if (indent.size()) {
        indent.erase(0,1);
      }
      std::string::size_type endline = indent.rfind("\n");
      if (endline != std::string::npos) {
        indent.erase(0, endline);
      }
    }

    // insert code at the top of the function
    std::stringstream ss;
    ss << indent << "InstrumentBlock __ib(\"" << fullname << "\");";
    patcher.insertBefore(csl, ss.str(), 1);

    // idempotently ensure that the top of the file includes the right
    // header
    ensureInclude(file);

    // prune traversal of the children of this AST node
    return false;
  }

  // insert an #include at the top of the file
  void ensureInclude(char const *file) {
    // idempotency: don't do this twice for the same file
    if (files.find(file) != files.end()) return;
    files.insert(file);

    // do the insertion
    std::ifstream f(file);
    std::string linestr;
    int line = 0;
    while (getline(f, linestr)) {
      ++line;
      // if the first line is an #include
      if (!strncmp("#include", linestr.c_str(), 8)) {
        // insert code before that line
        UnboxedLoc loc(line, 1);
        patcher.insertBefore(file, loc, "#include \"instrument.h\"\n");
        break;                  // break out of the loop
      }
    }
    f.close();
  }
};

int main(int argc, char **argv) {
  // dsw: NOTE this doesn't seem to actually work; but Taras put it in
  // here so I'll leave it for now
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

  // parse and run the stopwatch visitor across each translation unit
  PigletParser parser;
  Patcher p;
  Stopwatch visitor(p);
  for (int i=curarg; i<argc; ++i) {
    TranslationUnit *unit = parser.getASTNoExc(argv[i]);
    unit->traverse(visitor);
  }

  // return success
  return 0;
}
