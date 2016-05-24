/************************************************************************
Header File	: header_f.h
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"

typedef struct {
     int fileDesc;  // anagnoristikos aritmos anigmatos arxeiou apo to epipedo block
     char attrType; // o tipos tu pediou pou einai to klidi gia to sigekrimeno arxeio, 'c' h 'i'
     char *attrName;    // to onoma tu pediu pu einai to klidi gia to sigekrimeno arxeio
     int attrLength;    // to megethos tou pediu pu einai to klidi gia to sigekrimeno arxeio
     int depth; // to oliko vathos tou evretiriou
} HT_info;

typedef struct{
   int id;
   char name[15];
   char surname[20];
   char status[2]; // allagh apo 1 se 2 tou plh8ous ton 8eseon oste na diabazetai os stirng kai na yparxei xoros gia ton "\0" xarakthra
   char dateOfBirth[10];
   int salary;
   char section[2]; // allagh apo 1 se 2 tou plh8ous ton 8eseon oste na diabazetai os stirng kai na yparxei xoros gia ton "\0" xarakthra
   int daysOff;
   int prevYears;
} Record;

typedef struct {
   int filetype; //0 = heap , 1 = hash

} info_block;       // domh anagnorishs tou typou tou arxeiou

void getRecord( Record*, FILE*);  // diaxeirizetai thn egrafei pu teriazei me to "kleidi"
int hash_function( void*, int, int);
int power_function( int, int);
int get_overflow_buckets( HT_info*, int, Record*, FILE*);
int hash_id_search(HT_info* , char* , Record* , FILE* , int);


//******************************************************************************

/* Heap File Functions */
int HP_CreateFile( char *);
int HP_OpenFile( char *);
int HP_CloseFile( int);
int HP_InsertEntry( int, Record);
void HP_GetAllEntries( int, char *, void *);

/* Hash Table Functions */
int HT_CreateIndex( char *, char, char *, int, int);
HT_info* HT_OpenIndex( char *);
int HT_CloseIndex( HT_info *);
int HT_InsertEntry( HT_info*, Record);
void HT_GetAllEntries( HT_info*, void *);

