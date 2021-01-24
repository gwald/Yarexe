OLD VERSION

YarEXE Hacked together by gwald 
Yarexe is a single program which converts Net Yaroze executables to a single psx.exe file via a siocons script (auto).
yarexe.exe is win32 exec
yardos.exe is dos/win98 exec (compiled with DJGPP/GO32) 
yarexe.c is my hacky coding putting it together.
org-src is the orginal code


Syntax: yarEXEC <scriptfile>  [optional: libps.exe file]
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

