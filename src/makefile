pd2tts.dll: pd2tts.obj
	link /NODEFAULTLIB /MERGE:.rdata=.text /SAFESEH:NO /ENTRY:DllMain /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /DLL pd2tts.obj ntdll.lib kernel32.lib ws2_32.lib

pd2tts.obj: pd2tts.c
	cl pd2tts.c /nologo /c /O1 /GS- /Oi-
