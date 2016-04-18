#include <stdio.h>
#include <windows.h>

HANDLE g_event;

LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exception_pointers) {
  (void)exception_pointers;
  fprintf(stderr, "in UnhandledExceptionHandler\n");
  SetEvent(g_event);
  fprintf(stderr, "called SetEvent\n");
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

  // Test event to confirm calling SetEvent is OK.
  g_event = CreateEvent(nullptr, true, false, nullptr);

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
