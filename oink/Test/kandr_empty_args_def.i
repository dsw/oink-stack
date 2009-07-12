// In K&R C *sometimes* a function need not provide its parameters
// every time.  Here are some real examples.

//  /home/dsw/ballA/./4Suite-0.11.1-13/Oif.tab-ddlI.i:8948:1: error:
//  prior declaration of `Oifparse' at
//  /home/dsw/ballA/./4Suite-0.11.1-13/Oif.tab-ddlI.i:8943:5 had type
//  `int ()()', but this one uses `int ()(...)'
int Oifparse (void);
int Oifparse() {}

//  /home/dsw/ballA/./a2ps-4.13b-28/cc6V7yEj-yLJK.i:558:5: error:
//  prior declaration of `closedir' at
//  /home/dsw/ballA/./a2ps-4.13b-28/cc6V7yEj-yLJK.i:495:12 had type
//  `int ()(struct __dirstream *__dirp)', but this one uses `int
//  ()(...)'
extern int closedir (void *__dirp) ;
int closedir ();

//  /home/dsw/ballA/./a2ps-4.13b-28/long-options-gHMx.i:1086:1: error:
//  prior declaration of `parse_long_options' at
//  /home/dsw/ballA/./a2ps-4.13b-28/long-options-gHMx.i:895:3 had type
//  `void ()(int _argc, char **_argv, char const *_command_name, char
//  const *_package, char const *_version, char const *_authors, void
//  (*_usage)(int /*anon*/))', but this one uses `void ()(int argc,
//  char **argv, char const *command_name, char const *package, char
//  const *version, char const *authors, void (*usage_func)(...))'
void
parse_long_options (int _argc,
                    char **_argv,
                    const char *_command_name,
                    const char *_package,
                    const char *_version,
                    const char *_authors,
                    void (*_usage) (int));

void
parse_long_options (int argc,
                    char **argv,
                    const char *command_name,
                    const char *package,
                    const char *version,
                    const char *authors,
                    void (*usage_func)())
{}
