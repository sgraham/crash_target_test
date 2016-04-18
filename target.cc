#include <stdio.h>
#include <windows.h>

LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exception_pointers) {
  (void)exception_pointers;
  fprintf(stderr, "in UnhandledExceptionHandler\n");
  TerminateProcess(GetCurrentProcess(), 1);
  return EXCEPTION_CONTINUE_SEARCH;
}

/*
extern "C" __declspec(dllexport) LONG WINAPI SuicideThunk(void* data) {
  UnhandledExceptionHandler(nullptr);
  return 0;
}
*/

int main() {
  SetUnhandledExceptionFilter(UnhandledExceptionHandler);
  fprintf(stderr, "SetUEF\n");
  HMODULE dll = LoadLibrary("dll.dll");
  if (!dll)
    abort();
  fprintf(stderr, "Loaded dll\n");
  if (!FreeLibrary(dll))
    abort();
  fprintf(stderr, "Unloaded dll\n");
  Sleep(INFINITE);
}
