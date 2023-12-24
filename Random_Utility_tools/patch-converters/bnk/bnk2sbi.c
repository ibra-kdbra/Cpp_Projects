#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARBITRARY_LIMIT 20000 /*TODO use BNK count*/

#pragma pack(1)

typedef struct
{
   uint8_t file_id[4],
           instrument[32],
           Mcharacteristics,
           Ccharacteristics,
           MScalingOutput,
           CScalingOutput,
           MAttackDecay,
           CAttackDecay,
           MSustainRelease,
           CSustainRelease,
           MWave,
           CWave,
           FeedConnect,
           reserved[6];
} sbi_t;

/*
 * Structures below stolen from MF2ROL (C) 1991 Alejandro Kapauan
 * Also referenced format info from
 * http://www.shikadi.net/moddingwiki/BNK_Format_(Ad_Lib)
 * note: some information was incomplete/incorrect on site
 */
typedef struct
{
   char ksl;
   char multiple;
   char feedback; /* used by operator 0 only */
   char attack;
   char sustain;
   char eg;
   char decay;
   char releaseRate;
   char totalLevel;
   char am;
   char vib;
   char ksr;
   char con; /* used by operator 0 only */
} bnkop_t;

typedef struct
{
   char mode;      /* not used */
   char percVoice; /* not used */
   bnkop_t op0;
   bnkop_t op1;
   char wave0; /* wave form for operator 0 */
   char wave1; /* wave form for operator 1 */
} bnktimbre_t;

void printUsage()
{
   printf("Convert .BNK instruments to SBI patches\n"
          "Usage: <BNK file>\n");
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
void initBNKPatch(bnktimbre_t *sbi)
{
   sbi->mode = 0;
   sbi->percVoice = 0;
   initBNKOperator(&sbi->op0);
   initBNKOperator(&sbi->op1);
   sbi->wave0 = 0;
   sbi->wave1 = 0;
}

/*
 * Initialize OPL patch struct
 */
void initSBIPatch(sbi_t *sbi)
{
   size_t i;
   sbi->file_id[0] = 'S';
   sbi->file_id[1] = 'B';
   sbi->file_id[2] = 'I';
   sbi->file_id[3] = 0x1A;
   for (i = 0; i < 32; ++i)
      sbi->instrument[i] = 0;
   sbi->Mcharacteristics = 0; /*0x20*/
   sbi->MScalingOutput = 0; /*0x40*/
   sbi->MAttackDecay = 0; /*0x60*/
   sbi->MSustainRelease = 0; /*0x80*/
   sbi->MWave = 0; /*0xE0*/
   sbi->Ccharacteristics = 0; /*0x23*/
   sbi->CScalingOutput = 0; /*0x43*/
   sbi->CAttackDecay = 0; /*0x63*/
   sbi->CSustainRelease = 0; /*0x83*/
   sbi->CWave = 0; /*0xE3*/
   for (i = 0; i < 6; ++i)
      sbi->reserved[i] = 0;
   sbi->FeedConnect = 0; /*0xC0*/
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
 * Take offset at 0x0c of BNK, and then seek to location value
 */
void seekToBNKPatchNames(FILE *fp)
{
   size_t offset;

   fseek(fp, 0x0c, SEEK_SET);
   fread(&offset, sizeof(int), 1, fp);
   fseek(fp, offset, SEEK_SET);
   printf("DEBUG: reading from offset %x\n", ftell(fp));
}

/*
 * Conversion routine for register information with accordance to
 * the OPL3 programming guide.
 */
void convertBNKtoSBI(FILE *fp, sbi_t *sbiPat, bnktimbre_t *bnkPat)
{
   char patch_name[9];
   /*Operator 0*/
   sbiPat->MScalingOutput |= (bnkPat->op0.ksl << 6);
   sbiPat->Mcharacteristics |= (bnkPat->op0.multiple);
   sbiPat->FeedConnect |= (bnkPat->op0.feedback << 1);
   sbiPat->MAttackDecay |= (bnkPat->op0.attack << 4);
   sbiPat->MSustainRelease |= (bnkPat->op0.sustain << 4);
   sbiPat->Mcharacteristics |= (bnkPat->op0.eg != 0) ? (1 << 5) : 0;
   sbiPat->MAttackDecay |= (bnkPat->op0.decay);
   sbiPat->MSustainRelease |= (bnkPat->op0.releaseRate);
   sbiPat->MScalingOutput |= (bnkPat->op0.totalLevel);
   sbiPat->Mcharacteristics |= (bnkPat->op0.am << 7);
   sbiPat->Mcharacteristics |= (bnkPat->op0.vib << 6);
   sbiPat->Mcharacteristics |= (bnkPat->op0.ksr << 4);
   sbiPat->FeedConnect |= (bnkPat->op0.con == 0) ? 1 : 0;
   sbiPat->MWave |= (bnkPat->wave0);

   /*Operator 1*/
   sbiPat->CScalingOutput |= (bnkPat->op1.ksl << 6);
   sbiPat->Ccharacteristics |= (bnkPat->op1.multiple);

   sbiPat->CAttackDecay |= (bnkPat->op1.attack << 4);
   sbiPat->CSustainRelease |= (bnkPat->op1.sustain << 4);
   sbiPat->Ccharacteristics |= (bnkPat->op1.eg != 0) ? (1 << 5) : 0;
   sbiPat->CAttackDecay |= (bnkPat->op1.decay);
   sbiPat->CSustainRelease |= (bnkPat->op1.releaseRate);
   sbiPat->CScalingOutput |= (bnkPat->op1.totalLevel);
   sbiPat->Ccharacteristics |= (bnkPat->op1.am << 7);
   sbiPat->Ccharacteristics |= (bnkPat->op1.vib << 6);
   sbiPat->Ccharacteristics |= (bnkPat->op1.ksr << 4);

   sbiPat->CWave |= (bnkPat->wave1);
   fseek(fp, 3, SEEK_CUR); /* patch number padding (assuming it's all in order) */
   fread(patch_name, 9, 1, fp); /* 8 character, 9th character null termination */
   patch_name[8] = 0;
   strncpy((char*)(sbiPat->instrument), patch_name, 9); /* write it all in */
}

int main(int argc, char *argv[])
{
   FILE *fp, *outfp;
   bnktimbre_t bnkPat[ARBITRARY_LIMIT];
   sbi_t sbiPat[ARBITRARY_LIMIT];
   size_t i, count;

   /*Argument checks*/
   if (argc != 2)
      printUsage();
   if ((fp = fopen(argv[1], "rb")) == NULL)
      printError(argv[1]);
   
   /*Read in BNK instrument*/
   /*TODO seek to data on real BNK file*/
   seekToBNKData(fp);

   for (count = 0; !feof(fp) && count < ARBITRARY_LIMIT; ++count)
   {
      size_t bytesRead;

      initBNKPatch(&(bnkPat[count]));

      bytesRead = fread(&(bnkPat[count]), 1, sizeof(bnktimbre_t), fp);
      /*printf("DEBUG: bytes read %d vs %d\n", bytesRead, sizeof(bnktimbre_t));*/

      if (bytesRead != sizeof(bnktimbre_t) && !feof(fp))
         printFileError(argv[1]);
      else if (feof(fp) && bytesRead != sizeof(bnktimbre_t))
         --count;
   }

   printf("Read %d BNK patches\n", count);
   seekToBNKPatchNames(fp);

   for (i = 0; i < count; ++i)
   {
      char out_filename[80];
      sprintf(out_filename, "%s_%d.sbi", argv[1], i);
      if ((outfp = fopen(out_filename, "wb")) == NULL) {
         printError(out_filename);
         return EXIT_FAILURE;
      }
      
      /*Iteratively create, convert, and write each patch*/
      initSBIPatch(&(sbiPat[i]));
      convertBNKtoSBI(fp, &(sbiPat[i]), &(bnkPat[i]));

      fwrite(&(sbiPat[i]), sizeof(sbi_t), 1, outfp);
      fclose(outfp);
      printf("File %s written.\n", out_filename);
   }

   

   return EXIT_SUCCESS;
}