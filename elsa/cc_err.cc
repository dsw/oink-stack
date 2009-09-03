// cc_err.cc            see license.txt for copyright and terms of use
// code for cc_err.h

#include "cc_err.h"      // this module
#include "trace.h"       // tracingSys
#include "strutil.h"     // trimWhitespace


// ----------------- ErrorMsg -----------------
ErrorMsg::~ErrorMsg()
{}


string ErrorMsg::toString() const
{
  stringBuilder sb;
  sb << ::toString(loc) << ": ";
  if (flags & EF_WARNING) {
    sb << "warning";
  }
  else {
    sb << "error";
  }
  sb << ": " << msg;

  bool msgHasNL = !!strchr(sb.c_str(), '\n');
  bool addedNewline = false;

  if (instLoc[0] && msgHasNL) {
    // for a multi-line message, put instLoc on its own line
    // and with no leading whitespace
    sb << "\n" << trimWhitespace(instLoc);
  }
  else {
    sb << instLoc;
  }

  char const *extra = NULL;
  if (flags & EF_FROM_TEMPLATE)  {
    if (!(flags & EF_WARNING)) {
      extra = "(from template; would be suppressed in permissive mode)";
    }
    else if (flags & EF_STRICT_ERROR) {
      extra = "(from template; would have been an error in strict mode)";
    }
  }

  if (extra) {
    if (msgHasNL && !addedNewline) {
      sb << "\n";
    }
    else {
      sb << " ";
    }
    sb << extra;
  }

  return sb;
}


// ---------------- ErrorList -----------------
ErrorList::ErrorList()
  : list()
{}

ErrorList::~ErrorList()
{
  list.deleteAll();
}


void ErrorList::addError(ErrorMsg * /*owner*/ obj)
{
  list.prepend(obj);    // O(1)
}

void ErrorList::prependError(ErrorMsg * /*owner*/ obj)
{
  list.append(obj);     // O(n)
}


void ErrorList::takeMessages(ErrorList &src)
{ 
  if (list.isEmpty()) {
    // this is a common case, and 'concat' is O(1) in this case
    list.concat(src.list);
    return;
  }

  // put all of my messages (which semantically come first) onto the
  // end of the 'src' list; this takes time proportional to the
  // length of 'src.list'
  src.list.concat(list);

  // now put them back onto my list, which will take O(1) since
  // my list is now empty
  list.concat(src.list);
}


void ErrorList::prependMessages(ErrorList &src)
{
  list.concat(src.list);
}


void ErrorList::markAllAsWarnings()
{
  markAllWithFlag(EF_WARNING);
}

void ErrorList::markAllAsFromDisamb()
{
  markAllWithFlag(EF_FROM_DISAMB);
}


void ErrorList::markAllWithFlag(ErrorFlags flags)
{
  FOREACH_OBJLIST_NC(ErrorMsg, list, iter) {
    iter.data()->flags |= flags;
  }
}


void ErrorList::filter(bool (*pred)(ErrorMsg *msg))
{
  ObjListMutator<ErrorMsg> mut(list);
  while (!mut.isDone()) {
    if (pred(mut.data())) {
      // keep it
      mut.adv();
    }
    else {
      // drop it
      mut.deleteIt();
    }
  }
}


int ErrorList::count() const
{
  return list.count();
}

int ErrorList::numErrors() const
{
  return count() - numWarnings();
}

int ErrorList::numWarnings() const
{
  return countWithAnyFlag(EF_WARNING);
}


bool ErrorList::hasDisambErrors() const
{
  return countWithAnyFlag(EF_DISAMBIGUATES) > 0;
}


int ErrorList::countWithAnyFlag(ErrorFlags flags) const
{
  int ct=0;
  FOREACH_OBJLIST(ErrorMsg, list, iter) {
    if (iter.data()->flags & flags) {
      ct++;
    }
  }
  return ct;
}


bool ErrorList::hasFromNonDisambErrors() const
{
  return countWithAnyFlag(EF_WARNING | EF_FROM_DISAMB | EF_FROM_TEMPLATE) <
         count();
}


void ErrorList::print(std::ostream &os) const
{
  os << printToString();
}


string ErrorList::printToString() const
{
  stringBuilder sb;

  // need to temporarily reverse it, but I promise to restore it
  // when I'm done
  ObjList<ErrorMsg> &nclist = const_cast<ObjList<ErrorMsg>&>(list);

  bool printWarnings = !tracingSys("nowarnings");

  nclist.reverse();
  FOREACH_OBJLIST(ErrorMsg, nclist, iter) {
    if (printWarnings || !iter.data()->isWarning()) {
      sb << iter.data()->toString() << "\n";
    }
  }
  nclist.reverse();

  return sb;
}


// EOF
