// Yaroze PS-X EXE builder v2.31 1st July 2018
// Copyright (C) Barubary 1998.  All rights not expressly granted reserved.
// Yaroze and PS-X are trademarks of Sony Computer Entertainment, Inc.

// THIS SOURCE CODE MAY ONLY BE MODIFIED FOR PERSONAL USE.  YOU MAY NOT
// DISTRIBUTE MODIFIED VERSIONS OF THIS SOURCE CODE OR THE PROGRAM COMPILED
// FROM MODIFIED SOURCE CODE, BUT YOU MAY MODIFY IT FOR PERSONAL USE.

// THIS PROGRAM AND ITS DOCUMENTATION MAY BE DISTRIBUTED TO ANYONE, PROVIDED
// IT IS NOT MODIFIED NOR ANY CHARGE IS ASSOCIATED WITH DISTRIBUTION.
// NO WARRANTY.

// I was given permission to use eco2exe source by Silpheed (thank you), but
// I cannot distribute it.  Comment this to use the old style combine, which
// calls eco2exe.exe via system().
  #define USE_ECO2EXE_SOURCE

#ifdef USE_ECO2EXE_SOURCE
  void eco2exe_main(char *_filename);
  void exeFixUp_main( char *filename);
#endif

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

FILE *handle1, *target_fileHnd, *LibPS_fileHnd, *temphandle;
long length1, length2, length3, length4, memorysize = 0;
unsigned long exec_address = 0, stack_address = 0;
unsigned char *memspace, *script, scriptstring[1024];
unsigned char *eco2exe_filename, systemcalldata[256];
unsigned char *filebuffer;
char foundYarExe = 0;

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

    while (length > 0)
    {
        templength = (length > BUFSIZ) ? BUFSIZ : length;
        fwrite((char *) buffer + templocation, templength, 1, handle);
        length -= templength;
        templocation += templength;
    }
}

void MemoryWriteFile(FILE *handle, unsigned long address, long length)
{
unsigned long endaddress;

    // Ignore high nibble
    address &= 0x0FFFFFFF;
    if ((address > 0x001FFFFF) || (address < 0x00010000))
    {
        printf("ERROR: Address %-8.8X invalid!\n");
        exit(1);
    }
    if (!length)
    {
        printf("ERROR: Zero length file detected!\n");
        exit(1);
    }
    if ((address + length) > 0x001FFFFF)
    {
        printf("WARNING: File exceeds memory bounds\n");
        length = 0x00200000 - address;
    }
    endaddress = length + address;
    readfile(handle, memspace + address, length);
    if (endaddress > memorysize)
        memorysize = endaddress;
}

void cmd_dload(unsigned char *params[])
{
unsigned long address, length;
FILE *handle;

    if (!params[2] || !params[3])
    {
        printf("ERROR: Not enough parameters for command DLOAD\n");
        exit(1);
    }
    handle = fopen(params[2], "rb");
    if (!handle)
    {
        printf("ERROR: Could not find input file %s\n", params[2]);
        exit(1);
    }
    address = strtoul(params[3], NULL, 16);
    length = (unsigned long) file_length(handle);
    strupr(params[2]);
    printf("Data %-11.11s length %-8.8X address %-8.8X\n", params[2], length,
        address);
    MemoryWriteFile(handle, address, length);
    fclose(handle);
}

void cmd_load(unsigned char *params[])
{
FILE *handle;
unsigned long exe_org, length;

//not this -COMBINE <scriptfile> <outfile> <libps location> "
//but this: local load main
  printf("\n params0 = %s\n", params[0]);
  printf("\n params1 = %s\n", params[1]);
printf("\n Using Net Yaroze exec = %s\n\n", params[2]);
  
    if (!params[2])
    {
        printf("ERROR: Not enough parameters for command DLOAD\n");
        exit(1);
    }
    handle = fopen(params[2], "rb");
    if (!handle)
    {
			char yarFile[150];
		
		    strcpy(yarFile,params[2] );
						
			strcat(yarFile,".exe");
     
			handle = fopen(yarFile, "rb");
				if (!handle)
				{
					
					strcpy(yarFile,params[2] );
								
					strcat(yarFile,".sys"); // used on code::block 
			 
					handle = fopen(yarFile, "rb");
					if (!handle)
					{
							
						strcpy(yarFile,"main" ); //most common yaroze exe
						
						handle = fopen(yarFile, "rb");
						if (!handle)
						{
							printf("ERROR: Could not find yaroze main program file >%s< (with/without .exe nor .sys)  - auto file (Net Yaroze script) must have the last line as:  go\nDo not include tabs or other non alphanumeric chars.\nEtentions .exe and .sys are ingnored, ie main.exe main.sys are the same as main.", params[2]);
							exit(1);
						}
					 
				}
			}
			
			// yarFile has the correct name
			strcpy(params[2],yarFile );
			

    }
	
	foundYarExe++; //global var
	
	
    length = file_length(handle);
    readfile(handle, filebuffer, length);
    writefile(temphandle, filebuffer, length);
    fclose(handle);
    fclose(temphandle);
#ifndef USE_ECO2EXE_SOURCE
    sprintf(systemcalldata, "%s -p COMBINE.TMP", eco2exe_filename);
    system(systemcalldata);
#else // USE_ECO2EXE_SOURCE
    eco2exe_main(params[2]);
#endif
 
 
    temphandle = fopen("COMBINE.TMP", "rb");
    if (!temphandle)
    {
        printf("ERROR: Could not open temporary output file COMBINE.TMP\n");
        exit(1);
    }
    length = file_length(temphandle);
    readfile(temphandle, filebuffer, length);
    if ((length < 2048) || memcmp(filebuffer, "PS-X EXE", 8))
    {
        printf("ERROR: COMBINE.TMP has an invalid file format!\n");
        exit(1);
    }
    exec_address = *((unsigned long *) (filebuffer + 0x10));
    exe_org = *((unsigned long *) (filebuffer + 0x18));
    stack_address = *((unsigned long *) (filebuffer + 0x30));
    fseek(temphandle, 2048, SEEK_SET);
    MemoryWriteFile(temphandle, exe_org, length - 2048);
    fclose(temphandle);
    strupr(params[2]);
    printf("Prog %-11.11s length %-8.8X address %-8.8X\n", params[2], length,
        exe_org);
}

// This is a pretty useful algorithm that you can copy from the old (public
// domain) version of COMBINE.
void parse(unsigned char *string)
{
int str_len = strlen(string), x, y;
unsigned char *params[60];
 

    // Ignore blank strings
    if (!str_len) return;

    // Ignore strings with only spaces
    for (x = 0; x < str_len; x++)
        if (string[x] != ' ') break;
    if (x == str_len) return;

    y = 0;

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
            break;
        }
        // End of parameter.  Increment X and Y.
        x++;
        y++;
    }
    params[y] = NULL;

    strupr(params[0]);
    if (params[1]) strupr(params[1]);
    if (!strcmp(params[0], "GO"))
        printf("GO command received - writing to disk...\n");
    
	if (params[1] && !strcmp(params[0], "LOCAL"))
    {
        if (!strcmp(params[1], "DLOAD"))
            cmd_dload(&params);
		
        if (!strcmp(params[1], "LOAD"))
		{
			cmd_load(&params);			
		}
    }
	
	
}

int main(int argc, char *argv[])
{
unsigned char *data, execaddress[4], stackaddress[4], lengthoffile[4];
int index, x;
unsigned char temp;
char targetFilename[12] = "combEco.exe";

    printf("\nLIBPS combine program v2.20\n"
        "Copyright (C) Barubary 1998.  All rights not expressly granted "
        "reserved.\n"
#ifdef USE_ECO2EXE_SOURCE
        "Internal ECO2EXE v0.5 (C) Silpheed/HITMEN 1998.  All rights "
        "reserved.\n"
#endif
        "\nWritten by Barubary for Gridlock\n"
        "THIS PROGRAM AND ITS DOCUMENTATION MAY BE DISTRIBUTED TO ANYONE, "
        "PROVIDED\n"
        "IT IS NOT MODIFIED NOR ANY CHARGE IS ASSOCIATED WITH "
        "DISTRIBUTION.\n"
        "NO WARRANTY.\n\n");

		 printf("eco2exe v0.01 Andrew Kieschnick <andrewk@mail.utexas.edu>\n");
		printf("exefixup v0.02 Andrew Kieschnick <andrewk@mail.utexas.edu>\n");

		printf("\n\n        ###### YAREXE ######\n\n");
		 printf("YarEXE Hacked together by gwald @xpcoin\n\n");
		  printf("Syntax: yarEXEC <scriptfile>  [optional: libps.exe file]\n"
            "You must have LIBPS.EXE in the local folder or the root folder of the current drive ie C:\\ D:\\ etc \n\nOutput files is psx.exe = exefixup file which is ready to run! - if it didn't create it, something went wrong in combine or eco2exe\n  auto file (Net Yaroze script) must have the last line as:  go\nFile combEco.exe is a temporary file which is the siocons script combined file and ecoff to exe applied\n\n");
			
		printf("\nCopyright is of the respective owners, wherever they are! We thank you!\n\n");
		 		 
    if (!argv[1]  )
    {
        return 1;
    }

    LibPS_fileHnd = fopen("libps.psx", "rb");
	if (!LibPS_fileHnd)
        LibPS_fileHnd = fopen("\\libps.psx", "rb");    
    if (!LibPS_fileHnd)
        LibPS_fileHnd = fopen("libps.exe", "rb");
	if (!LibPS_fileHnd)
        LibPS_fileHnd = fopen("\\libps.exe", "rb");    
    if (!LibPS_fileHnd)
        LibPS_fileHnd = fopen("libps.bin", "rb");
	if (!LibPS_fileHnd)
        LibPS_fileHnd = fopen("\\libps.bin", "rb");
    if (!LibPS_fileHnd && argv[2])
        LibPS_fileHnd = fopen(argv[2], "rb");
    if (!LibPS_fileHnd)
    {
        printf("Could not find libps.psx .exe .bin in current dir or \\ \n");
        return 1;
    }
    length3 = file_length(LibPS_fileHnd);

    // Check LIBPS format
    if (length3 == 399360)
        printf("\n\n\n\n\n\n ------------------------ RUNNING -----------------\n\nUsing PS-X EXE format LIBPS\n");
    else if (length3 == 397312)
        printf("\n\n\n\n\n\n ------------------------ RUNNING -----------------\n\nUsing raw binary format LIBPS\n");
    else
    {
        printf("LIBPS.EXE is corrupt!\n");
        return 1;
    }
 

    handle1 = fopen(argv[1], "rb");
    if (!handle1)
    {
        printf("Could not find input file %s\n", argv[1]);
        return 1;
    }

    target_fileHnd = fopen(targetFilename, "w+b");
    if (!target_fileHnd)
    {
        printf("Could not open output file %s\n", argv[2]);
        return 1;
    }

    temphandle = fopen("COMBINE.TMP", "w+b");
    if (!temphandle)
    {
        printf("Could not open temporary output file COMBINE.TMP\n");
        return 1;
    }

    // Allocate memory
    memspace = malloc(2 * 1024 * 1024 + 2048);
    filebuffer = malloc(2 * 1024 * 1024 + 2048);
    if (!memspace || !filebuffer)
    {
        printf("Not enough memory to build image (need 4 megs)\n");
        return 1;
    }
    memset(memspace, 0, 2 * 1024 * 1024 + 2048);

    // Put LIBPS in first.
    // Skip EXE header, if any
    if (length3 == 399360)
        fseek(LibPS_fileHnd, 0x800, SEEK_SET);
    MemoryWriteFile(LibPS_fileHnd, 0x00010000, 397312);

    // Read script into RAM
    length1 = file_length(handle1);
    script = malloc(length1 + 1);
    if (!script)
    {
        printf("ERROR: Out of memory trying to read script\n");
        return 1;
    }
    readfile(handle1, script, length1);
    script[length1] = 0;

    for (index = 0, x = 0; index < length1; index++)
    {
        temp = script[index];
        if ((temp == 0x0D) || (temp == 0x0A))
        {
            scriptstring[x] = 0;
            x = 0;
            parse(scriptstring);
            continue;
        }
        if (!temp) continue;
        scriptstring[x++] = temp;
    }
    // If for some reason there is a string without a return:
    if (x)
    {
        scriptstring[x] = temp;
        parse(scriptstring);
    }

	if(foundYarExe == 0)
		 {
        printf("\n ERROR: auto file incorrect, the word local must be before load \n IE: local load main.exe\n");
        return 1;
		}
		
    memspace += 0xF800;
    memset(memspace, 0, 2048);
    memcpy(memspace, "PS-X EXE", 8);
    strcpy(memspace + 0x4C, "COMBINE version 1.00");
    *((unsigned long *) (memspace + 0x10)) = exec_address;
    *((unsigned long *) (memspace + 0x18)) = 0x80010000;
    *((unsigned long *) (memspace + 0x1C)) = memorysize - 0x10000;
    *((unsigned long *) (memspace + 0x30)) = stack_address;
    // Write target to disk
    fwrite(memspace, 1, memorysize - 0xF800, target_fileHnd);
    printf("EXE size: %lu bytes\n ---- Combine2 & eco2exe finished successfully -------\n", memorysize - 0xF800);
    // Delete temp file
	fclose(target_fileHnd);
    unlink("COMBINE.TMP");
	 

	printf("\nexefixup filename=%s \n",  targetFilename);
	
	exeFixUp_main( targetFilename);
    return 0;
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
 
void eco2exe_main(char *filename)
{
  FILE *ecoff;
  FILE *exe;
  unsigned char data[4];
   int i,j,k=2;
  unsigned int header_data[14];
  unsigned int size;
  unsigned int address;
  unsigned int pc0;
  unsigned char patch=1; /* 0=don't, 1=do */

  printf("\neco2exe v0.01 Andrew Kieschnick <andrewk@mail.utexas.edu>\n\n\n");
  
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

		
  ecoff=fopen(filename, "rb");

  if (!ecoff)
    {
      printf("100 ERROR: Can't open %s\n",filename);
      exit(-1);
    }

  //strcat(filename, ".exe"); /* output filename is same as input but with .exe appended */


  exe=fopen("COMBINE.TMP", "wb");

  if (!exe)
    {
      printf("2 ERROR: Can't open %s\n",filename);
      exit(-1);
    }

  for(i=0;i<2;i++)
    fscanf(ecoff, "%c", &data[i]);
  
  if ((data[0]!=0x62)||(data[1]!=0x01))
    {
      printf("3 ERROR: Not a MIPSEL-LE binary!\n");
      exit(-1);
    }
  
  fseek(ecoff, 0x24, SEEK_SET);

  for(i=0;i<4;i++)
    fscanf(ecoff, "%c", &data[i]);
  
  header_data[0]=char2int(data);

  fseek(ecoff, 0x48, SEEK_SET);

  for(i=0;i<4;i++)
    fscanf(ecoff, "%c", &data[i]);
  
  header_data[1]=char2int(data);

  for(i=0;i<4;i++)
    {
      fseek(ecoff, 0x54+i*0x28, SEEK_SET);
      for(j=0;j<4;j++)
	fscanf(ecoff, "%c", &data[j]);
      header_data[k]=char2int(data);
      k++;
      fseek(ecoff, 0x54+i*0x28+0x08, SEEK_SET);
      for(j=0;j<4;j++)
	fscanf(ecoff, "%c", &data[j]);
      header_data[k]=char2int(data);
      k++;
      for(j=0;j<4;j++)
	fscanf(ecoff, "%c", &data[j]);
      header_data[k]=char2int(data);
      k++;
    }
    
  size=(header_data[11]-header_data[2])+header_data[12];
  if (patch)
    {
      pc0=header_data[2]+size;
      size+=16;
    }
  else
    pc0=header_data[0];

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
  int2char(size, data);
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

  if(patch)
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
 

void exeFixUp_main(char *filenameEXEC)
{
  FILE *exeHnd;
  FILE *outHnder;
  unsigned char dataEXEC[8];
  // char filename[256];
  int i;
  unsigned int headerDATA[12];
  unsigned int sizeEXEC;
  unsigned int padsizeEXE;
  int psxExeCreated = 0;
  //char yesno='Z';

  printf("exefixup v0.02 Andrew Kieschnick <andrewk@mail.utexas.edu>\n\nfilename = %s\n\n", filenameEXEC);
  unlink("psx.exe"); // psx.exe must be deleted first!
  
  //if (argc!=2)
   // usage();

  //strncpy(filenameEXEC,argv[1],256);

  exeHnd=fopen(filenameEXEC, "rb");

  if (!exeHnd)
    {
      printf("ERROR: Can't open %s\n",filenameEXEC);
      exit(-1);
    }

  for(i=0;i<8;i++)
    fscanf(exeHnd, "%c", &dataEXEC[i]);
  dataEXEC[8]=0;

  if (strncmp(dataEXEC, "PS-X EXE", 8))
    {
      printf("ERROR: Not a PS-X EXE file\n");
      exit(-1);
    }
  
  for(i=0;i<12;i++)
    {
      fscanf(exeHnd, "%c", &dataEXEC[0]);
      fscanf(exeHnd, "%c", &dataEXEC[1]);
      fscanf(exeHnd, "%c", &dataEXEC[2]);
      fscanf(exeHnd, "%c", &dataEXEC[3]);
      headerDATA[i]=char2intExec(dataEXEC);
    }

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

  
  /*
  
  fseek(exe, 0, SEEK_END);
  
  size=ftell(exe)-2048;

  padsize=2048-(size%2048);

  
  
  */
  
  
  
  
  fseek(exeHnd, 0, SEEK_END);
  
  sizeEXEC=ftell(exeHnd)-2048;
printf("sizeEXEC\t %d\n", sizeEXEC);


  padsizeEXE=2048-(sizeEXEC%2048);
printf("padsizeEXE\t %d\n", padsizeEXE);
  
  
  if (padsizeEXE!=2048)
    {
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
		  exit(-1);
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
	else 
		printf("padsizeEXE already 2048 \n");
  
 // yesno='Z';
  
  if (sizeEXEC!=headerDATA[5])
    {
      printf("2 Fixing: EXE header t_size does not match filesize-2048\n");
      printf("EXE header:\t 0x%.8x bytes\n", headerDATA[5]);
      printf("filesize-2048:\t 0x%.8x bytes\n", sizeEXEC);
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
		  exit(-1);
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
	else 
		printf("sizeEXEC already equal to headerDATA[5] \n");
	
	printf("exeFixUp_main finished\n");
	
  fclose(exeHnd);
  
  if(psxExeCreated>0)
  {
	  	
	printf("%s deleted\n", filenameEXEC);	
	  unlink(filenameEXEC);
  }
 else
 {
	
	  
	 int ret = rename(filenameEXEC, "psx.exe");
	 printf("%s renamed to psx.exe\n", filenameEXEC);	
	 
	   if(ret == 0) 
	   {
		  printf("File renamed successfully");
	   }
	   else 
	   {
		  printf("Error: unable to rename the file");
	   }
 }
 
 printf("\n\n\n #### YarEXE builder v2.31 4 July 2016 ###\n");
 printf("\n Finished correctly\n");
 
}


  
  

  



