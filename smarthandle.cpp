#include "smarthandle.h"

smartHandle::smartHandle(HANDLE handle) {
    _handle = handle;
}

smartHandle::~smartHandle() {
    if (_handle) {
        CloseHandle(_handle);
    }
}
