// yarexe - Net Yaroze PS-X EXE package builder.
// https://github.com/gwald/Yarexe

// Note: Standard PS-X elf executables (ie Net Yaroze ecoff files processed with eco2elf) can be used but they have to use the CodeWarrior .pxe extension.

// Build: gcc -static-libgcc -static-libstdc++ -static -O3 yarexe.c -o yarexe
// Linux note: Yarexe is programmed for windows, it uses \ for folders, and it does NOT handle case differences, it will not find files.


// Yarexe is a poor hack job of the following:


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

// CodeWarrior PS-X EXE File Patcher By ~imilco (Peter Armstrong)


// ecoff a.out format from siocon src
/*
 *	Copyright (C) 1997 by Sony Computer Entertainment
 *			All rights Reserved
 */






#define YAREXE_VER "YAREXE - v5 June 2023 - bug fixes"
#define YAREXE "Net Yaroze PS-X packager"

/*
 * v5 - bug fixes
 * v4 - CodeWarrior and included Libps.exe
 */


#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h> // tolower/upper



#define USE_ECO2EXE_SOURCE
#define COMBINE_TMP_FILENAME "COMBINE.TMP"

#ifdef USE_ECO2EXE_SOURCE
int eco2exe_main(char *filename);
int exeFixUp_main( char *filename);
#endif



// ecoff a.out format from siocon src
typedef struct filehdr {
	unsigned short	f_magic;	//00 -02 /* magic number */
	unsigned short	f_nscns;	//02 -04 /* number of sections */
	unsigned int		f_timdat;	// 04 -08 /* time & date stamp */
	unsigned int		f_symptr;	// 08 -12 /* file pointer to symbolic header */
	unsigned int		f_nsyms;	// 12 -16 /* sizeof(symbolic hdr) */
	unsigned short	f_opthdr; // 16 -18		/* sizeof(optional hdr) */
	unsigned short	f_flags; // 18 -20		/* flags */
}FILEHDR;

typedef	struct aouthdr {
	unsigned short	magic;   // 20 -22		 00 -02 0x62 and 0x01
	unsigned short	vstamp;  // 22 -24
	unsigned int	tsize;  // 24 -28
	unsigned int	dsize;  //28 -32
	unsigned int	bsize;  //32 -36
	unsigned int	entry; //36 -40
	unsigned int	text_start;  //40 -44
	unsigned int	data_start;	//44 -48
	unsigned int	bss_start; //48 -52
	unsigned int	gprmask; //52 -56
	unsigned int	cprmask[4]; //56 -72
	unsigned int	gp_value; // 72 -76
} AOUTHDR;


typedef struct scnhdr { // 40bytes
	char		s_name[8];		/* 76  - 84 section name */
	unsigned int		s_paddr;		/* 84  - 88 physical address, aliased s_nlib */
	unsigned int		s_vaddr;		/* 88  - 92 virtual address */
	unsigned int		s_size;			/* 92  - 96 section size */
	unsigned int		s_scnptr;		/* 96  - 100 file ptr to raw data for section */
	unsigned int		s_relptr;		/* 100 - 104 file ptr to relocation */
	unsigned int		s_lnnoptr;		/* 104 - 108 file ptr to gp histogram */
	unsigned short	s_nreloc;	/* 108 - 110 number of relocation entries */
	unsigned short	s_nlnno;	/* 110 - 112 number of gp histogram entries */
	unsigned int		s_flags;		/* 112 - 116 flags */
}SCNHDR;



typedef	struct allEcoffHdr{
	FILEHDR fileHDR;
	AOUTHDR ecoHDR;
	SCNHDR  sections[10];
} ECOHDRS;


int patchPXE_CodeWarrior(char *filename);// returns open file pointer to PSX-EXE or fails with NULL



/* Generated by bin2c, do not edit manually */

/* Contents of file libmon22.exe */
extern const long int psx_exe_size;
extern const unsigned char psx_exe[458752];
/* __psx_h_included */



FILE *auto_fileHnd = NULL, *target_fileHnd = NULL, *combine_fileHnd = NULL;
long length1, length2, length3, length4, memorysize = 0;
char *scriptstring = NULL, *memspace = NULL, *memstore = NULL;
unsigned long exec_address = 0, stack_address = 0;
unsigned char *auto_fileBuff=NULL, *filebuffer=NULL,
	      *eco2exe_filename=NULL, systemcalldata[256];


int g_foundYarExe = 0;
int g_verbose = 0;
char g_pxe_filename[256];

long file_length(FILE *handle)
{
	long retval;
	fseek(handle, 0, SEEK_END);
	retval = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	return retval;
}

void readfile(FILE *handle, void *buffer, long length)
{
	long templength, templocation = 0;

	while (length > 0)
	{
		templength = (length > BUFSIZ) ? BUFSIZ : length;
		fread((char *) buffer + templocation, templength, 1, handle);
		length -= templength;
		templocation += templength;
	}
}

void writefile(FILE *handle, void *buffer, long length)
{
	long templength, templocation = 0;
	char test[]="lit4";


	if(handle == NULL) // not opened
		return; //do nothing

	while (length > 0)
	{
		templength = 1;//(length > BUFSIZ) ? BUFSIZ : length;

		//printf("%d\n", strcmp(test, (char *) buffer + templocation ) );

		if(strcmp(test,(char *) buffer + templocation ) == 0)
		{
#if 0
			char sdata[]="sdat";
			long i;

			printf("lit4 found %d\n",templocation);

			for(i=0;i<length;i++)
			{
				printf("%d %d %x\n",i, strcmp(sdata, (char *) (buffer +templocation + i ) ), *((char *)(buffer +templocation + i ) ) );

				// if(strcmp(sdata,(char *) buffer + templocation + i ) == 0)
				if( ( (char *)(buffer +templocation + i ) == 73) &&
						( (char *)(buffer +templocation + i +1 ) == 64) &&
						( (char *)(buffer +templocation + i +2  ) == 61) &&
						( (char *)(buffer +templocation + i  +3 ) == 74)
				)
				{
					templocation += i;
					break;
				}
			}
#endif

			templocation += 40;
		}

		fwrite((char *) buffer + templocation, templength, 1, handle);

		length -= templength;
		templocation += templength;
	}
}

int MemoryWriteMemory(char *handle, unsigned long address, long length)
{
	unsigned long endaddress;

	// Ignore high nibble
	address &= 0x0FFFFFFF;
	if ((address > 0x001FFFFF) || (address < 0x00010000))
	{
		printf("ERROR: Address %-8.8lX invalid!\n", address);
		return (1);
	}
	if (!length)
	{
		printf("ERROR: Zero length file detected!\n");
		return (1);
	}
	if ((address + length) > 0x001FFFFF)
	{
		if(g_verbose==1)
			printf("WARNING: File exceeds memory bounds\n");

		length = 0x00200000 - address;
	}
	endaddress = length + address;

	memcpy(memspace + address, handle, length);
	//readfile(handle, memspace + address, length);

	if (endaddress > memorysize)
		memorysize = endaddress;

	return (0);
}


int MemoryWriteFile(FILE *handle, unsigned long address, long length)
{
	unsigned long endaddress;

	// Ignore high nibble
	address &= 0x0FFFFFFF;
	if ((address > 0x001FFFFF) || (address < 0x00010000))
	{
		printf("ERROR: Address %-8.8lX invalid!\n", address);
		return (1);
	}
	if (!length)
	{
		printf("ERROR: Zero length file detected!\n");
		return (1);
	}
	if ((address + length) > 0x001FFFFF)
	{
		if(g_verbose==1)
			printf("WARNING: File exceeds memory bounds\n");

		length = 0x00200000 - address;
	}
	endaddress = length + address;
	readfile(handle, memspace + address, length);

	if (endaddress > memorysize)
		memorysize = endaddress;

	return (0);
}


char *str2lwr(char *str)
{
	unsigned char *p = (unsigned char *)str;

	while (*p) {
		*p = tolower((unsigned char)*p);
		p++;
	}

	return str;
}

char *str2upr(char *str)
{
	unsigned char *p = (unsigned char *)str;

	while (*p) {
		*p = toupper((unsigned char)*p);
		p++;
	}

	return str;
}




int cmd_dload(char (*params[]) )
{
	unsigned long address, length;
	FILE *handle;

	if (!params[2] || !params[3])
	{
		printf("ERROR: Not enough parameters for command DLOAD\n");
		return (1);
	}
	handle = fopen(params[2], "rb");
	if (!handle)
	{
		printf("ERROR: Could not find input file '%s' \nCheck your path and filename!\n", params[2]);
		return (1);
	}
	address = (unsigned long) strtoul(params[3], NULL, 16);
	length = (unsigned long) file_length(handle);
	str2upr(params[2]);

	if(g_verbose==1)
		printf("Data %-11.11s length %-8.8lX address %-8.8lX\n", params[2], length,	address);

	if(MemoryWriteFile(handle, address, length))
		return (1);

	fclose(handle);

	fflush(stdout);
	return 0;
}

#if 0
int loadEcoff(FILE *ecoff, char *PSX_RAM) // code from case COFF_EXE below - assumes file is open and close it after loading
{
	EXE_HEADER tmpHead;
	FILHDR coffHead;
	AOUTHDR_YAROZE optHead;
	SCNHDR section;
	int type, i;
	int retval = 0;
	u8 opcode;
	u32 section_address, section_size;

	fseek(ecoff, 0x0, SEEK_SET);
	fread(&coffHead, sizeof(coffHead), 1, ecoff);
	fread(&optHead, sizeof(optHead), 1, ecoff);

	psxRegs.pc = SWAP32(optHead.entry);
	psxRegs.GPR.n.gp = SWAP32(optHead.gp_value); //set gp from siocons
	psxRegs.GPR.n.sp = 0x801ffff0; //orig 0x801fff00 siocons 0x801ffff0

	SysPrintf(_("psxRegs.pc %X \n"), psxRegs.pc);
	SysPrintf(_("psxRegs.GPR.n.gp %X \n"),psxRegs.GPR.n.gp);
	SysPrintf(_("psxRegs.GPR.n.sp %X \n"), psxRegs.GPR.n.sp);


	for (i = 0; i < SWAP16(coffHead.f_nscns); i++) {
		fseek(ecoff, sizeof(FILHDR) + SWAP16(coffHead.f_opthdr) + sizeof(section) * i, SEEK_SET); //              ( 20            +                 56    ) +    40 =   * i
		// fread(&section, sizeof(section), 1, ecoff);
		readfile(ecoff, memspace + address, length);

		if (section.s_scnptr != 0) {
			fseek(ecoff, SWAP32(section.s_scnptr), SEEK_SET);
			fread(PSXM(SWAP32(section.s_paddr)), SWAP32(section.s_size), 1, ecoff);
		} else {
			memset(PSXM(SWAP32(section.s_paddr)), 0, SWAP32(section.s_size));
		}
	}

	return fclose(ecoff); // lazy close!
}
#endif

int cmd_load(char (*params[]) )
{
	FILE *handle;
	int size = 0;
	unsigned long exe_org, length;

	//not this -COMBINE <scriptfile> <outfile> <libps location> "
	//but this: local load main
	if(g_verbose==1)
	{

		printf("\nparams0 = %s\n", params[0]);
		printf("params1 = %s\n", params[1]);
		printf("Net Yaroze exec = %s\n\n", params[2]);
	}

	if (!params[2])
	{
		printf("ERROR: Not enough parameters for command DLOAD\n");
		return (1);
	}
	handle = fopen(params[2], "rb");

	//trying a few common miss naming in the auto file
	if (!handle)
	{
		while(params[2][size])
			size++;
		char yarFile[size];

		strcpy(yarFile,params[2] );

		if(strlen(yarFile) > 3)
		{
			if(yarFile[strlen(yarFile)-3] == '.')
				yarFile[strlen(yarFile)-3] = 0; // removed .XXX
			else
				strcat(yarFile,".exe"); // or add .exe
		}
		else
			strcat(yarFile,".exe"); // or add .exe



		handle = fopen(yarFile, "rb");
		if (!handle)
		{


			if(strlen(yarFile) > 3)
			{
				if(yarFile[strlen(yarFile)-3] == '.')
					yarFile[strlen(yarFile)-3] = 0; // removed .XXX
				else
					strcat(yarFile,".sys"); // used on code::block
			}
			else
				strcat(yarFile,".sys"); // used on code::block




			handle = fopen(yarFile, "rb");
			if (!handle)
			{

				strcpy(yarFile,"main" ); //most common yaroze exe

				handle = fopen(yarFile, "rb");
				if (!handle)
				{
					printf("ERROR: Could not find yaroze main program file (%s) (with/without .exe nor .sys)  - auto file (Net Yaroze script) must have the last line as:  go\nDo not include tabs or other non alphanumeric chars.\nEtentions .exe and .sys are ignored, ie main.exe main.sys are the same as main.\nMake sure the batch file ends with a new line or a go statement and run yarexe auto.batch -v", params[2]);
					return (1);
				}

			}

		}




		printf("\n\n!!WARNING!! \n!Script executable file '%s' was not found!! Using file: %s\n\n", params[2], yarFile);
		fflush(stdout);
		// yarFile has corrected name, copy it
		strcpy(params[2],yarFile );

	}


	g_foundYarExe++; //global var



	// read all of the ecoff file into a temphandle file for eco2exe - not required here
#if 0
	length = file_length(handle);
	readfile(handle, filebuffer, length);
	writefile(combine_fileHnd, filebuffer, length);
#endif

	fclose(handle);



	if( strstr(params[2], ".pxe") || strstr(params[2], ".PXE") ) // codewarrior exe need patching
	{
		if(patchPXE_CodeWarrior(params[2]))
		{
			printf("Code Warrior exe failed patching or load <%s> - see console for more info.\n", params[2]);
			unlink(COMBINE_TMP_FILENAME);// just in case!

			if(g_pxe_filename[0]) // if PXE EXE was created, remove it
				unlink(g_pxe_filename);

			return (1);
		}



	}

	{

#ifndef USE_ECO2EXE_SOURCE
		sprintf(systemcalldata, "%s -p %s", eco2exe_filename, COMBINE_TMP_FILENAME);
		system(systemcalldata);
#else // USE_ECO2EXE_SOURCE
		//do eco2exe


		if(eco2exe_main(params[2]) > 0)
		{
			unlink(COMBINE_TMP_FILENAME);// just in case!
		}
#endif
	}


	combine_fileHnd = fopen(COMBINE_TMP_FILENAME, "rb");
	if (!combine_fileHnd)
	{
		printf("ERROR: Could not open temporary output file %s\n", COMBINE_TMP_FILENAME);
		fflush(stdout);
		unlink(COMBINE_TMP_FILENAME);// just in case!

		if(g_pxe_filename[0]) // if PXE EXE was created, remove it
				unlink(g_pxe_filename);
		return (1);
	}


	if(g_verbose==1)
	{
		printf("eco2exe - finished correctly\n");
		fflush(stdout);
	}

	length = file_length(combine_fileHnd);
	readfile(combine_fileHnd, filebuffer, length);
	if ((length < 2048) || memcmp(filebuffer, "PS-X EXE", 8))
	{
		printf("ERROR: %s has an invalid file format!\n", COMBINE_TMP_FILENAME);
		fflush(stdout);
		unlink(COMBINE_TMP_FILENAME);
		return (1);
	}
	exec_address = *((unsigned long *) (filebuffer + 0x10));
	exe_org = *((unsigned long *) (filebuffer + 0x18));
	stack_address = *((unsigned long *) (filebuffer + 0x30));


	if(g_verbose==1)
	{
		printf("Net Yaroze exe (.sbss) address = (%lX)\n", exec_address);
		printf("Net Yaroze exe (.text) text org address = (%lX)\n", exe_org);
		printf("Net Yaroze exe stack start address = (%lX)\n", stack_address);
		fflush(stdout);

	}


	fseek(combine_fileHnd, 2048, SEEK_SET);

	if( MemoryWriteFile(combine_fileHnd, exe_org, length - 2048) )
		return (1);

	fclose(combine_fileHnd);
	unlink(COMBINE_TMP_FILENAME);// remove tmp file
	//str2upr(params[2]);



	if(g_verbose==1)
	{
		char *file;

		if(g_pxe_filename[0]) // delete CW PS-X EXE file created
			file = g_pxe_filename;
		else
			file = params[2];


		printf("NY exe %s length %lX (%lu) address %lX\n", file, length,	length, exe_org);
		printf("Leaving patched CodeWarrior PS-X EXE file: %s " , g_pxe_filename);
		fflush(stdout);
	}
	else // no verboase
	{

		if(g_pxe_filename[0]) // delete CW PS-X EXE file created
			unlink(g_pxe_filename);
	}



	fflush(stdout);
	return 0;
}

// This is a pretty useful algorithm that you can copy from the old (public
// domain) version of COMBINE.
int parse(char *string)
{
	int str_len = strlen(string), x, y;
	char **params;


	// Ignore blank strings
	if (!str_len) return 0;

	// Ignore strings with only spaces
	for (x = 0; x < str_len; x++)
		if (string[x] != ' ') break;
	if (x == str_len) return 0;

	y = 0;

	params = malloc(sizeof(char *));
	for (;;)
	{
		// Skip all initial spaces for this parameter
		// If end of string occurs meanwhile, break completely
		while ((x < str_len) && (string[x] == ' ')) x++;
		if (x == str_len) break;
		params[y] = &string[x];

		while ((x < str_len) && (string[x] != ' ')) x++;
		// Always set to 0 in either exit condition
		string[x] = 0;
		// But if at the end of string, break
		if (x == str_len)
		{
			y++;
			params = realloc(params, sizeof(char *) * (y + 1));
			params[y] = NULL;
			break;
		}
		// End of parameter.  Increment X and Y.
		x++;
		y++;
		params = realloc(params, sizeof(char *) * (y + 1));
		params[y] = NULL;
	}

	str2upr(params[0]);

	if (params[1])
		str2upr(params[1]);

	if (!strcmp(params[0], "GO") && g_verbose==1)
		{
		printf("GO command received - writing to disk...\n");
		}

	if (params[1] && !strcmp(params[0], "LOCAL"))
	{
		if (!strcmp(params[1], "DLOAD"))
		{
			if( cmd_dload(params) )
				goto err;
		}
		else if (!strcmp(params[1], "LOAD"))
		{
			if(cmd_load(params) )
				goto err;
		}
		else if(g_verbose==1)
		{
			printf("SKIPPED: %s\n", string);

		}
	}
	else if(g_verbose==1)
	{
		printf("SKIPPED: %s\n", string);

	}

	free(params);
	fflush(stdout);
	return 0;

err:
	free(params);
	return 1;
}

void print_authors(void)

{
	printf("\n------------------------------------------------------------------------------\n");

	printf(YAREXE_VER);
	printf("\n");
	printf(YAREXE);



	printf("\nHacked together (poorly) by gwald, from:");
	printf("\n\nCombine program v2.20\n"
			"By Barubary 1998.\n"

			"\neco2exe v0.5\n"
			"By  Silpheed/HITMEN 1998\n"

			"\nexefixup v0.02\n"
			"By Andrew Kieschnick <andrewk@mail.utexas.edu>\n"

			"\nCodeWarrior PS-X EXE File Patcher\n"
			"By ~imilco (Peter Armstrong)\n"

			"\nCopyright is of the respective owners, wherever they are... we thank you!"
			"\n------------------------------------------------------------------------------\n\n");

	fflush(stdout);
}



int main(int argc, char *argv[])
{
	int index, curline, ret =1;
	unsigned char temp;
	char target_combEcoExe[] = "combEco.exe";

	memset(g_pxe_filename, 0, sizeof g_pxe_filename);




	//first check for verbose mode
	if( argc > 2)
	{


		if (strcmp(argv[2], "/v")==0 || strcmp(argv[2], "/V") ==0)
			g_verbose =1;


		if (strcmp(argv[2], "-v")==0 || strcmp(argv[2], "-V") ==0)
			g_verbose =1;

	}


	if(g_verbose)
		print_authors();
	else
	{
		printf("\n");
		printf(YAREXE_VER);
		printf("\n");
		printf(YAREXE);
	}




	if(argc>1) // if options entered
		if( strcmp(argv[1], "/?")==0 || strcmp(argv[1], "/h") ==0  || strcmp(argv[1], "/H" )==0 || strcmp(argv[1], "-h") ==0 || strcmp(argv[1], "--h") ==0  )
			argc = 0;

	if( argc < 2) // 1 is just the exe name
	{



		printf("\n\nSyntax: yarexe scriptfile [-v]\n"
				"scriptfile - is the siocons AUTO or CW's .sio, .dat batch files\n"
				"-v or /v for verbose output and leaves PS-X exe from CodeWarrior .pxe file.\n"

				"\nOutput files is psx.exe, a exefixup combined file which is ready to run! \n"
				"If it didn't create it, something went wrong in combine or eco2exe, etc. \n"
				"auto file (Net Yaroze script) must have the last line as:  go\n"
				"File combEco.exe is a temporary file which is the siocons script combined file and ecoff to exe applied.\n\n");


		goto RUN_EXIT;
	}


	printf("\n");

	{
		int i;
		for(i=0; i<argc; i++)
			printf("argv[%d]=%s\n",i, argv[i]);

	}


	// argv 1 is always auto file!

	auto_fileHnd = fopen(argv[1], "rb");
	if (!auto_fileHnd)
	{

		//try upper case!

		str2upr(argv[1]);
		auto_fileHnd = fopen(argv[1], "rb");

		if (!auto_fileHnd)
		{
			// try again in all lower!
			str2lwr(argv[1]);
			auto_fileHnd = fopen(argv[1], "rb");

		}


		if (!auto_fileHnd)
		{
			//fail
			printf("Could not find input file %s (in original case and both upper and lower case)\n", argv[1]);
			goto RUN_EXIT;
		}
	}

	target_fileHnd = fopen(target_combEcoExe, "w+b");
	if (!target_fileHnd)
	{
		printf("Could not open output file %s\n", argv[2]);
		goto RUN_EXIT;
	}

	combine_fileHnd = fopen(COMBINE_TMP_FILENAME, "w+b");
	if (!combine_fileHnd)
	{
		printf("Could not open temporary output file %s\n", COMBINE_TMP_FILENAME);
		goto RUN_EXIT;
	}
	//tmp file not need until later!
	fclose(combine_fileHnd);

	// Allocate memory
	memstore = calloc(1, 2 * 1024 * 1024 + 2048);
	if (memstore == NULL)
	{
		printf("Not enough memspace memory to build image (need 4 megs)\n");
		goto RUN_EXIT;
	}

	// keep memstore for free, memspace pointer gets moved
	memspace = memstore;

	filebuffer = calloc(1, 2 * 1024 * 1024 + 2048);
	if(filebuffer == NULL)
	{
		printf("Not enough filebuffer memory to build image (need 4 megs)\n");
		goto RUN_EXIT;
	}



	// Put LIBPS in first.
	// Skip EXE header, if any
	//if (length3 == 399360)
	//	fseek(LibPS_fileHnd, 0x800, SEEK_SET);

	// MemoryWriteFile(LibPS_fileHnd, 0x00010000, 397312);

	if( MemoryWriteMemory((char *)psx_exe+0x800, (unsigned long) 0x00010000, (long) 397312) )
		goto RUN_EXIT;


	// Read script into RAM
	length1 = file_length(auto_fileHnd);
	auto_fileBuff = calloc(1, length1 + 1);


	if(auto_fileBuff == NULL)
	{
		printf("ERROR: Out of memory trying to read script\n");
		goto RUN_EXIT;
	}


	readfile(auto_fileHnd, auto_fileBuff, length1);
	auto_fileBuff[length1] = 0;

	scriptstring = malloc(1);
	for (index = 0, curline = 0; index < length1; index++)
	{
		temp = auto_fileBuff[index];
		if ((temp == 0x0D) || (temp == 0x0A)) // - or * ASCII
		{
			scriptstring[curline] = 0;
			curline = 0;
			if( parse(scriptstring) )
			{
				//bad giving up
				goto RUN_EXIT;

			}

			continue;
		}
		if (!temp) continue;
		scriptstring[curline++] = temp;
		scriptstring = realloc(scriptstring, curline + 1);
	}

	// single line, single exe file, no data, no go statement
	if (curline)
	{
		// scriptstring[x] = temp;
		if(g_verbose)
			printf("\nASSUMMING a single line, one executable file with no data, no go statement or even new line!!\n");

		parse(scriptstring);
	}

	if(g_foundYarExe == 0)
	{
		printf("\n ERROR: auto file incorrect, the word local must be before load \n IE: local load main.exe\n");
		goto RUN_EXIT;
	}

	// notes from: https://problemkaputt.de/psx-spx.htm#gpustatusregister
	memspace += 0xF800; // 63,488 bytes from zero
	memset(memspace, 0, 2048);
	memcpy(memspace, "PS-X EXE", 8);
	strcpy(memspace + 0x4C, YAREXE_VER); // +76 bytes
	*((unsigned long *) (memspace + 0x10)) = exec_address;            //  010h      Initial PC                   (usually 80010000h, or higher)
	*((unsigned long *) (memspace + 0x18)) = 0x80010000;              // 018h      Destination Address in RAM   (usually 80010000h, or higher)
	*((unsigned long *) (memspace + 0x1C)) = memorysize - 0x10000;    //01Ch      Filesize (must be N*800h)    (excluding 800h-byte header)
	*((unsigned long *) (memspace + 0x30)) = stack_address;           //030h      Initial SP/R29 & FP/R30 Base (usually 801FFFF0h) (or 0=None)
	// Write target to disk
	fwrite(memspace, 1, memorysize - 0xF800, target_fileHnd); //1050640 PXE

	if(g_verbose==1)
		printf("EXE size: %lu bytes\n ---- Combine2 & eco2exe finished successfully -------\n", memorysize - 0xF800);


	fclose(target_fileHnd);
	target_fileHnd =0;


	if(g_verbose==1)
		printf("\nexefixup filename=%s \n",  target_combEcoExe);


	ret = exeFixUp_main( target_combEcoExe);

	//GOTO RUN_EXIT - single point of exit!
	RUN_EXIT:


	fflush(stdout);


	// remove temp files
	unlink(target_combEcoExe);
	unlink(COMBINE_TMP_FILENAME);



	if(auto_fileHnd)
		fclose(auto_fileHnd);


	if(memstore)
		free(memstore);

	if(filebuffer)
		free(filebuffer);

	if(auto_fileBuff)
		free(auto_fileBuff);

	if(scriptstring)
		free(scriptstring);

	return ret;


}




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

unsigned int char2int(unsigned char *foo)
{
	return foo[3]*16777216 + foo[2]*65536 + foo[1]*256 + foo[0];
}

void int2char(unsigned int foo, unsigned char *bar)
{
	bar[3]=foo>>24;
	bar[2]=foo>>16;
	bar[1]=foo>>8;
	bar[0]=foo;
}


// converts CW .PXE to .EXE to new file
int copyPXE(char *filename)
{
	FILE *ffrom;
	FILE *fto;
	int size, ramSize, i;
	char *pxedata;

	// new file ends with exe, not pxe
	filename[strlen(filename) -3] = 'p'; // make sure filename is pxe
	ffrom = fopen(filename, "rb");
	if (!ffrom)
		return 1;

	//filename[strlen(filename) -3] = 'e'; // change to exe

	fto = fopen(COMBINE_TMP_FILENAME, "wb");
	if (!fto)
		return 1;


	fseek(ffrom,0,SEEK_END);
	size = ftell(ffrom);
	fseek(ffrom,0,SEEK_SET);
	fseek(fto,0,SEEK_SET);

	ramSize = size + 16; // exe size increased


	ramSize += ramSize % 8; // make it more hardware friendly

	pxedata = malloc(ramSize);

	if(pxedata == NULL)
		return 1;

	memset(pxedata, 0,ramSize);

	fread(pxedata, 1, size,  ffrom);
	fflush(ffrom); //force update



	// from: cwpatch: ~imilco (Peter Armstrong)
	/* 08/07/01	Console app to patch Codewarrior PS-X EXE's to run on emulators etc,
	based on the code from Andrew Kieschnick's eco2exe. The patching adds two mips
	jump instructions at the end of the file, the first is an address in system
	memory, the second is the exe's start address. The first address is jumped to
	at the start of program execution, when it returns the original start address
	is executed. The pc0 and t_size values are altered accordingly */

	//patch!
	{


		struct data_s
		{
			unsigned	int	pc0, dummy, start_addr, size;
		} *data_ptr;

		struct mips_s
		{
			unsigned int	sysjump, nop, oldpc0, nop2;
		}*mips_ptr, mips = {0x0C00400C};		/* jal 0x80010030 */



		printf("CodeWarrior PS-X EXE File Patcher by ~imilco (Peter Armstrong)\nFile: %s\n", filename);






		//fflush(file);
		//fseek(file, 16, SEEK_SET);

		/* read in required values, we don't need 2nd */
		//fread(&data, 4, 4, file);

		data_ptr = (struct data_s *) ( (char *)pxedata+16 ); /* seek to pc0 */

		mips.oldpc0 = data_ptr->pc0;						/* keep original start address */
		data_ptr->pc0 = data_ptr->start_addr + data_ptr->size;			/*  new code added at end of exe */
		data_ptr->size += 16;								/* exe size increased */
		/* write new pc0 and size back to file */
		//fseek(file, 16, SEEK_SET);
		//fwrite(&data, 4, 4, file);
		/* add jump address instructions to exe end */
		//fseek(file, 0, SEEK_END);
		/* create instruction to jump to original start address */

		mips.oldpc0 = 0x08000000 + ((mips.oldpc0&0x03ffffff)>>2);

		mips_ptr =  (struct mips_s *) ( (char *)pxedata+size ); //point to end of orig exe file

		memcpy(mips_ptr, &mips, sizeof(mips));//copy to file in ram

		//fwrite(&mips, 4, 4, file);
		//printf("File patched.\n");


		//fwrite(pxedata, size + 16, 1, fto);
		// fwrite(pxedata, sizeof(char),size + 16, fto);
		// fflush(fto); //force update


		// create patch PS-X EXE file
		{
			FILE *PSX;


			if(g_verbose==1)
				printf("Creating PS-X EXE file: %s " , g_pxe_filename);


			PSX = fopen(g_pxe_filename, "wb");

			if (!PSX)
				return 1;


			fwrite(pxedata, sizeof(char),size + 16, PSX);
			fflush(PSX); //force update
			fclose(PSX);
		}

#if 0
		if(g_verbose==1)
		{
			FILE *PSX;
			filename[strlen(filename) -3] = 'e'; // change back to pxe

			printf("Creating PS-X EXE file: %s " , filename);


			PSX = fopen(filename, "wb");

			if (!PSX)
				return 1;


			filename[strlen(filename) -3] = 'p'; // change back to pxe - just incase

			fwrite(pxedata, sizeof(char),size + 16, PSX);
			fflush(PSX); //force update
			fclose(PSX);
		}
#endif

	}



	fclose(ffrom);
	fclose(fto);

	free(pxedata);
	
	fflush(stdout);

	return 0;
}



int patchPXE_CodeWarrior(char *filename)// returns open file pointer to PSX-EXE or fails with NULL
{


	FILE *file;


	strcpy(g_pxe_filename, filename );
	// new file ends with exe, not pxe
	g_pxe_filename[strlen(g_pxe_filename) -3] = 'e';

	file = fopen(g_pxe_filename, "rb");

	if(file  != NULL) //exe file already exists
	{
		fclose(file);
		remove(g_pxe_filename); //remove it and continue
		// return 0;
	}

	// doesn't exist - create


	if(copyPXE(filename))
	{
		printf("Failed to copy CodeWarrior file to new PS-X EXE file <%s> .\n", g_pxe_filename);
		return 1;

	}

	// file = fopen(filename, "w+");
	fflush(stdout);
	return 0;

}


int eco2exe_main(char *filename)
{
	FILE *ecoff;
	FILE *exe;
	unsigned char data[4];
	int i,j,k=2;
	unsigned int header_data[14];
	unsigned int ecoff_size;
	unsigned int address;
	unsigned int pc0;
	unsigned char ecoffpatch=1; /* 0=don't, 1=do */
	ECOHDRS ecoffHDR;

	memset(header_data, 0, sizeof header_data);
	memset(data, 0, sizeof data);

	//float lit sections PITA
	int litPos=-1;
	int litVal[3];


	/*
  if ((argc<2)||(argc>3))
    usage();

  for(i=1;i<argc;i++)
    {
      if(argv[i][0]=='-')
        switch(argv[i][1])
          {
          case 'P':
          case 'p':
	    patch=1;
	    break;
	  default:
	    usage();
	  }
      else
	strncpy(filename,argv[i],256);
    }
	 */



	if(g_pxe_filename[0]) // codewarrior file has been patched?
		filename = g_pxe_filename; // use patched PS=X EXE instead

	ecoff=fopen(filename, "rb");

	if (!ecoff)
	{
		printf("100 ERROR: Can't open %s\n",filename);
		return 1;
	}

	//strcat(filename, ".exe"); /* output filename is same as input but with .exe appended */


	exe=fopen(COMBINE_TMP_FILENAME, "wb");

	if (!exe)
	{
		printf("2 ERROR: Can't open %s\n",filename);

		return 2;
	}

	fseek(ecoff,0,SEEK_SET);
	fread(&ecoffHDR, sizeof(ecoffHDR),1,  ecoff);
	fseek(ecoff,0,SEEK_SET);

	for(i=0;i<2;i++)
		fscanf(ecoff, "%c", &data[i]);
	//0x02




	if (memcmp((char *) &ecoffHDR, "PS-X EXE", 8) == 0)
	{
		int size;
		char *exedata;

		if(g_verbose==1)
			printf("%s is PS-X EXE. \n",filename);

		fseek(ecoff,0,SEEK_END);
		size = ftell(ecoff);
		fseek(ecoff,0,SEEK_SET);

		fseek(exe,0,SEEK_SET);





		exedata = malloc(size);

		if(exedata == NULL)
		{
			printf("PS-X EXE binary memory failed allocation\n");
			return 3;
		}

		memset(exedata, 0,size);

		fread(exedata, sizeof(char),size,  ecoff);
		fflush(ecoff); //force update


		fwrite(exedata, sizeof(char),size, exe);
		fflush(exe); //force update



		fclose(exe);
		fclose(ecoff);
		free(exedata);

		fflush(stdout);
		return 0; // return ok
	}
	else if ( ((char *)&ecoffHDR)[0] == 'C' &&  ((char *)&ecoffHDR)[1] == 'P' &&  ((char *)&ecoffHDR)[2] == 'E')
	{
		printf("CRITICAL ERROR - %s is a CPE binary file which is not supported.\n",filename);
		printf("Create a PS-X EXE with CPE2X or CPE2PSX and try again.\n");
		return 111;
	}
	else if (( ((char *)&ecoffHDR)[0]==0x62)||( ((char *)&ecoffHDR)[1]==0x01))
	{



		//MIPS ECOFF A.OUT format -  NET YAROZE

		if(g_verbose==1)
		{
			printf("ECOFF NET YAROZE A.OUT binary - sections: %d\n", ecoffHDR.fileHDR.f_nscns);


			printf("ecoffHDR.ecoHDR.entry (pc) 0x%X \n", ecoffHDR.ecoHDR.entry );
			printf("ecoffHDR.ecoHDR.gp_value 0x%X \n",ecoffHDR.ecoHDR.gp_value);

		}

		for(i=0;i<ecoffHDR.fileHDR.f_nscns;i++)
		{

			/*
			 *  loading
			 * 	(long)STYP_TEXT, {_TEXT},
			(long)STYP_RDATA, {_RDATA},
			(long)STYP_DATA, {_DATA},
			(long)STYP_SDATA, {_SDATA}

			 */

#define	_TEXT	".text"
#define	TEXT_POS 2

#define	_RDATA	".rdata"
#define	RDATA_POS 5

#define	_DATA	".data"
#define	DATA_POS 8


#define	_SDATA	".sdata"
#define	SDATA_POS 11


			if(g_verbose==1)
				printf("\nECOFF  sections: %d/%d %s\n", i+1,ecoffHDR.fileHDR.f_nscns ,  ecoffHDR.sections[i].s_name);

			if(  !strcmp( ecoffHDR.sections[i].s_name, _TEXT))
			{
				k =TEXT_POS;//first section
				header_data[k]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

				if(g_verbose==1)
					printf("s_paddr: 0x%X\n",header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_size;  // scnhdr.s_size - 92  - 96 section size
				if(g_verbose==1)
					printf("s_size: 0x%X (%d)\n",header_data[k],header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section

				if(g_verbose==1)
					printf("s_scnptr: 0x%X\n",header_data[k] );

				k++;

			}
			else   if(  !strcmp( ecoffHDR.sections[i].s_name, _RDATA))
			{
				k = RDATA_POS;//2 section

				header_data[k]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

				if(g_verbose==1)
					printf("s_paddr: 0x%X\n",header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_size;  // scnhdr.s_size - 92  - 96 section size

				if(g_verbose==1)
					printf("s_size: 0x%X (%d)\n",header_data[k],header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section

				if(g_verbose==1)
					printf("s_scnptr: 0x%X\n",header_data[k] );

				k++;

			}
			else   if(  !strcmp( ecoffHDR.sections[i].s_name, _DATA))
			{
				k = DATA_POS;//3 section

				header_data[k]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

				if(g_verbose==1)
					printf("s_paddr: %X\n",header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_size;  // scnhdr.s_size - 92  - 96 section size

				if(g_verbose==1)
					printf("s_size: 0x%X (%d)\n",header_data[k],header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section


				if(g_verbose==1)
					printf("s_scnptr: 0x%X\n",header_data[k] );

				k++;


			}

			// last section can be either SDATA or .LIT
			// https://www3.physnet.uni-hamburg.de/physnet/Tru64-Unix/HTML/APS31DTE/DOCU_013.HTM

			else   if( !strcmp( ecoffHDR.sections[i].s_name,  _SDATA))
			{
				k = SDATA_POS;//4 section
				header_data[k]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

				if(g_verbose==1)
					printf("s_paddr: 0x%X\n",header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_size;  // scnhdr.s_size - 92  - 96 section size


				if(g_verbose==1)
					printf("s_size: 0x%X (%d)\n",header_data[k],header_data[k] );

				k++;

				header_data[k]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section

				if(g_verbose==1)
					printf("s_scnptr: 0x%X\n",header_data[k] );

				k++;

			}
			else   if(strstr(  ecoffHDR.sections[i].s_name, ".lit8") > 0)  // C:\PSX3 compiler soft float stuff??
			{

				if(g_verbose==1)
				{
					printf("\n\nWARNING: Experimental section detected: %s\n",  ecoffHDR.sections[i].s_name);
					printf("WARNING: Conversion with this section is buggy and may crash your program!!!\n");
					printf("WARNING: Address space in question: 0x%X\n",ecoffHDR.sections[i].s_paddr );
					printf("WARNING: Size in question: 0x%X (%d) bytes \n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
					printf("WARNING: This typically happens with the C:\\PSX3 compiler when using software floats.\n" );
					printf("WARNING: If your program crashes:\n" );
					printf("WARNING: 1) Note address of crash, check mapfile, remove floats or doubles.  \n" );
					printf("WARNING: 2) Use a different compiler..... Sorry :/ \n" );
					printf("WARNING: 3) Fix this code?? :/ \n" );
					printf("WARNING:  \n" );

					printf(".lit8: s_paddr: 0x%X\n",ecoffHDR.sections[i].s_paddr );
					printf(".lit8: s_size: 0x%X (%d)\n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
					printf(".lit8: s_scnptr: 0x%X\n",ecoffHDR.sections[i].s_scnptr );
					printf("\n");

				}
				else
				{
					printf("\n\nWARNING: Experimental section detected: %s\n",  ecoffHDR.sections[i].s_name);
					printf("WARNING: The psx.exe may results in crashing.\n");
					printf("WARNING: Turn verbose on -v to see full message.\n");

				}






				litPos=i;

				litVal[0]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

				if(g_verbose==1)
					printf("s_paddr: 0x%X\n",litVal[0]);



				litVal[1]= ecoffHDR.sections[i].s_size;// fixes issue on RS +0x100;  // scnhdr.s_size - 92  - 96 section size

				if(g_verbose==1)
					printf("s_size: 0x%X (%d)\n",litVal[1], litVal[1] );



				litVal[2]= ecoffHDR.sections[i].s_scnptr;  // s


			}
			else   if( strstr(  ecoffHDR.sections[i].s_name, ".lit4") > 0)  // C:\PSX3 compiler soft float stuff??
			{
				printf("Skipped: s_paddr: 0x%X\n",ecoffHDR.sections[i].s_paddr );
				printf("Skipped: s_size: 0x%X (%d)\n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
				printf("Skipped: s_scnptr: 0x%X\n",ecoffHDR.sections[i].s_scnptr );
				printf("\n");
				//continue;


				printf("litPos : %d\n",litPos);
				if(litPos==-1)
				{
					litPos=i;

					if(g_verbose==1)
					{
						printf("\n\nWARNING: Experimental section detected: %s\n",  ecoffHDR.sections[i].s_name);
						printf("WARNING: Conversion with this section is buggy and may crash your program!!!\n");
						printf("WARNING: Address space in question: 0x%X\n",ecoffHDR.sections[i].s_paddr );
						printf("WARNING: Size in question: 0x%X (%d) bytes \n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
						printf("WARNING: This typically happens with the C:\\PSX3 compiler when using software floats.\n" );
						printf("WARNING: If your program crashes:\n" );
						printf("WARNING: 1) Note address of crash, check mapfile, remove floats or doubles.  \n" );
						printf("WARNING: 2) Use a different compiler..... Sorry :/ \n" );
						printf("WARNING: 3) Fix this code?? :/ \n" );
						printf("WARNING:  \n" );


						printf(".lit4: s_paddr: 0x%X\n",ecoffHDR.sections[i].s_paddr );
						printf(".lit4: s_size: 0x%X (%d)\n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
						printf(".lit4: s_scnptr: 0x%X\n",ecoffHDR.sections[i].s_scnptr );
						printf("\n");
					}
					else
					{
						printf("\n\nWARNING: Experimental section detected: %s\n",  ecoffHDR.sections[i].s_name);
						printf("WARNING: The psx.exe may results in crashing.\n");
						printf("WARNING: Turn verbose on -v to see full message.\n");

					}


					litVal[0]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

					if(g_verbose==1)
						printf("s_paddr: 0x%X\n",litVal[0]);



					litVal[1]= ecoffHDR.sections[i].s_size;// fixes issue on RS +0x100;  // scnhdr.s_size - 92  - 96 section size

					if(g_verbose==1)
						printf("s_size: 0x%X (%d)\n",litVal[1], litVal[1] );



					litVal[2]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section

					if(g_verbose==1)
						printf("s_scnptr: 0x%X\n",litVal[2] );
				}
				else
				{
					if(g_verbose==1)
						printf(".lit8 processed, now adding .lit4 size \n" );


					//litVal[0]= ecoffHDR.sections[i].s_paddr;  //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib

					if(g_verbose==1)
						printf("s_paddr: 0x%X\n",litVal[0]);



					litVal[1]+= ecoffHDR.sections[i].s_size;// fixes issue on RS +0x100;  // scnhdr.s_size - 92  - 96 section size

					if(g_verbose==1)
						printf("s_size: 0x%X (%d)\n",litVal[1], litVal[1] );



					//	litVal[2]= ecoffHDR.sections[i].s_scnptr;  // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section

					if(g_verbose==1)
						printf("s_scnptr: 0x%X\n",litVal[2] );

				}



			}

			else // not used
			{
				if(g_verbose==1)
				{
					printf("Skipped: s_paddr: 0x%X\n",ecoffHDR.sections[i].s_paddr );
					printf("Skipped: s_size: 0x%X (%d)\n",ecoffHDR.sections[i].s_size, ecoffHDR.sections[i].s_size );
					printf("Skipped: s_scnptr: 0x%X\n",ecoffHDR.sections[i].s_scnptr );
					printf("\n");
				}
			}







			/*


				{_TEXT},
				(long)STYP_DATA, {_DATA},
				    (long)STYP_RDATA, {_RDATA},

				    (long)STYP_SDATA, {_SDATA}



				    if ( s->s_flags & section_types[ i ].flags &&
	    !strcmp( s->s_name, section_types[i].name)) {
	    return 1;
	}


				// 84b start of scnhdr.s_paddr sizeof(scnhdr) = 40b
				fseek(ecoff, 0x54+i*0x28, SEEK_SET); // 84 (i*40)
				for(j=0;j<4;j++)
					fscanf(ecoff, "%c", &data[j]);
				header_data[k]=char2int(data); //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib
				k++;
				fseek(ecoff, 0x54+i*0x28+0x08, SEEK_SET);
				for(j=0;j<4;j++)
					fscanf(ecoff, "%c", &data[j]);
				header_data[k]=char2int(data); // scnhdr.s_size - 92  - 96 section size
				k++;
				for(j=0;j<4;j++)
					fscanf(ecoff, "%c", &data[j]);
				header_data[k]=char2int(data); // scnhdr.s_scnptr;		96  - 100 file ptr to raw data for section
				k++;
			 */
		}



		if(litPos>0)
		{
			if(g_verbose==1)
			{
				printf("\nProcessing litPos was %d \n", litPos);
				printf("s_paddr: 0x%X\n",litVal[0]);
				printf("s_size: 0x%X (%d)\n", litVal[1], litVal[1]);
				printf("s_scnptr: 0x%X\n", litVal[2]);
				printf("\n");
			}
#define	_DATA	".data"
#define	DATA_POS 8

#if 1
			// both work - I'm confused?
#if 1
			header_data[SDATA_POS] = litVal[0]; // replace address with lit
			header_data[SDATA_POS+1] += litVal[1]; //add size
			header_data[SDATA_POS+2] = litVal[2]; // not sure

#else
			//header_data[DATA_POS] = litVal[0]; // replace address with lit
			header_data[DATA_POS+1] += litVal[1]; //add size
			//header_data[DATA_POS+2] = litVal[2]; // not sure
#endif

#endif

		}



	}





	k =2;//first section

	fseek(ecoff, 0x24, SEEK_SET);
	//0x24 = 36bytes: cprmask 4*4 bytes

	for(i=0;i<4;i++)
		fscanf(ecoff, "%c", &data[i]);

	// 40bytes
	header_data[0]=char2int(data); //entry 0x80128180

	fseek(ecoff, 0x48, SEEK_SET);
	//72bytes  gp_value
	for(i=0;i<4;i++)
		fscanf(ecoff, "%c", &data[i]);

	header_data[1]=char2int(data); // gp_value 0x80131d40



	if(g_verbose==1)
	{
		printf("ecoffHDR.ecoHDR.entry (pc) 0x%X \n", ecoffHDR.ecoHDR.entry );
		printf("ecoffHDR.ecoHDR.gp_value 0x%X \n",ecoffHDR.ecoHDR.gp_value);
		printf("reread: entry (pc) 0x%X \n",header_data[0] );
		printf("reread: gp_value 0x%X \n",header_data[1]);
	}


#if 0 //old
	for(i=0;i<4;i++)
	{
		char section[8]={0};
		fseek(ecoff, 0x4C+i*0x28, SEEK_SET); // 84 (i*40)
		fscanf(ecoff, "%c", &section[0]);
		fscanf(ecoff, "%c", &section[1]);
		fscanf(ecoff, "%c", &section[2]);
		fscanf(ecoff, "%c", &section[3]);
		fscanf(ecoff, "%c", &section[4]);
		fscanf(ecoff, "%c", &section[5]);
		fscanf(ecoff, "%c", &section[6]);
		fscanf(ecoff, "%c", &section[7]);
		printf("section: %d %s\n",i, section);

		// 84b start of scnhdr.s_paddr sizeof(scnhdr) = 40b
		fseek(ecoff, 0x54+i*0x28, SEEK_SET); // 84 (i*40)
		for(j=0;j<4;j++)
			fscanf(ecoff, "%c", &data[j]);
		header_data[k]=char2int(data); //  scnhdr.s_paddr - 84  - 88 physical address, aliased s_nlib
		printf("s_paddr: %X\n",header_data[k] );
		k++;
		fseek(ecoff, 0x54+i*0x28+0x08, SEEK_SET);
		for(j=0;j<4;j++)
			fscanf(ecoff, "%c", &data[j]);
		header_data[k]=char2int(data); // scnhdr.s_size - 92  - 96 section size
		printf("s_size:  %X\n",header_data[k] );
		k++;
		for(j=0;j<4;j++)
			fscanf(ecoff, "%c", &data[j]);
		header_data[k]=char2int(data); // scnhdr.s_scnptr;		/* 96  - 100 file ptr to raw data for section
		printf("s_scnptr:  %X\n",header_data[k] );
		k++;
	}


#define	_TEXT	".text"
#define	TEXT_POS 2

#define	_RDATA	".rdata"
#define	RDATA_POS 5

#define	_DATA	".data"
#define	DATA_POS 8


#define	_SDATA	".sdata"
#define	SDATA_POS 11

#endif
	//0x80129d40 - 0x80120000 + 0x130 = 0x9E70 = 40560
	ecoff_size=(header_data[SDATA_POS]-header_data[TEXT_POS])+header_data[SDATA_POS+1];

	if(ecoffpatch)
	{
		pc0=header_data[TEXT_POS]+ecoff_size;
		ecoff_size+=16;


		if(g_verbose==1)
		{
			printf("ECOFF new patched: entry (pc) 0x%X = PC 0x%X  + size %d  \n",pc0 , header_data[TEXT_POS], ecoff_size-16);
			printf("ECOFF new patched: size (%d) 0x%X = 0x%X =  (%d+16) \n",ecoff_size, ecoff_size, ecoff_size, ecoff_size-16 );
		}

	}
	else
		pc0=header_data[0];


	if(g_verbose==1)
	{
		printf("ecoffHDR.ecoHDR.entry (pc) 0x%u\n", pc0 );
	}


	fprintf(exe, "PS-X EXE");
	int2char(0, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* text */
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* data */
	int2char(pc0, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* pc0 */
	int2char(0, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* gp0 */
	int2char(header_data[2], data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* t_addr */
	int2char(ecoff_size, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* t_size */
	int2char(0, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* d_addr */
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* d_size */
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* b_addr */
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* b_size */
	int2char(0x801fff00, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* s_addr */
	int2char(0, data);
	fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]); /* s_size */

	for(i=0;i<1992;i++)
		fprintf(exe, "%c", 0);

	fseek(ecoff, header_data[4], SEEK_SET);
	address=header_data[2];
	for(i=0;i<header_data[3];i++)
	{
		fscanf(ecoff, "%c", &data[0]);
		fprintf(exe, "%c", data[0]);
		address++;
	}
	fseek(ecoff, header_data[7], SEEK_SET);
	if (address!=header_data[5]) /* wonder if this can happen */
		for(i=address;i<=header_data[5];i++)
		{
			fprintf(exe, "%c", 0);
			address++;
		}
	for(i=0;i<header_data[6];i++)
	{
		fscanf(ecoff, "%c", &data[0]);
		fprintf(exe, "%c", data[0]);
		address++;
	}
	fseek(ecoff, header_data[10], SEEK_SET);
	if (address!=header_data[8]) /* wonder if this can happen */
		for(i=address;i<=header_data[8];i++)
		{
			fprintf(exe, "%c", 0);
			address++;
		}
	for(i=0;i<header_data[9];i++)
	{
		fscanf(ecoff, "%c", &data[0]);
		fprintf(exe, "%c", data[0]);
		address++;
	}

	fseek(ecoff, header_data[13], SEEK_SET);
	if (address!=header_data[11]) /* wonder if this can happen */
		for(i=address;i<=header_data[11];i++)
		{
			fprintf(exe, "%c", 0);
			address++;
		}
	for(i=0;i<header_data[12];i++)
	{
		fscanf(ecoff, "%c", &data[0]);
		fprintf(exe, "%c", data[0]);
		address++;
	}

	if(ecoffpatch)
	{
		int2char(0x0c00400c, data); /* jal 0x80010030 */
		fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]);
		int2char(0, data); /* nop */
		fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]);
		int2char(0x08000000 + ((header_data[0]&0x03ffffff)>>2),data); /*j realpc0 */
		fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]);
		int2char(0, data); /* nop */
		fprintf(exe, "%c%c%c%c", data[0], data[1], data[2], data[3]);
	}

	fclose(exe);
	fclose(ecoff);
	
	fflush(stdout);
	return 0;

}














/*
 * exefixup.c v0.02 Andrew Kieschnick <andrewk@mail.utexas.edu>
 *
 * displays PS-X EXE header information
 * offers to fix incorrect t_size
 * offers to pad to 2048-byte boundary for cd-rom use
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

#include <ctype.h>

unsigned int char2intExec(unsigned char *fooExec)
{
	return fooExec[3]*16777216 + fooExec[2]*65536 + fooExec[1]*256 + fooExec[0];
}

void int2charExec(unsigned int fooExec, unsigned char *barEXEC)
{
	barEXEC[3]=fooExec>>24;
	barEXEC[2]=fooExec>>16;
	barEXEC[1]=fooExec>>8;
	barEXEC[0]=fooExec;
}


int exeFixUp_main(char *filenameEXEC)
{
	FILE *exeHnd;
	FILE *outHnder;
	unsigned char dataEXEC[9];
	// char filename[256];
	int i;
	unsigned int headerDATA[12];
	unsigned int sizeEXEC;
	unsigned int padsizeEXE;
	int psxExeCreated = 0;
	//char yesno='Z';

	unlink("psx.exe"); // psx.exe must be deleted first!

	//if (argc!=2)
	// usage();

	//strncpy(filenameEXEC,argv[1],256);

	exeHnd=fopen(filenameEXEC, "rb");

	if (!exeHnd)
	{
		printf("ERROR: Can't open %s\n",filenameEXEC);
		fflush(stdout);
		return (1);
	}

	for(i=0;i<8;i++)
		fscanf(exeHnd, "%c", &dataEXEC[i]);
	dataEXEC[sizeof dataEXEC - 1]=0;

	if (strncmp((char *) dataEXEC, "PS-X EXE", sizeof dataEXEC - 1))
	{
		printf("ERROR: Not a PS-X EXE file\n");
		fflush(stdout);

		unlink(filenameEXEC);
		unlink(COMBINE_TMP_FILENAME);

		return (1);
	}

	for(i=0;i<12;i++)
	{
		fscanf(exeHnd, "%c", &dataEXEC[0]);
		fscanf(exeHnd, "%c", &dataEXEC[1]);
		fscanf(exeHnd, "%c", &dataEXEC[2]);
		fscanf(exeHnd, "%c", &dataEXEC[3]);
		headerDATA[i]=char2intExec(dataEXEC);
	}

	if(g_verbose==1)
	{
		printf("id\tPS-X EXE\n");
		printf("text\t0x%.8x\n", headerDATA[0]);
		printf("data\t0x%.8x\n", headerDATA[1]);
		printf("pc0\t0x%.8x\n", headerDATA[2]);
		printf("gp0\t0x%.8x\n", headerDATA[3]);
		printf("t_addr\t0x%.8x\n", headerDATA[4]);
		printf("t_size\t0x%.8x\n", headerDATA[5]);
		printf("d_addr\t0x%.8x\n", headerDATA[6]);
		printf("d_size\t0x%.8x\n", headerDATA[7]);
		printf("b_addr\t0x%.8x\n", headerDATA[8]);
		printf("b_size\t0x%.8x\n", headerDATA[9]);
		printf("s_addr\t0x%.8x\n", headerDATA[10]);
		printf("s_size\t0x%.8x\n\n", headerDATA[11]);
	}

	/*

  fseek(exe, 0, SEEK_END);

  size=ftell(exe)-2048;

  padsize=2048-(size%2048);



	 */




	fseek(exeHnd, 0, SEEK_END);

	sizeEXEC=ftell(exeHnd)-2048;

	if(g_verbose==1)
		printf("sizeEXEC\t %d\n", sizeEXEC);


	padsizeEXE=2048-(sizeEXEC%2048);

	if(g_verbose==1)
		printf("padsizeEXE\t %d\n", padsizeEXE);


	if (padsizeEXE!=2048)
	{
		if(g_verbose==1)
			printf("Creating psx.exe: Fixing EXE size to a multiple of 2048!\n");

		/*    while ((yesno!='Y')&&(yesno!='N'))
	{
          printf("Write a padded EXE (to psx.exe) ? ");
	  scanf("%c%*c", &yesno);
	  yesno=toupper(yesno);
	}
      if (yesno=='Y')
		 */
		{
			outHnder = fopen("psx.exe", "wb");

			if (!outHnder)
			{
				printf("ERROR in padsizeEXE: Can't open psx.exe");
				return (1);
			}

			headerDATA[5]=sizeEXEC+padsizeEXE;

			fprintf(outHnder, "PS-X EXE");
			for(i=0;i<12;i++)
			{
				int2charExec(headerDATA[i], dataEXEC);
				fprintf(outHnder, "%c%c%c%c", dataEXEC[0], dataEXEC[1], dataEXEC[2], dataEXEC[3]);
			}

			fseek(exeHnd, 56, SEEK_SET);

			for(i=0;i<sizeEXEC+1992;i++)
			{
				fscanf(exeHnd, "%c", &dataEXEC[0]);
				fprintf(outHnder, "%c", dataEXEC[0]);
			}
			for(i=0;i<padsizeEXE;i++)
				fprintf(outHnder, "%c", 0);

			sizeEXEC=headerDATA[5];
			fclose(outHnder);
			psxExeCreated++;
		}
	}
	else if(g_verbose==1)
		printf("padsizeEXE already 2048 \n");

	// yesno='Z';

	if (sizeEXEC!=headerDATA[5])
	{
		if(g_verbose==1)
		{
			printf("2 Fixing: EXE header t_size does not match filesize-2048\n");
			printf("EXE header:\t 0x%.8x bytes\n", headerDATA[5]);
			printf("filesize-2048:\t 0x%.8x bytes\n", sizeEXEC);
		}

		/*    while ((yesno!='Y')&&(yesno!='N'))
	{
          printf("Write a corrected EXE (to psx.exe) ? ");
	  scanf("%c%*c", &yesno);
	  yesno=toupper(yesno);
	}
      if (yesno=='Y')
		 */
		{
			outHnder = fopen("psx.exe", "wb");

			if (!outHnder)
			{
				printf("ERROR in sizeEXEC: Can't open psx.exe");
				fflush(stdout);
				return 1;
			}

			fprintf(outHnder, "PS-X EXE");
			for(i=0;i<5;i++)
			{
				int2charExec(headerDATA[i], dataEXEC);
				fprintf(outHnder, "%c%c%c%c", dataEXEC[0], dataEXEC[1], dataEXEC[2], dataEXEC[3]);
			}
			int2charExec(sizeEXEC, dataEXEC);
			fprintf(outHnder, "%c%c%c%c", dataEXEC[0], dataEXEC[1], dataEXEC[2], dataEXEC[3]);
			for(i=6;i<12;i++)
			{
				int2charExec(headerDATA[i], dataEXEC);
				fprintf(outHnder, "%c%c%c%c", dataEXEC[0], dataEXEC[1], dataEXEC[2], dataEXEC[3]);
			}

			fseek(exeHnd, 56, SEEK_SET);

			for(i=0;i<sizeEXEC+1992;i++)
			{
				fscanf(exeHnd, "%c", &dataEXEC[0]);
				fprintf(outHnder, "%c", dataEXEC[0]);
			}
			fclose(outHnder);
		}
		psxExeCreated++;
	}
	else if(g_verbose==1)
		printf("sizeEXEC already equal to headerDATA[5] \n");

	if(g_verbose==1)
		printf("exeFixUp_main finished\n");

	fclose(exeHnd);

	if(psxExeCreated>0)
	{

		if(g_verbose==1)
		{
			printf("%s removed.\n", filenameEXEC);
		}


		unlink(filenameEXEC);



	}
	else
	{


		int ret = rename(filenameEXEC, "psx.exe");

		if(g_verbose==1)
			printf("%s renamed to psx.exe\n", filenameEXEC);

		if(ret == 0)
		{
			if(g_verbose==1)
				printf("File renamed successfully\n");
		}
		else
		{
			printf("Error: unable to rename the file");
			fflush(stdout);
			return 1;
		}
	}



	printf("\nOutput: psx.exe built correctly!\n");

	fflush(stdout);
	return 0;

}
#include "yarexe.inc"
