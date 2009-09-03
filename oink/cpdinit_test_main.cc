// see License.txt for copyright and terms of use

#include "cpdinit_test.h"
#include "cpdinit_test_visitor.h"
#include "oink_global.h"        // oinkCmd
#include "oink_util.h"          // ExitCode
#include "oink.gr.gen.h"        // CCParse_Oink

int main(int argc, char **argv) {
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = new OinkCmd();
  ExitCode code = NORMAL_ExitCode;
  CpdInitTest m;
  try {
    m.init_stage(argc, argv);
    CCParse_Oink ccParse(globalStrTable, globalLang);
    m.parseUserActions = &ccParse;
    m.parseEnv = &ccParse;
    m.parse_stage();
    m.typecheck_stage();
    m.markRealVars_stage();
    m.filter_stage();
    m.compoundTest_stage();
  } catch (xBase &e) {
    std::cerr << e << std::endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }
  delete oinkCmd;
  return code;
}
