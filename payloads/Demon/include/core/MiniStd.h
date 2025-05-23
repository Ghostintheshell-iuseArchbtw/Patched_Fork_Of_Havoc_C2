#ifndef DEMON_DSTDIO_H
#define DEMON_DSTDIO_H

#include <Demon.h>

#define MemCopy         __builtin_memcpy
#define MemSet(d, v, l) __builtin_memset(d, v, l)
#define MemZero(p, l)   __builtin_memset(p, 0, l)
#define NO_INLINE       __attribute__ ((noinline))

INT     StringCompareA( LPCSTR String1, LPCSTR String2 );
INT     StringCompareW( LPWSTR String1, LPWSTR String2 );
INT     StringNCompareW( LPWSTR String1, LPWSTR String2, INT Length );
INT     StringNCompareIW( LPWSTR String1, LPWSTR String2, INT Length );
PCHAR   StringCopyA( PCHAR String1, PCHAR String2 );
PWCHAR  StringCopyW(PWCHAR String1, PWCHAR String2);
SIZE_T  StringLengthA( LPCSTR String );
SIZE_T  StringLengthW( LPCWSTR String );
PCHAR   StringConcatA(PCHAR String, PCHAR String2);
PWCHAR  StringConcatW(PWCHAR String, PWCHAR String2);
PCHAR   StringTokenA(PCHAR String, CONST PCHAR Delim);
LPWSTR  WcsStr( PWCHAR String, PWCHAR String2 );
LPWSTR  WcsIStr( PWCHAR String, PWCHAR String2 );
INT     MemCompare( PVOID s1, PVOID s2, INT len );
UINT64  GetSystemFileTime( );
BYTE    HideChar( BYTE C );

SIZE_T  WCharStringToCharString( PCHAR Destination, PWCHAR Source, SIZE_T MaximumAllowed );
SIZE_T  CharStringToWCharString( PWCHAR Destination, PCHAR Source, SIZE_T MaximumAllowed );

BOOL StringEndWithW(LPCWSTR str, LPCWSTR suffix);
INT StringCompareIW(LPCWSTR String1, LPCWSTR String2);
int NameCompare(PBUFFER a, PBUFFER b);

#endif
