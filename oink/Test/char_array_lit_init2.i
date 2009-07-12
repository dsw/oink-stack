struct capi_driver
{
  char revision[32];
};

static struct capi_driver b1isa_driver = {
  revision:"0.0",
};

static int
b1isa_init (void)
{
  struct capi_driver *driver = &b1isa_driver;
  driver->revision[sizeof (driver->revision) - 1] = 0;
}
