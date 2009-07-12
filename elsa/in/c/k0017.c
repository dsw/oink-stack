// k0017.c:5:53: error: E_fieldAcc is not constEval'able

// from aio.c on IA-64.

struct S {
	int A[32];
};

long x[(((char *) &((struct S *) 0)->A[0]) - (char *) 0)];

