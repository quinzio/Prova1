///// CLIENT PROGRAM /////
#include <iostream>
#include <windows.h>
using namespace std;
int main(int argc, const char** argv)
{
    HANDLE h = OpenEvent(EVENT_MODIFY_STATE, FALSE, TEXT("MyEvent1"));
    if (!h) {
        std::cout << "Can't create OpenEvent\n";
        return - 1;
    }

    wcout << "Connecting to pipe..." << endl;
    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    HANDLE pipe = CreateFile(
        L"\\\\.\\pipe\\my_pipe",
        GENERIC_READ, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        NULL,
        NULL
    );
    if (pipe == INVALID_HANDLE_VALUE) {
        wcout << "Failed to connect to pipe." << endl;
        // look up error code here using GetLastError()
        system("pause");
        return 1;
    }
    wcout << "Reading data from pipe..." << endl;
    // The read operation will block until there is data to read
//    wchar_t buffer[128];
    char buffer[128];
    DWORD numBytesRead = 0;
    BOOL result;
    do {
        WaitForSingleObject(h, INFINITE);
        result = ReadFile(
            pipe,
            buffer, // the data from the pipe will be put here
            //127 * sizeof(char), // number of bytes allocated
            127 * sizeof(char), // number of bytes allocated
            &numBytesRead, // this will store number of bytes actually read
            NULL // not using overlapped IO
        );

        if (result) {
//            buffer[numBytesRead / sizeof(wchar_t)] = '\0'; // null terminate the string
            buffer[numBytesRead / sizeof(char)] = '\0'; // null terminate the string
            cout << "Number of bytes read: " << numBytesRead << endl;
            cout << "Message: " << buffer << endl;
        }
        else {
            wcout << "Failed to read data from the pipe." << endl;
        }
    }
    while (1);

    // Close our pipe handle
    CloseHandle(pipe);
    wcout << "Done." << endl;
    system("pause");
    return 0;
}