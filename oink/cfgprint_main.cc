// see License.txt for copyright and terms of use

// Just the main() function.

#include "cfgprint.h"
#include "cfgprint_cmd.h"
#include "cfgprint_global.h"
#include "trace.h"
#include "oink_util.h"
#include "oink.gr.gen.h"        // CCParse_Oink

int main(int argc, char **argv) {
  traceAddFromEnvVar();
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = cfgPrintCmd = new CfgPrintCmd;
  ExitCode code = NORMAL_ExitCode;
  CfgPrint m;
  m.typePrinter = &typePrinterOink;
  try {
    m.init_stage(argc, argv);
    CCParse_Oink ccParse(globalStrTable, globalLang);
    m.parseUserActions = &ccParse;
    m.parseEnv = &ccParse;
    m.parse_stage();
    m.typecheck_stage();
    m.elaborate_stage();
    m.markRealVars_stage();
    m.filter_stage();
    m.computeCfg_stage();
    m.prettyPrint_stage();
    if (cfgPrintCmd->print_cfg) m.printCfg_stage();
  } catch (xBase &e) {
    std::cerr << e << std::endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }
  delete cfgPrintCmd;
  return code;
}
