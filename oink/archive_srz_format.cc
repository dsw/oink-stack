// see License.txt for copyright and terms of use

// $Id: archive_srz_format.cc 1924 2006-05-16 09:17:55Z quarl $

#include "archive_srz_format.h"
#include "oink_util.h"

void ArchiveSrzFormatChecker::input(std::istream &in)
{
  std::string line;
  while (std::getline(in, line)) {
    std::string::size_type i = line.find('=');
    if (i == std::string::npos) {
      userFatalError(SL_UNKNOWN, "Error reading format file %s, invalid line '%s'",
                     fname, line.c_str());
    }
    std::string key = line.substr(0,i);
    std::string value = line.substr(i+1);

    vmap[key] = value;
  }
}

void ArchiveSrzFormatChecker::opt(std::string const &key, std::string const &expectedValue)
{
  std::string const &value = vmap[key];

  if (value.empty()) {
    userFatalError(SL_UNKNOWN, "Error reading format file %s, missing required value for '%s'",
                   fname, key.c_str());
  }

  if (value != expectedValue) {
    userFatalError(SL_UNKNOWN, "Inconsistent format or options (from %s): %s=%s; expected %s=%s",
                   fname,
                   key.c_str(), value.c_str(),
                   key.c_str(), expectedValue.c_str());
  }
}

void ArchiveSrzFormatWriter::opt(std::string const &key, std::string const &value)
{
  out << key << "=" << value << '\n';
}
