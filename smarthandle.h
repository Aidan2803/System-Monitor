#ifndef SMARTHANDLE_H
#define SMARTHANDLE_H

#include <windows.h>
#include <TlHelp32.h>

class smartHandle{
public:
    smartHandle(HANDLE handle);
    ~smartHandle();

    operator bool() { return _handle != NULL; };
    operator HANDLE() { return _handle; }

    HANDLE handle() { return _handle; }

private:
    HANDLE _handle = NULL;

};

#endif // SMARTHANDLE_H
