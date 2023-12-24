/*
 *  Creative IBK to MSOPL/JungleVision instrument bank
 *  For MidiPlay / Windows OPL3 MIDI driver
 * 
 *  TODO: mode to export as patch.h for WinFM software OPL3 driver
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARBITRARY_LIMIT 128

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
 * Perc bank map struct
 */
typedef struct
{
   char percvoc,
        drum_pitch;
} oplpercmap_t;

/*
 * Structures below stolen from (C) 1991 Jamie O'Connell
 */
typedef struct 
{
   char modchar;   /* AM, VIB, SUS, KSR, MUL */
   char carchar;
   char modscal;   /* KSL, TL */
   char carscal;
   char modad;     /* Attack/Decay */
   char carad;
   char modsr;     /* Sustain/Release */
   char carsr;
   char modwave;   /* Wave Select */
   char carwave;
   char feedback;  /* FB, Connection *(inverse of Adlib)* <- not true? */
   char percvoc;   /* Percussion voice number                    : JWO */
   char transpos;  /* Number of notes to transpose timbre, signed: JWO */
   char dpitch;    /* percussion pitch: MIDI Note 0 - 127        : JWO */
   char rsv[2];    /* unsused - so far */
} ibktimbre_t;     

/* IBK Instrument Bank File */
typedef struct 
{                     /* 3204 Bytes (0x0C83) */
   char     sig[4];             /* signature: "IBK\x1A"  */
   ibktimbre_t snd[128];           /* Instrument block */
   char     name[128][9];       /* name block: NUL terminated strings */
} ibk_t;

void printUsage()
{
   printf("Convert .IBK instruments to OPL registers\n"
      "Usage: <IBK file> <OP3 file> [-header]\n"
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
 * Conversion routine for register information with accordance to
 * the OPL3 programming guide.
 */
void convertIBKToOPL(oplinst_t *oplPat, ibktimbre_t *ibkPat)
{
   /*char tvsksrm0 = 0, tvsksrm1 = 0, //0x20
          ksclout0 = 0, ksclout1 = 0, //0x40
          attdecy0 = 0, attdecy1 = 0, //0x60
          susrels0 = 0, susrels1 = 0, //0x80
          wavslct0 = 0, wavslct1 = 0, //0xE0
          fbkmodsyn = 0;              //0xC0
   */
   /*Operator 0*/
   oplPat->tvsksrm0 |= ibkPat->modchar;
   oplPat->ksclout0 |= ibkPat->modscal;
   oplPat->attdecy0 |= ibkPat->modad;
   oplPat->susrels0 |= ibkPat->modsr;
   oplPat->wavslct0 |= ibkPat->modwave;
   
   /*Operator 1*/
   oplPat->tvsksrm1 |= ibkPat->carchar;
   oplPat->ksclout1 |= ibkPat->carscal;
   oplPat->attdecy1 |= ibkPat->carad;
   oplPat->susrels1 |= ibkPat->carsr;
   oplPat->wavslct1 |= ibkPat->carwave;
   
   oplPat->fbkmodsyn |= ibkPat->feedback;
}

int main(int argc, char *argv[])
{
   FILE *fp, *outfp;
   ibk_t readBank;
   oplinst_t oplPat[ARBITRARY_LIMIT];
   oplpercmap_t oplPercMap[ARBITRARY_LIMIT];
   
   size_t i, j, num_patches;
   
   /*Argument checks*/
   if (argc < 3 || argc > 4) printUsage();
   if ((fp = fopen(argv[1], "rb")) == NULL) printError(argv[1]);
   if ((outfp = fopen(argv[2], "wb")) == NULL) printError(argv[2]);
   
   /*Read IBK*/
   fread(&readBank, 1, sizeof(ibk_t), fp);
   
   fclose(fp);
   
   for(i = 0; i < ARBITRARY_LIMIT; ++i)
   {  
      /*Iteratively create, convert, and write each patch*/
      initOPLPatch(&(oplPat[i]));
      convertIBKToOPL(&(oplPat[i]), &(readBank.snd[i]));
      
      if (argc == 3)
         fwrite(&(oplPat[i]), sizeof(oplinst_t), 1, outfp);
      else
      {
         oplPercMap[i].percvoc = /*(readBank.snd[i].percvoc) ? (readBank.snd[i].percvoc + 35) : 0;*/i;
         oplPercMap[i].drum_pitch = readBank.snd[i].dpitch;
      }
         
   }
   
   num_patches = i;
   
   /* Write patch.h header file */
   if (argc == 4 && strcmp(argv[3],"-header")==0)
   {
      fprintf(outfp, "/* %s - %d patches */\n\n"
         "// Percussion key/pitch map\n"
         "//    [0] - Patch preset from position 128 to 255 (where n=0, patch = 128)\n"
         "//    [1] - Fixed MIDI note to play patch\n"
         "BYTE BCODE gbPercMap[128][2] = \n{\n   ",
         
         argv[1], (int)num_patches
      );
      
      
      for (i = 0; i < ARBITRARY_LIMIT; ++i)
      {
         fprintf(outfp, "{ %3d, %3d }, ", /*(int)i*/oplPercMap[i].percvoc, oplPercMap[i].drum_pitch & 0xFF);
         if (i > 0 && (i+1) % 12 == 0) fprintf(outfp, "\n   ");
      }
      
      fprintf(outfp, "\n};\n\n"
         "// Operator voices\n"
         "patchStruct glpPatch[] = \n{\n   "
      );
      
      for (i = 0; i < ARBITRARY_LIMIT; ++i)
      {
         fprintf(outfp, "{ ");
         
         for (j = 0; j < 28; ++j) 
         {
            fprintf(outfp, "0x%02X%c", ((char*)(&(oplPat[i])))[j] & 0xFF, (j<28)?',':' ');
         };
         fprintf(outfp, " },\n   ");
      }
      fprintf(outfp, "\n};\n");
      
   }
   
   fclose(outfp);
   printf("File %s written.\n", argv[2]);
   
   return EXIT_SUCCESS;
}