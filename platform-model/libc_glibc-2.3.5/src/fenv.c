#include <fenv.h>

int feclearexcept (int excepts) {}

int fegetexceptflag (fexcept_t *flagp, int excepts) {}

int feraiseexcept (int excepts) {}

int fesetexceptflag (const fexcept_t *flagp, int excepts) {}

int fetestexcept (int excepts) {}

int fegetround (void) {}

int fesetround (int rounding_direction) {}

int fegetenv (fenv_t *envp) {}

int feholdexcept (fenv_t *envp) {}

int fesetenv (const fenv_t *envp) {}

int feupdateenv (const fenv_t *envp) {}

int feenableexcept (int excepts) {}

int fedisableexcept (int excepts) {}

int fegetexcept (void) {}
