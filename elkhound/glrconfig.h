// glrconfig.h            see license.txt for copyright and terms of use
// compile-time configuration options which affect the generated
// GLR parser, and the interface to the user actions

#ifndef GLRCONFIG_H
#define GLRCONFIG_H


// when NO_GLR_SOURCELOC is #defined, we disable all support for
// automatically propagating source location information in the
// parser; user actions can still refer to 'loc', but they just get
// a dummy no-location value
#ifndef GLR_SOURCELOC
  #define GLR_SOURCELOC 1        // set by ./configure
#endif

#if GLR_SOURCELOC
  #define SOURCELOC(stuff) stuff
  #define ENDSOURCELOC(stuff) stuff

  // this one adds a leading comma (I can't put that into the
  // argument <stuff>, because then it looks like the macro is
  // being passed 2 arguments)
  #define SOURCELOCARG(stuff) , stuff
  #define ENDSOURCELOCARG(stuff) , stuff

  #define NOSOURCELOC(stuff)
#else
  #define SOURCELOC(stuff)
  #define SOURCELOCARG(stuff)
  #define NOSOURCELOC(stuff) stuff
#endif


// when enabled, NODE_COLUMN tracks in each stack node the
// appropriate column to display it for in debugging dump.
// in the new RWL core, this is required to always be 1.
#ifndef ENABLE_NODE_COLUMNS
  #define ENABLE_NODE_COLUMNS 1
#endif
#if ENABLE_NODE_COLUMNS
  #define NODE_COLUMN(stuff) stuff
#else
  #define NODE_COLUMN(stuff)
#endif


// when enabled, YIELD_COUNT keeps track of the number of times a
// given semantic value is yielded; this is useful for warning the
// user when a merge is performed but one of the merged values has
// already been yielded to another semantic action, which implies
// that the induced parse forest is incomplete
#ifndef ENABLE_YIELD_COUNT
  #define ENABLE_YIELD_COUNT 1
#endif
#if ENABLE_YIELD_COUNT
  #define YIELD_COUNT(stuff) stuff
#else
  #define YIELD_COUNT(stuff)
#endif


// when true, error entries in the action table are extracted into
// their own bitmap; this then enables compression on the action
// table, since it makes it sparse
#ifndef ENABLE_EEF_COMPRESSION
  #define ENABLE_EEF_COMPRESSION 0
#endif

// when true, the action and goto tables are compressed using
// graph coloring
#ifndef ENABLE_GCS_COMPRESSION
  #define ENABLE_GCS_COMPRESSION 0
#endif

// when true, action and goto *columns* are merged during GCS;
// otherwise, only rows are merged
#ifndef ENABLE_GCS_COLUMN_COMPRESSION
  #define ENABLE_GCS_COLUMN_COMPRESSION 0
#endif

// when true, entries in the action and goto tables are a
// 1-byte index into an appropriate map
#ifndef ENABLE_CRS_COMPRESSION
  #define ENABLE_CRS_COMPRESSION 0
#endif



#endif // GLRCONFIG_H
