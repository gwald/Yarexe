yarexe234.c  Jan 26th 2021 
- added -v for verbose switching

*nix CC - compiles with: 
gcc yarexeXXX.c -o yarexe

Attached win32 version compiled with:
tcc yarexe.c -o yarexe.exe


# Yarexe
converts Net Yaroze ecoff and siocon's script to a psx.exe


YarEXE Hacked together by me, gwald
Yarexe is a single program which converts Net Yaroze executable to a single psx.exe file via a siocons script (auto).
yarexe.exe is win32 exec
yardos.exe is dos/win98 exec (compiled with DJGPP/GO32) see YARDOS folder
yarexe.c is my hacky coding putting it together.
org-src is the orginal code of all programs required

-- DOSEN'T WORK ON CODEWORRIOR COMPILER OR BUILDS WHICH ACCESS THE NY CDROM --

Syntax: yarexe <scriptfile> [optional: libps.exe file]
You must have LIBPS.EXE in the local folder or the root folder of current drive ie C:\ D:\ etc
Output files is psx.exe, which is ready to run! - if it didn't create it, something went wrong in combine or eco2exe
Temporary files are combine.tmp and combEco.exe which is a siocon's script combined file and ecoff to exe applied.

Copyright is of the respective owners, wherever they are! We thank you!


TIPs:
auto (Net Yaroze script) must have the last line as:
go

To run on the no$psx emulator [http://problemkaputt.de/psx.htm <- tip the man!] it MUST be a .exe file!
Create a runpsx.bat like:
C:\no$PSX\NO$PSX.EXE psx.exe

Save it in your bin path then it
To run no$PSX in the background in winXP+ use:
START /B C:\no$PSX\NO$PSX.EXE psx.exe


Thanks to @SCPH-1002 for the combine code!! and of cause the guys that wrote the 3 programs required! 
Originally posted: http://www.psxdev.net/forum/viewtopic.php?f=67&t=917
