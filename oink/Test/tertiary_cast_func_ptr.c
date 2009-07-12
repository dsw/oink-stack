
typedef void (*__sighandler_t) (int);
void die(int sig){}

int main()
{
    0 ? die : (__sighandler_t) die;
}
