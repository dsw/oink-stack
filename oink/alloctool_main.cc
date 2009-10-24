// see License.txt for copyright and terms of use

// Just the main() function.

#include "alloctool.h"
#include "alloctool_cmd.h"
#include "alloctool_global.h"
#include "qual.gr.gen.h"        // CCParse_Qual
// #include "oink.gr.gen.h"        // CCParse_Oink
#include "trace.h"
#include "oink_util.h"

int main(int argc, char **argv) {
  traceAddFromEnvVar();
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = alloctoolCmd = new AllocToolCmd;
  ExitCode code = NORMAL_ExitCode;
  AllocTool m;
  m.typePrinter = &typePrinterOink;
  try {
    m.init_stage(argc, argv);
    CCParse_Qual ccParse(globalStrTable, globalLang);
    m.parseUserActions = &ccParse;
    m.parseEnv = &ccParse;
    m.parse_stage();
    m.typecheck_stage();
    m.elaborate_stage();
    m.markRealVars_stage();
    m.filter_stage();
    m.prettyPrint_stage();
    // alloctool-specific features go here
    if (alloctoolCmd->print_stack_alloc) {
      m.printStackAlloc_stage();
    }
    if (alloctoolCmd->print_stack_alloc_addr_taken) {
      m.printStackAllocAddrTaken_stage();
    }
    if (alloctoolCmd->heapify_stack_alloc_addr_taken) {
      m.heapifyStackAllocAddrTaken_stage();
    }
  } catch (xBase &e) {
    std::cerr << e << std::endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }
  delete alloctoolCmd;
  return code;
}
