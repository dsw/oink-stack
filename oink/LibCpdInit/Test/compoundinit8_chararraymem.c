struct pm2fb_par {
  int x;
  int video;
};

struct A {
  // this is a point-type, which won't work if we do something
  // perverse and initialize a char array with a sequence of
  // characters
  char name[16];
  struct pm2fb_par par;
};

struct A user_mode[] = {
  {"640x480-60", {8,121}},
};
