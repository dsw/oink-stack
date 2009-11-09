// see License.txt for copyright and terms of use

// Just the main() function.

#include "alloctool.h"
#include "alloctool_cmd.h"
#include "alloctool_global.h"

#include "qual_value_print.h"    // QualCTypePrinter
#include "qual_value_children.h" // ValueFactory_Q
#include "qual_ast_aux.h"        // ASTPrinter_Q
#include "qual.gr.gen.h"         // CCParse_Qual
// #include "oink.gr.gen.h"        // CCParse_Oink

#include "trace.h"
#include "oink_util.h"

int main(int argc, char **argv) {
  set_argv0(argv[0]);
  traceAddFromEnvVar();
  tFac = new TypeFactory_Q;
  vFac = new ValueFactory_Q;
  astPrinter = new ASTPrinter_Q;
  QualCTypePrinter typePrinterCQ(/*printTransQual*/false);
  oinkCmd = alloctoolCmd = new AllocToolCmd;
  ExitCode code = NORMAL_ExitCode;
  Value::allow_annotation = true;
  CCParse_Qual ccParse(globalStrTable, globalLang);
  AllocTool m;
  m.parseUserActions = &ccParse;
  m.parseEnv = &ccParse;
  m.typePrinter = &typePrinterCQ;
  try {
    m.init_stage(argc, argv);

    if (oinkCmd->print_sizes) { m.printSizes(); return 0; }

    m.parse_stage();
    m.typecheck_stage();
    m.elaborate_stage();
    m.markRealVars_stage();

    if (oinkCmd->func_gran) {
      m.compute_funcGran();
      m.print_funcGran();
      return code;            // perhaps this should be done differently
    }

    if (moduleList.isNotEmpty()) m.build_classFQName2Module();
    m.filter_stage();
    m.prettyPrint_stage();

    // testing
    if (alloctoolCmd->print_stack_alloc) {
      m.printStackAlloc_stage();
    }
    if (alloctoolCmd->print_stack_alloc_addr_taken) {
      m.printStackAllocAddrTaken_stage();
    }

    // transforms
    if (alloctoolCmd->heapify_stack_alloc_addr_taken) {
      m.heapifyStackAllocAddrTaken_stage();
    }
    if (alloctoolCmd->verify_cross_module_params) {
      m.verifyCrossModuleParams_stage();
    }
    if (alloctoolCmd->localize_heap_alloc) {
      m.localizeHeapAlloc_stage();
    }
  } catch (xBase &e) {
    std::cerr << e << std::endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }

  delete alloctoolCmd;
  return code;
}
