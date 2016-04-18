cl /Zi /W4 /nologo host.cc && cl /Zi /W4 /nologo target.cc /link user32.lib && cl /W4 /Zi /nologo dll.cc /link /dll /out:dll.dll
