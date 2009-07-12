# $Id$

# libstdc++ not working yet

.PHONY: all link linkcheck clean
all link linkcheck clean:
	cd libc_glibc-2.3.5 && $(MAKE) $@
#cd libstdc++_gcc-3.4 && $(MAKE) $@

# FIX: dsw: what is check0 ?
.PHONY: check check-quick check-full check0
check check-quick check-full:
	cd test && $(MAKE) $@

.PHONY: distclean
distclean: clean
	rm -f config.status
