file /Users/dsw/Notes/oink-stack/oink/qual
# set args -q-config ho.lattice  -tr no-orig-offset -fq-poly -fo-verbose -fo-no-instance-sensitive -fq-casts-preserve-below-functions -fq-module-access -fq-module-trust -o-module foo -o-module-default main hello.i
set args -q-config ho.lattice  -tr no-orig-offset -fq-poly -fo-verbose -fo-no-instance-sensitive -fq-casts-preserve-below-functions -fq-module-access -fq-module-trust -o-module foo -o-module not_foo -o-module-default not_foo hello.i lib_foo.i

break main
break breaker

run
