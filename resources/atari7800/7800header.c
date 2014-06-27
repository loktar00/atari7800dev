#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#ifdef _WIN32
  # include <io.h>
  # include <fcntl.h>
  #define SET_BINARY_MODE(handle) _setmode(_fileno(handle), _O_BINARY)
#else
  #define SET_BINARY_MODE(handle) ((void)0)
#endif

//	7800header - a simple app to generate a a78 header.
//			Michael Saarna (aka RevEng@AtariAge)

#define HEADER_VERSION_INFO "7800header 0.2"

void usage(char *binaryname);
uint32_t phtole32(uint32_t value);
void loadfile(char *filename);
void report(void);

struct header7800 {

	unsigned char version;		//0

	char console[16];		//1-16

	char gamename[32];		//17-48

	unsigned char romsize1;		//49
	unsigned char romsize2;		//50
	unsigned char romsize3;		//51
	unsigned char romsize4;		//52

	unsigned char carttype1;	//53
	unsigned char carttype2;	//54

	unsigned char controller1;	//55
	unsigned char controller2;	//56

	unsigned char tvformat;		//57
	unsigned char saveperipheral;	//58

	unsigned char unused1[4];	//59-62

	unsigned char xm;		//63
	
	unsigned char unused2[36];	//64-99

	char headertext[28];		//100-127
	} myheader;


int main(int argc, char **argv)
{

	fprintf(stderr,"\n%s %s %s\n",HEADER_VERSION_INFO,__DATE__,__TIME__);

	uint32_t gamesize = phtole32(32768);

	// *************************** our 128 byte header...

	assert(sizeof(myheader)==128);

	// *************************** Give our header some default values...

	// a78 header format version...
	myheader.version = 1;

	strncpy(myheader.console,"ATARI7800        ",16);
	strncpy(myheader.gamename,"My Game                          ",32);

	myheader.romsize1 = (gamesize>>24) & 0xff;
	myheader.romsize2 = (gamesize>>16) & 0xff;
	myheader.romsize3 = (gamesize>>8)  & 0xff;
	myheader.romsize4 = (gamesize>>0)  & 0xff;

	// bit 0    = pokey cart
	// bit 1    = supergame bank switched
	// bit 2    = supergame ram at $4000
	// bit 3    = rom at $4000
	// bit 4    = bank 6 at $4000
	// bit 5    = supergame banked ram
	// bit 8-15 = special
	myheader.carttype1 = 0;
	myheader.carttype2 = 0;

	// controller 0=none, 1=joystick, 2=light gun
	myheader.controller1 = 1;
	myheader.controller2 = 1;

	// tv format 0=NTSC 1=PAL
	myheader.tvformat = 0;

	// save peripheral 0=none, 1=hsc, 2=savekey/avox
	myheader.saveperipheral = 0;

	memset(myheader.unused1,0,4);

	myheader.xm = 0;

	memset(myheader.unused2,0,36);
	
	// some expected static text...
	strncpy(myheader.headertext,"ACTUAL CART DATA STARTS HERE",28);

	// *************************** change any header values specified as arguments

	int c;
	FILE *in;
	int hsize;
	opterr = 0;
	while ((c = getopt (argc, argv, "r:n:s:c:t:j:l:f:e:p:x")) != -1)
	{
		switch (c)
		{
			case 'r': // read an existing file and report
				in=fopen(optarg,"r");
				if(in==NULL)
				{
					fprintf(stderr,"ERROR - couldn't open file %s\n",optarg);
					exit(1);
				}
				hsize=fread(&myheader,1,128,in);
				if(hsize<128)
				{
					fprintf(stderr,"ERROR - couldn't read 128 bytes from file %s\n",optarg);
					exit(1);
				}
				report();
				exit(0);
			case 'n': // NAME
				//format has 32 spaces, to ensure we get space-padding and no \0 terminator...
				snprintf(myheader.gamename,32,"%s                               ",optarg); 
				break;
			case 's': // SIZE
				gamesize=phtole32(atol(optarg)*1024);
				myheader.romsize1 = (gamesize>>24) & 0xff;
				myheader.romsize2 = (gamesize>>16) & 0xff;
				myheader.romsize3 = (gamesize>>8)  & 0xff;
				myheader.romsize4 = (gamesize>>0)  & 0xff;
				break;
			case 'c': // CART type
				if (strcmp(optarg,"pokey")==0)
					myheader.carttype2 = myheader.carttype2 | 1;
				else if (strcmp(optarg,"supergame")==0)
					myheader.carttype2 = myheader.carttype2 | 2;
				else if (strcmp(optarg,"supergameram")==0)
					myheader.carttype2 = myheader.carttype2 | 2 | 4;
				else if (strcmp(optarg,"supergamebankram")==0)
					myheader.carttype2 = myheader.carttype2 | 2 | 4 | 32;
				else if (strcmp(optarg,"romat4k")==0)
					myheader.carttype2 = myheader.carttype2 | 8;
				else if (strcmp(optarg,"bank6at4k")==0)
					myheader.carttype2 = myheader.carttype2 | 16;
				else if (strcmp(optarg,"absolute")==0)
					myheader.carttype1 = myheader.carttype1 | 1;
				else if (strcmp(optarg,"activision")==0)
					myheader.carttype1 = myheader.carttype1 | 2;
				else
				{
					fprintf(stderr,"ERROR - unrecognized CART type %s\n",optarg);
					exit(1);
				}
				break;
			case 'j': // JOYSTICK
				if (strcmp(optarg,"1")==0)
					myheader.controller1 = 1;
				else if (strcmp(optarg,"2")==0)
					myheader.controller2 = 1;
				break;
			case 'l': // LIGHTGUN
				if (strcmp(optarg,"1")==0)
					myheader.controller1 = 2;
				else if (strcmp(optarg,"2")==0)
					myheader.controller2 = 2;
				break;
			case 'e': // EMPTY controller
				if (strcmp(optarg,"1")==0)
					myheader.controller1 = 0;
				else if (strcmp(optarg,"2")==0)
					myheader.controller2 = 0;
				break;
			case 'f': // load options from configuration file
				loadfile(optarg);
				break;
			case 't': // TV format
				if (strcmp(optarg,"ntsc")==0)
					myheader.tvformat = 0;
				else if (strcmp(optarg,"pal")==0)
					myheader.tvformat = 1;
				break;
			case 'p': // save PERIPHERAL
				// save peripheral 0=none, 1=hsc, 2=savekey/avox
				if (strcmp(optarg,"none")==0)
					myheader.saveperipheral = 0;
				if (strcmp(optarg,"hsc")==0)
					myheader.saveperipheral = 1;
				if (strcmp(optarg,"savekey")==0)
					myheader.saveperipheral = 2;
				break;
			case 'x': // XM attachment
				myheader.xm = 0;
				break;
			default:
				usage(argv[0]);
				exit(1);
           	}
	}

	SET_BINARY_MODE(stdin);
	SET_BINARY_MODE(stdout);

	// *************************** output the header to stdout...
	fwrite(&myheader,128,1,stdout);

	// *************************** write any remaining bytes waiting in stdin to stdout...
	int t;
	while((t=fgetc(stdin))!=EOF)
	 fputc(t,stdout);
	exit(0);
}

void loadfile(char *filename)
{
	char buffer[200];
	FILE *in;
	int n,t;
	uint32_t gamesize;
	in=fopen(filename,"r");
	if(in==NULL)
	{
		fprintf(stderr,"error: couldn't open '%s'\n",filename);
		exit(1);
	}
	fprintf(stderr,"  opened parameter file %s\n",filename);
	while(fgets(buffer, sizeof(buffer),in) != NULL)
	{
		if(strncmp(buffer,"size",4)==0)
		{
			for(t=4;(buffer[t]==' ')&&(t<strlen(buffer));t++) ;
			gamesize=phtole32(atol(buffer+t)*1024);
			myheader.romsize1 = (gamesize>>24) & 0xff;
			myheader.romsize2 = (gamesize>>16) & 0xff;
			myheader.romsize3 = (gamesize>>8)  & 0xff;
			myheader.romsize4 = (gamesize>>0)  & 0xff;
			fprintf(stderr,"  set rom size: %dk\n",gamesize/1024);
			continue;
		}
		else if(strncmp(buffer,"cart",4)==0)
		{
			for(t=4;(buffer[t]==' ')&&(t<strlen(buffer));t++) ;
			if (strncmp(buffer+t,"pokey",5)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 1;
				fprintf(stderr,"  set cart option: pokey\n");
			}
			else if (strncmp(buffer+t,"supergamebankram",16)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 2 | 4 | 32;
				fprintf(stderr,"  set cart option: supergame+bank ram\n");
			}

			else if (strncmp(buffer+t,"supergameram",12)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 2 | 4;
				fprintf(stderr,"  set cart option: supergame+ram\n");
			}
			else if (strncmp(buffer+t,"supergame",9)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 2;
				fprintf(stderr,"  set cart option: supergame\n");
			}
			else if (strncmp(buffer+t,"romat4k",7)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 8;
				fprintf(stderr,"  set cart option: rom at 4k\n");
			}
			else if (strncmp(buffer+t,"bank6at4k",9)==0)
			{
				myheader.carttype2 = myheader.carttype2 | 16;
				fprintf(stderr,"  set cart option: bank6 at 4k\n");
			}
			else if (strncmp(buffer+t,"absolute",8)==0)
			{
				myheader.carttype1 = myheader.carttype1 | 1;
				fprintf(stderr,"  set cart option: absolute\n");
			}
			else if (strncmp(buffer+t,"activision",10)==0)
			{
				myheader.carttype1 = myheader.carttype1 | 2;
				fprintf(stderr,"  set cart option: activision\n");
			}

			continue;
		}
		else if(strncmp(buffer,"tv",2)==0)
		{
			for(t=4;(buffer[t]==' ')&&(t<strlen(buffer));t++) ;
			if (strncmp(buffer+t,"pal",3)==0)
			{
				myheader.tvformat = 1;
				fprintf(stderr,"  set tv option: pal\n");
			}
			else if (strncmp(buffer+t,"ntsc",4)==0)
			{
				myheader.tvformat = 0;
				fprintf(stderr,"  set tv option: ntsc\n");
			}
			continue;
		}
		else if(strncmp(buffer,"save",4)==0)
		{
			for(t=4;(buffer[t]==' ')&&(t<strlen(buffer));t++) ;
			if (strncmp(buffer+t,"savekey",7)==0)
			{
				myheader.saveperipheral = 2;
				fprintf(stderr,"  set save peripheral: savekey\n");
			}
			else if (strncmp(buffer+t,"hsc",3)==0)
			{
				myheader.saveperipheral = 1;
				fprintf(stderr,"  set save peripheral: hsc\n");
			}
			continue;
		}

	}
	fclose(in);
}

void usage(char *binaryname)
{
	fprintf(stderr,"Usage:\n\n");
	fprintf(stderr,"\"%s [options]\", where options are zero or more of the following... \n\n",binaryname);
	fprintf(stderr,"\t[-n GAMENAME]\n\t\t...set the game name.\n\n");
	fprintf(stderr,"\t[-r filename.a78]\n\t\t...report on the a78 header contents.\n\n");
	fprintf(stderr,"\t[-s ROMSIZE]\n\t\t...set the rom size in kilobytes.\n\n");
	fprintf(stderr,"\t[-c pokey | supergame | supergameram | supergamebankram\n");
	fprintf(stderr,"\t    romat4k | bank6at4k | absolute | activision]\n\t\t...set special cart format\n\n");
	fprintf(stderr, "\t[-j1] [-j2] [-l1] [-l2] [-e1] [-e2]\n\t\t...set p1 or p2 control as joystick, lightgun, or empty\n\n"); 
	fprintf(stderr,"\t[-t ntsc | pal]\n\t\t...set console tv format.\n\n");
	fprintf(stderr,"\t[-p none | hsc | savekey]\n\t\t...set attached save peripheral.\n\n");
	fprintf(stderr,"\t[-x]\n\t\t...set XM as attached.\n\n");
	fprintf(stderr,"\t[-f]\n\t\t...use parameter file for options.\n\n");
}

uint32_t phtole32(uint32_t value)
{
	// Our portable endian conversion routine.
	// Sadly, it won't work on the mixed-endian PDP-11. 

	union
	{
		uint8_t c[4];
		uint32_t i;
	} u;
	u.i = 1;
	if (u.c[0]==1) //leave the value unchanged if we're little-endian
		return(value);

	//we're big endian. we need to swap all 4 bytes
	value=((value&0xff000000)>>24) | 
		((value&0x00ff0000)>>8) | 
		((value&0x0000ff00)<<8) | 
		((value&0x000000ff)<<24);
	return(value);
}

void report(void)
{

	fprintf(stderr,"\n7800 A78 Header Report...\n\n");
	fprintf(stderr,"    header version     : %d\n",myheader.version);
	fprintf(stderr,"    embedded game name : %s\n",myheader.gamename);
	fprintf(stderr,"    rom size           : %d\n",	(myheader.romsize4)|
							(myheader.romsize3<<8)|
							(myheader.romsize2<<16)|
							(myheader.romsize1<<24));
	fprintf(stderr,"    cart format        : ");
	if((myheader.carttype2 == 0) && (myheader.carttype2 == 0))
		fprintf(stderr,"Non-Banked ");
	if((myheader.carttype2 & 1) > 0)
		fprintf(stderr,"Pokey ");
	if((myheader.carttype2 & 2) > 0)
		fprintf(stderr,"SuperGame ");
	if((myheader.carttype2 & 4) > 0)
		fprintf(stderr,"SGRAM ");
	if((myheader.carttype2 & 32) > 0 )
		fprintf(stderr,"SGBankRAM ");
	if((myheader.carttype2 & 8) > 0 )
		fprintf(stderr,"ROM@4k ");
	if((myheader.carttype1 & 1) > 0 )
		fprintf(stderr,"Absolute ");
	if((myheader.carttype1 & 2) > 0 )
		fprintf(stderr,"Activision ");
	fprintf(stderr,"\n");

	fprintf(stderr,"    controller 1       : ");
	if(myheader.controller1 == 0 )
		fprintf(stderr,"None ");
	if(myheader.controller1 == 1 )
		fprintf(stderr,"Joystick ");
	if(myheader.controller1 == 2 )
		fprintf(stderr,"Light Gun ");
	fprintf(stderr,"\n");

	fprintf(stderr,"    controller 2       : ");
	if(myheader.controller2 == 0 )
		fprintf(stderr,"None ");
	if(myheader.controller2 == 1 )
		fprintf(stderr,"Joystick ");
	if(myheader.controller2 == 2 )
		fprintf(stderr,"Light Gun ");
	fprintf(stderr,"\n");

	fprintf(stderr,"    save peripheral    : ");
	if(myheader.saveperipheral == 0 )
		fprintf(stderr,"None ");
	if(myheader.saveperipheral == 1 )
		fprintf(stderr,"HSC ");
	if(myheader.saveperipheral == 2 )
		fprintf(stderr,"SaveKey/AtariVOX ");
	fprintf(stderr,"\n");

	fprintf(stderr,"    xm/xboard          : ");
	if(myheader.xm  > 0 )
		fprintf(stderr,"enabled ");
	else 
		fprintf(stderr,"not enabled ");
	fprintf(stderr,"\n");

	fprintf(stderr,"    tv format          : ");
	if(myheader.tvformat == 0 )
		fprintf(stderr,"NTSC");
	else if(myheader.tvformat == 1 )
		fprintf(stderr,"PAL");
	else
		fprintf(stderr,"unknown");
	fprintf(stderr,"\n");


	fprintf(stderr,"\n");
}
