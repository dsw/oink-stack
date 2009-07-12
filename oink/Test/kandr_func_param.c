//  ./amanda-2.4.3-4/amadmin-HkXE.i.c_out:/home/ballAruns/tmpfiles/./amanda-2.4.3-4/amadmin-HkXE.i:8605:6: error: prior declaration of `diskloop' at /home/ballAruns/tmpfiles/./amanda-2.4.3-4/amadmin-HkXE.i:8368:6 had type `void ()(int argc, char **argv, char *cmdname, void (*func)(struct disk_s *dp))', but this one uses `void ()(int argc, char **argv, char *cmdname, void *func)'

typedef struct disk_s {
    int line;
} disk_t;

void diskloop (void (*func) (disk_t *dp));
void diskloop(func)
    void (*func) (disk_t *dp);
{}
