add_library(
    smbase
    autofile.cc
    autofile.h
    bflatten.cc
    bflatten.h
    bit2d.cc
    bit2d.h
    bitarray.cc
    bitarray.h
    boxprint.cc
    boxprint.h
    breaker.cpp
    breaker.h
    crc.cpp
    crc.h
    cycles.c
    cycles.h
    d2vector.c
    d2vector.h
    datablok.cpp
    datablok.h
    exc.cpp
    exc.h
    flatten.cc
    flatten.h
    gprintf.c
    gprintf.h
    growbuf.cc
    growbuf.h
    hashline.cc
    hashline.h
    hashtbl.cc
    hashtbl.h
    missing.cpp
    missing.h
    nonport.cpp
    nonport.h
    ofstreamts.cc
    ofstreamts.h
    point.cc
    point.h
    pprint.cc
    pprint.h
    srcloc.cc
    srcloc.h
    str.cpp
    strdict.cc
    strdict.h
    str.h
    strhash.cc
    strhash.h
    stringset.cc
    stringset.h
    strtable.cc
    strtable.h
    strtokp.cpp
    strtokp.h
    strutil.cc
    strutil.h
    svdict.cc
    svdict.h
    syserr.cpp
    syserr.h
    trace.cc
    trace.h
    trace.html
    trdelete.cc
    trdelete.h
    unixutil.c
    unixutil.h
    vdtllist.cc
    vdtllist.h
    voidlist.cc
    voidlist.h
    vptrmap.cc
    vptrmap.h
    warn.cpp
    warn.h)

target_sources(smbase PRIVATE $<IF:$<CONFIG:Debug>,malloc.c,malloc_stub.c>)
target_compile_definitions(smbase PRIVATE $<IF:$<CONFIG:Debug>,DEBUG_HEAP DEBUG,>)

# srcloc.test2.cc
# srcloc.test.cc
if(NOT MINGW)
    target_sources(
        smbase
        PRIVATE
            mypopen.c
            mypopen.h
            mysig.cc
            mysig.h
            smregexp.cc
            smregexp.h)
endif()

target_include_directories(smbase PUBLIC .)
if(NOT MSVC)
    target_compile_options(smbase PUBLIC -Wno-deprecated)
endif()
# These are M4 inputs:
# xassert.h
# xobjlist.h
# xstrobjdict.h

add_executable(nonport nonport.cpp nonport.h gprintf.c)
target_compile_definitions(nonport PRIVATE TEST_NONPORT)
add_test(NAME nonport COMMAND nonport)

add_executable(voidlist voidlist.cc voidlist.h)
target_compile_definitions(voidlist PRIVATE TEST_VOIDLIST)
target_link_libraries(voidlist PRIVATE smbase)
add_test(NAME voidlist COMMAND voidlist)

add_executable(vdtllist vdtllist.cc vdtllist.h)
target_compile_definitions(vdtllist PRIVATE TEST_VDTLLIST)
target_link_libraries(vdtllist PRIVATE smbase)
add_test(NAME vdtllist COMMAND vdtllist)

add_executable(taillist_test taillist_test.cc taillist.h)
target_link_libraries(taillist_test PRIVATE smbase)
add_test(NAME taillist_test COMMAND taillist_test)

add_executable(tobjlist tobjlist.cc objlist.h voidlist.cc voidlist.h)
target_link_libraries(tobjlist PRIVATE smbase)
add_test(NAME tobjlist COMMAND tobjlist)

add_executable(tsobjlist tsobjlist.cc sobjlist.h voidlist.cc voidlist.h)
target_link_libraries(tsobjlist PRIVATE smbase)
add_test(NAME tsobjlist COMMAND tsobjlist)

add_executable(bit2d bit2d.cc bit2d.h)
target_compile_definitions(bit2d PRIVATE TEST_BIT2D)
target_link_libraries(bit2d PRIVATE smbase)
add_test(NAME bit2d COMMAND bit2d)

add_executable(growbuf growbuf.cc growbuf.h)
target_compile_definitions(growbuf PRIVATE TEST_GROWBUF)
target_link_libraries(growbuf PRIVATE smbase)
add_test(NAME growbuf COMMAND growbuf)

add_executable(strdict strdict.cc strdict.h)
target_compile_definitions(strdict PRIVATE TEST_STRDICT)
target_link_libraries(strdict PRIVATE smbase)
add_test(NAME strdict COMMAND strdict)

add_executable(svdict svdict.cc svdict.h)
target_compile_definitions(svdict PRIVATE TEST_SVDICT)
target_link_libraries(svdict PRIVATE smbase)
add_test(NAME svdict COMMAND svdict)

add_executable(str str.cpp str.h)
target_compile_definitions(str PRIVATE TEST_STR)
target_link_libraries(str PRIVATE smbase)
add_test(NAME str COMMAND str)

add_executable(strutil strutil.cc strutil.h)
target_compile_definitions(strutil PRIVATE TEST_STRUTIL)
target_link_libraries(strutil PRIVATE smbase)
add_test(NAME strutil COMMAND strutil)

add_executable(strhash strhash.cc strhash.h)
target_compile_definitions(strhash PRIVATE TEST_STRHASH)
target_link_libraries(strhash PRIVATE smbase)
add_test(NAME strhash COMMAND strhash)

add_executable(trdelete trdelete.cc trdelete.h)
target_compile_definitions(trdelete PRIVATE TEST_TRDELETE)
target_link_libraries(trdelete PRIVATE smbase)
add_test(NAME trdelete COMMAND trdelete)

add_executable(bflatten bflatten.cc bflatten.h)
target_compile_definitions(bflatten PRIVATE TEST_BFLATTEN)
target_link_libraries(bflatten PRIVATE smbase)
add_test(NAME bflatten COMMAND bflatten)
if(NOT MINGW)

    add_executable(mysig mysig.cc mysig.h)
    target_compile_definitions(mysig PRIVATE TEST_MYSIG)
    target_link_libraries(mysig PRIVATE smbase)

    add_test(NAME mysig COMMAND mysig)
endif()

add_executable(testmalloc testmalloc.cc)
target_compile_definitions(testmalloc PRIVATE TEST_TESTMALLOC)
target_link_libraries(testmalloc PRIVATE smbase)
add_test(NAME testmalloc COMMAND testmalloc)

if(NOT MINGW)

    add_executable(mypopen mypopen.c mypopen.h)
    target_compile_definitions(mypopen PRIVATE TEST_MYPOPEN)

    add_test(NAME mypopen COMMAND mypopen)
endif()
# # this test is only useful when malloc is compiled with DEBUG_HEAP

# add_executable(tmalloc tmalloc.c)
# target_link_libraries(testmalloc PRIVATE tmalloc)

# add_test(NAME tmalloc COMMAND tmalloc)
# TODO doesn't link

# add_executable(tobjpool tobjpool.cc objpool.h)

# add_test(NAME tobjpool COMMAND tobjpool)

add_executable(cycles cycles.h cycles.c)
target_compile_definitions(cycles PRIVATE TEST_CYCLES)
add_test(NAME cycles COMMAND cycles)

# add_executable(crc crc.cpp)
# add_test(NAME crc COMMAND crc)

add_executable(srcloc srcloc.cc)
target_compile_definitions(srcloc PRIVATE TEST_SRCLOC)
target_link_libraries(srcloc PRIVATE smbase)
add_test(
    NAME srcloc
    COMMAND srcloc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(hashline hashline.cc)
target_compile_definitions(hashline PRIVATE TEST_HASHLINE)
target_link_libraries(hashline PRIVATE smbase)
add_test(NAME hashline COMMAND hashline)

add_executable(gprintf gprintf.c gprintf.h)
target_compile_definitions(gprintf PRIVATE TEST_GPRINTF)
add_test(NAME gprintf COMMAND gprintf)

if(NOT MINGW)
    add_executable(smregexp smregexp.cc)
    target_compile_definitions(smregexp PRIVATE TEST_SMREGEXP)
    target_link_libraries(smregexp PRIVATE smbase)
    add_test(NAME smregexp COMMAND smregexp)
endif()

add_executable(vptrmap vptrmap.cc)
target_compile_definitions(vptrmap PRIVATE TEST_VPTRMAP)
target_link_libraries(vptrmap PRIVATE smbase)
add_test(NAME vptrmap COMMAND vptrmap)

add_executable(pprint pprint.cc)
target_compile_definitions(pprint PRIVATE TEST_PPRINT)
target_link_libraries(pprint PRIVATE smbase)
add_test(NAME pprint COMMAND pprint)

add_executable(boxprint boxprint.cc)
target_compile_definitions(boxprint PRIVATE TEST_BOXPRINT)
target_link_libraries(boxprint PRIVATE smbase)
add_test(NAME boxprint COMMAND boxprint)

add_executable(tarrayqueue tarrayqueue.cc)
target_link_libraries(tarrayqueue PRIVATE smbase)
add_test(NAME tarrayqueue COMMAND tarrayqueue)

add_executable(testarray testarray.cc)
target_link_libraries(testarray PRIVATE smbase)
add_test(NAME testarray COMMAND testarray)

add_executable(autofile autofile.cc)
target_compile_definitions(autofile PRIVATE TEST_AUTOFILE)
target_link_libraries(autofile PRIVATE smbase)
add_test(
    NAME autofile
    COMMAND autofile autofile.cc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bitarray bitarray.cc)
target_compile_definitions(bitarray PRIVATE TEST_BITARRAY)
target_link_libraries(bitarray PRIVATE smbase)
add_test(NAME bitarray COMMAND bitarray)

add_executable(d2vector d2vector.c)
target_compile_definitions(d2vector PRIVATE TEST_D2VECTOR)
target_link_libraries(d2vector PRIVATE smbase)
add_test(NAME d2vector COMMAND d2vector)
