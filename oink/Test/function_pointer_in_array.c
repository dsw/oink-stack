char $tainted * getenv(char const*) {}
int printf(char const $untainted * fmt, ...) {}

void printer(const char* s) { printf(s); }

typedef void (*printer_t)(const char*);

int main() {
  const char* danger;
  danger = getenv("HOME");                          // bad

  printer_t printers[] = { printer };

  printer_t p;
  // p = printer;
  p = printers[0];

  p(danger);
}

