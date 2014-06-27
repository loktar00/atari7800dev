#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libgen.h>
#include "statements.h"
#include "keywords.h"

extern char stdoutfilename[256];
extern FILE *stdoutfilepointer;


#define PNG_DEBUG 3
#include <png.h>


void currdir_foundmsg(char *foundfile)
{
	char sBuffer[200];
	sprintf(sBuffer,"User-defined '%s' found in the current directory",foundfile);
	prinfo(sBuffer);
}

void doreboot()
{
	printf("	JMP START\n");
}


int linenum()
{
	// returns current line number in source
	return line;
}

void jsr(char *location)
{
	printf(" jsr %s\n",location);
	return;
}

int switchjoy(char *input_source)
{
	// place joystick/console switch reading code inline instead of as a subroutine
	// standard routines needed for pretty much all games
	// read switches, joysticks now compiler generated (more efficient)

	// returns 0 if we need beq/bne, 1 if bvc/bvs, and 2 if bpl/bmi

	//  invalidate_Areg()  // do we need this?

	if (!strncmp(input_source,"switchreset\0",11))
	{
		printf(" lda #1\n");
		printf(" bit SWCHB\n");
		return 0;
	}
	if (!strncmp(input_source,"switchselect\0",12))
	{
		printf(" lda #2\n");
		printf(" bit SWCHB\n");
		return 0;
	}
	if (!strncmp(input_source,"switchleftb\0",11))
	{
		//     printf(" lda #$40\n");
		printf(" bit SWCHB\n");
		return 1;
	}
	if (!strncmp(input_source,"switchrightb\0",12))
	{
		//     printf(" lda #$80\n");
		printf(" bit SWCHB\n");
		return 2;
	}
	if (!strncmp(input_source,"switchpause\0",11))
	{
		printf(" lda #8\n");
		printf(" bit SWCHB\n");
		return 0;
	}
	if (!strncmp(input_source,"joy0up\0",6))
	{
		printf(" lda #$10\n");
		printf(" bit SWCHA\n");
		return 0;
	}
	if (!strncmp(input_source,"joy0down\0",8))
	{
		printf(" lda #$20\n");
		printf(" bit SWCHA\n");
		return 0;
	}
	if (!strncmp(input_source,"joy0left\0",8))
	{
		//     printf(" lda #$40\n");
		printf(" bit SWCHA\n");
		return 1;
	}
	if (!strncmp(input_source,"joy0right\0",9))
	{
	//     printf(" lda #$80\n");
		printf(" bit SWCHA\n");
		return 2;
	}
	if (!strncmp(input_source,"joy1up\0",6))
	{
		printf(" lda #1\n");
		printf(" bit SWCHA\n");
		return 0;
	}
	if (!strncmp(input_source,"joy1down\0",8))
	{
		printf(" lda #2\n");
		printf(" bit SWCHA\n");
		return 0;
	}
	if (!strncmp(input_source,"joy1left\0",8))
	{
		printf(" lda #4\n");
		printf(" bit SWCHA\n");
		return 0;
	}
	if (!strncmp(input_source,"joy1right\0",9))
	{
		printf(" lda #8\n");
		printf(" bit SWCHA\n");
		return 0;
	}

	if (!strncmp(input_source,"joy0fire0\0",9))
	{
		printf(" bit INPT0\n");
		return 3;
	}
	if (!strncmp(input_source,"joy0fire1\0",9))
	{
		printf(" bit sINPT1\n");
		return 3;
	}
	if (!strncmp(input_source,"joy1fire0\0",9))
	{
		printf(" bit INPT2\n");
		return 3;
	}
	if (!strncmp(input_source,"joy1fire1\0",9))
	{
		printf(" bit sINPT3\n");
		return 3;
	}

	if (!strncmp(input_source,"joy0fire\0",8))
	{
		//     printf(" lda #$80\n");
		//printf(" bit INPT4\n");
		printf(" bit sINPT1\n");
		return 3;
	}
	if (!strncmp(input_source,"joy1fire\0",8))
	{
		//     printf(" lda #$80\n");
		//printf(" bit INPT5\n");
		printf(" bit sINPT3\n");
		return 3;
	}

	prerror("invalid console switch/controller reference.");
}

float immed_fixpoint(char *fixpointval)
{
	int i=findpoint(fixpointval);
	if (i==50) return 0;  // failsafe
	char decimalpart[50];
	fixpointval[i]='\0';
	sprintf(decimalpart,"0.%s",fixpointval+i+1);
	return atof(decimalpart);
}

int findpoint(char *item) // determine if fixed point var
{
	int i;
	for (i=0; i<50; ++i)
	{
		if (item[i] == '\0') return 50;
		if (item[i] == '.') return i;
	}
	return i;
}

void freemem(char **statement)
{
	int i;
	for (i=0; i<200; ++i) free(statement[i]);
	free(statement);
}

void printfrac(char *item)
{ // prints the fractional part of a declared 8.8 fixpoint variable
	char getvar[50];
	int i;
	for (i=0; i<numfixpoint88; ++i)
	{
		strcpy(getvar,fixpoint88[1][i]);
		if (!strcmp(fixpoint88[0][i],item))
		{
			printf("%s\n",getvar);
			return;
		}
	}
	// must be immediate value
	if (findpoint(item) < 50)
		printf("#%d\n",(int)(immed_fixpoint(item)*256.0));
	else printf("#0\n");
}

int isfixpoint(char *item)
{
	// determines if a variable is fixed point, and if so, what kind
	int i;
	removeCR(item);
	for (i=0; i<numfixpoint88; ++i)
		if (!strcmp(item, fixpoint88[0][i])) return 8;
	for (i=0; i<numfixpoint44; ++i)
		if (!strcmp(item, fixpoint44[0][i])) return 4;
	if (findpoint(item) < 50) return 12;
	return 0;
}

void set_romsize(char *size)
{
	if (!strncmp(size,"32k\0",3))
	{
		strcpy(redefined_variables[numredefvars++],"ROM32k = 1");
		append_a78info("size 32");
	}

	else if (!strncmp(size,"48k\0",3))
	{
		strcpy(redefined_variables[numredefvars++],"ROM48k = 1");
		append_a78info("size 48");
	}
	else if (!strncmp(size,"128k\0",4))
	{
		strcpy(redefined_variables[numredefvars++],"ROM128K = 1");
		strcpy(redefined_variables[numredefvars++],"bankswitchmode = 8");
		bankcount = 8;
		currentbank = 0 ;
		append_a78info("size 128k");
		if(strncmp(size+4,"BANKRAM",7)==0)
		{
			append_a78info("cart supergamebankram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
			strcpy(redefined_variables[numredefvars++],"BANKRAM = 1");
		}
		else if(strncmp(size+4,"RAM",3)==0)
		{
			append_a78info("cart supergameram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
		}
		else
			append_a78info("cart supergame");
	}
	else if (!strncmp(size,"256k\0",4))
	{
		strcpy(redefined_variables[numredefvars++],"ROM256K = 1");
		strcpy(redefined_variables[numredefvars++],"bankswitchmode = 16");
		bankcount = 16;
		currentbank = 0 ;
		append_a78info("size 256k");
		if(strncmp(size+4,"BANKRAM",7)==0)
		{
			append_a78info("cart supergamebankram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
			strcpy(redefined_variables[numredefvars++],"BANKRAM = 1");
		}
		else if(strncmp(size+4,"RAM",3)==0)
		{
			append_a78info("cart supergameram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
		}
		else
			append_a78info("cart supergame");
	}
	else if (!strncmp(size,"512k\0",4))
	{
		strcpy(redefined_variables[numredefvars++],"ROM512K = 1");
		strcpy(redefined_variables[numredefvars++],"bankswitchmode = 32");
		bankcount = 32;
		currentbank = 0 ;
		append_a78info("size 512k");
		if(strncmp(size+4,"BANKRAM",7)==0)
		{
			append_a78info("cart supergamebankram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
			strcpy(redefined_variables[numredefvars++],"BANKRAM = 1");
		}
		else if(strncmp(size+4,"RAM",3)==0)
		{
			append_a78info("cart supergameram");
			strcpy(redefined_variables[numredefvars++],"SGRAM = 1");
		}
		else
			append_a78info("cart supergame");
	}
	else
	{
		prerror("unsupported ROM size.");
	}
}

void bank(char **statement)
{
	//  1   2 
	// bank #

	removeCR(statement[2]);

	int requestedbank=atoi(statement[2])-1;

	if(bankcount==0)
		prerror("bank statement encountered, but non-banking rom format was selected.");

	if(requestedbank==0) //we don't care about the first "bank" definition
		return; 

	if(requestedbank>=bankcount) 
		prerror("bank statement exceedes total banks in rom format.");

	// 1. dump and clear any incgraphics from the current bank before changing
	barf_graphic_file();

	// 2.issue ORG,RORG
	currentbank=requestedbank;
	printf(" ORG $%04X\n",(currentbank+2)*0x4000);
	if(currentbank==(bankcount-1)) //last bank
		printf(" RORG $C000\n");
	else
		printf(" RORG $8000\n");
}

void dmahole(char **statement)
{
	//  1      2 
	// dmahole #

	FILE *holefilepointer;
	int requestedhole;

	removeCR(statement[2]);

	requestedhole=atoi(statement[2]);
	sprintf(stdoutfilename,"7800hole.%d.asm",requestedhole);
        if( (stdoutfilepointer = freopen(stdoutfilename, "w", stdout)) == NULL)
        {
		char sBuffer[200];
		sprintf(sBuffer,"couldn't create the %s file.",stdoutfilename);
                prerror(sBuffer);
        }
	
}


void characterset(char **statement)
{
	removeCR(statement[2]);
	printf("    lda #>%s\n",statement[2]);
	printf("    sta CHARBASE\n\n");

	printf("    lda #(%s_mode | %%01100000)\n",statement[2]);
	printf("    sta charactermode\n\n");

	strcpy(currentcharset,statement[2]);
}

void displaymode(char **statement)
{
	removeCR(statement[2]);
	if ( (strncmp(statement[2],"160A",4)==0) ||
	     (strncmp(statement[2],"160B",4)==0) )
	{
		if(doublewide==1)
		{
			printf("    lda #%%01010000 ;Enable DMA, mode=160x2/160x4, 2x character width\n");
		}
		else
		{
			printf("    lda #%%01000000 ;Enable DMA, mode=160x2/160x4\n");
		}
	}
	else if ( (strncmp(statement[2],"320A",4)==0) ||
	     (strncmp(statement[2],"320C",4)==0) )
	{
		if(doublewide==1)
		{
			printf("    lda #%%01010011 ;Enable DMA, mode=160x2/160x4, 2x character width\n");
		}
		else
		{
			printf("    lda #%%01000011 ;Enable DMA, mode=160x2/160x4\n");
		}
	}
	else if ( (strncmp(statement[2],"320B",4)==0) ||
	     (strncmp(statement[2],"320D",4)==0) )
	{
		if(doublewide==1)
		{
			printf("    lda #%%01010010 ;Enable DMA, mode=160x2/160x4, 2x character width\n");
		}
		else
		{
			printf("    lda #%%01000010 ;Enable DMA, mode=160x2/160x4\n");
		}
	}
	else
	{
		char sBuffer[200];
		sprintf(sBuffer,"illegal '%s' display mode specified.",statement[2]);
		prerror(sBuffer);
	}


	printf("    sta CTRL\n\n");
}

void plotsprite(char **statement)
{
        //    1          2         3    4 5   6
	//plotsprite spritename palette x y [frame]

	// temp1 = lowbyte_of_sprite (adjusted for "frame")
	// temp2 = hibyte_of_sprite
	// temp3 = palette | width
	// temp4 = x
	// temp5 = y

	if((statement[6][0]!=0)&&(statement[6][0]!=':'))
	{
		removeCR(statement[6]);

		printf("    lda #<%s\n",statement[2]);

		printf("    ldy "); printimmed(statement[6]); printf("%s\n",statement[6]);


		printf("      clc\n");
		printf("      beq plotspritewidthskip%d\n",templabel);
		printf("plotspritewidthloop%d\n",templabel);
		printf("      adc #%s_width\n",statement[2]);
		printf("      dey\n");
		printf("      bne plotspritewidthloop%d\n",templabel);
		printf("plotspritewidthskip%d\n",templabel);
		printf("    sta temp1\n\n");

		templabel++;
	}
	else
	{
		printf("    lda #<%s\n",statement[2]);
		printf("    sta temp1\n\n");
	}

	printf("    lda #>%s\n",statement[2]);
	printf("    sta temp2\n\n");

        if((statement[3][0]>='0')&&(statement[3][0]<='9'))
        {
		int palette;
		palette=atoi(statement[3]);
		printf("    lda #(%d|%s_width_twoscompliment)\n",palette<<5,statement[2]); 
		printf("    sta temp3\n\n");
	}
	else
        {
		printf("    lda "); printimmed(statement[3]); printf("%s\n",statement[3]);
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    ora #%s_width_twoscompliment\n",statement[2]);
		printf("    sta temp3\n\n");
	}

	printf("    lda "); printimmed(statement[4]); printf("%s\n",statement[4]);
	printf("    sta temp4\n\n");

	printf("    lda "); printimmed(statement[5]); printf("%s\n",statement[5]);
	printf("    sta temp5\n\n");

	printf("    lda #(%s_mode|%%01000000)\n",statement[2]);
	printf("    sta temp6\n\n");

	jsr("plotsprite");

}

int inlinealphadata(char **statement)
{
	// utility function. takes statement[2] inline quoted text and fixes the space characters.
	// it then replaces the statement[2] literal string with the new data statement

       //      1          2             3            4
       //   alphadata uniquelabel graphicslabel [extrawide]
       //  'mytext strings here'

	int t,charoffset;
	int quotelen=0;

	if(currentcharset[0]=='\0')
		prerror("the characterset statement needs to precede plotchars with an inline string.");

	for(t=0;statement[2][t]!='\0';t++)
		if(statement[2][t]=='^')
			statement[2][t]=' ';
	
	printf("	JMP skipalphadata%d\n",templabel);
	printf("alphadata%d\n",templabel);

	for(t=1;(statement[2][t]!='\'')&&(statement[2][t]!='\0');t++)
	{
		charoffset=lookupcharacter(statement[2][t]);
		quotelen++;
		if(charoffset<0)
		{
			char sBuffer[200];
			sprintf(sBuffer,"plotchars character '%c' is missing from alphachars.",statement[2][t]);
			prerror(sBuffer);
		}
		if(doublewide==1)
			charoffset=charoffset*2;
		if(statement[4][0]=='e') //extrawide
		{
			quotelen++;
			charoffset=charoffset*2;
		}
		if(quotelen>32)
			prerror("greater than 32 characters used in plotchars statement.");
		printf(" .byte (<%s + $%02x)\n",currentcharset,charoffset);
		if(statement[4][0]=='e') //extrawide
		{
			printf(" .byte (<%s + $%02x)\n",currentcharset,charoffset+1);
		}
	}
	printf("skipalphadata%d\n",templabel);
	sprintf(statement[2],"alphadata%d",templabel);
	templabel++;
	return(quotelen);
}

void plotchars(char **statement)
{
	//            1        2          3        4     5           6
	//        plotchars text_data palette screen_x screen_y output_text_width


	// temp1 = lowbyte of character map
	// temp2 = hibyte of character map
	// temp3 = palette | width
	// temp4 = x
	// temp5 = y

	int paletteval;
	int widthval;
	int autotextwidth=0;

	if(statement[2][0]=='\'')
	{
		autotextwidth=inlinealphadata(statement);
	}

	printf("    lda #<%s\n",statement[2]);
	printf("    sta temp1\n\n");

	printf("    lda #>%s\n",statement[2]);
	printf("    sta temp2\n\n");

	removeCR(statement[6]);

	//check if width is a decimal constant. If so, we can avoid opcodes and cycles.
	if (autotextwidth>0)
	{
		widthval=autotextwidth;
		widthval=((0-widthval)&31);
		printf("    lda #%d ; width in two's complement\n",widthval);
	}
	else if ( ((statement[6][0]>='0') && (statement[6][0]<='9')) || (statement[6][0]=='$') || (statement[6][0]=='%') )
	{
		widthval=myatoi(statement[6]);
		if((widthval>32)||(widthval==0))
		{
			char sBuffer[200];
			sprintf(sBuffer,"width value must range from 1 to 32. '%s' was used.",statement[6]);
			prerror(sBuffer);
		}

		widthval=((0-widthval)&31);
		printf("    lda #%d ; width in two's complement\n",widthval);
	}
	else
	{
		//width
		printf("    lda #0\n");
		printf("    sec\n");
		printf("    sbc "); printimmed(statement[6]); printf("%s\n",statement[6]);
		printf("    and #%%00011111\n");
	}


	//check if palette is  a decimal constant. If so, we can avoid opcodes and cycles.
	if ( ((statement[3][0]>='0')&&(statement[3][0]<='9')) || (statement[3][0]=='$') || (statement[3][0]=='%'))
	{
		paletteval=myatoi(statement[3]);
		if(paletteval>7)
		{
			prerror("palette value must range from 0 to 7.");
		}

		printf("    ora #%d ; palette left shifted 5 bits\n",paletteval<<5);
		printf("    sta temp3\n");
	}
	else
	{
		//palette
		printf("    sta temp3\n");
		printf("    lda "); printimmed(statement[3]); printf("%s\n",statement[3]);
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    ora temp3\n");
		printf("    sta temp3\n");
	}

	printf("    lda "); printimmed(statement[4]); printf("%s\n",statement[4]);
	printf("    sta temp4\n\n");

	printf("    lda "); printimmed(statement[5]); printf("%s\n",statement[5]);
	printf("    sta temp5\n\n");

	jsr("plotcharacters");


}

void plotvalue(char **statement)
{
	//            1        2          3          4             5             6      7          8
	//        plotvalue digit_gfx palette variable/data number_of_digits screen_x screen_y [extrawide]


	// asm sub arguments:
	// temp1=lo charactermap
	// temp2=hi charactermap
	// temp3=palette | width byte
	// temp4=x
	// temp5=y
	// temp6=number of digits
	// temp7=lo variable
	// temp8=hi variable

	int paletteval;
	int widthval;

	printf("    lda #<%s\n",statement[2]);
	printf("    sta temp1\n\n");

	printf("    lda #>%s\n",statement[2]);
	printf("    sta temp2\n\n");

	printf("    lda charactermode\n");
	printf("    sta temp9\n");
	printf("    lda #(%s_mode | %%01100000)\n",statement[2]);
	printf("    sta charactermode\n");

	removeCR(statement[7]);
	removeCR(statement[8]);

	//check if width is a decimal constant. If so, we can avoid opcodes and cycles.
	if ( ((statement[5][0]>='0')&&(statement[5][0]<='9')) || (statement[5][0]=='$') || (statement[5][0]=='%'))
	{
		widthval=myatoi(statement[5]);
		if (strncmp(statement[8],"extrawide",9)==0)
			widthval=widthval*2;
		if((widthval>32)||(widthval==0))
		{
			char sBuffer[200];
			sprintf(sBuffer,"width value must range from 1 to 32. '%s' was used.",statement[5]);
			prerror(sBuffer);
		}
		widthval=((0-widthval)&31);
		printf("    lda #%d ; width in two's complement\n",widthval);
	}
	else //width is a label of some sort...
	{
		printf("    lda #0\n");
		printf("    sec\n");
		printf("    sbc "); printimmed(statement[5]); printf("%s\n",statement[5]);
		printf("    and #%%00011111\n");
	}


	//check if palette is a decimal constant. If so, we can avoid opcodes and cycles.
	if ( ((statement[3][0]>='0')&&(statement[3][0]<='9')) || (statement[3][0]=='$') || (statement[3][0]=='%'))
	{
		paletteval=myatoi(statement[3]);
		if(paletteval>7)
		{
			prerror("palette value must range from 0 to 7.");
		}

		printf("    ora #%d ; palette left shifted 5 bits\n",paletteval<<5);
		printf("    sta temp3\n");
	}
	else
	{
		//palette
		printf("    sta temp3\n");
		printf("    lda "); printimmed(statement[3]); printf("%s\n",statement[3]);
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    ora temp3\n");
		printf("    sta temp3\n");
	}

	removeCR(statement[7]);

	printf("    lda "); printimmed(statement[6]); printf("%s\n",statement[6]);
	printf("    sta temp4\n\n");

	printf("    lda "); printimmed(statement[7]); printf("%s\n",statement[7]);
	printf("    sta temp5\n\n");

	printf("    lda "); printimmed(statement[5]); printf("%s\n",statement[5]);
	printf("    sta temp6\n\n");

	printf("    lda #<%s\n",statement[4]);
	printf("    sta temp7\n\n");

	printf("    lda #>%s\n",statement[4]);
	printf("    sta temp8\n\n");

	if (strncmp(statement[8],"extrawide",9)==0)
	{
		jsr("plotvalueextra");
	}
	else
	{
		jsr("plotvalue");
	}

	printf("    lda temp9\n");
	printf("    sta charactermode\n");

}



void plotmap(char **statement)
{

	//         1      2          3        4        5          6             7       [      8                9                 10       ]
	//     plotmap map_data    palette screen_x screen_y screen_width screen_height [text_map_x_offset text_map_y_offset text_map_width]

	// temp1 = lowbyte of map data
	// temp2 = hibyte of map data
	// temp3 = palette | width
	// temp4 = screen_x
	// temp5 = screen_y
	// ...

	int paletteval;
	int widthval;


	//check if palette is decimal constant. If so, we can avoid opcodes and cycles.
	if ( ((statement[6][0]>='0')&&(statement[6][0]<='9')) || (statement[6][0]=='$') || (statement[6][0]=='%') )
	{
		widthval=myatoi(statement[6]);
		if((widthval>32)||(widthval==0))
		{
			char sBuffer[200];
			sprintf(sBuffer,"width value must range from 1 to 32. '%s' was used.",statement[6]);
			prerror(sBuffer);
		}

		widthval=((0-widthval)&31);
		printf("    lda #%d ; width in two's complement\n",widthval);
	}
	else
	{
		//width
		printf("    lda #0\n");
		printf("    sec\n");
		printf("    sbc "); printimmed(statement[6]); printf("%s\n",statement[6]);
		printf("    and #%%00011111\n");
	}

	printf("    sta temp3\n");

	//check if palette is  a decimal constant. If so, we can avoid opcodes and cycles.
	if ( ((statement[3][0]>='0')&&(statement[3][0]<='9')) || (statement[3][0]=='$') || (statement[3][0]=='%') )
	{
		paletteval=myatoi(statement[3]);
		if(paletteval>7)
		{
			prerror("palette value must range from 0 to 7.");
		}

		printf("    ora #%d ; palette left shifted 5 bits\n",paletteval<<5);
		printf("    sta temp3\n");
	}
	else
	{
		//palette
		printf("    sta temp3\n");
		printf("    lda "); printimmed(statement[3]); printf("%s\n",statement[3]);
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    asl\n");
		printf("    ora temp3\n");
		printf("    sta temp3\n");
	}




	printf("    lda "); printimmed(statement[4]); printf("%s\n",statement[4]);
	printf("    sta temp4\n\n");

	printf("    lda "); printimmed(statement[5]); printf("%s\n",statement[5]);
	printf("    sta temp5\n\n");

	//         1      2          3        4        5          6             7       [      8                9                 10       ]
	//     plotmap map_data    palette screen_x screen_y screen_width screen_height [text_map_x_offset text_map_y_offset text_map_width]
	// temp1 = lowbyte of map data
	// temp2 = hibyte of map data
	// temp3 = palette | width
	// temp4 = screen_x
	// temp5 = screen_y
	// temp6 = screen_height
	// ...

	printf("    lda "); printimmed(statement[7]); printf("%s\n",statement[7]);
	printf("    sta temp6\n");

	if ((statement[8][0]==0)&&(statement[8][0]!=':'))
	{

		printf("    lda #<%s\n",statement[2]);
		printf("    sta temp1\n\n");
		printf("    lda #>%s\n",statement[2]);
		printf("    sta temp2\n\n");

		printf("plotcharactersloop%d\n",templabel);
		//the map is the same size as the character block we're displaying...
		jsr("plotcharacters");
		printf("    clc\n");
		printf("    lda "); printimmed(statement[6]); printf("%s\n",statement[6]);
		printf("    adc temp1\n");
		printf("    sta temp1\n");
		printf("    lda #0\n");
		printf("    adc temp2\n");
		printf("    sta temp2\n");
		printf("    inc temp5\n");
		printf("    dec temp6\n");
		printf("    bne plotcharactersloop%d\n",templabel);
		templabel++;
	}
	else
	{
		//the map is a larger size compared to the character block we're displaying...


		printf("    lda #<%s\n",statement[2]);
		printf("    sta temp1\n\n");
		printf("    lda #>%s\n",statement[2]);
		printf("    sta temp2\n\n");

		// multiply text_map_y_offset*text_map_width
		printf("    ldy "); printimmed(statement[9]); printf("%s\n",statement[9]);
		printf("    cpy #0\n");
		printf("    beq skipmapyadjust%d\n",templabel);
		printf("    lda "); printimmed(statement[10]); printf("%s\n",statement[10]);
		printf("    jsr mul16\n");
		printf("    ;result is in A, temp1 contains overflow\n");
		printf("    sta temp2\n");

		printf("    clc\n");
		printf("    lda #<%s\n",statement[2]);
		printf("    adc temp1\n",statement[2]);
		printf("    sta temp1\n",statement[2]);
		printf("    lda #>%s\n",statement[2]);
		printf("    adc temp2\n",statement[2]);
		printf("    sta temp2\n",statement[2]);

		printf("skipmapyadjust%d\n",templabel);
		templabel++;

		//adjust for the x-offset...
		printf("    clc\n");
		printf("    lda "); printimmed(statement[8]); printf("%s\n",statement[8]);
		printf("    adc temp1\n");
		printf("    sta temp1\n");
		printf("    lda #0\n");
		printf("    adc temp2\n");
		printf("    sta temp2\n");

		printf("plotcharactersloop%d\n",templabel);

		jsr("plotcharacters");

		//add the map width to get to the next row...
		printf("    clc\n");
		printf("    lda "); printimmed(statement[10]); printf("%s\n",statement[10]);
		printf("    adc temp1\n");
		printf("    sta temp1\n");
		printf("    lda #0\n");
		printf("    adc temp2\n");
		printf("    sta temp2\n");

		printf("    inc temp5\n"); //new row on the screen

		//reduce the line count, and continue if we're non-zero...
		printf("    dec temp6\n");
		printf("    bne plotcharactersloop%d\n",templabel);
		templabel++;
	}

}

void domemcpy(char **statement)
{
        //   1      2     3   4
	// memcpy dest  src  count

	int count,page;

	removeCR(statement[4]);
	count=myatoi(statement[4]);

	if(count>255) 
	{
		// chip away at count a page at a time.
		// this may be a bit less rom-efficient for large ranges compared to a "proper"
		// indirect addressing solution, but this should be a lot faster and smaller
		// for ranges that aren't a lot bigger than a couple of pages.
		page=0;
		while(count>255)
		{
			printf(" ldy #0\n");
			printf("memcpyloop%d\n",templabel);
			printf(" lda %s+%d,y\n",statement[3],page*256);
			printf(" sta %s+%d,y\n",statement[2],page*256);
			printf(" dey\n");
			printf(" bne memcpyloop%d\n",templabel);
			templabel++;
			page=page+1;
			count=count-256;
		}
		if(count>0)
		{
			printf(" ldy #%d\n",count);
			printf("memcpyloop%d\n",templabel);
			printf(" lda %s-1+%d,y\n",statement[3],page*256);
			printf(" sta %s-1+%d,y\n",statement[2],page*256);
			printf(" dey\n");
			printf(" bne memcpyloop%d\n",templabel);
			templabel++;
		}

	}

	if(count==0)
		return;
	if(count==256)
	{
		printf(" ldy #0\n");
		printf("memcpyloop%d\n",templabel);
		printf(" lda %s,y\n",statement[3]);
		printf(" sta %s,y\n",statement[2]);
		printf(" dey\n");
		printf(" bne memcpyloop%d\n",templabel);
		templabel++;
	}
	else
	{
		printf(" ldy #%d\n",count);
		printf("memcpyloop%d\n",templabel);
		printf(" lda %s-1,y\n",statement[3]);
		printf(" sta %s-1,y\n",statement[2]);
		printf(" dey\n");
		printf(" bne memcpyloop%d\n",templabel);
		templabel++;
	}

}

void savemultiplicationtable(char *tablename,int width, int height)
{
	int t;
	//check so we don't save the same table twice...
	for(t=0;t<multtableindex;t++)
	{
		if(strcmp(tablename,multtablename[t])==0)
			return;
	}
	//save multiplication table info. we'll later need to barf it.
	strncpy(multtablename[multtableindex],tablename,49);
	multtablewidth[multtableindex]=width;
	multtableheight[multtableindex]=height;
	multtableindex=multtableindex+1;
}

void barfmultiplicationtables(void)
{
	int s,t,incvalue;
	for(t=0;t<multtableindex;t++)
	{
		printf("%s_mult_lo\n",multtablename[t]);
		incvalue=0;
		for(s=0;s<multtableheight[t];s++)
		{
			printf("  .byte <(%s+%d)\n",multtablename[t],incvalue);
			incvalue=incvalue+multtablewidth[t];
		}
		printf("%s_mult_hi\n",multtablename[t]);
		incvalue=0;
		for(s=0;s<multtableheight[t];s++)
		{
			printf("  .byte >(%s+%d)\n",multtablename[t],incvalue);
			incvalue=incvalue+multtablewidth[t];
		}
	}

}

void peekchar(char **statement)
{
        //   4      5   6   7 8 9  10 11 12    13 14     15
	// peekchar (   mem , x ,  y  ,  width ,  height )

	int width,height;

	if(statement[15][0]!=')')
		prerror("bad argument count for peekchar");

	width=myatoi(statement[12]);
	height=myatoi(statement[14]);

	if((width*height<1)||(width*height>65536))
		prerror("bad width or height with peekchar");

	savemultiplicationtable(statement[6],width,height);

	printf("    ldy "); printimmed(statement[10]); printf("%s\n",statement[10]);
	printf("    lda %s_mult_lo,y\n",statement[6]);
	printf("    sta temp1\n");
	printf("    lda %s_mult_hi,y\n",statement[6]);
	printf("    sta temp2\n");
	printf("    ldy "); printimmed(statement[8]); printf("%s\n",statement[8]);
	printf("    lda (temp1),y\n");
	strcpy(Areg,"invalid");

}

void pokechar(char **statement)
{
        //   1      2   3  4 5     6      7
	// pokechar mem x  y width height value

	int width,height;

	removeCR(statement[7]);

	width=myatoi(statement[5]);
	height=myatoi(statement[6]);

	if((width*height<1)||(width*height>65536))
		prerror("bad width or height with pokechar");

	savemultiplicationtable(statement[2],width,height);

	printf("    ldy "); printimmed(statement[4]); printf("%s\n",statement[4]);
	printf("    lda %s_mult_lo,y\n",statement[2]);
	printf("    sta temp1\n");
	printf("    lda %s_mult_hi,y\n",statement[2]);
	printf("    sta temp2\n");
	printf("    ldy "); printimmed(statement[3]); printf("%s\n",statement[3]);
	printf("    lda "); printimmed(statement[7]); printf("%s\n",statement[7]);
	printf("    sta (temp1),y\n");

	strcpy(Areg,"invalid");
	
}

int  myatoi(char *numstring)
{
	if(numstring[0]=='\0')
	{
		prwarn("bad non-variable value");
		return(-1); 
	}
	//check if its a plain decimal argument...
	if((numstring[0]>='0')&&(numstring[0]<='9'))
		return(atoi(numstring));
	if((numstring[0]=='$')&&(numstring[1]!='\0'))
		return((int)strtol(numstring+1,NULL,16));
	if((numstring[0]=='%')&&(numstring[1]!='\0'))
		return((int)strtol(numstring+1,NULL,2));
	prwarn("bad non-variable value");
	return(-1); 
}

void tsound(char **statement)
{
        //   1      2     3   4
	// tsound channel , [freq] , [waveform] , [volume]

	int nextindex;
	int channel=myatoi(statement[2]);

	if((channel<0)||(channel>1))        
		prerror("illegal channel for tsound");
	nextindex=4;	
	if(statement[nextindex][0]==',')
		nextindex=nextindex+1;
	else
	{
		printf(" lda "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
		printf(" sta AUDF%d\n",channel);
		nextindex=nextindex+2;
	}
	if(statement[nextindex][0]==',')
		nextindex=nextindex+1;
	else
	{
		printf(" lda "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
		printf(" sta AUDC%d\n",channel);
		nextindex=nextindex+2;
	}
	removeCR(statement[nextindex]);
	if(statement[nextindex][0]=='\0')
		return;
	if(statement[nextindex][0]==',')
		nextindex=nextindex+1;
	else
	{
		printf(" lda "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
		printf(" sta AUDV%d\n",channel);
		nextindex=nextindex+2;
	}
}

void psound(char **statement)
{
        //   1      2     3   4      
	// psound channel , [freq] [ , waveform , volume ]

	int nextindex;
	int channel=255;

	if ( ((statement[2][0]>='0') && (statement[2][0]<='9')) || (statement[2][0]=='$') || (statement[2][0]=='%'))
	{
		channel=myatoi(statement[2]);
		if((channel<0)||(channel>3))        
			prerror("illegal channel for psound");
	}

	nextindex=4;	
	if(statement[nextindex][0]==',')
		nextindex=nextindex+1;
	else
	{
		if(channel!=255)
			printf(" ldy #%d\n",(channel*2)+0); //FREQ
		else
		{
			printf(" lda "); printimmed(statement[2]); printf("%s\n",statement[2]);
			printf(" asl\n");
			printf(" tay\n");
		}
		printf(" lda "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
		printf(" sta (pokeybase),y\n");
		nextindex=nextindex+2;
	}

	removeCR(statement[nextindex]);
	removeCR(statement[nextindex+1]);
	removeCR(statement[nextindex+2]);

	if( (statement[nextindex]==0) || (statement[nextindex+1]==0) )
		prerror("malformed psound statement");

	if(statement[nextindex][0]==',')
		return;
	else
	{
	
		printf(" iny\n"); //WAVE/CTRL

		if ( ( ((statement[nextindex][0]>='0') && (statement[nextindex][0]<='9')) || (statement[nextindex][0]=='$') || (statement[nextindex][0]=='%') ) &&
		   ( ((statement[nextindex+2][0]>='0') && (statement[nextindex+2][0]<='9')) || (statement[nextindex+2][0]=='$') || (statement[nextindex+2][0]=='%') ) )
		{
			int wave,volume;

			wave=myatoi(statement[nextindex]);
			volume=myatoi(statement[nextindex+2]);
			printf(" lda #%d\n",(wave*16)+volume);
			printf(" sta (pokeybase),y\n");
	
		}
		else
		{

			printf(" lda "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
			printf(" asl\n");
			printf(" asl\n");
			printf(" asl\n");
			printf(" asl\n");
			printf(" clc\n");

			nextindex=nextindex+1;
			removeCR(statement[nextindex]);
			if(statement[nextindex][0]=='\0')
				prerror("malformed psound. volume is required with waveform.");
			nextindex=nextindex+1;
			removeCR(statement[nextindex]);
			if(statement[nextindex][0]=='\0')
				prerror("malformed psound. volume is required with waveform.");

			printf(" adc "); printimmed(statement[nextindex]); printf("%s\n",statement[nextindex]);
			printf(" sta (pokeybase),y\n");
			return;
		}
	}

}

void playsfx(char **statement)
{
        //   1       2     3  
	// playsfx data [pitch]

	removeCR(statement[2]);
	removeCR(statement[3]);

	printf("    lda #<%s\n",statement[2]);
	printf("    sta temp1\n");
	printf("    lda #>%s\n",statement[2]);
	printf("    sta temp2\n");

	if((statement[3][0]!=0)&&(statement[3][0]!=':'))
	{
		printf("    lda "); printimmed(statement[3]); printf("%s\n",statement[3]);
		printf("    sta temp3\n");
	}
	else
	{
		printf("    lda #0\n");
		printf("    sta temp3\n");
		
	}

	printf("    jsr schedulesfx\n");
}



char *ourbasename(char *fullpath)
{
	//replacement of basename() that works with Unix or Windows style directory slashes

	int t;

	for(t=strlen(fullpath)-2;t>=0;t--)
	{
		if((fullpath[t]=='/')||(fullpath[t]=='\\'))
			return(fullpath+t+1);
	}
	return(fullpath);
	
}

unsigned char graphiccolorindex[16];
unsigned char graphic7800colors[16];
unsigned char graphiccolormode;
void add_graphic(char **statement)
{
	int s,t,width;

	//incgraphic filename [mode] [colorindex0] [colorindex1] ...

	//setup our defaults...
	for(t=0; t<16; t++)
	{
		graphiccolorindex[t]=t;
		graphic7800colors[t]=0;
	}

	graphiccolormode=MODE160A;

	if((graphicsdatawidth[0]==0)&&(dmaplain=0))
	{
		//this is the first graphics statement encountered. initialize everything.
		memset(graphicsdata,0,16*256*100*sizeof(char));
		memset(graphicslabels,0,16*256*80*sizeof(char));
		memset(graphicsinfo,0,16*256*sizeof(char));
		memset(graphicsmode,0,16*256*sizeof(char));
	}

	removeCR(statement[2]); //remove CR from the filename, if present

	// ensure no bad characters in the filename...
	if ( (strchr(statement[2],'-')!=NULL) ||
	     (strchr(statement[2],'+')!=NULL) ||
	     ((statement[2][0]>='0')&&(statement[2][0]<='9')) )
	{
		char sBuffer[200];
		sprintf(sBuffer,"problem characters in filename '%s'.",statement[2]);
		prerror(sBuffer);
	}

 #ifdef WIN32
	// convert any Unix slashes to Windows.
	for(t=0;statement[2][t]!='\0';t++)
	{
		if(statement[2][t]=='/')
		{
			statement[2][t]='\\';
		}
	}
 #else
	// convert any Win slashes to Unix.
	for(t=0;statement[2][t]!='\0';t++)
	{
		if(statement[2][t]=='\\')
		{
			statement[2][t]='/';
		}
	}
 #endif

	//check if the user is setting the color mode...
	if((statement[3][0]!=0)&&(statement[3][0]!=':'))
	{
		removeCR(statement[3]);
		if (strcasecmp(statement[3],"160A")==0)
		{
			graphiccolormode=MODE160A;
		}
		else if (strcasecmp(statement[3],"160B")==0)
		{
			graphiccolormode=MODE160B;
			// We need to reorder the default color indexes. For 160B the 7800 makes color
			// indexes 0, 4, 8, and 12 transparent, but A 16 color PNG will likely have
			// non-transparent colors in slots 4, 8, and 12, so we move these to the end.
			s=1;
			for(t=1; t<12; t++)
			{
				if(s%4==0)
					s=s+1;
				graphiccolorindex[t]=s;
				s=s+1;
			}
			graphiccolorindex[12]=15;
			graphiccolorindex[13]=4;
			graphiccolorindex[14]=8;
			graphiccolorindex[15]=12;
		}
		else if (strcasecmp(statement[3],"320A")==0)
		{
			graphiccolormode=MODE320A;
		}
		else if (strcasecmp(statement[3],"320B")==0)
		{
			graphiccolormode=MODE320B;
		}
		else if (strcasecmp(statement[3],"320C")==0)
		{
			graphiccolormode=MODE320C;
		}
		else if (strcasecmp(statement[3],"320D")==0)
		{
			graphiccolormode=MODE320D;
		}
	}




	width=getgraphicwidth(statement[2]);
        if (width>256)
	{
		char sBuffer[200];
		sprintf(sBuffer,"'%s' is wider than 256 bytes.",statement[2]);
		prerror(sBuffer);
	}



	if ( (graphicsdatawidth[dmaplain]+width) > 256)
	{
		//the next graphic will overflow the DMA plain, so we skip to the next one...
		dmaplain=dmaplain+1;
		graphicsdatawidth[dmaplain]=0;
	}



	//our label is based on the filename...
	snprintf(graphicslabels[dmaplain][graphicsdatawidth[dmaplain]],80,"%s",ourbasename(statement[2]));



	//but remove the extension...
	for(t=(strlen(graphicslabels[dmaplain][graphicsdatawidth[dmaplain]])-3); t>0; t--)
		if (strcasecmp(graphicslabels[dmaplain][graphicsdatawidth[dmaplain]]+t,".png")==0)
			graphicslabels[dmaplain][graphicsdatawidth[dmaplain]][t]=0;

	//chech if the user is reordering the color indexes of the imported graphic
	if(statement[4][0]!=0)
	{
		for(t=0; t<16; t++)
		{
			if(statement[t+4][0]==':')
				break;
			if (statement[t+4][0]!=0)
			{
				removeCR(statement[t+4]);
				if ((statement[t+4][0]!=0)&&(statement[t+4][0]!='\0'))
					graphiccolorindex[t]=myatoi(statement[t+4]);
			}
		}

	}

	// Now read the png into memory...
	incgraphic(statement[2]);

}

int getgraphicwidth(char *file_name)
{
	int x, y;

	int width, height, rowbytes;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;


	//******* read file

	unsigned char header[8];        // 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
	{
		char sBuffer[200];
		sprintf(sBuffer,"couldn't open png file '%s' for reading.", file_name);
		prerror(sBuffer);
	}
	if(fread(header, 1, 8, fp)==0)
	{
		char sBuffer[200];
		sprintf(sBuffer,"couldn't read from png file '%s'.", file_name);
		prerror(sBuffer);
	}
	if (png_sig_cmp(header, 0, 8))
	{
		char sBuffer[200];
		sprintf(sBuffer,"'%s' doesn't appear to be a png file.", file_name);
		prerror(sBuffer);
	}
	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		char sBuffer[200];
		sprintf(sBuffer,"when preparing '%s', png_create_read_struct failed.",file_name);
		prerror(sBuffer);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		char sBuffer[200];
		sprintf(sBuffer,"when preparing '%s', png_create_info_struct failed.",file_name);
		prerror(sBuffer);
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		prerror("libpng error during init_io.");
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr,info_ptr);

	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

	fclose(fp);

	if(graphiccolormode==MODE160A)
		width=width/4; //4 pixels per byte
	if(graphiccolormode==MODE160B)
		width=width/2; //2 pixels per byte
	if(graphiccolormode==MODE320A)
		width=width/8; //8 pixels per byte
	if(graphiccolormode==MODE320B)
		width=width/2; //2 pixels per byte
	if(graphiccolormode==MODE320C)
		width=width/2; //2 pixels per byte
	if(graphiccolormode==MODE320D)
		width=width/8; //8 pixels per byte

	return(width);
}

void newblock()
{
	//skips to the next graphic area
	dmaplain=dmaplain+1;
	graphicsdatawidth[dmaplain]=0;
}

void incgraphic(char *file_name)
{
	int x, y;

	int width, height, rowbytes;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	int num_palette;
	png_colorp palette;
	int num_trans;
	png_bytep trans;


	//******* read file

	unsigned char header[8];        // 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
	{
		char sBuffer[200];
		sprintf(sBuffer,"couldn't open png file '%s' for reading.", file_name);
		prerror(sBuffer);
	}
	if(fread(header, 1, 8, fp)==0)
	{
		char sBuffer[200];
		sprintf(sBuffer,"couldn't read from png file '%s'.", file_name);
		prerror(sBuffer);
	}
	if (png_sig_cmp(header, 0, 8))
	{
		char sBuffer[200];
		sprintf(sBuffer,"'%s' doesn't appear to be a png file.", file_name);
		prerror(sBuffer);
	}
	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		char sBuffer[200];
		sprintf(sBuffer,"when preparing '%s', png_create_read_struct failed.",file_name);
		prerror(sBuffer);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		char sBuffer[200];
		sprintf(sBuffer,"when preparing '%s', png_create_info_struct failed.",file_name);
		prerror(sBuffer);
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		prerror("libpng error during init_io.");
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);


	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr,info_ptr);
	height = png_get_image_height(png_ptr,info_ptr);
	color_type = png_get_color_type(png_ptr,info_ptr);
	bit_depth = png_get_bit_depth(png_ptr,info_ptr);


	if(bit_depth>4)
	{
		char sBuffer[200];
		sprintf(sBuffer,"the png file '%s' has too many colors.",file_name);
		prerror(sBuffer);
	}

	if(!png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans,  NULL))
		num_trans=-1;

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
        {
                double r,g,b,y,i,q,angle,colorangle;
                int s,t;

                const double convertval = 7.5 / M_PI;

                double start_angle = 0.0;

                int finalcolor;
                for(t=0; t<num_palette;t++)
                {

                        r=(double)palette[t].red/255.0;
                        g=(double)palette[t].green/255.0;
                        b=(double)palette[t].blue/255.0;

			if(num_trans>0)
			{
                        	for(s=0;s<num_trans;s++)
					if(trans[s]==t)
					{
						break;
					}
			}

                        //convert RGB to YIQ
                        y = 0.299900 * r + 0.587000 * g + 0.114000 * b;
                        i = 0.595716 * r - 0.274453 * g - 0.321264 * b;
                        q = 0.211456 * r - 0.522591 * g + 0.311350 * b;

			//get angle of I,Q to provide colorburst phase (cartesian->polar)
			colorangle=atan(q/i);
			//account for i being negative, otherwise (-q/-i)==(q/i), (-q/i)==(q/-i), ...
			if(i<0)
				colorangle=colorangle+M_PI;
			colorangle=(colorangle*convertval)+3.50;

			//clamps
			if(colorangle<1)
				colorangle=1;
			if(colorangle>16)
				colorangle=16;
			if((r==g)&&(g==b))
				colorangle=0;
			if(y>1.0)
				y=1.0;

			y=y*13; //lum=15 is too hot

			finalcolor=((int)(colorangle*16))&0xF0;
			finalcolor=finalcolor|(int)y;

			sprintf(redefined_variables[numredefvars++],"%s_color%d = $%02x",graphicslabels[dmaplain][graphicsdatawidth[dmaplain]],t,finalcolor);
			sprintf(constants[numconstants++],"%s_color%d",graphicslabels[dmaplain][graphicsdatawidth[dmaplain]],t); // record to queue


		}
	}

	//TODO: accurate warning for pixel width on non-byte boundary

	//change png to one byte per pixel, rather than tight packing...
	if(bit_depth < 8) //not really needed.
		png_set_packing(png_ptr);


	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);


	/* read in the rest of the png */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		prerror("libpng error during read_image.");
	}

	rowbytes = png_get_rowbytes(png_ptr,info_ptr);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) malloc(rowbytes);

	png_read_image(png_ptr, row_pointers);

	fclose(fp);


	//******* process file

	if ((color_type & PNG_COLOR_TYPE_PALETTE) == 0)
	{
		char sBuffer[200];
		sprintf(sBuffer,"the png file %s isn't a palette-type/indexed image.",file_name);
		prerror(sBuffer);
	}


	if((bit_depth<=2)&&(graphiccolormode==MODE160A))
	{
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/4; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=0;
		for(x=0; x<width; x=x+4)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;
			for (y=height-1; y>-1; y=y-1)
			{
				int l,val;

				png_byte* row = row_pointers[y];
				val=0;
				for(l=0; l<4; l++)
				{
					val=val<<2;
					val=val|graphiccolorindex[row[x+l]];
				}
				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
		for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}

	if((bit_depth<=4)&&(graphiccolormode==MODE160B))
	{
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/2; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=128;
		for(x=0; x<width; x=x+2)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;
			for (y=height-1; y>-1; y=y-1)
			{
				int l,val,workval;

				png_byte* row = row_pointers[y];

				// official recipe for each 160B byte...
				// take two pixels, p1, p2
				// 4 bits of p1 go 3276
				// 4 bits of p2 go 1054

				val=0;
				workval=graphiccolorindex[row[x]];
				if((workval&8)>0)
					val=val|8;
				if((workval&4)>0)
					val=val|4;
				if((workval&2)>0)
					val=val|128;
				if((workval&1)>0)
					val=val|64;

				workval=graphiccolorindex[row[x+1]];
				if((workval&8)>0)
					val=val|2;
				if((workval&4)>0)
					val=val|1;
				if((workval&2)>0)
					val=val|32;
				if((workval&1)>0)
					val=val|16;

				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
		for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}

	if((bit_depth==1)&&(graphiccolormode==MODE320A))
	{
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/8; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=0;
		for(x=0; x<width; x=x+8)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;
			for (y=height-1; y>-1; y=y-1)
			{
				int l,val;

				png_byte* row = row_pointers[y];
				val=0;
				for(l=0; l<8; l++)
				{
					val=val<<1;
					val=val|graphiccolorindex[row[x+l]];
				}
				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
		for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}


	if((bit_depth<=2)&&(graphiccolormode==MODE320B))
	{
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/4; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=128;
		for(x=0; x<width; x=x+4)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;
			for (y=height-1; y>-1; y=y-1)
			{
				int l,val,workval;
				png_byte* row = row_pointers[y];

				// official recipe for each 320B byte...
				// take four pixels, p1, p2, p3, p4
				// 2 bits of p1 go to 73
				// 2 bits of p2 go to 62
				// 2 bits of p3 go to 51
				// 2 bits of p4 go to 40

				val=0;
				workval=graphiccolorindex[row[x]];
				if((workval&2)>0)
					val=val|128;
				if((workval&1)>0)
					val=val|8;

				workval=graphiccolorindex[row[x+1]];
				if((workval&2)>0)
					val=val|64;
				if((workval&1)>0)
					val=val|4;

				workval=graphiccolorindex[row[x+2]];
				if((workval&2)>0)
					val=val|32;
				if((workval&1)>0)
					val=val|2;

				workval=graphiccolorindex[row[x+3]];
				if((workval&2)>0)
					val=val|16;
				if((workval&1)>0)
					val=val|1;

				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
		for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}


	if((bit_depth<=2)&&(graphiccolormode==MODE320C))
	{
		int supresserror=0;
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/4; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=128;
		for(x=0; x<width; x=x+4)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;

			for (y=height-1; y>-1; y=y-1)
			{
				int l,val,workval1,workval2;

				png_byte* row = row_pointers[y];

				// official recipe for each 320C byte...
				// take four pixels, p1, p2, p3, p4
				// where p1 must be the same palette as p2,
				// and   p3 must be the same palette as p4.
				// p1 goes to 7
				// p2 goes to 6
				// p3 goes to 5
				// p4 goes to 4
				// 2 bits of palette for p1,p2 goes to 32
				// 2 bits of palette for p3,p4 goes to 10

				val=0; 

				workval1=graphiccolorindex[row[x]]&3;
				if(workval1>0)
					val=val|128;

				workval2=graphiccolorindex[row[x+1]]&3;
				if(workval2>0)
					val=val|64;

				if( (workval1>0) && (workval2>0) && (workval1!=workval2) )
				{
					if(supresserror==0)
					{
						char sBuffer[400];
						sprintf(sBuffer,"The png file %s doesn't appear to meet the requirements for 320C\n",file_name);
						strcat(sBuffer,"         Each pair of pixels must use the same color if they aren't turned off.\n");
						strcat(sBuffer,"         The colors for this sprite will be converted incorrectly.");
						prwarn(sBuffer);
						supresserror=1;
					}
					val=val|(workval1<<2);
				}
				else
					val=val|(workval1<<2)|(workval2<<2);

				workval1=graphiccolorindex[row[x+2]]&3;
				if(workval1>0)
					val=val|32;

				workval2=graphiccolorindex[row[x+3]]&3;
				if(workval2>0)
					val=val|16;

				if( (workval1>0) && (workval2>0) && (workval1!=workval2) )
				{
					if(supresserror==0)
					{
						char sBuffer[400];
						sprintf(sBuffer,"The png file %s doesn't appear to meet the requirements for 320C\n",file_name);
						strcat(sBuffer,"         Each pair of pixels must use the same color if they aren't turned off.\n");
						strcat(sBuffer,"         The colors for this sprite will be converted incorrectly.");
						prwarn(sBuffer);
						supresserror=1;
					}

					val=val|workval1;
				}
				else
					val=val|workval1|workval2;

				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
        	for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}

	if((bit_depth<=2)&&(graphiccolormode==MODE320D))
	{
		int supresserror=0;
		graphicsinfo[dmaplain][graphicsdatawidth[dmaplain]]=width/8; //image width in 6502 bytes
		graphicsmode[dmaplain][graphicsdatawidth[dmaplain]]=0;
		for(x=0; x<width; x=x+8)
		{
			graphicsdatawidth[dmaplain]=graphicsdatawidth[dmaplain]+1;

			for (y=height-1; y>-1; y=y-1)
			{
				int l,val,workval;

				png_byte* row = row_pointers[y];

				// official recipe for each 320D byte (I think) ...
				// take 8 pixels, p1, p2, p3, p4, p5, p6, p7, p8
				// where p1 color = p3, p5, p7 colors (color1/color3=on/off)
				//   and p2 color = p4, p6, p8 colors (color2/color4=on/off)

				// some error checking first...
				
				for(l=0;l<7;l=l+1)
				{
					if( (row[x+l]==row[x+l+1]) ) // not a perfect check, but what a weird scheme. :|
					{
						char sBuffer[400];
						sprintf(sBuffer,"The png file %s doesn't appear to meet the requirements for 320D\n",file_name);
						strcat(sBuffer,"         All even pixels must be color 0 or color 2.\n");
						strcat(sBuffer,"         All odd pixels  must be color 1 or color 3.\n");
						strcat(sBuffer,"         The colors for this sprite will be converted incorrectly.");
						prwarn(sBuffer);

						break;
					}
				}

				val=0;
				for(l=0;l<8;l++)
				{
					val=val<<1;
					workval=graphiccolorindex[row[x+l]];

					//the color varies due to the bit position, so we just encode it as on or off.
					if(workval>1)
						val=val|1;
				}
				graphicsdata[dmaplain][graphicsdatawidth[dmaplain]-1][y]=val;
			}
		}
		for(y=0;y<height;y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return;
	}


	char sBuffer[200];
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	sprintf(sBuffer,"the bitdepth '%d' of '%s' doesn't match the requested display mode.",bit_depth,file_name);
	prerror(sBuffer);
	return;
}


void add_includes(char *myinclude)
{
	if (includesfile_already_done) 
		prwarn("include ignored (includes should typically precede other statements)");
	strcat(user_includes,myinclude);
}

void add_inline(char *myinclude)
{
	printf(" include %s\n",myinclude);
}

void init_includes(char *path)
{
	if (path) strcpy(includespath,path);
	else includespath[0]='\0';
	user_includes[0]='\0';
}

void barf_graphic_file(void)
{
	int s,t,currentplain;
	FILE *out;
        int linewidth;
	char sBuffer[200];
	int ADDRBASE; 
	int ABADDRBASE;

	int BANKSTART,REALSTART,DMASIZE;

	if(zoneheight==8)
	{
		DMASIZE=4096;
		if(bankcount==0)
		{
			BANKSTART=0xF000;
		}
		else if(currentbank==(bankcount-1))
		{
			BANKSTART=0xF000;
			REALSTART=0x8000+0x1000;
		}
		else
		{
			BANKSTART=0xB000;
			REALSTART=0x8000+0x1000;
		}
	}
	if(zoneheight==16)
	{
		DMASIZE=8192;
		if(bankcount==0)
		{
			BANKSTART=0xE000;
		}
		else if(currentbank==(bankcount-1))
		{
			BANKSTART=0xE000;
			REALSTART=0x8000;
		}
		else
		{
			BANKSTART=0xA000;
			REALSTART=0x8000;
		}
	}

	ADDRBASE=BANKSTART-(dmaplain*DMASIZE);

	if(bankcount == 0)
	{
		if((graphicsdatawidth[dmaplain]>0)||(dmaplain>0)) //calculate from graphics area...
			printf(" echo \" \",[($%04X - gameend)]d , \"bytes of ROM space left in the main area.\"\n",ADDRBASE);
		else
			printf(" echo \" \",[($%04X - gameend)]d , \"bytes of ROM space left in the main area.\"\n",0xF000);
	}

	else if ((currentbank+1)==bankcount)
	{

		if((graphicsdatawidth[dmaplain]>0)||(dmaplain>0)) //calculate from graphics area...
			printf(" echo \" \",[($%04X - .)]d , \"bytes of ROM space left in the main area of bank %d.\"\n",ADDRBASE,currentbank+1);
		else
			printf(" echo \" \",[($%04X - .)]d , \"bytes of ROM space left in the main area of bank %d.\"\n",0xEFFF,currentbank+1);
	}
		
	else
	{
		if((graphicsdatawidth[dmaplain]>0)||(dmaplain>0)) //calculate from graphics area...
			printf(" echo \" \",[($%04X - .)]d , \"bytes of ROM space left in the main area of bank %d.\"\n",ADDRBASE,currentbank+1);
		else
			printf(" echo \" \",[($%04X - .)]d , \"bytes of ROM space left in the main area of bank %d.\"\n",0xBFFF,currentbank+1);
	}


	//if stdout is redirected, its time change it back to 7800.asm
	if(strcmp(stdoutfilename,"7800.asm")!=0)
	{
		strcpy(stdoutfilename,"7800.asm");
		if( (stdoutfilepointer = freopen(stdoutfilename, "a", stdout)) == NULL)
		{
			prerror("couldn't reopen the 7800.asm file.");
		}
	}



	if((graphicsdatawidth[dmaplain]>0)||(dmaplain>0)) //only process if the incgraphic command was encountered.
	{
		if( ((bankcount>0)&&(zoneheight==16)&&(dmaplain>1)) ||
			((bankcount>0)&&(zoneheight==8)&&(dmaplain>3)))
		{
			char sBuffer[200];
			sprintf(sBuffer,"graphics overrun in bank %d.",currentbank);
			prerror(sBuffer);
		}
		for(currentplain=0;currentplain<=dmaplain;currentplain++)
		{
			if(bankcount == 0)
				ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE);
			else 
			{
				ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE);
				ABADDRBASE=REALSTART+(currentbank*0x4000)+0x2000-((dmaplain-currentplain)*DMASIZE); 

			}
			fprintf(stderr,"\n");
			if(bankcount==0)
				sprintf(sBuffer,"GFX Block #%d starts @ $%04X",currentplain,ADDRBASE);
			else
				sprintf(sBuffer,"bank #%d, GFX Block #%d starts @ $%04X",currentbank+1,currentplain,ADDRBASE);
			prinfo(sBuffer);
			linewidth=0;
			fprintf(stderr,"       ");

			for(s=zoneheight-1;s>=0;s--)
			{
				if(bankcount==0)
					printf("\n ORG $%04X  ; *************\n",ADDRBASE);
				else
				{
					printf("\n ORG $%04X  ; *************\n",ABADDRBASE);
					printf("\n RORG $%04X ; *************\n",ADDRBASE);
				}
				
				for(t=0; t<graphicsdatawidth[currentplain]; t++)
				{
					if(graphicslabels[currentplain][t][0]!=0)
					{
						if(s==(zoneheight-1))
						{
							printf("\n%s\n       HEX ",graphicslabels[currentplain][t]);
							if((linewidth+strlen(graphicslabels[currentplain][t]))>60)
							{
								linewidth=0;
								fprintf(stderr,"\n       ");
							}
							fprintf(stderr," %s",graphicslabels[currentplain][t]);
							linewidth=linewidth+strlen(graphicslabels[currentplain][t]);
						}
						else
							printf("\n;%s\n       HEX ",graphicslabels[currentplain][t]);
					}
					printf("%02x",graphicsdata[currentplain][t][s]);
					if((t%32==31)&&((t+1)<graphicsdatawidth[currentplain]))
						printf("\n       HEX ");
				}
				printf("\n");
				ADDRBASE=ADDRBASE+256;
				if(bankcount>0)
				ABADDRBASE=ABADDRBASE+256;
			}
			fprintf(stderr,"\n");
			if(bankcount == 0)
				sprintf(sBuffer,"GFX block #%d has %d bytes left (%d x %d bytes)\n",currentplain,(256-graphicsdatawidth[currentplain])*zoneheight,(256-graphicsdatawidth[currentplain]),zoneheight);
			else
				sprintf(sBuffer,"bank #%d, GFX block #%d has %d bytes left (%d x %d bytes)\n",currentbank+1,currentplain,(256-graphicsdatawidth[currentplain])*zoneheight,(256-graphicsdatawidth[currentplain]),zoneheight);
			prinfo(sBuffer);

			// if we're in a DMA hole, report on it and barf any code that was saved for it...
			if ( (currentplain<dmaplain) || ((currentplain==dmaplain) && (bankcount>0) && (currentbank+1<bankcount)) )
			{
				if(bankcount == 0)
					ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE)+(DMASIZE/2);
				else 
				{
					ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE)+(DMASIZE/2);
					ABADDRBASE=REALSTART+(currentbank*0x4000)+0x2000-((dmaplain-currentplain)*DMASIZE)+(DMASIZE/2); 

				}
				fprintf(stderr,"\n");
				if(bankcount==0)
					sprintf(sBuffer,"DMA hole #%d starts @ $%04X",currentplain,ADDRBASE);
				else
					sprintf(sBuffer,"bank #%d, DMA hole #%d starts @ $%04X",currentbank+1,currentplain,ADDRBASE);
				prinfo(sBuffer);

				if(bankcount==0)
					printf("\n ORG $%04X  ; *************\n",ADDRBASE);
				else
				{
					printf("\n ORG $%04X  ; *************\n",ABADDRBASE);
					printf("\n RORG $%04X ; *************\n",ADDRBASE);
				}

				FILE *holefilepointer;
				char holefilename[256];
				sprintf(holefilename,"7800hole.%d.asm",currentplain);
				holefilepointer=fopen(holefilename,"r");
				if(holefilepointer!=NULL)
				{
					fprintf(stderr,"        DMA hole code found and imported\n");
					int c;
					while ((c=getc(holefilepointer))!=EOF)
						putchar(c);
					fclose(holefilepointer);
					remove(holefilename);
				}
				else
				{
					fprintf(stderr,"        no code defined for DMA hole\n");
				}

				if(bankcount == 0)
					ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE)+DMASIZE;
				else 
				{
					ADDRBASE=BANKSTART-((dmaplain-currentplain)*DMASIZE)+DMASIZE;
					ABADDRBASE=REALSTART+(currentbank*0x4000)+0x2000-((dmaplain-currentplain)*DMASIZE)+(DMASIZE/2); 

				}

				printf(" echo \"  \",\"  \",\"  \",\"  \",[($%04X - .)]d , \"bytes of ROM space left in DMA hole %d.\"\n",ADDRBASE,currentplain);
			}
		}

		for(currentplain=0;currentplain<=dmaplain;currentplain++)
		{
			//stick extra graphic info into variable defines
			for(t=0; t<256; t++)
			{
				if(graphicslabels[currentplain][t][0]!=0)
				{
					sprintf(redefined_variables[numredefvars++],"%s_width = $%02x",graphicslabels[currentplain][t],graphicsinfo[currentplain][t]);
					sprintf(redefined_variables[numredefvars++],"%s_width_twoscompliment = $%02x",graphicslabels[currentplain][t],(0-(graphicsinfo[currentplain][t])&31));
					sprintf(redefined_variables[numredefvars++],"%s_mode = $%02x",graphicslabels[currentplain][t],graphicsmode[currentplain][t]);
				}
			}
		}


		if(bankcount>0)
		{
			// clear out the structure for the next bank...
			memset(graphicsdata,0,16*256*100*sizeof(char));
			memset(graphicslabels,0,16*256*80*sizeof(char));
			memset(graphicsinfo,0,16*256*sizeof(char));
			memset(graphicsmode,0,16*256*sizeof(char));
			for(currentplain=0;currentplain<=dmaplain;currentplain++)
				graphicsdatawidth[dmaplain]=0;
			dmaplain=0;
		}

		fprintf(stderr,"\n");
	}
}

void create_a78info(void)
{
	FILE *out;
	out=fopen("a78info.cfg","w");
	if(out==NULL)
		prerror("couldn't create a78info.cfg parameter file.");
	fprintf(out,"### parameter file for 7800header, created by 7800basic\n");
	fclose(out);

}

void append_a78info(char *line)
{
	FILE *out;
	out=fopen("a78info.cfg","a");
	if(out==NULL)
		prerror("couldn't add to a78info.cfg parameter file.");
	fprintf(out,"%s\n",line);
	fclose(out);
}

void create_includes(char *includesfile)
{
	FILE *includesread,*includeswrite;
	char dline[500];
	char fullpath[500];
	int i;
	int writeline;
	removeCR(includesfile);
	if (includesfile_already_done) return;
	includesfile_already_done=1;
	fullpath[0]='\0';
	if (includespath)
	{
		strcpy(fullpath,includespath);
		if ((includespath[strlen(includespath)-1]=='\\') || (includespath[strlen(includespath)-1]=='/'))
			strcat(fullpath,"includes/");
		else
			strcat(fullpath,"/includes/");
	}
	strcat(fullpath,includesfile);
	if ((includesread = fopen(includesfile, "r")) == NULL) // try again in current dir
	{
		if ((includesread = fopen(fullpath, "r")) == NULL) // open file
		{
			char sBuffer[200];
			sprintf(sBuffer,"can't open '%s' for reading.",includesfile);
			prerror(sBuffer);
		}
	}
	else currdir_foundmsg(includesfile);
	if ((includeswrite = fopen("includes.7800", "w")) == NULL) // open file
	{

		prerror("can't open 'includes.7800' for writing.");
	}

	while (fgets(dline,500,includesread))
	{
		for (i=0; i<500; ++i)
		{
			if (dline[i]==';')
			{
				writeline=0;
				break;
			}
			if (dline[i]==(unsigned char)0x0A)
			{
				writeline=0;
				break;
			}
			if (dline[i]==(unsigned char)0x0D)
			{
				writeline=0;
				break;
			}
			if (dline[i]>(unsigned char)0x20)
			{
				writeline=1;
				break;
			}
			writeline=0;
		}
		if (writeline)
		{
			if (!strncasecmp(dline,"7800.asm\0",6))
				if (user_includes[0] != '\0') fprintf(includeswrite,"%s",user_includes);
			fprintf(includeswrite,"%s",dline);
		}
	}
	fclose(includesread);
	fclose(includeswrite);
}

void printindex(char *mystatement, int myindex, int indirectflag)
{
	if (myindex==0)
	{
		printimmed(mystatement);
		printf("%s\n",mystatement);
	}
	else if(indirectflag!=0)
		printf("(%s),y\n",mystatement);  // indexed with y!
	else 
		printf("%s,x\n",mystatement);  // indexed with x!
}

void loadindex(char *myindex, int indirectflag)
{
	if (strncmp(myindex,"TSX\0",3)) {
		if(indirectflag==0)
			printf("	LDX "); // get index
		else
			printf("	LDY "); // get index
		printimmed(myindex);
		printf("%s\n",myindex);
	}
}

int getindex(char *mystatement, char *myindex, int *indirectflag)
{
	int i,j,index=0;
	*indirectflag=0;
	for (i=0; i<200; ++i)
	{
		if (mystatement[i]=='\0') {i=200; break; }
		if (mystatement[i]=='[') {index=1; break; }
	}
	if ( (i<198) && (mystatement[i]=='[') && (mystatement[i+1]=='[') )
		*indirectflag=1;
	if (i<200)
	{
		if (*indirectflag==0)
		{
			strcpy(myindex,mystatement+i+1);
			myindex[strlen(myindex)-1]='\0';
		}
		else
		{
			strcpy(myindex,mystatement+i+2);
			myindex[strlen(myindex)-2]='\0';
		}
		if (myindex[strlen(myindex)-2]==']')
			myindex[strlen(myindex)-2]='\0';
		if (myindex[strlen(myindex)-1]==']')
			myindex[strlen(myindex)-1]='\0';
		for (j=i; j<200; ++j) mystatement[j]='\0';
	}
	return index;
}

int checkmul(int value)
{
	// check to see if value can be optimized to save cycles

	if (!(value %2)) return 1;  //REVENG - still faster than sub

	if (value < 11) return 1;  // always optimize these

	while (value!=1)
	{
		if (!(value%9)) value/=9;
		else if (!(value%7)) value/=7;
		else if (!(value%5)) value/=5;
		else if (!(value%3)) value/=3;
		else if (!(value%2)) value/=2;
		else break;
		if (!(value%2) && (optimization&1) != 1) break;  // do not optimize multplications
	}
	if (value == 1) return 1; else return 0;
}

int checkdiv(int value)
{
	// check to see if value is a power of two - if not, run standard div routine
	while (value!=1)
	{
		if (value%2) break;
		else value/=2;
	}
	if (value == 1) return 1; else return 0;
}


void mul(char **statement, int bits)
{
	// this will attempt to output optimized code depending on the multiplicand
	int multiplicand=atoi(statement[6]);
	int tempstorage=0;
	// we will optimize specifically for 2,3,5,7,9
	if (bits == 16)
	{
		printf("	ldx #0\n");
		printf("	stx temp1\n");
	}
	while (multiplicand != 1)
	{
		if (!(multiplicand % 9))
		{
			if (tempstorage)
			{
				strcpy(statement[4],"temp2");
				printf("	sta temp2\n");
			}
			multiplicand /= 9;
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	clc\n");
			printf("	adc ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			if (bits == 16)
			{
				printf("	tax\n");
				printf("	lda temp1\n");
				printf("	adc #0\n");
				printf("	sta temp1\n");
				printf("	txa\n");
			}
			tempstorage=1;
		}
		else if (!(multiplicand % 7))
		{
			if (tempstorage)
			{
				strcpy(statement[4],"temp2");
				printf("	sta temp2\n");
			}
			multiplicand /= 7;
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	sec\n");
			printf("	sbc ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			if (bits == 16)
			{
				printf("	tax\n");
				printf("	lda temp1\n");
				printf("	sbc #0\n");
				printf("	sta temp1\n");
				printf("	txa\n");
			}
			tempstorage=1;
		}
		else if (!(multiplicand % 5))
		{
			if (tempstorage)
			{
				strcpy(statement[4],"temp2");
				printf("	sta temp2\n");
			}
			multiplicand /= 5;
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	clc\n");
			printf("	adc ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			if (bits == 16)
			{
				printf("	tax\n");
				printf("	lda temp1\n");
				printf("	adc #0\n");
				printf("	sta temp1\n");
				printf("	txa\n");
			}
			tempstorage=1;
		}
		else if (!(multiplicand % 3))
		{
			if (tempstorage)
			{
				strcpy(statement[4],"temp2");
				printf("	sta temp2\n");
			}
			multiplicand /= 3;
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
			printf("	clc\n");
			printf("	adc ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			if (bits == 16)
			{
				printf("	tax\n");
				printf("	lda temp1\n");
				printf("	adc #0\n");
				printf("	sta temp1\n");
				printf("	txa\n");
			}
			tempstorage=1;
		}
		else if (!(multiplicand % 2))
		{
			multiplicand /= 2;
			printf("	asl\n");
			if (bits == 16) printf("  rol temp1\n");
		}
		else
		{
			printf("	LDY #%d\n",multiplicand);
			printf("	jsr mul%d\n",bits);
			prwarn("there seems to be a problem. your code may not run properly. please report seeing this message.");

		}
	}
}

void divd(char **statement, int bits)
{
	int divisor=atoi(statement[6]);
	if (bits == 16)
	{
		printf("	ldx #0\n");
		printf("	stx temp1\n");
	}
	while (divisor != 1)
	{
		if (!(divisor % 2)) // div by power of two is the only easy thing
		{
			divisor /= 2;
			printf("	lsr\n");
			if (bits == 16) printf("  rol temp1\n");  // I am not sure if this is actually correct
		}
		else
		{
			printf("	LDY #%d\n",divisor);
			printf("	jsr div%d\n",bits);
			prwarn("there seems to be a problem. your code may not run properly. please report seeing this message.");
		}
	}

}

void endfunction()
{
	if (!doingfunction) prerror("extraneous end keyword encountered.");
	doingfunction=0;
}

void function(char **statement)
{
	// declares a function - only needed if function is in bB.  For asm functions, see
	// the help.html file.
	// determine number of args, then run until we get an end.
	doingfunction=1;
	printf("%s\n",statement[2]);
	printf("	STA temp1\n");
	printf("	STY temp2\n");
}

void callfunction(char **statement)
{
	// called by assignment to a variable
	// does not work as an argument in another function or an if-then... yet.
	int i,j,index=0;
	int indirectflag=0;
	char getindex0[200];
	int arguments=0;
	int argnum[10];
	for (i=6; statement[i][0]!=')'; ++i)
	{
		if (statement[i][0] != ',')
		{
			argnum[arguments++]=i;
		}
		if (i > 195) prerror("missing \")\" at end of function call.");
	}
	if (!arguments) 
		prwarn("function call with no arguments");
	while (arguments)
	{
		// get [index]
		index=0;
		index |= getindex(statement[argnum[--arguments]],&getindex0[0],&indirectflag);
		if(indirectflag!=0)
			prerror("indirect arrays not supported as function call arguments.");
		if (index) loadindex(&getindex0[0],indirectflag);

		if (arguments == 1) printf("	LDY ");
		else printf("	LDA ");
		printindex(statement[argnum[arguments]],index,indirectflag);

		if (arguments>1) printf("	STA temp%d\n",arguments+1);
	}
	printf(" jsr %s\n",statement[4]);

	strcpy(Areg,"invalid");

}

void incline()
{
	line++;
}

int bbgetline()
{
	return line;
}

void invalidate_Areg()
{
	strcpy(Areg,"invalid");
}

int islabel(char **statement)
{ // this is for determining if the item after a "then" is a label or a statement
	// return of 0=label, 1=statement
	int i;
	// find the "then" or a "goto"
	for (i=0; i<198; ) if (!strncmp(statement[i++],"then\0",4)) break;
	return findlabel(statement,i);
}

int islabelelse(char **statement)
{ // this is for determining if the item after an "else" is a label or a statement
	// return of 0=label, 1=statement
	int i;
	// find the "else"
	for (i=0; i<198; ) if (!strncmp(statement[i++],"else\0",4)) break;
	return findlabel(statement,i);
}

int findlabel(char **statement, int i)
{
	char statementcache[200];
	// 0 if label, 1 if not
	if ((statement[i][0]>(unsigned char)0x2F) && (statement[i][0]<(unsigned char)0x3B)) return 0;
	if ((statement[i+1][0]==':') && (strncmp(statement[i+2],"rem\0",3))) return 1;
	if ((statement[i+1][0]==':') && (!strncmp(statement[i+2],"rem\0",3))) return 0;

	if (!strncmp(statement[i+1],"else\0",4)) return 0;
	if (statement[i+1][0]!='\0') return 1;
	// only possibilities left are: drawscreen, asm, next, return, maybe others added later?
	strcpy(statementcache,statement[i]);
	removeCR(statementcache);
	if (!strncmp(statementcache,"incgraphic\0",10)) return 1;
	if (!strncmp(statementcache,"drawscreen\0",10)) return 1;
	if (!strncmp(statementcache,"clearscreen\0",11)) return 1;
	if (!strncmp(statementcache,"restorescreen\0",13)) return 1;
	if (!strncmp(statementcache,"savescreen\0",10)) return 1;
	if (!strncmp(statementcache,"characterset\0",12)) return 1;
	if (!strncmp(statementcache,"plotsprite\0",10)) return 1;
	if (!strncmp(statementcache,"plotchars\0",9)) return 1;
	if (!strncmp(statementcache,"plotmap\0",7)) return 1;
	if (!strncmp(statementcache,"plotvalue\0",9)) return 1;
	if (!strncmp(statementcache,"peekchar\0",8)) return 1;
	if (!strncmp(statementcache,"pokechar\0",8)) return 1;
	if (!strncmp(statementcache,"displaymode\0",11)) return 1;
	if (!strncmp(statementcache,"characterset\0",12)) return 1;
	if (!strncmp(statementcache,"tsound\0",6)) return 1;
	if (!strncmp(statementcache,"psound\0",6)) return 1;
	if (!strncmp(statementcache,"alphachars\0",10)) return 1;
	if (!strncmp(statementcache,"alphadata\0",9)) return 1;
	if (!strncmp(statementcache,"loadrambank\0",11)) return 1;
	if (!strncmp(statementcache,"loadrombank\0",11)) return 1;
	if (!strncmp(statementcache,"memcpy\0",6)) return 1;
	if (!strncmp(statementcache,"lives:\0",6)) return 1;
	if (!strncmp(statementcache,"asm\0",4)) return 1;
	if (!strncmp(statementcache,"pop\0",4)) return 1;
	if (!strncmp(statementcache,"stack\0",5)) return 1;
	if (!strncmp(statementcache,"push\0",4)) return 1;
	if (!strncmp(statementcache,"pull\0",4)) return 1;
	if (!strncmp(statementcache,"rem\0",3)) return 1;
	if (!strncmp(statementcache,"next\0",4)) return 1;
	if (!strncmp(statementcache,"reboot\0",6)) return 1;
	if (!strncmp(statementcache,"return\0",6)) return 1;
	if (!strncmp(statementcache,"callmacro\0",9)) return 1;
	if (statement[i+1][0]=='=') return 1;  // it's a variable assignment

	return 0; // I really hope we've got a label !!!!
}

void sread(char **statement)
{
	// read sequential data
	printf("	ldx #%s\n",statement[6]);
	printf("	lda ($100,x)\n");
	printf("	inc $100,x\n");
	printf("	bne *+4\n");
	printf("	inc $101,x\n");
	strcpy(Areg,"invalid");
}

void sdata(char **statement)
{
	// sequential data, works like regular basics and doesn't have the 256 byte restriction
	char data[200];
	int i;
	removeCR(statement[4]);
	sprintf(redefined_variables[numredefvars++],"%s = %s",statement[2],statement[4]);
	printf("	lda #<%s_begin\n",statement[2]);
	printf("	sta %s\n",statement[4]);
	printf("	lda #>%s_begin\n",statement[2]);
	printf("	sta %s+1\n",statement[4]);

	printf("	JMP .skip%s\n",statement[0]);
	// not affected by noinlinedata

	printf("%s_begin\n",statement[2]);
	while (1) {
		if ( ((!fgets(data,200,stdin))
		      || ( (data[0]<(unsigned char)0x3A) && (data[0]>(unsigned char)0x2F) ))
		     && (data[0] != 'e')) {
			prerror("missing \"end\" keyword at end of data.");
			exit(1);
		}
		line++;
		if (!strncmp(data,"end\0",3)) break;
		remove_trailing_commas(data);
		for (i=0; i<200; ++i)
		{
			if ((int)data[i]>32) break;
			if (((int)data[i]<14) && ((int)data[i]!=9)) i=200;
		}
		if (i<200) printf("	.byte %s\n",data);
	}
	printf(".skip%s\n",statement[0]);

}

void data(char **statement)
{
	char data[200];
	char **data_length;
	char **deallocdata_length;
	int i,j;
	data_length=(char **)malloc(sizeof(char *)*200);
	for (i=0; i<200; ++i)
	{
		data_length[i]=(char*)malloc(sizeof(char)*200);
		for (j=0; j<200; ++j) data_length[i][j]='\0';
	}
	deallocdata_length=data_length;
	removeCR(statement[2]);

	if (!(optimization&4)) printf("	JMP .skip%s\n",statement[0]);
	// if optimization level >=4 then data cannot be placed inline with code!

	printf("%s\n",statement[2]);
	while (1) {
		if ( ((!fgets(data,200,stdin))
		      || ( (data[0]<(unsigned char)0x3A) && (data[0]>(unsigned char)0x2F) ))
		     && (data[0] != 'e')) {
			prerror("missing \"end\" keyword at end of data.");
			exit(1);
		}
		line++;
		if (!strncmp(data,"end\0",3)) break;
		remove_trailing_commas(data);
		for (i=0; i<200; ++i)
		{
			if ((int)data[i]>32) break;
			if (((int)data[i]<14) && ((int)data[i]!=9)) i=200;
		}
		if (i<200) printf("	.byte %s\n",data);
	}
	printf(".skip%s\n",statement[0]);
	strcpy(data_length[0]," ");
	strcpy(data_length[1],"const");
	sprintf(data_length[2],"%s_length",statement[2]);
	strcpy(data_length[3],"=");
	sprintf(data_length[4],".skip%s-%s",statement[0],statement[2]);
	strcpy(data_length[5],"\n");
	data_length[6][0]='\0';
	keywords(data_length);
	freemem(deallocdata_length);
}

void alphachars(char **statement)
{
        //   1            2
	//alphachars 'characters' 
	//N.B. if 'characters' has a space, it will be split across 2 and 3.

	//alphachars ASCII

	//this command is really a compiler directive, but couldn't be implemented
	//as "set" without weirding up the preprocessing for other "set" commands.

	char *charset=charactersetchars;
	int i;

	removeCR(statement[2]);
	removeCR(statement[3]);

	if((statement[2][0]=='\'')&&(statement[2][1]=='\''))
		prerror("alphachars syntax error. empty set.");
	else if(strncmp(statement[2],"ASCII",5)==0)
	{
		for(i=0;i<256;i++)
			charactersetchars[i]=i;
		charactersetchars[0]=1;
		charactersetchars[256]=0;
	}
	else if(strncmp(statement[2],"default",7)==0)
	{
		memset(charactersetchars,0,257);
		strcpy(charactersetchars," abcdefghijklmnopqrstuvwxyz.!?,\"$():");
	}
	else if(statement[2][0]=='\'')
	{
		i=1;
		while(statement[2][i]!='\0' && statement[2][i]!='\'')
		{
			*charset=statement[2][i];
			charset=charset+1;
			i=i+1;
		}
		if(statement[2][i]=='\0') // assume there was a space in the list
		{
			*charset=' ';
			charset=charset+1;
			i=0;
			while(statement[3][i]!='\0' && statement[3][i]!='\'')
			{
				*charset=statement[3][i];
				charset=charset+1;
				i=i+1;
			}
		}
		*charset='\0';
	}
	else
	{
		prerror("alphachars syntax error.");
	}
}

void alphadata(char **statement)
{
       //      1          2             3            4
       //   alphadata uniquelabel graphicslabel [extrawide]
       //  'mytext strings here'
       //  'as many lines as I like'
       // end

       //default characters are ' abcdefghijklmnopqrstuvwxyz.!?,"$():'
       //this will be changeable via "alphachars 'characters'

	removeCR(statement[3]);
	removeCR(statement[4]);

	char data[200];
	char **data_length;
	char **deallocdata_length;
	char *alphachr, *alphaend;
        int charoffset;
	int i,j;

	data_length=(char **)malloc(sizeof(char *)*200);
	for (i=0; i<200; ++i)
	{
		data_length[i]=(char*)malloc(sizeof(char)*200);
		for (j=0; j<200; ++j) data_length[i][j]='\0';
	}
	deallocdata_length=data_length;

	removeCR(statement[3]);
	removeCR(statement[4]);

	if (!(optimization&4)) printf("	JMP .skip%s\n",statement[0]);
	// if optimization level >=4 then data cannot be placed inline with code!

	printf("%s\n",statement[2]);
	while (1) 
	{
		if ( ((!fgets(data,200,stdin))
		      || ( (data[0]<(unsigned char)0x3A) && (data[0]>(unsigned char)0x2F) ))
		     && (data[0] != 'e')) 
		{
			prerror("missing \"end\" keyword at end of alphadata.");
			exit(1);
		}
		line++;
		if (!strncmp(data,"end\0",3)) break;

		alphachr=strchr(data,'\'');
		alphaend=strrchr(data,'\'');
		if((alphaend==NULL)||(alphachr==alphaend)||(alphachr==(alphaend-1)))
		{
			prerror("malformed alphadata line.");
			exit(1);
		}
		alphachr=alphachr+1;
		*alphaend='\0';
		for (;*alphachr!='\0';alphachr++)
		{
			charoffset=lookupcharacter(*alphachr);
			if(charoffset<0)
			{
				char sBuffer[200];
				sprintf(sBuffer,"alphadata character '%c' is missing from alphachars.",*alphachr);
				prerror(sBuffer);
			}
			if(doublewide==1)
				charoffset=charoffset*2;
			if (strncmp(statement[4],"extrawide",9)==0)
				charoffset=charoffset*2;
				
			printf(" .byte (<%s + $%02x)",statement[3],charoffset);
			if (strncmp(statement[4],"extrawide",9)==0)
				printf(", (<%s + $%02x)",statement[3],charoffset+1);
			if(alphachr[1]!='\0')
				printf("\n");
		}
		printf("\n");
	}
	printf(".skip%s\n",statement[0]);
	strcpy(data_length[0]," ");
	strcpy(data_length[1],"const");
	sprintf(data_length[2],"%s_length",statement[2]);
	strcpy(data_length[3],"=");
	sprintf(data_length[4],".skip%s-%s",statement[0],statement[2]);
	strcpy(data_length[5],"\n");
	data_length[6][0]='\0';
	keywords(data_length);
	freemem(deallocdata_length);
}

int lookupcharacter(char mychar)
{
	int characterindex=0;
	char *haystack=charactersetchars;
	for(;;)
	{
		if(*haystack==mychar)
			return characterindex;
		haystack=haystack+1;
		characterindex=characterindex+1;
		if(*haystack==0)
			return(-1);
		if(characterindex>255)
		{
			//something very wrong has happened to our charactersetchars string!!!
			prerror("search through valid character string didn't end.");
			exit(1);
		}
	}
}

void shiftdata(char **statement, int num)
{
	int i,j;
	for (i=199; i>num; i--)
		for (j=0; j<200; ++j)
			statement[i][j]=statement[i-1][j];
}

void compressdata(char **statement, int num1, int num2)
{
	int i,j;
	for (i=num1; i<200-num2; i++)
		for (j=0; j<200; ++j)
			statement[i][j]=statement[i+num2][j];
}

void ongoto(char **statement)
{
	int k,i=4;

	if (!strncmp(statement[3],"gosub\0",5))
	{
		printf("	lda #>(ongosub%d-1)\n",ongosub);
		printf("	PHA\n");
		printf("	lda #<(ongosub%d-1)\n",ongosub);
		printf("	PHA\n");
	}
	if (strcmp(statement[2],Areg)) printf("	LDX %s\n",statement[2]);
	printf("	LDA .%sjumptablehi,x\n",statement[0]);
	printf("	PHA\n");
	printf("	LDA .%sjumptablelo,x\n",statement[0]);
	printf("	PHA\n");
	printf("	RTS\n");
	printf(".%sjumptablehi\n",statement[0]);
	while ((statement[i][0]!=':') && (statement[i][0]!='\0'))
	{
		for (k=0; k<200; ++k)
			if ((statement[i][k]==(unsigned char)0x0A) || (statement[i][k]==(unsigned char)0x0D))
				statement[i][k]='\0';
		printf("	.byte >(.%s-1)\n",statement[i++]);
	}
	printf(".%sjumptablelo\n",statement[0]);
	i=4;
	while ((statement[i][0]!=':') && (statement[i][0]!='\0'))
	{
		for (k=0; k<200; ++k)
			if ((statement[i][k]==(unsigned char)0x0A) || (statement[i][k]==(unsigned char)0x0D))
				statement[i][k]='\0';
		printf("	.byte <(.%s-1)\n",statement[i++]);
	}
	if (!strncmp(statement[3],"gosub\0",5)) printf("ongosub%d\n",ongosub++);
}

void dofor(char **statement)
{
	if (strcmp(statement[4],Areg))
	{
		printf("	LDA ");
		printimmed(statement[4]);
		printf("%s\n",statement[4]);
	}

	printf("	STA %s\n",statement[2]);

	forlabel[numfors][0]='\0';
	sprintf(forlabel[numfors],"%sfor%s",statement[0],statement[2]);
	printf(".%s\n",forlabel[numfors]);

	forend[numfors][0]='\0';
	strcpy(forend[numfors],statement[6]);

	forvar[numfors][0]='\0';
	strcpy(forvar[numfors],statement[2]);

	forstep[numfors][0]='\0';

	if (!strncasecmp(statement[7],"step\0",4)) strcpy(forstep[numfors],statement[8]);
	else strcpy(forstep[numfors],"1");

	numfors++;
}

void next(char **statement)
{
	int immed=0;
	int immedend=0;
	int failsafe=0;
	char failsafelabel[60];

	invalidate_Areg();

	if (!numfors) {
		prerror("next without for.");
	}
	numfors--;
	if (!strncmp(forstep[numfors],"1\0",2)) // step 1
	{
		printf("	LDA %s\n",forvar[numfors]);
		printf("	CMP ");
		printimmed(forend[numfors]);
		printf("%s\n",forend[numfors]);
		printf("	INC %s\n",forvar[numfors]);
		bcc(forlabel[numfors]);
	}
	else if ((!strncmp(forstep[numfors],"-1\0",3)) || (!strncmp(forstep[numfors],"255\0",4)))
	{ // step -1
		printf("	DEC %s\n",forvar[numfors]);
		if (strncmp(forend[numfors],"1\0",2))
		{
			printf("	LDA %s\n",forvar[numfors]);
			printf("	CMP ");
			printimmed(forend[numfors]);
			printf("%s\n",forend[numfors]);
			bcs(forlabel[numfors]);
		}
		else
			bne(forlabel[numfors]);
	}
	else // step other than 1 or -1
	{
		// normally, the generated code adds to or subtracts from the for variable, and checks
		// to see if it's less than the ending value.
		// however, if the step would make the variable less than zero or more than 255
		// then this check will not work.  The compiler will attempt to detect this situation
		// if the step and end are known.  If the step and end are not known (that is,
		// either is a variable) then much more complex code must be generated.

		printf("	LDA %s\n",forvar[numfors]);
		printf("	CLC\n");
		printf("	ADC ");
		immed=printimmed(forstep[numfors]);
		printf("%s\n",forstep[numfors]);

		if (immed && isimmed(forend[numfors])) // the step and end are immediate
		{
			if (atoi(forstep[numfors])&128) // step is negative
			{
				if ((256-(atoi(forstep[numfors])&255)) >= atoi(forend[numfors]))
				{ // if we are in danger of going < 0...we will have carry clear after ADC
					failsafe=1;
					sprintf(failsafelabel,"%s_failsafe",forlabel[numfors]);
					bcc(failsafelabel);
				}
			}
			else { // step is positive
				if ((atoi(forstep[numfors]) + atoi(forend[numfors])) > 255)
				{ // if we are in danger of going > 255...we will have carry set after ADC
					failsafe=1;
					sprintf(failsafelabel,"%s_failsafe",forlabel[numfors]);
					bcs(failsafelabel);
				}
			}

		}
		printf("	STA %s\n",forvar[numfors]);

		printf("	CMP ");
		immedend=printimmed(forend[numfors]);
		// add 1 to immediate compare for increasing loops
		if (immedend && !(atoi(forstep[numfors])&128))
			strcat(forend[numfors],"+1");
		printf("%s\n",forend[numfors]);
		// if step number is 1 to 127 then add 1 and use bcc, otherwise bcs
		// if step is a variable, we'll need to check for every loop iteration
		//
		// Warning! no failsafe checks with variables as step or end - it's the
		// programmer's job to make sure the end value doesn't overflow
		if (!immed)
		{
			printf("	LDX %s\n",forstep[numfors]);
			printf("	BMI .%sbcs\n",statement[0]);
			bcc(forlabel[numfors]);
			printf("	CLC\n");
			printf(".%sbcs\n",statement[0]);
			bcs(forlabel[numfors]);
		}
		else if (atoi(forstep[numfors])&128) bcs(forlabel[numfors]);
		else
		{
			bcc(forlabel[numfors]);
			if (!immedend) beq(forlabel[numfors]);
		}
	}
	if (failsafe) printf(".%s\n",failsafelabel);
}

void dim(char **statement)
{
	// just take the statement and pass it right to a header file
	int i;
	char *fixpointvar1;
	char fixpointvar2[50];
	// check for fixedpoint variables
	i=findpoint(statement[4]);
	if (i<50)
	{
		removeCR(statement[4]);
		strcpy(fixpointvar2,statement[4]);
		fixpointvar1=fixpointvar2+i+1;
		fixpointvar2[i]='\0';
		if (!strcmp(fixpointvar1,fixpointvar2)) // we have 4.4
		{
			strcpy(fixpoint44[1][numfixpoint44],fixpointvar1);
			strcpy(fixpoint44[0][numfixpoint44++],statement[2]);
		}
		else // we have 8.8
		{
			strcpy(fixpoint88[1][numfixpoint88],fixpointvar1);
			strcpy(fixpoint88[0][numfixpoint88++],statement[2]);
		}
		statement[4][i]='\0'; // terminate string at '.'
	}
	i=2;
	redefined_variables[numredefvars][0]='\0';
	while ((statement[i][0] != '\0')&&(statement[i][0] != ':'))
	{
		strcat(redefined_variables[numredefvars],statement[i++]);
		strcat(redefined_variables[numredefvars]," ");
	}
	numredefvars++;
}

void doconst(char **statement)
{
	// basically the same as dim, except we keep a queue of variable names that are constant
	int i=2;
	redefined_variables[numredefvars][0]='\0';
	while ((statement[i][0] != '\0')&&(statement[i][0] != ':'))
	{
		strcat(redefined_variables[numredefvars],statement[i++]);
		strcat(redefined_variables[numredefvars]," ");
	}
	numredefvars++;
	strcpy(constants[numconstants++],statement[2]); // record to queue
}


void doreturn(char **statement)
{

	int i,j,index=0;
	int indirectflag=0;
	char getindex0[200];
	int bankedreturn=0;
	// 0=no special action
	// 1=return thisbank
	// 2=return otherbank

	if (!strncmp(statement[2],"thisbank\0",8) ||
		!strncmp(statement[3],"thisbank\0",8)) bankedreturn=1;
	else if (!strncmp(statement[2],"otherbank\0",9) ||
		!strncmp(statement[3],"otherbank\0",9)) bankedreturn=2;

	// several types of returns:
	// return by itself (or with a value) can return to any calling bank
	// this one has the most overhead in terms of cycles and ROM space
	// use sparingly if cycles or space are an issue

	// return [value] thisbank will only return within the same bank
	// this one is the fastest

	// return [value] otherbank will slow down returns to the same bank
	// but speed up returns to other banks - use if you are primarily returning
	// across banks

	if (statement[2][0] && (statement[2][0]!=' ') && (statement[2][0]!=':') &&
		(strncmp(statement[2],"thisbank\0",8)) &&
		(strncmp(statement[2],"otherbank\0",9)))
	{
		index |= getindex(statement[2],&getindex0[0],&indirectflag);
		if(indirectflag!=0)
			prerror("indirect arrays not supported as return arguments.");

		if (index&1) loadindex(&getindex0[0],indirectflag);

		if (!bankedreturn) printf("    LDY ");
			else printf("       LDA ");
		printindex(statement[2],index&1,indirectflag);
	}

	if (bankedreturn == 1)
	{
		printf("    RTS\n");
		return;
	}
	if (bankedreturn == 2)
	{
		printf("    JMP BS_return\n");
		return;
	}

	if (bankcount>0) // check if we need a regular or bank switched return...
	{
		printf("  tsx\n");
		// peek at the high address on the stack...
		printf("  lda $102,x\n");

		// if it's 0 it must be a bank location rather than an actual address
		printf("  beq bankswitchret%d\n",++templabel); //if it's 0, it's not an address
		if (statement[2][0] && (statement[2][0]!=' ')) printf("  tya\n");
		printf("  RTS\n");

		// if it's non-0 it must be a plain address
		printf("bankswitchret%d\n",templabel);
		if (statement[2][0] && (statement[2][0]!=' ')) printf("  tya\n");
		printf("  JMP BS_return\n");
		return;
	}

	if (statement[2][0] && (statement[2][0]!=' ') && (statement[2][0]!=':')) printf("  tya\n");
	printf("  RTS\n");
}

void doasm()
{
	char data[200];
	while (1) {
		if ( ((!fgets(data,200,stdin))
		      || ( (data[0]<(unsigned char)0x3A) && (data[0]>(unsigned char)0x2F) ))
		     && (data[0] != 'e')) {
			prerror("missing \"end\" keyword at end of inline asm.");
			exit(1);
		}
		line++;
		if (!strncmp(data,"end\0",3))
			break;
		printf("%s\n",data);

	}
}

void domacro(char **statement)
{
	int k,j=1,i=3;
	macroactive=1;
	printf(" MAC %s\n",statement[2]);

	while ((statement[i][0]!=':') && (statement[i][0]!='\0'))
	{
		for (k=0; k<200; ++k)
			if ((statement[i][k]==(unsigned char)0x0A) || (statement[i][k]==(unsigned char)0x0D))
				statement[i][k]='\0';
		if (!strncmp(statement[i],"const\0",5)) strcpy(constants[numconstants++],statement[i+1]);  // record to const queue
		else printf("%s SET {%d}\n",statement[i],j++);
		i++;
	}
}

void callmacro(char **statement)
{
	int k,i=3;
	macroactive=1;
	printf(" %s",statement[2]);

	while ((statement[i][0]!=':') && (statement[i][0]!='\0'))
	{
		for (k=0; k<200; ++k)
			if ((statement[i][k]==(unsigned char)0x0A) || (statement[i][k]==(unsigned char)0x0D))
				statement[i][k]='\0';
		if (isimmed(statement[i]))
			printf(" #%s,",statement[i],i-2);  // we're assuming the assembler doesn't mind extra commas!
		else
			printf(" %s,",statement[i],i-2);  // we're assuming the assembler doesn't mind extra commas!
		i++;
	}
	printf("\n");
}

void doextra(char *extrano)
{
	extraactive=1;
	printf("extra set %d\n",++extra);
	printf(" MAC extra%c",extrano[5]);
	if (extrano[6]!=':') printf("%c",extrano[6]);
	printf("\n");
}

void doend()
{
	if (extraactive)
	{
		printf(" ENDM\n");
		extraactive=0;
	}
	else if (macroactive)
	{
		printf(" ENDM\n");
		macroactive=0;
	}
	else prerror("extraneous end statement found.");
}

void lives(char **statement)
{
	int height=8,i=0; //height is always 8
	char label[200];
	char data[200];
	if (!lifekernel)
	{
		lifekernel=1;
		//strcpy(redefined_variables[numredefvars++],"lifekernel = 1");
	}

	sprintf(label,"lives__%s\n",statement[0]);
	removeCR(label);

	printf("	LDA #<%s\n",label);
	printf("	STA lifepointer\n");

	printf("	LDA lifepointer+1\n");
	printf("	AND #$E0\n");
	printf("	ORA #(>%s)&($1F)\n",label);
	printf("	STA lifepointer+1\n");

	sprintf(sprite_data[sprite_index++]," if (<*) > (<(*+8))\n");
	sprintf(sprite_data[sprite_index++],"	repeat ($100-<*)\n	.byte 0\n");
	sprintf(sprite_data[sprite_index++],"	repend\n	endif\n");

	sprintf(sprite_data[sprite_index++],"%s\n",label);

	for (i=0; i<9; ++i) {
		if ( ((!fgets(data,200,stdin))
		      || ( (data[0]<(unsigned char)0x3A) && (data[0]>(unsigned char)0x2F) ))
		     && (data[0] != 'e')) {

			prerror("not enough data or missing \"end\" keyword at end of lives declaration.");
			exit(1);
		}
		line++;
		if (!strncmp(data,"end\0",3)) break;
		sprintf(sprite_data[sprite_index++],"	.byte %s",data);
	}
}

void doif(char **statement)
{
	int index=0;
	int situation=0;
	char getindex0[200];
	char getindex1[200];
        int indirectflag0,indirectflag1;
	int not=0;
	int complex_boolean=0;
	int i,j,k,h;
	int push1=0;
	int push2=0;
	int bit=0;
	int Aregmatch=0;
	char Aregcopy[200];
	char **cstatement; //conditional statement
	char **dealloccstatement; //for deallocation

	strcpy(Aregcopy,"index-invalid");

	cstatement = (char **)malloc(sizeof(char*)*200);
	for (k=0; k<200; ++k) cstatement[k]=(char*)malloc(sizeof(char)*200);
	dealloccstatement=cstatement;
	for (k=0; k<200; ++k) for (j=0; j<200; ++j) cstatement[j][k]='\0';
	if ((statement[2][0] == '!') && (statement[2][1] != '\0')) {
		not=1;
		for (i=0; i<199; ++i) {
			statement[2][i]=statement[2][i+1];
		}
	}
	else if (!strncmp(statement[2],"!\0",2))
	{
		not=1;
		compressdata(statement,2,1);
	}

	if (statement[2][0] == '(') {
		j=0; k=0;
		for (i=2; i<199; ++i) {
			if (statement[i][0]=='(') j++;
			if (statement[i][0]==')') j--;
			if (statement[i][0]=='<') break;
			if (statement[i][0]=='>') break;
			if (statement[i][0]=='=') break;
			if (statement[i][0]=='&' && statement[i][1]=='\0') k=j;
			if (!strncmp(statement[i],"then\0",4)) {complex_boolean=1; break; } //prerror("complex boolean not yet supported.");exit(1);}
		}
		if (i==199 && k) j=k;
		if (j)
		{
			compressdata(statement,2,1); //remove first parenthesis
			for (i=2; i<199; ++i) if ((!strncmp(statement[i],"then\0",4)) ||
				                 (!strncmp(statement[i],"&&\0",2)) ||
				                 (!strncmp(statement[i],"||\0",2))) break;
			if (i != 199)
			{
				if (statement[i-1][0] != ')') {prerror("unbalanced parentheses in if-then."); exit(1); }
				compressdata(statement,i-1,1);
			}
		}
	}

	if ( (!strncmp(statement[2],"joy\0",3))
	     || (!strncmp(statement[2],"switch\0",6)) ) {
		i=switchjoy(statement[2]);
		if (!islabel(statement)) {
			if (!i)
			{
				if (not) bne(statement[4]); else beq(statement[4]);
			}
			else if (i==1) // bvc/bvs
			{
				if (not) bvs(statement[4]); else bvc(statement[4]);
			}
			else if (i==2) // bpl/bmi
			{
				if (not) bmi(statement[4]); else bpl(statement[4]);
			}
			else if (i==3) // bmi/bpl
			{
				if (not) bpl(statement[4]); else bmi(statement[4]);
			}


			freemem(dealloccstatement);
			return;
		}
		else // then statement
		{
			if (!i)
			{
				if (not) printf("	BEQ "); else printf("	BNE ");
			}
			if (i==1)
			{
				if (not) printf("	BVC "); else printf("	BVS ");
			}
			if (i==2)
			{
				if (not) printf("	BPL "); else printf("	BMI ");
			}
			else if (i==3) // bmi/bpl
			{
				if (not) printf("	BMI "); else printf("	BPL ");
			}

			printf(".skip%s\n",statement[0]); 
			// separate statement
			for (i=3; i<200; ++i)
			{
				for (k=0; k<200; ++k) {
					cstatement[i-3][k]=statement[i][k];
				}
			}
			printf(".condpart%d\n",condpart++);
			keywords(cstatement);
			printf(".skip%s\n",statement[0]);
			freemem(dealloccstatement);
			return;
		}


		if (!islabel(statement)) {
			if (!not) {if (bit==7) bmi(statement[4]); else bvs(statement[4]); }
			else {if (bit==7) bpl(statement[4]); else bvc(statement[4]); }
			freemem(dealloccstatement);
			return;
		}
		else // then statement
		{
			if (not) {if (bit==7) printf("	BMI "); else printf("	BVS "); }
			else {if (bit==7) printf("	BPL "); else printf("	BVC "); }

			printf(".skip%s\n",statement[0]);
			// separate statement
			for (i=3; i<200; ++i)
			{
				for (k=0; k<200; ++k) {
					cstatement[i-3][k]=statement[i][k];
				}
			}
			printf(".condpart%d\n",condpart++);
			keywords(cstatement);
			printf(".skip%s\n",statement[0]);

			freemem(dealloccstatement);
			return;
		}
	}

	if (!strncmp(statement[2],"boxcollision\0",12))
	{

                //        2     3 4  5 6  7 8  9 10 11 12 13 14 15 16 17 18 19 20
		// boxcollision ( X1 , Y1 , W1 , H1 ,  X2  , Y2 ,  W2 ,  H2 )
                
		boxcollision(statement);
		if (!islabel(statement))
		{
			if (not) bcs(statement[21]); else bcc(statement[21]);
			freemem(dealloccstatement);
			return;
		}
		else // then statement
		{
			if (not) printf(" BCS "); else printf("   BCC ");
			printf(".skip%s\n",statement[0]);
			// separate statement
			for (i=20;i<200;++i)
			{
				for (k=0;k<200;++k) 
				{
					cstatement[i-20][k]=statement[i][k];
				}
			}
			printf(".condpart%d\n",condpart++);
			keywords(cstatement);
			printf(".skip%s\n",statement[0]);
			freemem(dealloccstatement);
			return;
		}
	}

	if (!strncmp(statement[2],"CARRY\0",5))
	{

		if (!islabel(statement))
		{
			if (not) bcs(statement[4]); else bcc(statement[4]);
			freemem(dealloccstatement);
			return;
		}
		else // then statement
		{
			if (not) printf(" BCS "); else printf("   BCC ");
			printf(".skip%s\n",statement[0]);
			// separate statement
			for (i=3;i<200;++i)
			{
				for (k=0;k<200;++k) 
				{
					cstatement[i-3][k]=statement[i][k];
				}
			}
			printf(".condpart%d\n",condpart++);
			keywords(cstatement);
			printf(".skip%s\n",statement[0]);
			freemem(dealloccstatement);
			return;
		}
	}


	// check for array, e.g. x{1} to get bit 1 of x
	for (i=3; i<200; ++i)
	{
		if (statement[2][i]=='\0') {i=200; break; }
		if (statement[2][i]=='}') break;
	}
	if (i<200) // found array
	{
		// extract expression in parantheses - for now just whole numbers allowed
		bit=(int)statement[2][i-1]-'0';
		if ((bit>9)||(bit<0))
		{
			prerror("variables in bit access not supported.");
		}
		if ((bit==7) || (bit==6)) // if bit 6 or 7, we can use BIT and save 2 bytes
		{
			printf("	BIT ");
			for (i=0; i<200; ++i)
			{
				if (statement[2][i]=='{') break;
				printf("%c",statement[2][i]);
			}
			printf("\n");
			if (!islabel(statement)) {
				if (!not) {if (bit==7) bmi(statement[4]); else bvs(statement[4]); }
				else {if (bit==7) bpl(statement[4]); else bvc(statement[4]); }
				freemem(dealloccstatement);
				return;
			}
			else // then statement
			{
				if (not) {if (bit==7) printf("	BMI "); else printf("	BVS "); }
				else {if (bit==7) printf("	BPL "); else printf("	BVC "); }

				printf(".skip%s\n",statement[0]);
				// separate statement
				for (i=3; i<200; ++i)
				{
					for (k=0; k<200; ++k) {
						cstatement[i-3][k]=statement[i][k];
					}
				}
				printf(".condpart%d\n",condpart++);
				keywords(cstatement);
				printf(".skip%s\n",statement[0]);

				freemem(dealloccstatement);
				return;
			}
		}
		else
		{
			Aregmatch=0;
			printf("	LDA ");
			for (i=0; i<200; ++i)
			{
				if (statement[2][i]=='{') break;
				printf("%c",statement[2][i]);
			}
			printf("\n");
			if (!bit) // if bit 0, we can use LSR and save a byte
				printf("	LSR\n");
			else printf("	AND #%d\n",1<<bit);  //(int)pow(2,bit));
			if (!islabel(statement)) {
				if (not) {if (!bit) bcc(statement[4]); else beq(statement[4]); }
				else {if (!bit) bcs(statement[4]); else bne(statement[4]); }
				freemem(dealloccstatement);
				return;
			}
			else // then statement
			{
				if (not) {if (!bit) printf("	BCS "); else printf("	BNE "); }
				else {if (!bit) printf("	BCC "); else printf("	BEQ "); }

				printf(".skip%s\n",statement[0]);
				// separate statement
				for (i=3; i<200; ++i)
				{
					for (k=0; k<200; ++k) {
						cstatement[i-3][k]=statement[i][k];
					}
				}
				printf(".condpart%d\n",condpart++);
				keywords(cstatement);
				printf(".skip%s\n",statement[0]);

				freemem(dealloccstatement);
				return;
			}



		}

	}

	// generic if-then (no special considerations)
	//check for [indexing]
	strcpy(Aregcopy,statement[2]);
	if (!strcmp(statement[2],Areg)) Aregmatch=1;  // do we already have the correct value in A?

	for (i=3; i<200; ++i) if ((!strncmp(statement[i],"then\0",4)) ||
		                 (!strncmp(statement[i],"&&\0",2)) ||
		                 (!strncmp(statement[i],"||\0",2))) break;

	j=0;
	for (k=3; k<i; ++k) { if (statement[k][0] == '&' && statement[k][1]=='\0' ) j=k;
		              if ((statement[k][0] == '<') ||
		                  (statement[k][0] == '>') ||
		                  (statement[k][0] == '=')) break; }
	if ((k==i) && j) k=j;  // special case of & for efficient code

	if ((complex_boolean) || (k==i && i>4))
	{
		// complex boolean found
		// assign value to contents, reissue statement as boolean
		strcpy(cstatement[2],"Areg\0");
		strcpy(cstatement[3],"=\0");
		for (j=2; j<i; ++j) strcpy(cstatement[j+2],statement[j]);

		let(cstatement);

		if (!islabel(statement)) // then linenumber
		{
			if (not) beq(statement[i+1]); else bne(statement[i+1]);
		}
		else // then statement
		{ // first, take negative of condition and branch around statement
			j=i;
			if (not) printf("	BNE "); else printf("	BEQ ");
		}
		printf(".skip%s\n",statement[0]);
		// separate statement
		for (i=j; i<200; ++i)
		{
			for (k=0; k<200; ++k) {
				cstatement[i-j][k]=statement[i][k];
			}
		}
		printf(".condpart%d\n",condpart++);

		keywords(cstatement);
		printf(".skip%s\n",statement[0]);

		Aregmatch=0;
		freemem(dealloccstatement);
		return;
	}
	else if (((k<i) && (i-k != 2)) || ((k<i) && (k>3)))
	{
		printf("; complex condition detected\n");
		// complex statements will be changed to assignments and reissued as assignments followed by a simple compare
		// i=location of then
		// k=location of conditional operator
		// if is at 2
		if (not)
		{ // handles =, <, <=, >, >=, <>
			// & handled later
			if (!strncmp(statement[k],"=\0",2))
			{
				statement[3][0]='<'; // force beq/bne below
				statement[3][1]='>';
				statement[3][2]='\0';
			}
			else if (!strncmp(statement[k],"<>",2))
			{
				statement[3][0]='='; // force beq/bne below
				statement[3][1]='\0';
			}
			else if (!strncmp(statement[k],"<=",2))
			{
				statement[3][0]='>'; // force beq/bne below
				statement[3][1]='\0';
			}
			else if (!strncmp(statement[k],">=",2))
			{
				statement[3][0]='<'; // force beq/bne below
				statement[3][1]='\0';
			}
			else if (!strncmp(statement[k],"<\0",2))
			{
				statement[3][0]='>'; // force beq/bne below
				statement[3][1]='=';
				statement[3][2]='\0';
			}
			else if (!strncmp(statement[k],"<\0",2))
			{
				statement[3][0]='>'; // force beq/bne below
				statement[3][1]='=';
				statement[3][2]='\0';
			}
		}
		if (k>4) push1=1;  // first statement is complex
		if (i-k != 2) push2=1;  // second statement is complex

		// <, >=, &, = do not swap
		// > or <= swap

		if (push1==1 && push2==1 && (strncmp(statement[k],">\0",2)) && (strncmp(statement[k],"<=\0",2)))
		{
			// Assign to Areg and push
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=2; j<k; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j+2][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");
			// second statement:
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=k+1; j<i; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j-k+3][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");
			situation=1;
		}
		else if (push1==1 && push2==1) // two pushes plus swaps
		{
			// second statement first:
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=k+1; j<i; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j-k+3][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");

			// first statement second
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=2; j<k; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j+2][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");

			// now change operator
			// > or <= swap
			if (!strncmp(statement[k],">\0",2)) strcpy(statement[k],"<\0");
			if (!strncmp(statement[k],"<=\0",2)) strcpy(statement[k],">=\0");
			situation=2;
		}
		else if (push1==1 && (strncmp(statement[k],">\0",2)) && (strncmp(statement[k],"<=\0",2)))
		{
			// first statement only, no swap
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=2; j<k; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j+2][h]=statement[j][h];
				}
			}
			let(cstatement);
			//printf("  PHA\n");
			situation=3;

		}
		else if (push1==1)
		{
			// first statement only, swap
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=2; j<k; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j+2][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");

			// now change operator
			// > or <= swap
			if (!strncmp(statement[k],">\0",2)) strcpy(statement[k],"<\0");
			if (!strncmp(statement[k],"<=\0",2)) strcpy(statement[k],">=\0");

			// swap pushes and vars:
			push1=0;
			push2=1;
			strcpy(statement[2],statement[k+1]);
			situation=4;

		}
		else if (push2==1 && (strncmp(statement[k],">\0",2)) && (strncmp(statement[k],"<=\0",2)))
		{
			// second statement only, no swap:
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=k+1; j<i; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j-k+3][h]=statement[j][h];
				}
			}
			let(cstatement);
			printf("  PHA\n");
			situation=5;
		}
		else if (push2==1)
		{
			// second statement only, swap:
			strcpy(cstatement[2],"Areg\0");
			strcpy(cstatement[3],"=\0");
			for (j=k+1; j<i; ++j)
			{
				for (h=0; h<200; ++h) {
					cstatement[j-k+3][h]=statement[j][h];
				}
			}
			let(cstatement);
			//printf("  PHA\n");
			// now change operator
			// > or <= swap
			if (!strncmp(statement[k],">\0",2)) strcpy(statement[k],"<\0");
			if (!strncmp(statement[k],"<=\0",2)) strcpy(statement[k],">=\0");

			// swap pushes and vars:
			push1=1;
			push2=0;
			strcpy(statement[k+1],statement[2]);
			situation=6;
		}
		else // should never get here
		{prerror("parse error in complex if-then statement."); }
		if (situation != 6 && situation != 3)
		{
			printf("  TSX\n"); //index to stack
			if (push1) printf("  PLA\n");
			if (push2) printf("  PLA\n");
		}
		if (push1 && push2) strcpy(cstatement[2]," $102[TSX]\0");
		else if (push1) strcpy(cstatement[2]," $101[TSX]\0");
		else strcpy(cstatement[2],statement[2]);
		strcpy(cstatement[3],statement[k]);
		if (push2) strcpy(cstatement[4]," $101[TSX]\0");
		else strcpy(cstatement[4],statement[k+1]);
		for (j=5; j<40; ++j) strcpy(cstatement[j],statement[j-5+i]);
		strcpy (cstatement[0],statement[0]); // copy label
		if (situation != 4 && situation != 5) strcpy(Areg,cstatement[2]);  // attempt to suppress superfluous LDA

		if (not && statement[k][0]=='&')
		{
			shiftdata(cstatement,5);
			cstatement[5][0]=')';
			cstatement[5][1]='\0';
			shiftdata(cstatement,2);
			shiftdata(cstatement,2);
			cstatement[2][0]='!';
			cstatement[2][1]='\0';
			cstatement[3][0]='(';
			cstatement[3][1]='\0';
		}
		strcpy(cstatement[1],"if\0");
		if (statement[i][0]=='t')
			doif(cstatement);  // okay to recurse
		else if (statement[i][0]=='&')
		{
			//FIXME - kludge to fix spurious LDA...
			if (situation != 4 && situation != 5)
				printf("; todo: this LDA is spurious and should be prevented ->");
			keywords(cstatement);  // statement still has booleans - attempt to reanalyze
		}
		else
		{prerror("if-then too complex for logical OR."); }
		Aregmatch=0;
		freemem(dealloccstatement);
		return;
	}
	index |= getindex(statement[2],&getindex0[0],&indirectflag0);
	if (strncmp(statement[3],"then\0",4))
		index |= getindex(statement[4],&getindex1[0],&indirectflag1)<<1;

        /*
		REVENG - bug alert! seems to be introducing spurious LDA 1,x when two or
		more complex statements are used.
        */

	if (!Aregmatch) // do we already have the correct value in A?
	{
		if (index&1) loadindex(&getindex0[0],indirectflag0);
		printf("	LDA ");
		printindex(statement[2],index&1,indirectflag0);
		strcpy(Areg,Aregcopy);
	}

	if (index&2) loadindex(&getindex1[0],indirectflag1);
	//todo:check for cmp #0--useless except for <, > to clear carry
	if (strncmp(statement[3],"then\0",4))
	{
		if (statement[3][0] == '&')
		{
			printf("	AND ");
			if (not)
			{
				statement[3][0]='='; // force beq/bne below
				statement[3][1]='\0';
			}
			else
			{
				statement[3][0]='<'; // force beq/bne below
				statement[3][1]='>';
				statement[3][2]='\0';
			}
		}
		else
			printf("	CMP ");
		printindex(statement[4],index&2,indirectflag1);
	}

	if (!islabel(statement)) { // then linenumber
		if (statement[3][0] == '=') beq(statement[6]);
		if (!strncmp(statement[3],"<>\0",2)) bne(statement[6]);
		else if (statement[3][0] == '<') bcc(statement[6]);
		if (statement[3][0] == '>') bcs(statement[6]);
		if (!strncmp(statement[3],"then\0",4))
			if (not) beq(statement[4]); else bne(statement[4]);

	}
	else // then statement
	{ // first, take negative of condition and branch around statement
		if (statement[3][0] == '=') printf ("     BNE ");
		if (!strcmp(statement[3],"<>")) printf ("     BEQ ");
		else if (statement[3][0] == '<') printf ("     BCS ");
		if (statement[3][0] == '>') printf ("     BCC ");
		j=5;

		if (!strncmp(statement[3],"then\0",4))
		{
			j=3;
			if (not) printf("	BNE "); else printf("	BEQ ");
		}
		printf(".skip%s\n",statement[0]);
		// separate statement

		// separate statement
		for (i=j; i<200; ++i)
		{
			for (k=0; k<200; ++k) {
				cstatement[i-j][k]=statement[i][k];
			}
		}
		printf(".condpart%d\n",condpart++);
		keywords(cstatement);
		printf(".skip%s\n",statement[0]);

		freemem(dealloccstatement);
		return;
	}
	freemem(dealloccstatement);
}

void boxcollision(char **statement)
{
	//        2     3 4  5 6  7 8  9 10 11 12 13 14 15 16 17 18 19 20
	// boxcollision ( X1 , Y1 , W1 , H1 ,  X2  , Y2 ,  W2 ,  H2 )

	int t;

	invalidate_Areg();

	for(t=4;t<19;t=t+2)
	{
		if((statement[t][0]==0)||(statement[t][0]==')')||(statement[t][0]==',')||(statement[t+1][0]==0))
			prerror("malformed boxcollision statement.");

		if((t==8)||(t==10)||(t==16)||(t==18)) //widths and heights must be constants
		{
			printf("    lda #%s-1\n",statement[t]);
		}
		else
		{
			printf("    lda "); printimmed(statement[t]); printf("%s\n",statement[t]);
		}
		printf("    sta temp%d\n",(t/2)-1);
	}
	printf("   jsr boxcollision\n");
}

int getcondpart()
{
	return condpart;
}

int orderofoperations(char op1, char op2)
{
	// specify order of operations for complex equations
	// i.e.: parens, divmul (*/), +-, logical (^&|)
	if (op1 == '(') return 0;
	else if (op2 == '(') return 0;
	else if (op2 == ')') return 1;
	else if ( ((op1 == '^') || (op1 == '|') || (op1 == '&'))
	          && ((op2 == '^') || (op2 == '|') || (op2 == '&')))
		return 0;
	else if ((op1 == '*') || (op1 == '/')) return 1;
	else if ((op2 == '*') || (op2 == '/')) return 0;
	else if ((op1 == '+') || (op1 == '-')) return 1;
	else if ((op2 == '+') || (op2 == '-')) return 0;
	else return 1;
}

int isoperator(char op)
{
	if (!((op == '+') || (op == '-') || (op == '/') ||
	      (op == '*') || (op == '&') || (op == '|') ||
	      (op == '^') || (op == ')') || (op == '(')))
		return 0;
	else return 1;
}

void displayoperation(char *opcode, char *operand, int index)
{
	int indirectflag=0;
	if (!strncmp(operand,"stackpull\0",9))
	{
		if (opcode[0] == '-')
		{
			// operands swapped
			printf("  TAY\n");
			printf("  PLA\n");
			printf("  TSX\n");
			printf("  STY $100,x\n",opcode+1);
			printf("  SEC\n");
			printf("  SBC $100,x\n",opcode+1);
		}
		else if (opcode[0] == '/')
		{
			// operands swapped
			printf("  TAY\n");
			printf("  PLA\n");
		}
		else
		{
			printf("  TSX\n");
			printf("  INX\n");
			printf("  TXS\n");
			printf("  %s $100,x\n",opcode+1);
		}
	}
	else
	{
		printf("	%s ",opcode+1);
		printindex(operand,index,indirectflag);
	}
}

void dec(char **cstatement) //REVENG - "dec" is variation of "let" that uses decimal mode
{
	decimal=1;
	printf("	SED\n");
	let(cstatement);
	printf("	CLD\n");
	decimal=0;
}

void let(char **cstatement)
{
	int i,j=0,bit=0,k;
	int index=0;
	int immediate=0;
	char getindex0[200];
	char getindex1[200];
	char getindex2[200];
	int indirectflag0, indirectflag1, indirectflag2;
	int score[6]={0,0,0,0,0,0};
	char **statement;
	char *getbitvar;
	int Aregmatch=0;
	char Aregcopy[200];
	char operandcopy[200];
	int fixpoint1;
	int fixpoint2;
	int fixpoint3=0;
	char **deallocstatement;
	char **rpn_statement; // expression in rpn
	char rpn_stack[200][200]; // prolly doesn't need to be this big
	int sp=0; // stack pointer for converting to rpn
	int numrpn=0;
	char **atomic_statement; // singular statements to recurse back to here
	char tempstatement1[200],tempstatement2[200];

	strcpy(Aregcopy,"index-invalid");

	statement = (char **)malloc(sizeof(char*)*200);
	deallocstatement=statement;
	if (!strncmp(cstatement[2],"=\0",1)) {
		for (i=198; i>0; --i) {
			statement[i+1]=cstatement[i];
		}
	}
	else statement=cstatement;

	// check for unary minus (e.g. a=-a) and insert zero before it
	if ((statement[4][0] == '-') && (statement[5][0])>(unsigned char)0x3F)
	{
		shiftdata(statement,4);
		statement[4][0]='0';
	}


	fixpoint1=isfixpoint(statement[2]);
	fixpoint2=isfixpoint(statement[4]);
	removeCR(statement[4]);

	// check for complex statement
	if ( (!((statement[4][0] == '-') && (statement[6][0] == ':'))) &&
	     (statement[5][0]!=':') && (statement[7][0]!=':') && ( !((statement[5][0]=='(') && (statement[4][0] != '('))) &&
	     ((unsigned char)statement[5][0] > (unsigned char)0x20) &&
	     ((unsigned char)statement[7][0] > (unsigned char)0x20))
	{
		printf("; complex statement detected\n");
		// complex statement here, hopefully.
		// convert equation to reverse-polish notation so we can express it in terms of
		// atomic equations and stack pushes/pulls
		rpn_statement=(char **)malloc(sizeof(char *)*200);
		for (i=0; i<200; ++i) {rpn_statement[i]=(char *)malloc(sizeof(char)*200);
			               for (k=0; k<200; ++k) rpn_statement[i][k]='\0'; }

		atomic_statement=(char **)malloc(sizeof(char *)*10);
		for (i=0; i<10; ++i) {atomic_statement[i]=(char *)malloc(sizeof(char)*200);
			              for (k=0; k<200; ++k) atomic_statement[i][k]='\0'; }

		// this converts expression to rpn
		for (k = 4; (statement[k][0]!='\0') && (statement[k][0]!=':'); k++)
		{
			// ignore CR/LF
			if (statement[k][0]==(unsigned char)0x0A) continue;
			if (statement[k][0]==(unsigned char)0x0D) continue;

			strcpy(tempstatement1,statement[k]);
			if (!isoperator(tempstatement1[0]))
			{strcpy(rpn_statement[numrpn++], tempstatement1); }
			else
			{
				while ((sp) && (orderofoperations(rpn_stack[sp-1][0], tempstatement1[0])))
				{
					strcpy(tempstatement2, rpn_stack[sp-1]);
					sp--;
					strcpy(rpn_statement[numrpn++], tempstatement2);
				}
				if ((sp) && (tempstatement1[0] == ')'))
					sp--;
				else
					strcpy(rpn_stack[sp++],tempstatement1);
			}
		}



		// get stuff off of our rpn stack
		while (sp)
		{
			strcpy(tempstatement2, rpn_stack[sp-1]);
			sp--;
			strcpy(rpn_statement[numrpn++], tempstatement2);
		}

		// now parse rpn statement

		sp=0; // now use as pointer into rpn_statement
		while (sp < numrpn)
		{
			// clear atomic statement cache
			for (i=0; i<10; ++i) for (k=0; k<200; ++k) atomic_statement[i][k]='\0';
			if (isoperator(rpn_statement[sp][0]))
			{
				// operator: need stack pull as 2nd arg
				// Areg=Areg (op) stackpull
				strcpy(atomic_statement[2],"Areg");
				strcpy(atomic_statement[3],"=");
				strcpy(atomic_statement[4],"Areg");
				strcpy(atomic_statement[5],rpn_statement[sp++]);
				strcpy(atomic_statement[6],"stackpull");
				let(atomic_statement);
			}
			else if (isoperator(rpn_statement[sp+1][0]))
			{
				// val,operator: Areg=Areg (op) val
				strcpy(atomic_statement[2],"Areg");
				strcpy(atomic_statement[3],"=");
				strcpy(atomic_statement[4],"Areg");
				strcpy(atomic_statement[6],rpn_statement[sp++]);
				strcpy(atomic_statement[5],rpn_statement[sp++]);
				let(atomic_statement);
			}
			else if (isoperator(rpn_statement[sp+2][0]))
			{
				// val,val,operator: stackpush, then Areg=val1 (op) val2
				if (sp) printf("	PHA\n");
				strcpy(atomic_statement[2],"Areg");
				strcpy(atomic_statement[3],"=");
				strcpy(atomic_statement[4],rpn_statement[sp++]);
				strcpy(atomic_statement[6],rpn_statement[sp++]);
				strcpy(atomic_statement[5],rpn_statement[sp++]);
				let(atomic_statement);
			}
			else
			{
				if ((rpn_statement[sp]=='\0') || (rpn_statement[sp+1]=='\0') || (rpn_statement[sp+2]=='\0'))
				{
					// incomplete or unrecognized expression
					prerror("cannot evaluate expression.");
				}
				// val,val,val: stackpush, then load of next value
				if (sp) printf("	PHA\n");
				strcpy(atomic_statement[2],"Areg");
				strcpy(atomic_statement[3],"=");
				strcpy(atomic_statement[4],rpn_statement[sp++]);
				let(atomic_statement);
			}
		}
		// done, now assign A-reg to original value
		for (i=0; i<10; ++i) for (k=0; k<200; ++k) atomic_statement[i][k]='\0';
		strcpy(atomic_statement[2],statement[2]);
		strcpy(atomic_statement[3],"=");
		strcpy(atomic_statement[4],"Areg");
		let(atomic_statement);
		return; // bye-bye!
	}


	//check for [indexing]
	strcpy(Aregcopy,statement[4]);
	if (!strcmp(statement[4],Areg)) Aregmatch=1;  // do we already have the correct value in A?

	index |= getindex(statement[2],&getindex0[0],&indirectflag0);
	index |= getindex(statement[4],&getindex1[0],&indirectflag1)<<1;
	if (statement[5][0]!=':')
		index |= getindex(statement[6],&getindex2[0],&indirectflag2)<<2;


	// check for array, e.g. x(1) to access bit 1 of x
	for (i=3; i<200; ++i)
	{
		if (statement[2][i]=='\0') {i=200; break; }
		if (statement[2][i]=='}') break;
	}
	if (i<200) // found bit
	{
		strcpy(Areg,"invalid");
		// extract expression in parantheses - for now just whole numbers allowed
		bit=(int)statement[2][i-1]-'0';
		if ((bit>9)||(bit<0))
		{
			prerror("variables in bit access not supported.");
		}
		if (bit>7)
		{
			prerror("invalid bit access.");
		}

		if (statement[4][0]=='0')
		{
			printf("	LDA ");
			for (i=0; i<200; ++i)
			{
				if (statement[2][i]=='{') break;
				printf("%c",statement[2][i]);
			}
			printf("\n");
			printf("	AND #%d\n",255^(1<<bit)); //(int)pow(2,bit));
		}
		else if (statement[4][0]=='1')
		{
			printf("	LDA ");
			for (i=0; i<200; ++i)
			{
				if (statement[2][i]=='{') break;
				printf("%c",statement[2][i]);
			}
			printf("\n");
			printf("	ORA #%d\n",1<<bit); //(int)pow(2,bit));
		}
		else if (getbitvar=strtok(statement[4],"{"))
		{ // assign one bit to another
			if (getbitvar[0] == '!')
				printf("	LDA %s\n",getbitvar+1);
			else
				printf("	LDA %s\n",getbitvar);
			printf("	AND #%d\n",(1<<((int)statement[4][strlen(getbitvar)+1]-'0')));
			printf("  PHP\n");
			printf("	LDA ");
			for (i=0; i<200; ++i)
			{
				if (statement[2][i]=='{') break;
				printf("%c",statement[2][i]);
			}
			printf("\n	AND #%d\n",255^(1<<bit)); //(int)pow(2,bit));
			printf("  PLP\n");
			if (getbitvar[0] == '!')
				printf("	.byte $D0, $02\n");  //bad, bad way to do BEQ addr+5
			else
				printf("	.byte $F0, $02\n");  //bad, bad way to do BNE addr+5

			printf("	ORA #%d\n",1<<bit); //(int)pow(2,bit));

		}
		else
		{
			prerror("can only assign 0, 1 or another bit to a bit.");
		}
		printf("	STA ");
		for (i=0; i<200; ++i)
		{
			if (statement[2][i]=='{') break;
			printf("%c",statement[2][i]);
		}
		printf("\n");
		free(deallocstatement);
		return;
	}

	if (statement[4][0]=='-') // assignment to negative
	{
		strcpy(Areg,"invalid");
		if ((!fixpoint1) && (isfixpoint(statement[5]) != 12))
		{
			if (statement[5][0]>(unsigned char)0x39) // perhaps include constants too?
			{
				printf("	LDA #0\n");
				printf("  SEC\n");
				printf("	SBC %s\n",statement[5]);
			}
			else printf("	LDA #%d\n",256-atoi(statement[5]));
		}
		else
		{
			if (fixpoint1 == 4)
			{
				if (statement[5][0]>(unsigned char)0x39) // perhaps include constants too?
				{
					printf("	LDA #0\n");
					printf("  SEC\n");
					printf("	SBC %s\n",statement[5]);
				}
				else
					printf("	LDA #%d\n",(int)((16-atof(statement[5]))*16));
				printf("	STA %s\n",statement[2]);
				free(deallocstatement);
				return;
			}
			if (fixpoint1 == 8)
			{
				printf("	LDX ");
				sprintf(statement[4],"%f",256.0-atof(statement[5]));
				printfrac(statement[4]);
				printf("	STX ");
				printfrac(statement[2]);
				printf("	LDA #%s\n",statement[4]);
				printf("	STA %s\n",statement[2]);
				free(deallocstatement);
				return;
			}
		}
	}
	else if (!strncmp(statement[4],"rand\0",4)) {
		strcpy(Areg,"invalid");
		if (optimization & 8)
		{
			printf("        lda rand\n");
			printf("        lsr\n");
			printf(" ifconst rand16\n");
			printf("        rol rand16\n");
			printf(" endif\n");
			printf("        bcc *+4\n");
			printf("        eor #$B4\n");
			printf("        sta rand\n");
			printf(" ifconst rand16\n");
			printf("        eor rand16\n");
			printf(" endif\n");
		}
		else jsr("randomize");
	}
	else if ( ( (!strncmp(statement[2],"score0\0",7)) || (!strncmp(statement[2],"score1\0",7)) )
	          && (strncmp(statement[2],"scorec\0",6)))
	{
		char scorei;
		scorei=statement[2][5];
		if((scorei<'0')||(scorei>'1'))
		{
			prerror("malformed score manipulation");
			
		}
		// break up into three parts
		strcpy(Areg,"invalid");
		if (statement[5][0] == '+')
		{
			printf("	SED\n");
			printf("	CLC\n");
			for (i=5; i>=0; i--)
			{
				if (statement[6][i] != '\0') score[j]=number(statement[6][i]);
				score[j]=number(statement[6][i]);
				if ((score[j]<10) && (score[j]>=0)) j++;
			}
			if (score[0] | score[1])
			{
				printf("	LDA score%c+2\n",scorei);
				if (statement[6][0]>(unsigned char)0x3F) printf("	ADC %s\n",statement[6]);
				else printf("	ADC #$%d%d\n",score[1],score[0]);
				printf("	STA score%c+2\n",scorei);
			}
			if (score[0] | score[1] | score[2] | score[3])
			{
				printf("	LDA score%c+1\n",scorei);
				if (score[0]>9) printf("	ADC #0\n");
				else printf("	ADC #$%d%d\n",score[3],score[2]);
				printf("	STA score%c+1\n",scorei);
			}
			printf("	LDA score%c\n",scorei);
			if (score[0]>9) printf("	ADC #0\n");
			else printf("	ADC #$%d%d\n",score[5],score[4]);
			printf("	STA score%c\n",scorei);
			printf("	CLD\n");
		}
		else if (statement[5][0] == '-')
		{
			printf("	SED\n");
			printf("	SEC\n");
			for (i=5; i>=0; i--)
			{
				if (statement[6][i] != '\0') score[j]=number(statement[6][i]);
				score[j]=number(statement[6][i]);
				if ((score[j]<10) && (score[j]>=0)) j++;
			}
			printf("	LDA score%c+2\n",scorei);
			if (score[0]>9) printf("	SBC %s\n",statement[6]);
			else printf("	SBC #$%d%d\n",score[1],score[0]);
			printf("	STA score%c+2\n",scorei);
			printf("	LDA score%c+1\n",scorei);
			if (score[0]>9) printf("	SBC #0\n");
			else printf("	SBC #$%d%d\n",score[3],score[2]);
			printf("	STA score%c+1\n",scorei);
			printf("	LDA score%c\n",scorei);
			if (score[0]>9) printf("	SBC #0\n");
			else printf("	SBC #$%d%d\n",score[5],score[4]);
			printf("	STA score%c\n",scorei);
			printf("	CLD\n");
		}
		else
		{
			for (i=5; i>=0; i--)
			{
				if (statement[4][i] != '\0') score[j]=number(statement[4][i]);
				score[j]=number(statement[4][i]);
				if ((score[j]<10) && (score[j]>=0)) j++;
			}
			printf("	LDA #$%d%d\n",score[1],score[0]);
			printf("	STA score%c+2\n",scorei);
			printf("	LDA #$%d%d\n",score[3],score[2]);
			printf("	STA score%c+1\n",scorei);
			printf("	LDA #$%d%d\n",score[5],score[4]);
			printf("	STA score%c\n",scorei);
		}
		free(deallocstatement);
		return;

	}
	else if ( (statement[6][0] == '1') && ((statement[6][1] > (unsigned char)0x39)
	                                       || (statement[6][1] < (unsigned char)0x30)) &&
	          ((statement[5][0] == '+') || (statement[5][0] == '-')) &&
	          (!strncmp(statement[2], statement[4], 200)) &&
	          (strncmp(statement[2],"Areg\0",4)) &&
	          (statement[6][1]=='\0' || statement[6][1]==' ' || statement[6][1]=='\n') &&
	          (decimal==0)) //REVENG - suppress INC when using "dec" command
	{ // var=var +/- something
		strcpy(Areg,"invalid");
		if ((fixpoint1 == 4) && (fixpoint2 == 4))
		{
			if (statement[5][0] == '+')
			{
				printf("	LDA %s\n",statement[2]);
				printf("	CLC\n");
				printf("	ADC #16\n");
				printf("	STA %s\n",statement[2]);
				free(deallocstatement);
				return;
			}
			if (statement[5][0] == '-')
			{
				printf("	LDA %s\n",statement[2]);
				printf("	SEC\n");
				printf("	SBC #16\n");
				printf("	STA %s\n",statement[2]);
				free(deallocstatement);
				return;
			}
		}

		if (index&1) loadindex(&getindex0[0],indirectflag0);
		if (statement[5][0] == '+') printf("	INC ");
		else printf("	DEC ");
		if (!(index&1))
			printf("%s\n",statement[2]);
		else if(indirectflag0!=0)
			printf("(%s),y\n",statement[4]);  // indexed with y!
		else printf("%s,x\n",statement[4]);  // indexed with x!
		free(deallocstatement);

		return;
	}
	else
	{ // This is generic x=num or var

		if (!Aregmatch) // do we already have the correct value in A?
		{
			if (index&2) loadindex(&getindex1[0],indirectflag1);

			// if 8.8=8.8+8.8: this LDA will be superfluous - fix this at some point

			if (((!fixpoint1 && !fixpoint2) || (!fixpoint1 && fixpoint2 == 8)) && statement[5][0]!='(')
			{
				if (strncmp(statement[4],"Areg\n",4))
				{
					printf("	LDA ");
					printindex(statement[4],index&2,indirectflag1);
				}
			}
			strcpy(Areg,Aregcopy);
		}
	}
	if ((statement[5][0] != '\0') && (statement[5][0] != ':')) { // An operator was found
		fixpoint3=isfixpoint(statement[6]);
		strcpy(Areg,"invalid");
		if (index&4) loadindex(&getindex2[0],indirectflag2);
		if (statement[5][0] == '+') {
			if ((fixpoint1 == 8) && ((fixpoint2 & 8) == 8) && ((fixpoint3 & 8) == 8))
			{ //8.8=8.8+8.8
				printf("	LDA ");
				printfrac(statement[4]);
				printf("	CLC \n");
				printf("	ADC ");
				printfrac(statement[6]);
				printf("	STA ");
				printfrac(statement[2]);
				printf("	LDA ");
				printimmed(statement[4]);
				printf("%s\n",statement[4]);
				printf("	ADC ");
				printimmed(statement[6]);
				printf("%s\n",statement[6]);
			}
			else if ((fixpoint1 == 8) && ((fixpoint2 & 8) == 8) && (fixpoint3 == 4))
			{
				printf("	LDY %s\n",statement[6]);
				printf("	LDX ");
				printfrac(statement[4]);
				printf("	LDA ");
				printimmed(statement[4]);
				printf("%s\n",statement[4]);
				printf("	jsr Add44to88\n");
				printf("	STX ");
				printfrac(statement[2]);
			}
			else if ((fixpoint1 == 8) && ((fixpoint3 & 8) == 8) && (fixpoint2 == 4))
			{
				printf("	LDY %s\n",statement[4]);
				printf("	LDX ");
				printfrac(statement[6]);
				printf("	LDA ");
				printimmed(statement[6]);
				printf("%s\n",statement[6]);
				printf("	jsr Add44to88\n");
				printf("	STX ");
				printfrac(statement[2]);
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 8) && ((fixpoint3 & 4) == 4))
			{
				if (fixpoint3 == 4)
					printf("	LDY %s\n",statement[6]);
				else
					printf("	LDY #%d\n",(int)(atof(statement[6])*16.0));
				printf("	LDA %s\n",statement[4]);
				printf("	LDX ");
				printfrac(statement[4]);
				printf("	jsr Add88to44\n");
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 4) && (fixpoint3 == 12))
			{
				printf("	CLC\n");
				printf("	LDA %s\n",statement[4]);
				printf("	ADC #%d\n",(int)(atof(statement[6])*16.0));
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 12) && (fixpoint3 == 4))
			{
				printf("	CLC\n");
				printf("	LDA %s\n",statement[6]);
				printf("	ADC #%d\n",(int)(atof(statement[4])*16.0));
			}
			else // this needs work - 44+8+44 and probably others are screwy
			{
				if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
				if ((fixpoint3 == 4) && (fixpoint2 == 0))
				{
					printf("	LDA "); // this LDA might be superfluous
					printimmed(statement[4]);
					printf("%s\n",statement[4]);
				}
				displayoperation("+CLC\n	ADC",statement[6],index&4);
			}
		}
		else if (statement[5][0] == '-') {
			if ((fixpoint1 == 8) && ((fixpoint2 & 8) == 8) && ((fixpoint3 & 8) == 8))
			{ //8.8=8.8-8.8
				printf("	LDA ");
				printfrac(statement[4]);
				printf("	SEC \n");
				printf("	SBC ");
				printfrac(statement[6]);
				printf("	STA ");
				printfrac(statement[2]);
				printf("	LDA ");
				printimmed(statement[4]);
				printf("%s\n",statement[4]);
				printf("	SBC ");
				printimmed(statement[6]);
				printf("%s\n",statement[6]);
			}
			else if ((fixpoint1 == 8) && ((fixpoint2 & 8) == 8) && (fixpoint3 == 4))
			{
				printf("	LDY %s\n",statement[6]);
				printf("	LDX ");
				printfrac(statement[4]);
				printf("	LDA ");
				printimmed(statement[4]);
				printf("%s\n",statement[4]);
				printf("	jsr Sub44from88\n");
				printf("	STX ");
				printfrac(statement[2]);
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 8) && ((fixpoint3 & 4) == 4))
			{
				if (fixpoint3 == 4)
					printf("	LDY %s\n",statement[6]);
				else
					printf("	LDY #%d\n",(int)(atof(statement[6])*16.0));
				printf("	LDA %s\n",statement[4]);
				printf("	LDX ");
				printfrac(statement[4]);
				printf("	jsr Sub88from44\n");
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 4) && (fixpoint3 == 12))
			{
				printf("	SEC\n");
				printf("	LDA %s\n",statement[4]);
				printf("	SBC #%d\n",(int)(atof(statement[6])*16.0));
			}
			else if ((fixpoint1 == 4) && (fixpoint2 == 12) && (fixpoint3 == 4))
			{
				printf("	SEC\n");
				printf("	LDA #%d\n",(int)(atof(statement[4])*16.0));
				printf("	SBC %s\n",statement[6]);
			}
			else
			{
				if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
				if ((fixpoint3 == 4) && (fixpoint2 == 0)) printf("	LDA #%s\n",statement[4]);
				displayoperation("-SEC\n	SBC",statement[6],index&4);
			}
		}
		else if (statement[5][0] == '&') {
			if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
			displayoperation("&AND",statement[6],index&4);
		}
		else if (statement[5][0] == '^') {
			if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
			displayoperation("^EOR",statement[6],index&4);
		}
		else if (statement[5][0] == '|') {
			if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
			displayoperation("|ORA",statement[6],index&4);
		}
		else if (statement[5][0] == '*') {
			if (isimmed(statement[4]) && !isimmed(statement[6]) && checkmul(atoi(statement[4])))
			{
				// swap operands to avoid mul routine
				strcpy(operandcopy,statement[4]); // place here temporarily
				strcpy(statement[4],statement[6]);
				strcpy(statement[6],operandcopy);
			}
			if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
			if ((!isimmed(statement[6])) || (!checkmul(atoi(statement[6]))))
			{
				displayoperation("*LDY",statement[6],index&4);
				if (statement[5][1] == '*')
					printf("	jsr mul16\n");  // general mul routine
				else
					printf("	jsr mul8\n");
			}
			else if (statement[5][1] == '*') mul(statement, 16);
			else mul(statement, 8);  // attempt to optimize - may need to call mul anyway

		}
		else if (statement[5][0] == '/') {
			if (fixpoint2 == 4) printf("	LDA %s\n",statement[4]);
			if ((!isimmed(statement[6])) || (!checkdiv(atoi(statement[6]))))
			{
				displayoperation("/LDY",statement[6],index&4);
				if (statement[5][1] == '/')
					printf("	jsr div16\n");  // general div routine
				else
					printf("	jsr div8\n");
			}
			else if (statement[5][1] == '/') divd(statement, 16);
			else divd(statement, 8);  // attempt to optimize - may need to call divd anyway

		}
		else if (statement[5][0] == ':') {
			strcpy(Areg,Aregcopy); // A reg is not invalid
		}
		else if (statement[5][0] == '(') {
			// we've called a function, hopefully
			strcpy(Areg,"invalid");
			if (!strncmp(statement[4],"sread\0",5)) sread(statement);
			else if (!strncmp(statement[4],"peekchar\0",8)) peekchar(statement);
			else callfunction(statement);
		}
		else if (statement[4][0]!='-') // if not unary -
		{
			char sBuffer[200];	
			sprintf(sBuffer, "invalid operator: %s.",statement[5]);
			prerror(sBuffer);
		}

	}
	else // simple assignment
	{
		// check for fixed point stuff here
		// bugfix: forgot the LDA (?) did I do this correctly???
		if ((fixpoint1 == 4) && (fixpoint2 == 0))
		{
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			printf("  ASL\n");
			printf("  ASL\n");
			printf("  ASL\n");
			printf("  ASL\n");
		}
		else if ((fixpoint1 == 0) && (fixpoint2 == 4))
		{
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			printf("  LSR\n");
			printf("  LSR\n");
			printf("  LSR\n");
			printf("  LSR\n");
		}
		else if ((fixpoint1 == 4) && (fixpoint2 == 8))
		{
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			printf("  LDX ");
			printfrac(statement[4]);
			// note: this shouldn't be changed to jsr(); (why???)
			printf(" jsr Assign88to44");
			printf("\n");
		}
		else if ((fixpoint1 == 8) && (fixpoint2 == 4))
		{
			// note: this shouldn't be changed to jsr();
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
			printf("  JSR Assign44to88");
			printf("\n");
			printf("  STX ");
			printfrac(statement[2]);
		}
		else if ((fixpoint1 == 8) && ((fixpoint2 & 8) == 8))
		{
			printf("	LDX ");
			printfrac(statement[4]);
			printf("	STX ");
			printfrac(statement[2]);
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
		}
		else if ((fixpoint1 == 4) && ((fixpoint2 & 4) == 4))
		{
			if (fixpoint2 == 4)
				printf("	LDA %s\n",statement[4]);
			else
				printf("	LDA #%d\n",(int)(atof(statement[4])*16));
		}
		else if ((fixpoint1 == 8) && (fixpoint2 == 0))
		{ // should handle 8.8=number w/o point or int var
			printf("	LDA #0\n");
			printf("	STA ");
			printfrac(statement[2]);
			printf("	LDA ");
			printimmed(statement[4]);
			printf("%s\n",statement[4]);
		}
	}
	if (index&1) loadindex(&getindex0[0],indirectflag0);
	if (strncmp(statement[2],"Areg\0",4))
	{
		printf("	STA ");
		printindex(statement[2],index&1,indirectflag0);
	}
	free(deallocstatement);
}

void loadrombank(char **statement)
{

	int anotherbank=0;
	invalidate_Areg();
	if(!strncmp(statement[2],"bank",4))
	{
		anotherbank=atoi(statement[2]+4);
		anotherbank=anotherbank-1;
	}
	else if((statement[2][0]>='0')&&(statement[2][0]<='9'))
	{
		anotherbank=atoi(statement[2]);
		anotherbank=anotherbank-1;
	}
	else
		prerror("loadrombank statement with malformed argument.");

	if(currentbank!=(bankcount-1))
		prerror("loadrombank called from outside of the last bank.");
	if(anotherbank==(bankcount-1))
		prerror("loadrombank can not switch the last bank.");

	printf(" lda #currentbank\n",currentbank);
	printf(" ifconst BANKRAM\n");
	printf("   sta currentbank\n");
	printf("   ora currentrambank\n");
	printf(" endif\n");
	printf(" ifconst MCPDEVCART\n");
	printf("   ora #$18\n");
	printf("   sta $3000\n");
	printf(" else\n");
	printf("   sta $8000\n");
	printf(" endif\n");

}

void loadrambank(char **statement)
{
	int anotherbank=0;
	invalidate_Areg();
	if(!strncmp(statement[2],"bank",4))
	{
		anotherbank=atoi(statement[2]+4);
		anotherbank=anotherbank-1;
	}
	else if((statement[2][0]>='0')&&(statement[2][0]<='9'))
	{
		anotherbank=atoi(statement[2]);
		anotherbank=anotherbank-1;
	}
	else
		prerror("loadrambank statement with malformed argument.");

	if((anotherbank<0)||(anotherbank>1))
		prerror("bad bank# used with loadrambank.");

	printf(" lda #%d\n",anotherbank<<5);
	printf(" sta currentrambank\n");
	printf(" ora currentbank\n");
	printf(" ifconst MCPDEVCART\n");
	printf("   ora #$18\n");
	printf("   sta $3000\n");
	printf(" else\n");
	printf("   sta $8000\n");
	printf(" endif\n");
}

void dogoto(char **statement)
{
	int anotherbank=0;
	invalidate_Areg();
	if(!strncmp(statement[3],"bank",4))
	{
		anotherbank=atoi(statement[3]+4);
		anotherbank=anotherbank-1;
	}
	else
	{
		printf(" jmp .%s\n",statement[2]);
		return;
	}

	if(anotherbank==(bankcount-1))
	{
		prerror("bank switch not required to the last bank. its always present.");
	}

	printf(" sta temp9\n"); //save A register

	printf(" lda #>(.%s-1)\n",statement[2]);
	printf(" pha\n");
	printf(" lda #<(.%s-1)\n",statement[2]);
	printf(" pha\n");
	
	printf(" lda temp9\n");
	printf(" pha\n");
	printf(" txa\n");
	printf(" pha\n");

	printf(" ifconst BANKRAM\n");
	printf("   lda #%d\n",anotherbank);
	printf("   sta currentbank\n");
	printf("   ora currentrambank\n");
	printf(" else\n");
	printf("   lda #%d\n",anotherbank);
	printf(" endif\n");
	printf(" jmp BS_jsr\n");

}

void gosub(char **statement)
{
	int anotherbank=0;
	invalidate_Areg();
	if(!strncmp(statement[3],"bank",4))
	{
		anotherbank=atoi(statement[3]+4);
		anotherbank=anotherbank-1;
	}
	else
	{
		printf(" jsr .%s\n",statement[2]);
		return;
	}

	if(anotherbank==(bankcount-1))
	{
		prerror("bank switch not required to the last bank. its always present.");
	}

	printf(" sta temp9\n"); //save A register

	//return address
	printf(" lda #>(ret_point%d-1)\n",++numjsrs);
	printf(" pha\n");
	printf(" lda #<(ret_point%d-1)\n",numjsrs);
	printf(" pha\n");

	//we can detect this from a regular return address, because its less than $100
	printf(" lda #0\n"); 
	printf(" pha\n");
	printf(" lda #%d\n",currentbank);
	printf(" pha\n");

	printf(" lda #>(.%s-1)\n",statement[2]);
	printf(" pha\n");
	printf(" lda #<(.%s-1)\n",statement[2]);
	printf(" pha\n");
	
	printf(" lda temp9\n");
	printf(" pha\n");
	printf(" txa\n");
	printf(" pha\n");

	printf(" ifconst BANKRAM\n");
	printf("   lda #%d\n",anotherbank);
	printf("   sta currentbank\n");
	printf("   ora currentrambank\n");
	printf(" else\n");
	printf("   lda #%d\n",anotherbank);
	printf(" endif\n");
	printf(" jmp BS_jsr\n");
	printf("ret_point%d\n",numjsrs);

}


void set(char **statement)
{
	char **pass2const;
	int i;
	int v;
	pass2const=(char **)malloc(sizeof(char *)*5);
	if (!strncasecmp(statement[2],"tv\0",2))
	{
		if (!strncasecmp(statement[3],"ntsc\0",4))
		{
			strcpy(redefined_variables[numredefvars++],"NTSC = 1");
			append_a78info("tv ntsc");
		}
		else if (!strncasecmp(statement[3],"pal\0",3))
		{
			strcpy(redefined_variables[numredefvars++],"PAL = 1");
			append_a78info("tv pal");
		}
		else prerror("set TV using invalid TV type.");
	}
	else if (!strncmp(statement[2],"smartbranching\0",14))
	{
		if (!strncmp(statement[3],"on",2)) smartbranching=1;
		else smartbranching=0;
	}
	else if (!strncmp(statement[2],"collisionwrap\0",13))
	{
		if (!strncmp(statement[3],"on",2)) 
			strcpy(redefined_variables[numredefvars++],"collisionwrap = 1");
	}
	else if (!strncmp(statement[2],"romsize\0",7))
	{
		set_romsize(statement[3]);
	}
	else if (!strncmp(statement[2],"atarivox\0",8))
	{
		if (!strncmp(statement[3],"on",2))
		{
			strcpy(redefined_variables[numredefvars++],"ATARIVOX = 1");
			append_a78info("save savekey");
		}
	}
	else if (!strncmp(statement[2],"savekey\0",8))
	{
		if (!strncmp(statement[3],"on",2))
		{
			strcpy(redefined_variables[numredefvars++],"SAVEKEY = 1");
			append_a78info("save savekey");
		}
	}
	else if (!strncmp(statement[2],"hsc",3))
	{
		if (!strncmp(statement[3],"on",2))
		{
			strcpy(redefined_variables[numredefvars++],"HSC = 1");
			append_a78info("save hsc");
		}
	}
	else if (!strncmp(statement[2],"tiasfx",6))
	{
		if (!strncmp(statement[3],"mono",4))
		{
			strcpy(redefined_variables[numredefvars++],"TIASFXMONO = 1");
		}
	}
	else if (!strncmp(statement[2],"pokeysupport\0",12))
	{
		if (!strncmp(statement[3],"on",2))
		{
			strcpy(redefined_variables[numredefvars++],"pokeysupport = 1");
			append_a78info("cart pokey");
		}
	}

	else if (!strncmp(statement[2],"doublewide",10))
	{
		if (!strncmp(statement[3],"on",2))
		{
			strcpy(redefined_variables[numredefvars++],"DOUBLEWIDE = 1");
			doublewide=1;
		}
	}
	else if (!strncmp(statement[2],"pauseroutine",10))
	{
		if (!strncmp(statement[3],"off",3))
		{
			strcpy(redefined_variables[numredefvars++],"pauseroutineoff = 1");
		}
	}
	else if (!strncmp(statement[2],"debug",5))
	{
		if (!strncmp(statement[3],"color",5))
		{
			strcpy(redefined_variables[numredefvars++],"debugcolor = 1");
		}
	}

	else if (!strncmp(statement[2],"zoneheight",10))
	{
		if (!strncmp(statement[3],"8",1))
		{
			strcpy(redefined_variables[numredefvars++],"ZONEHEIGHT = 8");
			zoneheight=8;
		}
		else if (!strncmp(statement[3],"16",2))
		{
			strcpy(redefined_variables[numredefvars++],"ZONEHEIGHT = 16");
			zoneheight=16;
		}
		else
			prerror("set using illegal zone height. valid values are 8 and 16.");
	}
	else if (!strncmp(statement[2],"mcpdevcart",10))
	{
		if (strncmp(statement[3],"off",3)!=0)
			strcpy(redefined_variables[numredefvars++],"MCPDEVCART = 1");
	}
	else if (!strncmp(statement[2],"optimization",12))
	{
		if (!strncmp(statement[3],"speed",5))
		{
			optimization|=1;
		}
		if (!strncmp(statement[3],"size",4))
		{
			optimization|=2;
		}
		if (!strncmp(statement[3],"noinlinedata",12))
		{
			optimization|=4;
		}
		if (!strncmp(statement[3],"inlinerand",10))
		{
			optimization|=8;
		}
		if (!strncmp(statement[3],"none\0",4))
		{
			optimization=0;
		}
	}
	else prerror("unknown set parameter.");
}

void rem(char **statement)
{
	if (!strncmp(statement[2],"smartbranching\0",14))
	{
		if (!strncmp(statement[3],"on\0",2)) smartbranching=1;
		else smartbranching=0;
	}
}

void dopop()
{
	printf("	pla\n");
	printf("	pla\n");
}


void bmi(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bmi .%s\n",linenumber,linenumber,linenumber);
		// branches might be allowed as below - check carefully to make sure!
		// printf(" if ((* - .%s) < 127) && ((* - .%s) > -129)\n	bmi .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bpl .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bmi .%s\n",linenumber);
	}
}

void bpl(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bpl .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bmi .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bpl .%s\n",linenumber);
	}
}

void bne(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	BNE .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	beq .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bne .%s\n",linenumber);
	}
}

void beq(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	BEQ .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bne .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else
	{
		printf("	beq .%s\n",linenumber);
	}
}

void bcc(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bcc .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bcs .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else
	{
		printf("	bcc .%s\n",linenumber);
	}

}

void bcs(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bcs .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bcc .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bcs .%s\n",linenumber);
	}
}

void bvc(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bvc .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bvs .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bvc .%s\n",linenumber);
	}
}

void bvs(char *linenumber)
{
	removeCR(linenumber);
	if (smartbranching) {
		printf(" if ((* - .%s) < 127) && ((* - .%s) > -128)\n	bvs .%s\n",linenumber,linenumber,linenumber);
		printf(" else\n	bvc .%dskip%s\n	jmp .%s\n",branchtargetnumber,linenumber,linenumber);
		printf(".%dskip%s\n",branchtargetnumber++,linenumber);
		printf(" endif\n");
	}
	else {
		printf("	bvs .%s\n",linenumber);
	}
}

void clearscreen()
{
	invalidate_Areg();
	jsr("clearscreen");
}


void drawscreen(void)
{
	invalidate_Areg();
	jsr("drawscreen");
}

void savescreen(void)
{
	invalidate_Areg();
	jsr("savescreen");
}

void restorescreen(void)
{
	invalidate_Areg();
	jsr("restorescreen");
}

void prinfo(char *myinfo)
{
	fprintf(stderr, "*** (): INFO, %s\n", myinfo);
}

void prwarn(char *mywarn)
{
	fprintf(stderr, "*** (%d): WARNING, %s\n", line, mywarn);
}


void prerror(char *myerror)
{
	fprintf(stderr, "*** (%d): ERROR, %s\n", line, myerror);
	exit(1);
}

int printimmed(char *value)
{
	int immed=isimmed(value);
	if (immed) printf("#");
	return immed;
}

int isimmed(char *value)
{
	// search queue of constants
	int i;
	//removeCR(value);
	for (i=0; i<numconstants; ++i)
	{
		if (!strcmp(value,constants[i]))
		{
			// a constant should be treated as an immediate
			return 1;
		}
	}
	if(!strcmp(value+(strlen(value)>7 ? strlen(value)-7 : 0),"_length"))
	{
		// Warning about use of data_length before data statement
		char sBuffer[300];
		sprintf(sBuffer,"possible use of data statement length before data statement is defined\n      workaround: forward declaration may be done by const %s=%s at beginning of code",value,value);
		prwarn(sBuffer);
	}
	if ((value[0] == '$') || (value[0] == '%')
	    || (value[0] < (unsigned char)0x3A))
	{
		return 1;
	}
	else return 0;
}

int number(unsigned char value)
{
	return ((int)value)-'0';
}

void removeCR(char *linenumber)  // remove trailing CR from string
{
	char *CR;
	if(linenumber==0)
		return;
	CR=strrchr(linenumber,(unsigned char)0x0A);
	if(CR!=NULL)
		*CR=0;
	CR=strrchr(linenumber,(unsigned char)0x0D);
	if(CR!=NULL)
		*CR=0;
}

void remove_trailing_commas(char *linenumber)  // remove trailing commas from string
{
	int i;
	for (i=strlen(linenumber)-1; i>0; i--)
	{
		if ((linenumber[i] != ',') &&
		    (linenumber[i] != ' ') &&
		    (linenumber[i] != (unsigned char)0x0A) &&
		    (linenumber[i] != (unsigned char)0x0D)) break;
		if (linenumber[i] == ',')
		{
			linenumber[i] = ' ';
			break;
		}
	}
}

void header_open(FILE *header)
{
}

void header_write(FILE *header, char *filename)
{
	int i;
	if ((header = fopen(filename, "w")) == NULL) // open file
	{
		char sBuffer[200];
		sprintf(sBuffer,"can't open '%s' for writing",filename);
		prerror(sBuffer);
	}

	strcpy(redefined_variables[numredefvars],"; This file contains variable mapping and other information for the current project.\n");

	for (i=numredefvars; i>=0; i--)
	{
		fprintf(header, "%s\n", redefined_variables[i]);
	}
	fclose(header);

}
