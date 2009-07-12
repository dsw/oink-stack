
Software Stack
--------------

This repository contains generic scripts for what we call a "software stack":
it is a collection of repositories that together form one large project of
composable parts.  The name is reminiscent of the TCP/IP networking stack,
which we think of as being organized this way.


To use
------

To create a software stack, populate a new repository with:
    - StackRepositories.list, a text list of child repositories
    - Makefile and configure can either symlink or include the ones in this
      repository.

If you use Subversion, you can use the svn:externals property to softlink to
the child repositories.


License
-------

See License.txt for copyright and terms of use.  (Commonly known as the "BSD"
license.)


Contact
-------

Karl Chen <quarl@cs.berkeley.edu>
Daniel Wilkerson <dsw@cs.berkeley.edu>
