
// typedef union sigval
// {
//     int sival_int;
//     void *sival_ptr;
// } sigval_t;
// #endif

// typedef struct siginfo
// {
//     int si_signo;
//     int si_errno;
//     int si_code;

//     union
//     {
// 	int _pad[__SI_PAD_SIZE];

// 	struct
//         {
// 	    __pid_t si_pid;
// 	    __uid_t si_uid;
//         } _kill;

// 	struct
//         {
// 	    int si_tid;
// 	    int si_overrun;
// 	    sigval_t si_sigval;
//         } _timer;

// 	struct
//         {
// 	    __pid_t si_pid;
// 	    __uid_t si_uid;
// 	    sigval_t si_sigval;
//         } _rt;

// 	struct
//         {
// 	    __pid_t si_pid;
// 	    __uid_t si_uid;
// 	    int si_status;
// 	    __clock_t si_utime;
// 	    __clock_t si_stime;
//         } _sigchld;

// 	struct
//         {
// 	    void *si_addr;
//         } _sigfault;

// 	struct
//         {
// 	    long int si_band;
// 	    int si_fd;
//         } _sigpoll;
//     } _sifields;
// } siginfo_t;

// typedef struct sigevent
// {
//     sigval_t sigev_value;
//     int sigev_signo;
//     int sigev_notify;

//     union
//     {
// 	int _pad[__SIGEV_PAD_SIZE];

// 	__pid_t _tid;

// 	struct
//         {
// 	    void (*_function) (sigval_t);
// 	    void *_attribute;
//         } _sigev_thread;
//     } _sigev_un;
// } sigevent_t;
