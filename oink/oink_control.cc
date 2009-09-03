// see License.txt for copyright and terms of use

#include "oink_control.h"       // this module
#include "oink_global.h"        // globalStrTable
#include "oink_util.h"
#include "cxxcoroutine.h"
#include "strsobjdict.h"

namespace OinkControlStrings {
  StringTable strTable;
  StringRef const star = strTable("*");
  StringRef const empty = strTable("");

  StringSObjDict<const char> archiveNames;

  inline StringRef getArchiveName(const char *filename) {
    StringRef name = archiveNames.queryif(filename);
    if (name) {
      return name;
    } else {
      return OinkControlStrings::empty;
    }
  }

  void addArchiveName(const char *filename, const char *archiveName) {
    archiveNames.add(filename, OinkControlStrings::strTable(archiveName));
  };
}

// **** ControlStream

// get one line and keep track of everything
void ControlStream::getLine(string &line) {
  getline(in, line);
  ++lineCount;
  // comments: delete everything after the hash
  char const *hashPos = strstr(line.c_str(), "#");
  if (hashPos) line = line.substring(0, hashPos - line.c_str());
  line = trimWhitespace(line);
}

// break the line into name and value at the '=' sign
void ControlStream::splitLine(string const &line, string &name, string &value) {
  char const *eqPos = strstr(line.c_str(), "=");
  if (!eqPos) {
    userFatalError(SL_UNKNOWN, "Illegal line in paragraph, no '=' sign: %s", value.c_str());
  }
  name = line.substring(0, eqPos-line.c_str());
  name = trimWhitespace(name);
  int valueStart = eqPos-line.c_str() + 1; // the plus one is to skip over the "="
  value = line.substring(valueStart, line.length()-valueStart);
  value = trimWhitespace(value);
}

// VariableMatcher ****

struct VariableMatcher {
  VariableMatcher(Variable_O /*const*/ *var)
  {
    DeclFlags declFlags = var->flags;
    // NOTE: it is hard to decide exactly what is a good predicate for
    // whether a function is "static" or "extern"; I am using this one
    // for now.
//      cout << "declFlags " << toXml(declFlags) << endl;
    isStaticNonMember = (declFlags & DF_STATIC) && !(declFlags & DF_MEMBER);

    funcname = OinkControlStrings::strTable(var->fullyQualifiedName_StrRef());

    hashline_file = OinkControlStrings::strTable(
      sourceLocManager->getFile(var->loc, true));
    nohashline_file = OinkControlStrings::strTable(
      sourceLocManager->getFile(var->loc, false));

    hashline_archive = OinkControlStrings::getArchiveName(hashline_file);
    nohashline_archive = OinkControlStrings::getArchiveName(nohashline_file);
  }

  bool isStaticNonMember;
  StringRef funcname;
  StringRef hashline_file, nohashline_file;
  StringRef hashline_archive, nohashline_archive;

};

// OinkControl ****

void OinkControl::initFromNameValue(string const &name, string const &value) {
  if (streq(name, "name")) {
    // NOTE: this one is potentially confusing.  The name/value pair
    // here is for specifying a value that is a name.  That is, the
    // confusion is due to meta world vs. object world level-crossing.
    this->name = OinkControlStrings::strTable(value);
  } else if (streq(name, "file")) {
    this->file = OinkControlStrings::strTable(value);
  } else if (streq(name, "archive")) {
    this->archive = OinkControlStrings::strTable(value);
  } else if (streq(name, "visibility")) {
    // TODO: support visibility = *, i.e. either static or extern
    if (streq(value, "static")) {
      this->visibility = DF_STATIC;
    } else if (streq(value, "extern")) {
      this->visibility = DF_EXTERN;
    } else {
      userFatalError
        (SL_UNKNOWN, "Illegal line in 'ignore' paragraph, unrecognized 'visibility': %s\n",
         value.c_str());
    }
  } else {
    userFatalError(SL_UNKNOWN, "Illegal line in 'ignore' paragraph, unrecognized 'name': %s\n",
                   name.c_str());
  }
}

bool OinkControl::isComplete() {
  bool complete = name && (file || archive);
  if (!archive) archive = OinkControlStrings::star;
  if (!file) file = OinkControlStrings::star;
  return complete;
}

void OinkControl::print() const {
  print(std::cout);
}

void OinkControl::print(std::ostream &out) const {
  switch (kind) {
  default: xfailure("illegal ");
  case CK_IGNORE_BODY:      out << "ignore-body:";     break;
  case CK_WEAK_DEF:         out << "weak-def:";        break;
  case CK_UNSOUNED_IF_USED: out << "unsound-if-used:"; break;
  }

  out << "\n\tname=" << name
      << "\n\tfile=" << file
      << "\n\tarchive=" << archive
      << "\n\tvisibility=" << toString(visibility)
      << std::endl;
}

// Controls ****

Controls::Controls()
  : index(NULL)
{
  for (int i=0; i<OinkControl::NUM_CONTROLKIND; ++i) numControls[i] = 0;
  numControls[OinkControl::CK_UNDEFINED] = -1;
}

void Controls::load(ControlStream &in) {
  xassert(in);
  while(in) {
    string line;
    in.getLine(line);
    if (line.empty()) continue; // skip blank lines

    OinkControl::ControlKind kind = OinkControl::CK_UNDEFINED;
    if (streq(line, "ignore-body:") || streq(line, "ignore-body-qual:")) {
      kind = OinkControl::CK_IGNORE_BODY;
    } else if (streq(line, "weak-def:")) {
      kind = OinkControl::CK_WEAK_DEF;
    } else if (streq(line, "unsound-if-used:")) {
      kind = OinkControl::CK_UNSOUNED_IF_USED;
    } else {
      throw UserError(USER_ERROR_ExitCode, stringc << "Unrecognized control " << line);
    }
    OinkControl *ctl = new OinkControl(kind);
    ++numControls[kind];

    // read a paragraph
    while(in) {
      in.getLine(line);
      if (line.empty()) break; // we are done with this paragraph
      string name;
      string value;
      ControlStream::splitLine(line, name, value);
      ctl->initFromNameValue(name, value);
    }
    if (!ctl->isComplete()) {
      throw UserError(USER_ERROR_ExitCode, stringc << "Incomplete control");
    }
    controlList.append(ctl);
  }
}

void Controls::buildIndex() {
  xassert(!index);
  index = new StringRefMap<ASTList<OinkControl> >();
  FOREACH_ASTLIST_NC(OinkControl, controlList, iter) {
    OinkControl *ctl = iter.data();
    ASTList<OinkControl> *nameCtls = index->get(ctl->name);
    if (!nameCtls) {
      nameCtls = new ASTList<OinkControl>();
      index->add(ctl->name, nameCtls);
    }
    nameCtls->append(ctl);
  }
}

void Controls::print(std::ostream &out) {
  out << "---- CONTROL_START ----" << std::endl;
  FOREACH_ASTLIST_NC(OinkControl, controlList, iter) {
    OinkControl *ctl = iter.data();
    ctl->print(out);
    out << std::endl;
  }
  out << "---- CONTROL_STOP ----" << std::endl;
}

bool OinkControl::match(VariableMatcher const &vm) const
{
  if (! (name == OinkControlStrings::star || name == vm.funcname)) return false;

  if (! (file == OinkControlStrings::star ||
         file == vm.hashline_file ||
         file == vm.nohashline_file))
    return false;

  if (! (archive == OinkControlStrings::star ||
         archive == vm.hashline_archive ||
         archive == vm.nohashline_archive))
    return false;

  bool hasVisibility = false;
  if (visibility == DF_STATIC) {
    if (vm.isStaticNonMember) hasVisibility = true;
  } else if (visibility == DF_EXTERN) {
    if (!vm.isStaticNonMember) hasVisibility = true;
  }

  if (!hasVisibility) return false;

//        std::cout << "function " << fullyQualifiedName
//             << ", file " << filename
//             << ", declFlags " << toXml(declFlags)
//             << std::endl;
//        throw UserError(USER_ERROR_ExitCode);
  return true;
}

// A generator that yields controls matching a given variable
class MatchingControlGenerator : Coroutine {
  ASTListIterNC<OinkControl> iter;
  Controls *controls;

public:
  MatchingControlGenerator(Controls *controls0) : controls(controls0) {}

  // return next item, or NULL at end (must stop iterating at end)
  OinkControl *next (Variable_O *var) {
    ASTList<OinkControl> *nameCtls;
    OinkControl *ctl;
    VariableMatcher vm(var);
    cxxCrBegin;
    xassert(controls);
    xassert(controls->index); // make sure we built the index before trying to use it
    if (!var->linkerVisibleName(true)) cxxCrReturn(NULL);
    nameCtls = controls->index->get(vm.funcname);
    if (nameCtls) {
      for(iter.reset(*nameCtls); !iter.isDone(); iter.adv()) {
        ctl = iter.data();
        if (ctl->match(vm)) { // if control matches var
          cxxCrReturn(ctl);
        }
      }
    }
    nameCtls = controls->index->get(OinkControlStrings::star);
    if (nameCtls) {
      for(iter.reset(*nameCtls); !iter.isDone(); iter.adv()) {
        ctl = iter.data();
        if (ctl->match(vm)) {
          cxxCrReturn(ctl);
        }
      }
    }
    cxxCrFinish(NULL);
  }
};

#define foreachControlMatching(var) \
  MatchingControlGenerator gen(this); for (OinkControl *ctl; NULL!=(ctl=gen.next(var)); /*nothing*/)

bool Controls::isIgnored(Variable_O *var) {
  foreachControlMatching(var) {
    if (ctl->kind == OinkControl::CK_IGNORE_BODY) {
      usedControls.add(ctl);
      return true;
    }
  }
  return false;                 // no control has filtered it out
}

bool Controls::isWeakDef(Variable_O *var) {
  foreachControlMatching(var) {
    if (ctl->kind == OinkControl::CK_WEAK_DEF) {
      usedControls.add(ctl);
      return true;
    }
  }
  return false;                 // no control has filtered it out
}

bool Controls::isUnsoundIfUsed(Variable_O *var) {
  foreachControlMatching(var) {
    if (ctl->kind == OinkControl::CK_UNSOUNED_IF_USED) {
      usedControls.add(ctl);
      return true;
    }
  }
  return false;                 // no control has filtered it out
}

void Controls::makeEmptyUsedControls() {
  usedControls.empty();
}

bool Controls::allControlsUsed() {
  FOREACH_ASTLIST_NC(OinkControl, controlList, iter) {
    OinkControl *ctl = iter.data();
    if (!usedControls.contains(ctl)) return false;
  }
  return true;
}

void Controls::printUnusedControls() {
  FOREACH_ASTLIST_NC(OinkControl, controlList, iter) {
    OinkControl *ctl = iter.data();
    if (!usedControls.contains(ctl)) {
      ctl->print(std::cout);
    }
  }
}
