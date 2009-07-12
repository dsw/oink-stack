// (Info-goto-node "(gcc-4.0) Designated Inits")
//
// Note that the length of the array is the highest value specified plus one.

// ERR-MATCH: array index evaluates to

char *array[] = { [5] = "foo" };
