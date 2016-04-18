// DebugBreakProcess() doesn't get to the UnhandledExceptionFilter of the
// target; only seems to work when the target is actually being debugged.
//
// CreateRemoteThread() doesn't work when the target is holding the loader lock.
//
// NtCreateThreadEx() with THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH will manage to
// cause a crash when the target has the loader lock. More testing required to
// determine what is actually callable in that situation. Calling MessageBox()
// for example, will fail and crash the UEF.
//
// Another possibility is remote GetProcAddress() of a suicide function that
// directly calls the UEF. Not implemented yet, but should probably maybe work.

#include <stdio.h>
#include <windows.h>
#include <winternl.h>

typedef NTSTATUS(WINAPI *NtCreateThreadExFunc)(
    PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
    LPTHREAD_START_ROUTINE StartRoutine, LPVOID StartContext, ULONG Flags,
    ULONG_PTR StackZeroBits, SIZE_T SizeOfStackCommit,
    SIZE_T SizeOfStackReserve, PVOID /*PPROCESS_ATTRIBUTE_LIST*/ lpBytesBuffer);

int main() {
  STARTUPINFO si = {};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;
  if (!CreateProcess("target.exe", nullptr, nullptr, nullptr, false, 0, nullptr,
                     nullptr, &si, &pi)) {
    abort();
  }

  Sleep(1000);
  fprintf(stderr, "going to nuke child\n");

#if 0
  if (!DebugBreakProcess(pi.hProcess))
    abort();
#elif 0
  HANDLE remote_thread =
      CreateRemoteThread(pi.hProcess, nullptr, 0, 0, 0, 0, nullptr);
  if (!remote_thread)
    abort();
  if (!CloseHandle(remote_thread))
    abort();
#elif 1
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
  NtCreateThreadExFunc nt_create_thread_ex =
      reinterpret_cast<NtCreateThreadExFunc>(
          GetProcAddress(GetModuleHandle("ntdll.dll"), "NtCreateThreadEx"));
  HANDLE thread_handle;
  NTSTATUS status = nt_create_thread_ex(
      &thread_handle, 0x1FFFFF, nullptr, pi.hProcess, nullptr, nullptr,
      THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH, 0, 0, 0, nullptr);
  fprintf(stderr, "ntstatus=%d\n", status);
#elif 0
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
  NtCreateThreadExFunc nt_create_thread_ex =
      reinterpret_cast<NtCreateThreadExFunc>(
          GetProcAddress(GetModuleHandle("ntdll.dll"), "NtCreateThreadEx"));
  HMODULE remote_module = GetRemoteModuleHandle(pi.hProcess, "target.exe");
  fprintf(stderr, "remote_module: %p\n", remote_module);
  FARPROC proc = GetRemoteProcAddress(pi.hProcess, remote_module,
                                      "_SuicideThunk@4", 0, FALSE);
  fprintf(stderr, "proc: %p\n", proc);
  HANDLE thread_handle;
  NTSTATUS status = nt_create_thread_ex(
      &thread_handle, 0x1FFFFF, nullptr, pi.hProcess,
      reinterpret_cast<LPTHREAD_START_ROUTINE>(proc), nullptr,
      THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH, 0, 0, 0, nullptr);
  fprintf(stderr, "ntstatus=%d\n", status);
#endif

  if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
    abort();
  if (!CloseHandle(pi.hThread))
    abort();
  if (!CloseHandle(pi.hProcess))
    abort();
}
