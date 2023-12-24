#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARBITRARY_LIMIT 1024 /*TODO use BNK count*/

#pragma pack(1)

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
   char      mode;             /* TODO verify */
   char      percVoice;        /* TODO verify */
   bnkop_t   op0;
   bnkop_t   op1;
   char      wave0;            /* wave form for operator 0 */
   char      wave1;            /* wave form for operator 1 */
} bnktimbre_t;

void printUsage()
{
   printf("Convert .BNK instruments to OPL registers\n"
      "Usage: <BNK file> <TXT file> <Conversion mode>\n"
      "where:\n"
      "\t1 - (default) Converts to VGMCK MML for YM3812\n"
      "\t2 - Converts to VGMCK MML for YMF262\n"
      "\t3 - Converts to SiON MML for OPL\n"
      "\t4 - Converts to SiON MML for OPN\n"
      "\t5 - Converts to SiON MML for OPM\n"
      "\t6 - Converts to SiON MML for OPX\n"
      "\t7 - Converts to SiON MML for MA3\n"
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
 * Take offset at 0x10 of BNK, and then seek to location value  
 */
void seekToBNKData(FILE *fp)
{
   size_t offset;
   
   fseek(fp, 0x10, SEEK_SET);
   fread(&offset, sizeof(int), 1, fp);
   fseek(fp, offset, SEEK_SET);
   printf("DEBUG: reading from offset %x\n", (unsigned int)ftell(fp));
}

char getFbkAlgMode(bnktimbre_t *bnkPat)
{
   return (bnkPat->op0.feedback << 1)
      |((bnkPat->op0.con == 0) ? 1 : 0);
}

char getWaveform(bnktimbre_t *bnkPat, int op)
{
   switch(op)
   {
      case 0:
         return bnkPat->wave0;
      case 1:
         return bnkPat->wave1;
   }
   
   return ~0;
}

char getSusRel(bnktimbre_t *bnkPat, int op)
{
   switch(op)
   {
      case 0:
         return (bnkPat->op0.sustain << 4)
            |(bnkPat->op0.releaseRate);
      case 1:
         return (bnkPat->op1.sustain << 4)
            |(bnkPat->op1.releaseRate);
   }
   
   return ~0;
}

char getAttDec(bnktimbre_t *bnkPat, int op)
{
   switch(op)
   {
      case 0:
         return (bnkPat->op0.attack << 4)
            |(bnkPat->op0.decay);
      case 1:
         return (bnkPat->op1.attack << 4)
            |(bnkPat->op1.decay);
   }
   
   return ~0;
}

char getKSLTL(bnktimbre_t *bnkPat, int op)
{
   switch(op)
   {
      case 0:
         return (bnkPat->op0.ksl<<6)
            |(bnkPat->op0.totalLevel);
      case 1:
         return (bnkPat->op1.ksl<<6)
            |(bnkPat->op1.totalLevel);
   }
   
   return ~0;
}
   

char getTVSKSRFM(bnktimbre_t *bnkPat, int op)
{
   switch(op)
   {
      case 0:
         return (bnkPat->op0.multiple)
            |((bnkPat->op0.eg!=0)?(1<<5):0)
            |(bnkPat->op0.am << 7)
            |(bnkPat->op0.vib << 6)
            |(bnkPat->op0.ksr << 4);
      case 1:
         return (bnkPat->op1.multiple)
            |((bnkPat->op1.eg!=0)?(1<<5):0)
            |(bnkPat->op1.am << 7)
            |(bnkPat->op1.vib << 6)
            |(bnkPat->op1.ksr << 4);
   }
   
   return ~0;
}

char *getAlgString(bnktimbre_t *bnkPat)
{
   static char str[ARBITRARY_LIMIT];
   
   return (bnkPat->op0.con == 0) ? 
      strncpy(str, "(a+b)", ARBITRARY_LIMIT) :
      strncpy(str, "(a:b)", ARBITRARY_LIMIT);
}

void outVGMCKOPL2(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n@x%u = { ; \n" /*TODO read instrument names*/
      "  $%02X $%02X $%02X $%02X $%02X \n"
      "  $%02X $%02X $%02X $%02X $%02X \n"
      "  $%02X ; %s\n"
      "}\n",
      
      (unsigned int)index,
      getTVSKSRFM(bnkPat, 0) & 0xFF, getTVSKSRFM(bnkPat, 1) & 0xFF, 
      getKSLTL(bnkPat, 0) & 0xFF, getKSLTL(bnkPat, 1) & 0xFF,
      getAttDec(bnkPat, 0) & 0xFF, getAttDec(bnkPat, 1) & 0xFF,
      getSusRel(bnkPat, 0) & 0xFF, getSusRel(bnkPat, 1) & 0xFF,
      getWaveform(bnkPat, 0) & 0xFF, getWaveform(bnkPat, 1) & 0xFF,
      getFbkAlgMode(bnkPat) & 0xFF, getAlgString(bnkPat)
   );
}

void outVGMCKOPL3(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n@x%u = { ; \n" /*TODO read instrument names*/
      "  $%02X $%02X $%02X $%02X $%02X \n"
      "  $%02X $%02X $%02X $%02X $%02X \n"
      "  $00 $00 $00 $00 $00 \n"
      "  $00 $00 $00 $00 $00 \n"
      "  $%02d $00; %s\n"
      "}\n",
      
      (unsigned int)index,
      getTVSKSRFM(bnkPat, 0) & 0xFF, getKSLTL(bnkPat, 0) & 0xFF,
      getAttDec(bnkPat, 0) & 0xFF, getSusRel(bnkPat, 0) & 0xFF, 
      getWaveform(bnkPat, 0) & 0xFF, 
      
      getTVSKSRFM(bnkPat, 1) & 0xFF, getKSLTL(bnkPat, 1) & 0xFF,
      getAttDec(bnkPat, 1) & 0xFF, getSusRel(bnkPat, 1) & 0xFF,
      getWaveform(bnkPat, 1) & 0xFF,
      
      getFbkAlgMode(bnkPat), getAlgString(bnkPat)
   );
}

/*
 * Value mappings retrieved from the following (C) keim_at_Si
 * http://mmltalks.appspot.com/document/siopm_mml_ref_05_e.html
 */
void outSiONOPL(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n#OPL@%u{%d %d //\n" /*TODO read instrument names*/
      "  %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d\n"
      "  %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d\n"
      "};\n",
      
      (unsigned int)index,
      ((bnkPat->op0.con==0) ? 1 : 0), bnkPat->op0.feedback & 0x7,
      bnkPat->wave0 & 0x7, bnkPat->op0.attack & 0xF, bnkPat->op0.decay & 0xF, 
      bnkPat->op0.releaseRate & 0xF, bnkPat->op0.eg & 0x1, bnkPat->op0.sustain & 0xF,
      bnkPat->op0.totalLevel & 0x3F, bnkPat->op0.ksr & 0x1, bnkPat->op0.ksl & 0x3,
      bnkPat->op0.multiple & 0xF, bnkPat->op0.am & 0x3,
      
      bnkPat->wave1 & 0x7, bnkPat->op1.attack & 0xF, bnkPat->op1.decay & 0xF,  
      bnkPat->op1.releaseRate & 0xF, bnkPat->op1.eg & 0x1, bnkPat->op1.sustain & 0xF,
      bnkPat->op1.totalLevel & 0x3F, bnkPat->op1.ksr & 0x1, bnkPat->op1.ksl & 0x3,
      bnkPat->op1.multiple & 0xF, bnkPat->op1.am & 0x3
   );
}

void outSiONOPN(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n#OPN@%u{%d %d //\n" /*TODO read instrument names*/
      "  %02d %02d 00 %02d %02d %02d %02d %02d 00 %02d\n"
      "  %02d %02d 00 %02d %02d %02d %02d %02d 00 %02d\n"
      "};\n",
      
      (unsigned int)index,
      ((bnkPat->op0.con==0) ? 1 : 0), bnkPat->op0.feedback & 0x7,
      (bnkPat->op0.attack<<1) & 0x1F, (bnkPat->op0.decay<<1) & 0x1F, 
      bnkPat->op0.releaseRate & 0xF, bnkPat->op0.sustain & 0xF,
      (bnkPat->op0.totalLevel<<1) & 0x7F, bnkPat->op0.ksl & 0x3,
      bnkPat->op0.multiple & 0xF, bnkPat->op0.am & 0x3,
      
      (bnkPat->op1.attack<<1) & 0x1F, (bnkPat->op1.decay<<1) & 0x1F, 
      bnkPat->op1.releaseRate & 0xF, bnkPat->op1.sustain & 0xF,
      (bnkPat->op1.totalLevel<<1) & 0x7F, bnkPat->op1.ksl & 0x3,
      bnkPat->op1.multiple & 0xF, bnkPat->op1.am & 0x3
   );
}

void outSiONOPM(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n#OPM@%u{%d %d //\n" /*TODO read instrument names*/
      "  %02d %02d 00 %02d %02d %02d %02d %02d 00 00 %02d\n"
      "  %02d %02d 00 %02d %02d %02d %02d %02d 00 00 %02d\n"
      "};\n",
      
      (unsigned int)index,
      ((bnkPat->op0.con==0) ? 1 : 0), bnkPat->op0.feedback & 0x7,
      (bnkPat->op0.attack<<1) & 0x1F, (bnkPat->op0.decay<<1) & 0x1F, 
      bnkPat->op0.releaseRate & 0xF, bnkPat->op0.sustain & 0xF,
      (bnkPat->op0.totalLevel<<1) & 0x7F, bnkPat->op0.ksl & 0x3,
      bnkPat->op0.multiple & 0xF, bnkPat->op0.am & 0x3,
      
      (bnkPat->op1.attack<<1) & 0x1F, (bnkPat->op1.decay<<1) & 0x1F, 
      bnkPat->op1.releaseRate & 0xF, bnkPat->op1.sustain & 0xF,
      (bnkPat->op1.totalLevel<<1) & 0x7F, bnkPat->op1.ksl & 0x3,
      bnkPat->op1.multiple & 0xF, bnkPat->op1.am & 0x3
   );
}

void outSiONOPX(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n#OPX@%u{%d %d //\n" /*TODO read instrument names*/
      "  %02d %02d %02d 00 %02d %02d %02d %02d %02d 00 00 %02d\n"
      "  %02d %02d %02d 00 %02d %02d %02d %02d %02d 00 00 %02d\n"
      "};\n",
      
      (unsigned int)index,
      ((bnkPat->op0.con==0) ? 2 : 0), bnkPat->op0.feedback & 0x7,
      bnkPat->wave0 & 0x7, (bnkPat->op0.attack<<1) & 0x1F, (bnkPat->op0.decay<<1) & 0x1F, 
      bnkPat->op0.releaseRate & 0xF, bnkPat->op0.sustain & 0xF,
      (bnkPat->op0.totalLevel<<1) & 0x7F, bnkPat->op0.ksl & 0x3,
      bnkPat->op0.multiple & 0xF, bnkPat->op0.am & 0x3,
      
      bnkPat->wave1 & 0x7, (bnkPat->op1.attack<<1) & 0x1F, (bnkPat->op1.decay<<1) & 0x1F, 
      bnkPat->op1.releaseRate & 0xF, bnkPat->op1.sustain & 0xF,
      (bnkPat->op1.totalLevel<<1) & 0x7F, bnkPat->op1.ksl & 0x3,
      bnkPat->op1.multiple & 0x1F, bnkPat->op1.am & 0x3
   );
}
   
void outSiONMA3(FILE *outfp, bnktimbre_t *bnkPat, size_t index)
{
   fprintf(outfp,
      "\n#OPX@%u{%d %d //\n" /*TODO read instrument names*/
      "  %02d %02d %02d 00 %02d %02d %02d %02d %02d %02d 00 %02d\n"
      "  %02d %02d %02d 00 %02d %02d %02d %02d %02d %02d 00 %02d\n"
      "};\n",
      
      (unsigned int)index,
      ((bnkPat->op0.con==0) ? 1 : 0), bnkPat->op0.feedback & 0x7,
      bnkPat->wave0 & 0x7, bnkPat->op0.attack & 0xF, bnkPat->op0.decay & 0xF, 
      bnkPat->op0.releaseRate & 0xF, bnkPat->op0.sustain & 0xF,
      bnkPat->op0.totalLevel & 0x3F, bnkPat->op0.ksr & 0x1, bnkPat->op0.ksl & 0x3,
      bnkPat->op0.multiple & 0xF, bnkPat->op0.am & 0x3,
      
      bnkPat->wave1 & 0x7, bnkPat->op1.attack & 0xF, bnkPat->op1.decay & 0xF, 
      bnkPat->op1.releaseRate & 0xF, bnkPat->op1.sustain & 0xF,
      bnkPat->op1.totalLevel & 0x3F, bnkPat->op1.ksr & 0x1, bnkPat->op1.ksl & 0x3,
      bnkPat->op1.multiple & 0xF, bnkPat->op1.am & 0x3
   );
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

int main(int argc, char *argv[])
{
   FILE *fp, *outfp;
   bnktimbre_t bnkPat[ARBITRARY_LIMIT];
   size_t i, count;
   void (*convertBNKToMML)(FILE*,bnktimbre_t*,size_t);
   
   /*Argument checks*/
   if (argc != 4) printUsage();
   if ((fp = fopen(argv[1], "rb")) == NULL) printError(argv[1]);
   if ((outfp = fopen(argv[2], "w")) == NULL) printError(argv[2]);
   
   /*Read in BNK instrument*/
   seekToBNKData(fp);
   
   for(count = 0; !feof(fp) && count < ARBITRARY_LIMIT; ++count)
   {
      size_t bytesRead; 
      
      /*memset(&(bnkPat[count]), 0, sizeof(bnktimbre_t));*/
      initBNKPatch(&(bnkPat[count]));
      
      bytesRead = fread(&(bnkPat[count]), 1, sizeof(bnktimbre_t), fp);
      /*printf("DEBUG: bytes read %d vs %d\n", bytesRead, sizeof(bnktimbre_t));*/
      
      if(bytesRead != sizeof(bnktimbre_t) && !feof(fp))
         printFileError(argv[1]);
      else if(feof(fp) && bytesRead != sizeof(bnktimbre_t)) 
         --count;
      
   }
   
   fclose(fp);
   printf("Read %u BNK patches\n", (unsigned int)count);
   
   /*Set conversion callback based on parameter*/
   switch(((size_t)strtol(argv[3], NULL, 10)))
   {
      case 1:
         fprintf(outfp, 
            "#TITLE-E Patches from %s\n"
            "#EX-OPL2 ABCDEFGHI\n",
            argv[1]
         );
         convertBNKToMML = outVGMCKOPL2;
         break;
      
      case 2:
         fprintf(outfp, 
            "#TITLE-E Patches from %s\n"
            "#EX-OPL3 ABCDEFGHIJKLMNOPQR\n",
            argv[1]
         );
         convertBNKToMML = outVGMCKOPL3;
         break;
      case 3:
         fprintf(outfp, "#TITLE{Patches from %s};\n", argv[1]);
         convertBNKToMML = outSiONOPL;
         break;
      case 4:
         fprintf(outfp, "#TITLE{Patches from %s};\n", argv[1]);
         convertBNKToMML = outSiONOPN;
         break;
      case 5:
         fprintf(outfp, "#TITLE{Patches from %s};\n", argv[1]);
         convertBNKToMML = outSiONOPM;
         break;
      case 6:
         fprintf(outfp, "#TITLE{Patches from %s};\n", argv[1]);
         convertBNKToMML = outSiONOPX;
         break;
      case 7:
         fprintf(outfp, "#TITLE{Patches from %s};\n", argv[1]);
         convertBNKToMML = outSiONMA3;
         break;
         
      default:
         printf("Invalid mode.");
         printUsage();
         break;
   }
   
   for(i = 0; i < count; ++i)
   {  
      /*Iteratively create, convert, and write each patch*/
      convertBNKToMML(outfp, &(bnkPat[i]), i);
   }
   
   fclose(outfp);
   printf("File %s written.\n", argv[2]);
   
   return EXIT_SUCCESS;
}