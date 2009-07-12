// from the kernel
enum iscsiAuthKeyType_t {
  iscsiAuthKeyTypeAuthMethod,
};

void iscsiAuthClientCheckAuthMethodKey() {
  int recv[10];
  recv[iscsiAuthKeyTypeAuthMethod];
}
