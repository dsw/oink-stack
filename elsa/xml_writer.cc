// xml_writer.cc            see license.txt for copyright and terms of use

#include "xml_writer.h"
#include "xmlhelp.h"            // writeSpaces
#include "exc.h"                // xBase


bool sortNameMapDomainWhenSerializing = true;

XmlWriter::XmlWriter(IdentityManager &idmgr0, ostream *out0, int &depth0, bool indent0)
  : idmgr(idmgr0)
  , out(out0)
  , depth(depth0)
  , indent(indent0)
{}

void XmlWriter::newline() {
  xassert(out != NULL);
  *out << '\n';
  // FIX: turning off indentation makes the output go way faster, so
  // this loop is worth optimizing, probably by printing chunks of 10
  // if you can or something logarithmic like that.
  if (indent) {
    writeSpaces(*out, depth);
  }
}

