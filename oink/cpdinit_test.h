// see License.txt for copyright and terms of use

// test compoundInit

#ifndef CPDINIT_TEST_H
#define CPDINIT_TEST_H

#include "oink.h"
#include "oink_global.h"

class CpdInitTest : public virtual Oink {
  public:
  // tor ****
  CpdInitTest() {}
  
  // methods ****
  void compoundTest_stage();
};

#endif // CPDINIT_TEST_H
