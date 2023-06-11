# Yarexe

Yarexe is a single program which converts Net Yaroze executables to a single psx.exe file use the Net Yaroze siocons loading script file, often called auto or batch.

* yarexe.exe : a win32 executable (compiled with TinyCC).
* [yarexe.c](yarexe.c) & [yarexe.inc](yarexe.inc) : source files.


## Compiling for Linux etc:

    gcc -static-libgcc -static-libstdc++ -static -O3 yarexe.c -o yarexe

**Note:** Yarexe is programmed for windows, it will throw a lot of warnings, ignore them.

**Note:** Yarexe does NOT handle case differences and it will not find directories or files for you.


## Syntax

    yarexe siocons_batch_file [-v]

* `siocons_batch_file`
  * This your siocons auto batch script. Only `dload`, `load` and `go` are used, see: https://archive.org/details/net-yaroze-documents/020-1997-userguide/page/206/mode/1up
  * The last line of the script must be a single `go` statement.

* `-v`
  * Optional verbose mode, helpful for finding script or executable problems.

* Output:
  * psx.exe, which is ready to run! - If it didn't create it, something went wrong in combine or eco2exe.
  * Temporary files are combine.tmp and combEco.exe which are a siocon's script combined file and ecoff to exe applied.


## Tips:

Programs requiring the Net Yaroze CDROM (typically for music), may or may not work depending on the coding.
The work around is to put the psx.exe on a NY cdrom image, see: How to convert Net Yaroze games (siocons script with exe and assets) into PS1 ISO that runs in emulators: - https://www.youtube.com/watch?v=H0Ki7JnamIQ

Standard PS-X ELF executables (i.e. Net Yaroze ECOFF files patched with eco2elf) can be used but they have to use the CodeWarrior .pxe extension.

To run on the [no$psx emulator](http://problemkaputt.de/psx.htm) (<- tip the man!) it *MUST* be a .exe file!

Create a runpsx.bat like:

    C:\no$PSX\NO$PSX.EXE psx.exe

Save it in your bin path.

To run no$PSX in the background in winXP+ use:

    START /B C:\no$PSX\NO$PSX.EXE psx.exe


## Change log

* [v5](https://github.com/gwald/Yarexe/releases/tag/v5) - Jun 2023
  - Bug fixes

* [v4](https://github.com/gwald/Yarexe/releases/tag/v4) - Jan 2023
  - Improved CodeWarrior support
  - contains all previous versions (exe and code)

* [yarexe3.c](https://github.com/gwald/Yarexe/tree/3331cdf20a3c9bdf56c7c029210b2c7c9d3bcd44) - Aug 6th 2022
  - CodeWarrior PXE support with included Libps.exe

* [yarexe235.c](https://github.com/gwald/Yarexe/tree/44cb134fc67836ff616af2fd2c74240b947475c6) - March 10th 2021
  - Better error handling

* [yarexe234.c](https://github.com/gwald/Yarexe/tree/281b6984bc9135db4e01f45f9064d5d55eb007d2) - Jan 26th 2021
  - added `-v` for verbose switching

*nix CC - compiles with:

    gcc yarexeXXX.c -o yarexe

Attached win32 version compiled with:

    tcc yarexe.c -o yarexe.exe


## Original sources and credits

Thanks to @SCPH-1002 for the combine code!! and of course the guys that wrote the programs required!

Originally posted: http://www.psxdev.net/forum/viewtopic.php?f=67&t=917


Yarexe is a poor hack job of the following:

```
// combine.exe
// Yaroze PS-X EXE builder v2.10
// Copyright (C) Barubary 1998.  All rights not expressly granted reserved.
// Yaroze and PS-X are trademarks of Sony Computer Entertainment, Inc.

// THIS SOURCE CODE MAY ONLY BE MODIFIED FOR PERSONAL USE.  YOU MAY NOT
// DISTRIBUTE MODIFIED VERSIONS OF THIS SOURCE CODE OR THE PROGRAM COMPILED
// FROM MODIFIED SOURCE CODE, BUT YOU MAY MODIFY IT FOR PERSONAL USE.

// THIS PROGRAM AND ITS DOCUMENTATION MAY BE DISTRIBUTED TO ANYONE, PROVIDED
// IT IS NOT MODIFIED NOR ANY CHARGE IS ASSOCIATED WITH DISTRIBUTION.
// NO WARRANTY.
```

```
/*
 * eco2exe.c v0.01 Andrew Kieschnick <andrewk@mail.utexas.edu>
 *
 * converts MIPSEL-LE ecoff to PS-X EXE
 *
 * THIS SOURCE WAS MODIFIED (SLIGHTLY) TO WORK UNDER DOS
 * IF YOU USE UNIX, GET THE THE UNMODIFIED SOURCE
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * exefixup.c v0.02 Andrew Kieschnick <andrewk@mail.utexas.edu>
 *
 * displays PS-X EXE header information
 * offers to fix incorrect t_size
 * offers to pad to 2048-byte boundary for cd-rom use
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
```

`// CodeWarrior PS-X EXE File Patcher By ~imilco (Peter Armstrong)`

```
// ecoff a.out format from siocon src
/*
 *	Copyright (C) 1997 by Sony Computer Entertainment
 *			All rights Reserved
 */
```

Copyright is of the respective owners, wherever they are! We thank you!
