// see License.txt for copyright and terms of use

// Just the main() function.

#include "dfgprint.h"
#include "dfgprint_cmd.h"
#include "dfgprint_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "trace.h"
#include "oink_util.h"

int main(int argc, char **argv) {
  traceAddFromEnvVar();
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = dfgPrintCmd = new DfgPrintCmd;
  ExitCode code = NORMAL_ExitCode;
  DfgPrint m;
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
    m.registerVars_stage();
    m.value_stage();
    m.uniValues_stage();
    m.nameValues_stage();
    m.prettyPrint_stage();
    if (dfgPrintCmd->print_dfg) m.printDfg_stage();
  } catch (xBase &e) {
    cerr << e << endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }
  delete dfgPrintCmd;
  return code;
}
