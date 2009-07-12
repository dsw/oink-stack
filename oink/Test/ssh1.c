// from Matt
// This one is not legal:
//  int sprintf1(char $_1_2 * str, const char $untainted * format, $_1 ...);

int sprintf2(char $_1_2 * str, const char $untainted * format, ... $_1 );
int sprintf3(char $_1_2 * str, const char $untainted * format, ...);
int sprintf4(char $_1_2 * str, const char $untainted * format...);
int sprintf5(char $_1_2 * str, const char $untainted * format ...);
int sprintf6(char $_1_2 * str, const char $untainted * format, ... $tainted );
