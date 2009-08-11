// see License.txt for copyright and terms of use

// Just the main() function for the qual analysis proper.

#include "qual.h"
#include "qual_global.h"
#include "qual_dataflow_ex.h"   // DataFlowEx_Qual
#include "qual_value_children.h" // ValueFactory_Q
#include "qual_ast_aux.h"       // ASTPrinter_Q
#include "qual.gr.gen.h"        // CCParse_Qual

// FIX: remove this eventually
#include "qual_dataflow_ty.h"   // DataFlowTy_Qual

#include "trace.h"
#include "oink_util.h"
#include "warning_mgr.h"        // WarningManager

Qual *theQual = NULL;

int main(int argc, char **argv) {
  set_argv0(argv[0]);
  traceAddFromEnvVar();
  tFac = new TypeFactory_Q;
  vFac = new ValueFactory_Q;
  astPrinter = new ASTPrinter_Q;
  QualCTypePrinter typePrinterCQ(/*printTransQual*/false);
  oinkCmd = qualCmd = new QualCmd;
  ExitCode code = NORMAL_ExitCode;
  DataFlowTy_Qual dftq;
  DataFlowEx_Qual dfeq(&dftq);
  dftq.dfe = &dfeq;             // FIX: get rid of this
  Value::allow_annotation = true;
  CCParse_Qual ccParse(globalStrTable, globalLang);
  Qual m(dfeq);
  theQual = &m;
  m.parseUserActions = &ccParse;
  m.parseEnv = &ccParse;
  m.typePrinter = &typePrinterCQ;
  m.initArchiveSrzManager(".qdir", ".qz");

  try {
    try {
      m.init_stage(argc, argv);
      m.configure();

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
      m.filter_stage();
      m.registerVars_stage();
      m.value_stage();
      m.integrity_stage(); // FIX: I don't think this is needed anymore
      m.markInstanceSpecificValues_stage();
      m.moduleAnalysis_stages();
      m.exclude_syntax_stage();
      m.exclude_qual_stage();
      m.qualCompile_stage();
      m.checkForUsedVars_stage();

      m.deserialize_stage();
      m.link_stage();
      bool qualifierInconsistency = m.finishQuals_stage();

      m.markExtern_stage();
      m.compactify_stage();
      if (!qualCmd->srz.empty()) m.serialize_results();
      if (qualifierInconsistency && qualCmd->ret_inf) {
        code = INFERENCE_FAILURE_ExitCode;
      }

      Restorer<bool> res(typePrinterCQ.printTransQual,
                         qualCmd->print_trans_qual);
      m.prettyPrint_stage();
      if (oinkCmd->print_buckets) m.printBuckets();
      if (oinkCmd->print_stats) m.printStats();
      if (oinkCmd->print_proc_stats) m.printProcStats();

    } catch (xBase &e) {
      if (e.why()[0]) cerr << argv0 << ": " << e << endl;
      code = INTERNALERROR_ExitCode;
      if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
    }

    WarningManager::summarize(); // summarize omitted warnings
    m.printStage("cleanup");     // deallocate
    cleanup_libqual();
    // cleanup_qualAnnotMap();
    return code;

  } catch (std::bad_alloc & e) {
    explain_bad_alloc();
  }
}
