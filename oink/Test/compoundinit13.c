// qual: Assertion failed: stack0.isNotEmpty() &&
// "3ba6003d-d210-4211-b01b-59a3f41b5c0d", file LibCpdInit/member_type_iter.h
// line 67

// from bind9_1:9.2.4-1/i/journal.c.8cc83b1c768688af9a0f59ac31f68184.i

typedef struct {
  int serial;
  int offset;
} journal_pos_t;

typedef struct {
  unsigned char format[16];
  journal_pos_t begin;
  journal_pos_t end;
  int index_size;
} journal_header_t;

typedef struct {
  int size;
  int serial0;
  int serial1;
} journal_xhdr_t;

typedef struct {
  int size;
} journal_rrhdr_t;

journal_header_t initial_journal_header = {
  "foo",
  { 0, 0 },
  { 0, 0 },
  0
};

