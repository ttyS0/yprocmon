#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define BUFSIZE 1024

void PrintError(LPCTSTR errDesc);

int _tmain(int argc, TCHAR *argv[])
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hTempFile = INVALID_HANDLE_VALUE;

    BOOL fSuccess = FALSE;
    DWORD dwRetVal = 0;
    UINT uRetVal = 0;

    DWORD dwBytesRead = 0;
    DWORD dwBytesWritten = 0;

    TCHAR szTempFileName[MAX_PATH];
    TCHAR lpTempPathBuffer[MAX_PATH];
    char chBuffer[BUFSIZE];

    LPCTSTR errMsg;

    // if (argc != 2)
    // {
    //     _tprintf(TEXT("Usage: %s <file>\n"), argv[0]);
    //     return -1;
    // }

    //  Opens the existing file.
    hFile = CreateFile("C:\\Windows\\system.ini",               // file name
                       GENERIC_READ,          // open for reading
                       0,                     // do not share
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no template
    if (hFile == INVALID_HANDLE_VALUE)
    {
        PrintError(TEXT("First CreateFile failed"));
        return (1);
    }

    //  Gets the temp path env string (no guarantee it's a valid path).
    dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
                           lpTempPathBuffer); // buffer for path
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        PrintError(TEXT("GetTempPath failed"));
        if (!CloseHandle(hFile))
        {
            PrintError(TEXT("CloseHandle(hFile) failed"));
            return (7);
        }
        return (2);
    }

    //  Generates a temporary file name.
    uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
                              TEXT("DEMO"),     // temp file name prefix
                              0,                // create unique name
                              szTempFileName);  // buffer for name
    if (uRetVal == 0)
    {
        PrintError(TEXT("GetTempFileName failed"));
        if (!CloseHandle(hFile))
        {
            PrintError(TEXT("CloseHandle(hFile) failed"));
            return (7);
        }
        return (3);
    }

    //  Creates the new file to write to for the upper-case version.
    hTempFile = CreateFile((LPTSTR)szTempFileName, // file name
                           GENERIC_WRITE,          // open for write
                           0,                      // do not share
                           NULL,                   // default security
                           CREATE_ALWAYS,          // overwrite existing
                           FILE_ATTRIBUTE_NORMAL,  // normal file
                           NULL);                  // no template
    if (hTempFile == INVALID_HANDLE_VALUE)
    {
        PrintError(TEXT("Second CreateFile failed"));
        if (!CloseHandle(hFile))
        {
            PrintError(TEXT("CloseHandle(hFile) failed"));
            return (7);
        }
        return (4);
    }

    //  Reads BUFSIZE blocks to the buffer and converts all characters in
    //  the buffer to upper case, then writes the buffer to the temporary
    //  file.
    do
    {
        if (ReadFile(hFile, chBuffer, BUFSIZE, &dwBytesRead, NULL))
        {
            //  Replaces lower case letters with upper case
            //  in place (using the same buffer). The return
            //  value is the number of replacements performed,
            //  which we aren't interested in for this demo.
            CharUpperBuffA(chBuffer, dwBytesRead);

            fSuccess = WriteFile(hTempFile, chBuffer, dwBytesRead,
                                 &dwBytesWritten, NULL);
            if (!fSuccess)
            {
                PrintError(TEXT("WriteFile failed"));
                return (5);
            }
        }
        else
        {
            PrintError(TEXT("ReadFile failed"));
            return (6);
        }
        //  Continues until the whole file is processed.
    } while (dwBytesRead == BUFSIZE);

    //  The handles to the files are no longer needed, so
    //  they are closed prior to moving the new file.
    if (!CloseHandle(hFile))
    {
        PrintError(TEXT("CloseHandle(hFile) failed"));
        return (7);
    }

    if (!CloseHandle(hTempFile))
    {
        PrintError(TEXT("CloseHandle(hTempFile) failed"));
        return (8);
    }

    //  Moves the temporary file to the new text file, allowing for differnt
    //  drive letters or volume names.
    fSuccess = MoveFileEx(szTempFileName, TEXT("AllCaps.txt"),
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
    if (!fSuccess)
    {
        PrintError(TEXT("MoveFileEx failed"));
        return (9);
    }
    else
        _tprintf(TEXT("All-caps version of %s written to AllCaps.txt\n"),
                 argv[0]);
    return (0);
}

//  ErrorMessage support function.
//  Retrieves the system error message for the GetLastError() code.
//  Note: caller must use LocalFree() on the returned LPCTSTR buffer.
LPCTSTR ErrorMessage(DWORD error)
{
    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf, 0, NULL);

    return ((LPCTSTR)lpMsgBuf);
}

//  PrintError support function.
//  Simple wrapper function for error output.
void PrintError(LPCTSTR errDesc)
{
    LPCTSTR errMsg = ErrorMessage(GetLastError());
    _tprintf(TEXT("\n** ERROR ** %s: %s\n"), errDesc, errMsg);
    LocalFree((LPVOID)errMsg);
}