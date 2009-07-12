/* see License.txt for copyright and terms of use */

#ifndef LINKAGE_H
#define LINKAGE_H

#ifdef __cplusplus
#  define EXTERN_C extern "C"
#  define EXTERN_C_BEGIN extern "C" {
#  define EXTERN_C_END }
#else
#  define EXTERN_C
#  define EXTERN_C_BEGIN
#  define EXTERN_C_END
#endif

#endif // LINKAGE_H
