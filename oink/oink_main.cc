// see License.txt for copyright and terms of use

// The main() function for the oink empty-analysis proper.

#include "oink.h"
#include "oink_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "trace.h"
#include "oink_util.h"
#include "warning_mgr.h"        // WarningManager

int main(int argc, char **argv) {
  set_argv0(argv[0]);
  traceAddFromEnvVar();
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = new OinkCmd;
  ExitCode code = NORMAL_ExitCode;
  CCParse_Oink ccParse(globalStrTable, globalLang);
  Oink m;
  m.parseUserActions = &ccParse;
  m.parseEnv = &ccParse;
  m.typePrinter = &typePrinterOink;
  m.initArchiveSrzManager(".odir", ".oz");

  try {
    try {
      m.init_stage(argc, argv);

      if (oinkCmd->print_sizes) { m.printSizes(); return 0; }

      m.parse_stage();
      m.typecheck_stage();
      m.elaborate_stage();
      m.markRealVars_stage();

      if (moduleList.isNotEmpty()) m.build_classFQName2Module();
      if (oinkCmd->print_func_attrs) m.printFuncAttrs_stage();
      if (oinkCmd->func_gran) {
        m.compute_funcGran();
        m.print_funcGran();
        return code;            // perhaps this should be done differently
      }
      m.filter_stage();
      m.registerVars_stage();
      m.value_stage();
      m.integrity_stage();      // FIX: I don't think this is needed anymore
      m.exclude_syntax_stage();
      m.prettyPrint_stage();

      // NOTE: this is NOT parallel to qual because we do NOT save the
      // results of dataflow in oink, whereas we DO in qual; therefore
      // we have to re-do the dataflow in oink AFTER we deserialize,
      // whereas in qual we do the dataflow BEFORE we deserialize
      m.deserialize_stage();

      m.simpleDataflow_stage();
      m.checkForUsedVars_stage();
      m.link_stage();

      if (!oinkCmd->srz.empty()) m.serialize_results();
      if (oinkCmd->print_buckets) m.printBuckets();
      if (oinkCmd->print_stats) m.printStats();
      if (oinkCmd->print_proc_stats) m.printProcStats();

    } catch (xBase &e) {
      if (e.why()[0]) std::cerr << argv0 << ": " << e << std::endl;
      code = INTERNALERROR_ExitCode;
      if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
    }

    WarningManager::summarize();
    m.printStage("cleanup");
    return code;

  } catch (std::bad_alloc & e) {
    explain_bad_alloc();
  }
}
