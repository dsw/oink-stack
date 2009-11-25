// see License.txt for copyright and terms of use

// Just the main() function.

#include "xform.h"
#include "xform_cmd.h"
#include "xform_global.h"

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
  oinkCmd = xformCmd = new XformCmd;
  ExitCode code = NORMAL_ExitCode;
  Value::allow_annotation = true;
  CCParse_Qual ccParse(globalStrTable, globalLang);
  Xform m;
  m.parseUserActions = &ccParse;
  m.parseEnv = &ccParse;
  m.typePrinter = &typePrinterCQ;
  try {
    m.init_stage(argc, argv);

    if (oinkCmd->print_sizes) { m.printSizes(); return 0; }

    m.parse_stage();
    m.typecheck_stage();
    // transformations do not need the elaboration stage
//     m.elaborate_stage();
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
    if (xformCmd->print_stack_alloc) {
      m.printStackAlloc_stage();
    }
    if (xformCmd->print_stack_alloc_addr_taken) {
      m.printStackAllocAddrTaken_stage();
    }

    // transforms
    if (xformCmd->heapify_stack_alloc_addr_taken) {
      m.heapifyStackAllocAddrTaken_stage();
    }
    if (xformCmd->verify_cross_module_params) {
      m.verifyCrossModuleParams_stage();
    }
    if (xformCmd->localize_heap_alloc) {
      m.localizeHeapAlloc_stage();
    }
    if (xformCmd->jimmy) {
      m.jimmy_stage();
    }
  } catch (xBase &e) {
    std::cerr << e << std::endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }

  delete xformCmd;
  return code;
}
