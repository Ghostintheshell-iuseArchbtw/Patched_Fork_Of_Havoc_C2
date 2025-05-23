#include <Demon.h>

#include <core/MiniStd.h>
#include <core/Package.h>
#include <inject/InjectUtil.h>
#include <common/Defines.h>

#ifndef _WIN32
typedef ULONG NTSTATUS;
#endif

DWORD Rva2Offset( DWORD dwRva, UINT_PTR uiBaseAddress )
{
    PIMAGE_SECTION_HEADER   ImageSectionHeader;
    PIMAGE_NT_HEADERS       ImageNtHeaders;

    ImageNtHeaders     = RVA( PIMAGE_NT_HEADERS, uiBaseAddress, ( ( PIMAGE_DOS_HEADER ) uiBaseAddress )->e_lfanew );
    ImageSectionHeader = RVA( PIMAGE_SECTION_HEADER, &ImageNtHeaders->OptionalHeader, ImageNtHeaders->FileHeader.SizeOfOptionalHeader );

    if ( dwRva < ImageSectionHeader[ 0 ].PointerToRawData )
        return dwRva;

    for ( WORD wIndex = 0; wIndex < ImageNtHeaders->FileHeader.NumberOfSections; wIndex++ )
    {
        DWORD VirtualAddress = ImageSectionHeader[ wIndex ].VirtualAddress;

        if ( dwRva >= VirtualAddress && dwRva < ( VirtualAddress + ImageSectionHeader[ wIndex ].SizeOfRawData ) )
        {
            return ( dwRva - VirtualAddress + ImageSectionHeader[ wIndex ].PointerToRawData );
        }
    }

    return 0;
}

DWORD GetReflectiveLoaderOffset( PVOID ReflectiveLdrAddr )
{
    PIMAGE_NT_HEADERS       NtHeaders           = NULL;
    PIMAGE_EXPORT_DIRECTORY ExportDir           = NULL;
    UINT_PTR                AddrOfNames         = 0;
    UINT_PTR                AddrOfFunctions     = 0;
    UINT_PTR                AddrOfNameOrdinals  = 0;
    DWORD                   FunctionCounter     = 0;
    PCHAR                   FunctionName        = NULL;

    NtHeaders           = RVA( PIMAGE_NT_HEADERS, ReflectiveLdrAddr, ( ( PIMAGE_DOS_HEADER ) ReflectiveLdrAddr )->e_lfanew );
    ExportDir           = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)ReflectiveLdrAddr + Rva2Offset( NtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress, (UINT_PTR)ReflectiveLdrAddr ));
    AddrOfNames         = (UINT_PTR)((PBYTE)ReflectiveLdrAddr + Rva2Offset( ExportDir->AddressOfNames, (UINT_PTR)ReflectiveLdrAddr ));
    AddrOfNameOrdinals  = (UINT_PTR)((PBYTE)ReflectiveLdrAddr + Rva2Offset( ExportDir->AddressOfNameOrdinals, (UINT_PTR)ReflectiveLdrAddr ));
    FunctionCounter     = ExportDir->NumberOfNames;

    while ( FunctionCounter-- )
    {
        FunctionName = ( PCHAR )( (PBYTE)ReflectiveLdrAddr + Rva2Offset( DEREF_32( AddrOfNames ), (UINT_PTR)ReflectiveLdrAddr ) );
        //                                  ReflectiveLoader                             KaynLoader
        if ( HashStringA( FunctionName ) == 0xa6caa1c5 || HashStringA( FunctionName ) == 0xffe885ef )
        {
            PRINTF( "FunctionName => %s\n", FunctionName );
            AddrOfFunctions =   (UINT_PTR)((PBYTE)ReflectiveLdrAddr + Rva2Offset( ExportDir->AddressOfFunctions, (UINT_PTR)ReflectiveLdrAddr ));
            AddrOfFunctions +=  ( DEREF_16( AddrOfNameOrdinals ) * sizeof( DWORD ) );

            return Rva2Offset( DEREF_32( AddrOfFunctions ), (UINT_PTR)ReflectiveLdrAddr );
        }

        AddrOfNames        += sizeof( DWORD );
        AddrOfNameOrdinals += sizeof( WORD );
    }

    return 0;
}

DWORD GetPeArch( PVOID PeBytes )
{
    PIMAGE_NT_HEADERS NtHeader = NULL;
    DWORD             DllArch  = PROCESS_ARCH_UNKNOWN;

    if( ! PeBytes ) {
        return DllArch;
    }

    NtHeader = ( PIMAGE_NT_HEADERS ) ( ( ( UINT_PTR ) PeBytes ) + ( ( PIMAGE_DOS_HEADER ) PeBytes )->e_lfanew );

    if ( NtHeader->OptionalHeader.Magic == 0x010B ) {
        DllArch = PROCESS_ARCH_X86;
    } else if ( NtHeader->OptionalHeader.Magic == 0x020B ) {
        DllArch = PROCESS_ARCH_X64;
    }

    return DllArch;
}
