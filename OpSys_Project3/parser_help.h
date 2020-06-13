#ifndef PARSER_HELP_H /* Include guard */
#define PARSER_HELP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct __attribute__((packed)) BPB
{
     uint8_t   BS_impBoot[3];
     uint8_t   BS_OEMName[8];
     uint16_t  BPB_BytsPerSec;  // Important
     uint8_t   BPB_SecPerClus;  // Important
     uint16_t  BPB_RsvdSecCnt;  // Important
     uint8_t   BPB_NumFATs;     // Important
     uint16_t  BPB_RootEntCnt;
     uint16_t  BPB_TotSec16;
     uint8_t   BPB_Media;
     uint16_t  BPB_FATSz16;
     uint16_t  BPB_SecPerTrk;
     uint16_t  BPB_NumHeads;
     uint32_t  BPB_HiddSec;
     uint32_t  BPB_TotSec32;   // Important
     uint32_t  BPB_FATSz32;    // Important
     uint16_t  BPB_ExtFlags;
     uint16_t  BPB_FSVer;
     uint32_t  BPB_RootClus;   // Important
     uint16_t  BPB_FSInfo;
     uint16_t  BPB_BkBootSec;
     uint8_t   BPB_Reserved[12];
     uint8_t   BS_DrvNum;
     uint8_t   BS_Reserved1;
     uint8_t   BS_BootSig;
     uint32_t  BS_VolID;
     uint8_t   BS_VolLab[11];
     uint8_t   BS_FilSysType[8];
};

struct rootData
{
     uint32_t FirstSectorOfCluster;  // First sector of cluster in root directory
     uint32_t CurrentLocation;  // Current Cluster we're in
     uint32_t LastLocation; // Location of Parent Cluster
};

struct LocationStack
{
     uint32_t CurrentLoc[100];
     int top;
};

struct currDir
{
     uint32_t ClusMap[1000];
     int cmSize;
};

struct __attribute__((packed)) direcEnt
{
     char DIR_Name[11];
     uint8_t DIR_Attr;
     uint8_t DIR_NTRes;
     uint8_t DIR_CtrTimeTenth;
     uint16_t DIR_CrtTime;
     uint16_t DIR_CrtData;
     uint16_t DIR_LstAccDate;
     uint16_t DIR_FstClusHI;
     uint16_t DIR_WrtTime;
     uint16_t DIR_WrtDate;
     uint16_t DIR_FstClusLO;
     uint32_t DIR_FileSize;
};

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;



int getFATOffset(int FATloc);
int getDATAOffset(int FatClus);
int getFreeFAT(char *fileName);
int isClusFull(struct LocationStack* loc, struct currDir* cwd, char* fileName);
int ifDir(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
int ifFile(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void writeNewFile(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void creatDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void writeNewDir(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void enlargeDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void creatFile(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void printEnt(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void getLS(struct LocationStack * loc, struct direcEnt* direc, instruction* instr_ptr, struct currDir* cwd, char* fileName);
void getClusters(char *fileName, struct LocationStack *loc, struct currDir* cwd);
void push(struct LocationStack* loc, int location);
void pop(struct LocationStack* loc);
void changeDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd);
void getSize(struct BPB* b, struct LocationStack* loc, char* fileName, struct direcEnt* direc, instruction* instr_ptr, struct currDir* cwd);
void extractDirectoryEntries(struct BPB* b, struct LocationStack* loc, char* fileName, struct direcEnt* direc, instruction* instr_ptr);
void getRootDirectory(struct BPB* b, struct LocationStack* loc);
void parseBS(struct BPB* b, char* fileName);
void checkTokens(instruction *instr_ptr, struct BPB* b, struct direcEnt* direc, struct LocationStack* loc, char* fileName, struct currDir* cwd);
void printInfo(struct BPB* b);
void parseInput(struct BPB* b, char* fileName);
void printMenu(void);
void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

#endif
