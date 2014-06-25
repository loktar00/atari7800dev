#define _readpaddle 1
#define _background 64
#define MAX_EXTRAS 5

#define MODE160A 0
#define MODE160B 1
#define MODE320A 2
#define MODE320B 4
#define MODE320C 8
#define MODE320D 16

void printindex(char *, int, int);
void loadindex(char *, int);
void jsr(char *);
int  islabel(char **);
int  islabelelse(char **);
int  findlabel(char **, int i);
void add_includes(char *);
void create_includes(char *);
void incline();
void init_includes();
void invalidate_Areg();
void shiftdata(char **, int);
void compressdata(char **, int, int);
void data(char **);
void sdata(char **);
void alphadata(char **);
int lookupcharacter(char);
void function(char **);
void endfunction();
void callfunction(char **);
void ongoto(char **);
void doreturn(char **);
void clearscreen(void);
void tsound(char **);
void psound(char **);
void playsfx(char **);
void doconst(char **);
void dim(char **);
void dofor(char **);
void mul(char **, int);
void divd(char **, int);
void next(char **);
void gosub(char **);
void doif(char **);
void domemcpy(char **);
void pokechar(char **);
void let(char **);
void dec(char **);
void bank(char **);
void dmahole(char **);
void rem(char **);
void set(char **);
void dogoto(char **);
void loadrombank(char **);
void loadrambank(char **);
char *ourbasename(char *);
void add_graphic(char **);
void boxcollision(char **);
void dash2underscore(char *mystring);
void plotsprite(char **statement);
void plotchars(char **statement);
void plotmap(char **statement);
void plotvalue(char **statement);
void displaymode(char **statement);
void incgraphic(char *file_name);
void newblock();
int  getgraphicwidth(char *file_name);
void characterset(char **statement);
void savescreen(void);
void restorescreen(void);
void barf_graphic_file(void);
void barfmultiplicationtables(void);
void append_a78info(char *);
void create_a78info(void);
void drawscreen(void);
void prerror(char *);
void prwarn(char *);
void prinfo(char *);
void remove_trailing_commas(char *);
void removeCR(char *);
void bmi(char *);
void bpl(char *);
void bne(char *);
void beq(char *);
void bcc(char *);
void bcs(char *);
void bvc(char *);
void bvs(char *);
int  printimmed(char *);
int  isimmed(char *);
int  number(unsigned char);
void header_open(FILE *);
void header_write(FILE *, char *);

