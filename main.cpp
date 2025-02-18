#include <stdio.h>
#include <windows.h>
#include "Common.h"

#include "Span.h"
#include "CoroutineTest.h"

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
Span GetLastErrorAsString(Span* span)
{
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0) return EmptySpan();

    // Get the message buffer from windows.
    LPSTR messageBuffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    // Copy from windows buffer.
    i32 copiedSize = CopyToSpan(span, messageBuffer, (i32) size);
    
    // Free the windows buffer.
    LocalFree(messageBuffer);
    return SliceSpan(span, 0, copiedSize);
}

void PrintLastWindowsError()
{
    NEW_STACK_SPAN(errSpan, 1024);
    errSpan = GetLastErrorAsString(&errSpan);
    printf("Error %d: ", GetLastError());
    PrintSpan(&errSpan);
}

struct Vector2 {
    float X;
    float Y;
};

int main()
{
#if defined(__clang__)
printf("Clang!\n");
#elif defined(__GNUC__) || defined(__GNUG__)
printf("GCC\n");
#elif defined(_MSC_VER)
printf("MSVC\n");
#endif

    // Default initialization - data is junk (unless a default constructor or default values are present).
    Vector2 defaultInit;

    // Value initialization/Compiler generated constructor is called.
    Vector2 valueInit{};

    TestCoroutine();
    return 0;

	/*char runtimePath[MAX_PATH];
	GetFullPathNameA("Libs\\coreclr.dll", MAX_PATH, runtimePath, NULL);
	HMODULE coreClr = LoadLibraryExA(runtimePath, NULL, 0);
	if (!coreClr)
	{
        PrintLastWindowsError();
        return 0;
	}

    Span test = SliceSpanAtNullTerminated(&runtimePath[0], MAX_PATH);
    PrintSpan(&test);
	printf("\n");
	return 0;*/
}
