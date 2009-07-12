void $_1_2 * memcpy(void $_1_2 * dest, void const $_1 * src, unsigned int n) {}

struct S1 { char buf[20]; };

int main()
{
    struct S1 s1a;
    struct S1 s1b;

    char $tainted t;
    char $untainted u;

    s1a.buf[0] = t;

    memcpy(&s1b, &s1a, sizeof(struct S1));

    u = s1b.buf[0];
}
