#include <stdio.h>
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  (void)hinstDLL;
  (void)lpvReserved;
  switch (fdwReason) {
  case DLL_PROCESS_DETACH:
  case DLL_THREAD_DETACH:
    fprintf(stderr, "sleeping on loader lock\n");
    Sleep(INFINITE);
  }
  return TRUE;
}
