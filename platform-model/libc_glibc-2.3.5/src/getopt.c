#include <glibc-fs/prelude.h>
#include <getopt.h>

char *optarg;
int optind;
int opterr;
int optopt;

// struct option
// {
//     const char *name;

//     int has_arg;
//     int *flag;
//     int val;
// };

int getopt (int argc, char *const *argv, const char *shortopts)
{
    optarg[0] = argv[0][0];
    optarg[0] = shortopts[0];
}

int getopt_long (int argc, char *const *argv,
                 const char *shortopts,
                 const struct option *longopts, int *longind)
{
    optarg[0] = argv[0][0];
    optarg[0] = shortopts[0];
    optarg[0] = longopts->name[0];
}

int getopt_long_only (int argc, char *const *argv,
                      const char *shortopts,
                      const struct option *longopts, int *longind)
{
    optarg[0] = argv[0][0];
    optarg[0] = shortopts[0];
    optarg[0] = longopts->name[0];
}

int _getopt_internal (int argc, char *const *argv,
                      const char *shortopts,
                      const struct option *longopts, int *longind,
                      int long_only)
{
    optarg[0] = argv[0][0];
    optarg[0] = shortopts[0];
    optarg[0] = longopts->name[0];
}
