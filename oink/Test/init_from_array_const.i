//  /home/dsw/ballA/./am-utils-6.0.9-2/amq-rYdE.i:11464:13: error: prior declaration of `copyright' at /home/dsw/ballA/./am-utils-6.0.9-2/amq-rYdE.i:6:6 had type `char const [252]', but this one uses `char []'

// foo should remain a char[] not become a char const[].

char foo[] = "boink";
extern char foo[];
