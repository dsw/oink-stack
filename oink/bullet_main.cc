// see License.txt for copyright and terms of use

// Just the main() function.

#include "bullet.h"
#include "bullet_cmd.h"
#include "bullet_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "trace.h"
#include "oink_util.h"

int main(int argc, char **argv) {
  traceAddFromEnvVar();
  tFac = new TypeFactory_O;
  vFac = new ValueFactory;
  astPrinter = new ASTPrinter_O;
  oinkCmd = bulletCmd = new BulletCmd;
  ExitCode code = NORMAL_ExitCode;
  Bullet m;
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
    m.prettyPrint_stage();
    // put the compilation pass here
//     if (bulletCmd->print_ihg) m.printIhg_stage();
  } catch (xBase &e) {
    cerr << e << endl;
    code = INTERNALERROR_ExitCode;
    if (UserError *ue = dynamic_cast<UserError*>(&e)) code = ue->exitCode;
  }
  delete bulletCmd;
  return code;
}
