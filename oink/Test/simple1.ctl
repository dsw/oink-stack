# a control file

  ignore-body-qual:
                file = foo.i
      name = f
# use the default visibility
# visibility = extern


# a few extra blank lines

ignore-body-qual:
   file =    bar.i              # some comments mixed with data
name = ::foo::f2 SIG (int x, char y)
visibility = extern



ignore-body-qual:
file=bar.i
name=::foo::f3                  # some comments mixed with data
visibility=static


# final comment
