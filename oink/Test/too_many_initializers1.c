// ERR-MATCH: ef9ea6b0-3391-4057-92b1-4d49f225dc0f
// Too many initializers for type at expression initializer

// This should be a warning instead of error since gcc accepts with warning.

int foo[0] = { 0 };
