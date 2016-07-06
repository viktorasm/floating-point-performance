CC=cl.exe
LINK=link.exe

clean:
	rmdir .\out /s /q

outdir:
	if not exist out mkdir out

..\out\normalize.obj: outdir ..\src\normalize.cpp
	set PATH
    $(CC)  /c /GL /O2 /Oi /Ot /D NDEBUG /EHsc /FD /MT /Gy ..\src\normalize.cpp /Foout\normalize.obj

main: clean ..\out\normalize.obj
	$(LINK) /LTCG /MACHINE:X64 /OUT:out\app.exe out\*.obj
