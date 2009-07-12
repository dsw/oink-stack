
# usage: $(call myabspath,/foo/bar)

ifeq ($(abspath /tmp/../foo),/foo)
  # GNU make 3.81, which supports 'abspath' natively
  myabspath = $(abspath $1)
else
  # else use helper script
  myabspath = $(shell $(TOPLEVEL)/make-lib/abspath $1)
endif

.abspath-works:
	@test "$(call myabspath,/tmp/../foo)" = "/foo"

