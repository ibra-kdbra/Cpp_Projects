#include <stdio.h>
#include <stdlib.h>

#define ARBITRARY_LIMIT 1024 /*TODO use BNK count*/

#pragma pack(1)

/*
 * WinFM Instrument
 * Thanks to ValleyBell for the offset correspondence information
 */
typedef struct 
{
   char tvsksrm0,   /*OPL3 Register offset 0x20*/
        ksclout0,   /*OPL3 Register offset 0x40*/
        attdecy0,   /*OPL3 Register offset 0x60*/
        susrels0,   /*OPL3 Register offset 0x80*/
        wavslct0,   /*OPL3 Register offset 0xE0*/
        tvsksrm1,   /*OPL3 Register offset 0x23*/
        ksclout1,   /*OPL3 Register offset 0x43*/
        attdecy1,   /*OPL3 Register offset 0x63*/
        susrels1,   /*OPL3 Register offset 0x83*/
        wavslct1,   /*OPL3 Register offset 0xE3*/
        _pad[12],   /* set all to 0 - used for 4op but unused*/
        /*TODO: 4op support in the future?*/
        
        __tenpad,   /* set to 0x10 */
        __nullpad,  /* set to 0 */
        fbkmodsyn,  /*OPL3 Register offset 0xC0*/
        __nullpad2, /* set to 0 */
        twopad,     /* set to 0x02 */
        __nullpad3; /* set to 0 */
} oplinst_t;

/*
 * Structures below stolen from MF2ROL (C) 1991 Alejandro Kapauan
 * Also referenced format info from
 * http://www.shikadi.net/moddingwiki/BNK_Format_(Ad_Lib)
 * note: some information was incomplete/incorrect on site
 */
typedef struct 
{
   char  ksl;
   char  multiple;
   char  feedback; /* used by operator 0 only */
   char  attack;
   char  sustain;
   char  eg;
   char  decay;
   char  releaseRate;
   char  totalLevel;
   char  am;
   char  vib;
   char  ksr;
   char  con;      /* used by operator 0 only */
} bnkop_t;

typedef struct 
{
   char      mode;             /* not used */
   char      percVoice;        /* not used */
   bnkop_t   op0;
   bnkop_t   op1;
   char      wave0;            /* wave form for operator 0 */
   char      wave1;            /* wave form for operator 1 */
} bnktimbre_t;

void printUsage()
{
   printf("Convert .BNK instruments to OPL registers\n"
      "Usage: <BNK file> <OP3 file>\n"
   );
   exit(EXIT_SUCCESS);
}

void printError(char *s)
{
   printf("Cannot open file handle %s. Aborting...\n", s);
   exit(EXIT_FAILURE);
}

void printFileError(char *s)
{
   printf("File %s is not correct.\n", s);
   exit(EXIT_FAILURE);
}

/*
   * Initialize given operator struct of BNK instrument
 */
void initBNKOperator(bnkop_t *op)
{
   op->ksl = 0;
   op->multiple = 0;
   op->feedback = 0;
   op->attack = 0;
   op->sustain = 0;
   op->eg = 0;
   op->decay = 0;
   op->releaseRate = 0;
   op->totalLevel = 0;
   op->am = 0;
   op->vib = 0;
   op->ksr = 0;
   op->con = 0;
}

/*
 * Initialize Adlib .ROL BNK patch struct
 */
void initBNKPatch(bnktimbre_t *pat)
{
   pat->mode = 0;
   pat->percVoice = 0;
   initBNKOperator(&pat->op0);
   initBNKOperator(&pat->op1);
   pat->wave0 = 0;
   pat->wave1 = 0;
}

/*
 * Initialize OPL patch struct
 */ 
void initOPLPatch(oplinst_t *pat)
{
   size_t i;
   
   pat->tvsksrm0 = 0; /*0x20*/
   pat->ksclout0 = 0; /*0x40*/
   pat->attdecy0 = 0; /*0x60*/
   pat->susrels0 = 0; /*0x80*/
   pat->wavslct0 = 0; /*0xE0*/
   pat->tvsksrm1 = 0; /*0x23*/
   pat->ksclout1 = 0; /*0x43*/
   pat->attdecy1 = 0; /*0x63*/
   pat->susrels1 = 0; /*0x83*/
   pat->wavslct1 = 0; /*0xE3*/
   for(i=0; i<12; ++i) pat->_pad[i] = 0; 
   pat->__tenpad = 0x10;  
   pat->__nullpad = 0;    
   pat->fbkmodsyn = (1<<5)|(1<<4); /*0xC0*/
   pat->__nullpad2 = 0;
   pat->twopad = 0x02; 
   pat->__nullpad3 = 0;
}

/*
 * Take offset at 0x10 of BNK, and then seek to location value  
 */
void seekToBNKData(FILE *fp)
{
   size_t offset;
   
   fseek(fp, 0x10, SEEK_SET);
   fread(&offset, sizeof(int), 1, fp);
   fseek(fp, offset, SEEK_SET);
   printf("DEBUG: reading from offset %x\n", ftell(fp));
}

/*
 * Conversion routine for register information with accordance to
 * the OPL3 programming guide.
 */
void convertBNKToOPL(oplinst_t *oplPat, bnktimbre_t *bnkPat)
{
   /*char tvsksrm0 = 0, tvsksrm1 = 0, //0x20
          ksclout0 = 0, ksclout1 = 0, //0x40
          attdecy0 = 0, attdecy1 = 0, //0x60
          susrels0 = 0, susrels1 = 0, //0x80
          wavslct0 = 0, wavslct1 = 0, //0xE0
          fbkmodsyn = 0;              //0xC0
   */
   /*Operator 0*/
   oplPat->ksclout0 |= (bnkPat->op0.ksl<<6);
   oplPat->tvsksrm0 |= (bnkPat->op0.multiple);
   oplPat->fbkmodsyn |= (bnkPat->op0.feedback << 1);
   oplPat->attdecy0 |= (bnkPat->op0.attack << 4);
   oplPat->susrels0 |= (bnkPat->op0.sustain << 4);
   oplPat->tvsksrm0 |= (bnkPat->op0.eg != 0) ? (1<<5) : 0;
   oplPat->attdecy0 |= (bnkPat->op0.decay);
   oplPat->susrels0 |= (bnkPat->op0.releaseRate);
   oplPat->ksclout0 |= (bnkPat->op0.totalLevel);
   oplPat->tvsksrm0 |= (bnkPat->op0.am << 7);
   oplPat->tvsksrm0 |= (bnkPat->op0.vib << 6);
   oplPat->tvsksrm0 |= (bnkPat->op0.ksr << 4);
   oplPat->fbkmodsyn |= (bnkPat->op0.con == 0) ? 1 : 0;
   oplPat->wavslct0 |= (bnkPat->wave0);
   
   /*Operator 1*/
   oplPat->ksclout1 |= (bnkPat->op1.ksl << 6);
   oplPat->tvsksrm1 |= (bnkPat->op1.multiple);
   /*oplPat->fbkmodsyn |= (bnkPat->op1.feedback << 1);*/
   oplPat->attdecy1 |= (bnkPat->op1.attack << 4);
   oplPat->susrels1 |= (bnkPat->op1.sustain << 4);
   oplPat->tvsksrm1 |= (bnkPat->op1.eg != 0) ? (1<<5) : 0;
   oplPat->attdecy1 |= (bnkPat->op1.decay);
   oplPat->susrels1 |= (bnkPat->op1.releaseRate);
   oplPat->ksclout1 |= (bnkPat->op1.totalLevel);
   oplPat->tvsksrm1 |= (bnkPat->op1.am << 7);
   oplPat->tvsksrm1 |= (bnkPat->op1.vib << 6);
   oplPat->tvsksrm1 |= (bnkPat->op1.ksr << 4);
   /*oplPat->fbkmodsyn |= (bnkPat->op1.con == 0) ? 1 : 0;*/
   oplPat->wavslct1 |= (bnkPat->wave1);
}

int main(int argc, char *argv[])
{
   FILE *fp, *outfp;
   bnktimbre_t bnkPat[ARBITRARY_LIMIT];
   oplinst_t oplPat[ARBITRARY_LIMIT];
   size_t i, count;
   
   /*Argument checks*/
   if (argc != 3) printUsage();
   if ((fp = fopen(argv[1], "rb")) == NULL) printError(argv[1]);
   if ((outfp = fopen(argv[2], "wb")) == NULL) printError(argv[2]);
   
   /*Read in BNK instrument*/
   /*TODO seek to data on real BNK file*/
   seekToBNKData(fp);
   
   for(count = 0; !feof(fp) && count < ARBITRARY_LIMIT; ++count)
   {
      size_t bytesRead; 
      
      initBNKPatch(&(bnkPat[count]));
      
      bytesRead = fread(&(bnkPat[count]), 1, sizeof(bnktimbre_t), fp);
      /*printf("DEBUG: bytes read %d vs %d\n", bytesRead, sizeof(bnktimbre_t));*/
      
      if(bytesRead != sizeof(bnktimbre_t) && !feof(fp))
         printFileError(argv[1]);
      else if(feof(fp) && bytesRead != sizeof(bnktimbre_t)) 
         --count;
      
   }
   
   fclose(fp);
   printf("Read %d BNK patches\n", count);
   
   for(i = 0; i < count; ++i)
   {  
      /*Iteratively create, convert, and write each patch*/
      initOPLPatch(&(oplPat[i]));
      convertBNKToOPL(&(oplPat[i]), &(bnkPat[i]));
      
      fwrite(&(oplPat[i]), sizeof(oplinst_t), 1, outfp);
   }
   
   fclose(outfp);
   printf("File %s written.\n", argv[2]);
   
   return EXIT_SUCCESS;
}