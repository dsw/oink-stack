#include <glibc-fs/prelude.h>
#include <argp.h>

// struct argp_option
// {
//     const char *name;
//     int key;
//     const char *arg;
//     int flags;
//     const char *doc;
//     int group;
// };

// struct argp;
// struct argp_state;
// struct argp_child;

// typedef error_t (*argp_parser_t) (int key, char *arg, struct argp_state *state);

// struct argp
// {
//     const struct argp_option *options;
//     argp_parser_t parser;
//     const char *args_doc;
//     const char *doc;
//     const struct argp_child *children;
//     char *(*help_filter) (int key, const char *text, void *input);
//     const char *argp_domain;
// };

// struct argp_child
// {
//     const struct argp *argp;
//     int flags;
//     const char *header;
//     int group;
// };

// struct argp_state
// {
//     const struct argp *root_argp;
//     int argc;
//     char **argv;
//     int next;
//     unsigned flags;
//     unsigned arg_num;
//     int quoted;
//     void *input;
//     void **child_inputs;
//     void *hook;
//     char *name;
//     FILE *err_stream;
//     FILE *out_stream;
//     void *pstate;
// };

error_t argp_parse (const struct argp * argp, int argc, char ** argv, unsigned flags, int * arg_index, void * input)
{
    struct argp_state state;
    state.argv = argv;
    state.root_argp = argp;
    state.input = input;
    state.name = (char $tainted *) "";
    state.err_stream = stderr;
    state.out_stream = stdout;

    // typedef error_t (*argp_parser_t) (int key, char *arg, struct argp_state *state);
    (argp->parser) (0, argv[0], &state);
}

error_t __argp_parse (const struct argp * argp, int argc, char ** argv, unsigned flags, int * arg_index, void * input) {
    return argp_parse(argp, argc, argv, flags, arg_index, input);
}

const char *argp_program_version = (char const $tainted *) "";

void (*argp_program_version_hook) (FILE * stream, struct argp_state * state) = NULL;

const char *argp_program_bug_address = (char const $tainted *) "";

error_t argp_err_exit_status = 0;

// assume 'name' is an input that is not used as format string
void argp_help (const struct argp * argp, FILE * stream, unsigned flags, char * name) {}
void __argp_help (const struct argp * argp, FILE * stream, unsigned flags, char *name) {}

void argp_state_help (const struct argp_state * state, FILE * stream, unsigned int flags) {}
void __argp_state_help (const struct argp_state * state, FILE * stream, unsigned int flags) {}

void argp_usage (const struct argp_state *state) {}
void __argp_usage (const struct argp_state *state) {}

void argp_error (const struct argp_state * state, const char $untainted * fmt, ...) {}
void __argp_error (const struct argp_state * state, const char $untainted * fmt, ...) {}

void argp_failure (const struct argp_state * state, int status, int errnum, const char $untainted * fmt, ...) {}
void __argp_failure (const struct argp_state * state, int status, int errnum, const char $untainted * fmt, ...) {}

int _option_is_short (const struct argp_option *opt) {}
int __option_is_short (const struct argp_option *opt) {}

int _option_is_end (const struct argp_option *opt) {}
int __option_is_end (const struct argp_option *opt) {}

void *_argp_input (const struct argp * argp, const struct argp_state * state) {}
void *__argp_input (const struct argp * argp, const struct argp_state * state) {}
