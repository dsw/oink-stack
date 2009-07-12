// check that we can have newlines in string literals

int main() {
  char *x = "hello";
  char *y = "hello  
asdfasdf";
}

extern inline void __set_64bit (unsigned long long * ptr,
		unsigned int low, unsigned int high)
{
__asm__ __volatile__ (
	"1:	movl (%0), %%eax;
		movl 4(%0), %%edx;
		cmpxchg8b (%0);
		jnz 1b"
	::		"D"(ptr),
			"b"(low),
			"c"(high)
	:
			"ax","dx","memory");
}
