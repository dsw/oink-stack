# $Id: Makefile 3461 2006-05-15 09:36:07Z quarl $

include stack-scripts/Makefile

.PHONY: go
go:
	./configure
	$(MAKE) clean all check
