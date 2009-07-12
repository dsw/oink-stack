// see License.txt for copyright and terms of use

// quarl 2006-06-22
//    Debug macros.  By default, gcc -DNDEBUG disables expensive things.

#ifndef OINK_DEBUG_H
#define OINK_DEBUG_H

#ifdef NDEBUG
# define DEBUG_INSTANCE_SPECIFIC_VALUES 0
#else
# define DEBUG_INSTANCE_SPECIFIC_VALUES 1
#endif

#endif

