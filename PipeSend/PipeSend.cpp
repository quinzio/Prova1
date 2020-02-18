/*
https://peter.bloomfield.online/introduction-to-win32-named-pipes-cpp/
https://github.com/peter-bloomfield/win32-named-pipes-example
*/

///// SERVER PROGRAM /////
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

VOID startup(LPCTSTR lpApplicationName);

int main(int argc, const char** argv)
{
    HANDLE hEvent1 = CreateEvent(NULL, FALSE, false, L"MyEvent1");
    if (!hEvent1) {
        std::cout << "Can't create event MyEvent1\n";
        return -1;
    }


    startup(L"PipeReceive.exe");
    wcout << L"Creating an instance of a named pipe...\n" ;
    // Create a pipe to send data
    HANDLE pipe = CreateNamedPipe(
        L"\\\\.\\pipe\\my_pipe", // name of the pipe
        PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        1, // only allow 1 instance of this pipe
        0, // no outbound buffer
        0, // no inbound buffer
        0, // use default wait time
        NULL // use default security attributes
    );
    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
        wcout << "Failed to create outbound pipe instance.";
        // look up error code here using GetLastError()
        system("pause");
        return 1;
    }
    wcout << "Waiting for a client to connect to the pipe..." << endl;
    // This call blocks until a client process connects to the pipe
    BOOL result = ConnectNamedPipe(pipe, NULL);
    if (!result) {
        wcout << "Failed to make connection on named pipe." << endl;
        // look up error code here using GetLastError()
        CloseHandle(pipe); // close the pipe
        system("pause");
        return 1;
    }
    wcout << "Sending data to pipe..." << endl;
    // This call blocks until a client process reads all the data
    const wchar_t* data = L"*** Hello Pipe World ***";
    char str[256];
    DWORD numBytesWritten = 0;
    int ix = 0;
    while (1) {
        strcpy_s(str, (std::to_string(ix) + " eureka").c_str());
        cout << str << endl;
        result = WriteFile(
            pipe, // handle to our outbound pipe
            str, // data to send
            strlen(str), // length of data to send (bytes)
            &numBytesWritten, // will store actual amount of data sent
            NULL // not using overlapped IO
        );
        if (result) {
            wcout << "Number of bytes sent: " << numBytesWritten << endl;
            wcout << ix++ << endl;
        }
        else {
            wcout << "Failed to send data." << endl;
            // look up error code here using GetLastError()
        }
        SetEvent(hEvent1);
        Sleep(1000);
    }
    // Close the pipe (automatically disconnects client too)
    CloseHandle(pipe);
    wcout << "Done." << endl;
    system("pause");
    return 0;
}


VOID startup(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(lpApplicationName,   // the path
        NULL,        // Command line
        NULL,               // Process handle not inheritable
        NULL,               // Thread handle not inheritable
        FALSE,              // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, // No creation flags
        NULL,               // Use parent's environment block
        NULL,               // Use parent's starting directory 
        &si,                // Pointer to STARTUPINFO structure
        &pi                 // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}