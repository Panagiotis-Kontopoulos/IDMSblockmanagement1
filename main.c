/*******************************************************************************
Header File	: main.c
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header_f.h"

#define filetype   1   /* 0 = heap kai 1 = hash */
#define FILENAME_HEAP      "heap"
#define FILENAME_HASH_ID   "hash_id"

int main(int argc, char** argv) 
{
   int option, heapFile, depth, attrLength, open = 0, insert = 0, i, j;
   HT_info *hashId;
   Record record;
   FILE *records;
   char temp_s[20], attrType, attrName[20];
/*************************************************************************************************************************/
   if (filetype)
     {  attrType = 'c';
        strcpy(attrName,"name");
        attrLength = sizeof(char);
        depth = 7;  // megisto ba8os einai 7
	/* Create hash index based on attrName */
        if ( HT_CreateIndex(FILENAME_HASH_ID, attrType, attrName, attrLength, depth) < 0 )
          {  fprintf(stderr, "Error creating hash index.\n");
 	     exit(EXIT_FAILURE);
          }
        /* Open hash index based on attrName */
        if ((hashId = HT_OpenIndex(FILENAME_HASH_ID)) == NULL)
          {  fprintf(stderr, "Error opening hash index.\n");
             HT_CloseIndex(hashId);
             exit(EXIT_FAILURE);
          }
     }
   else
     {
        /* Create heap file */
        if (HP_CreateFile(FILENAME_HEAP) < 0)
          {  fprintf(stderr, "Error creating heap file.\n");
             exit(EXIT_FAILURE);
          }
	/* Open heap file */
        if ((heapFile = HP_OpenFile(FILENAME_HEAP)) < 0) 
          {  fprintf(stderr, "Error opening heap file.\n");
             exit(EXIT_FAILURE);
          }
     }
/*************************************************************************************************************************/
   if ((records = fopen("./records", "r")) == NULL) /* Open records File */
      {  perror("fopen Records-file");
         return 1;
      }
/* Commend one of the below lines {---for (j=0;j<6;j++) |||| -----while (!feof(records)) } and uncomment the other one */
   //for (j=0;j<6;j++)
   while (!feof(records))
      {  fscanf(records, "%d\t", &i);
         record.id = i;
         fscanf(records, "%s\t", temp_s);
         strcpy(record.name, temp_s);
         fscanf(records, "%s\t", temp_s);
         strcpy(record.surname, temp_s);
         fscanf(records, "%s\t", temp_s);
         strcpy(record.status, temp_s);
         fscanf(records, "%s\t", temp_s);
         strcpy(record.dateOfBirth, temp_s);
         fscanf(records, "%d\t", &i);
         record.salary = i;
         fscanf(records, "%s\t", temp_s);
         strcpy(record.section, temp_s);
         fscanf(records, "%d\t", &i);
         record.daysOff = i;
         fscanf(records, "%d\t", &i);
         record.prevYears = i;
         if (filetype)
           {
              /* Insert record in hash index based on id */
	      if (HT_InsertEntry(hashId, record) < 0) 
	        {  fprintf(stderr, "Error inserting entry in hash index\n");
	           HT_CloseIndex(hashId);
	           exit(EXIT_FAILURE);
                }
           }
         else
           {
             /* Insert record in heap file */
             if (HP_InsertEntry(heapFile, record) < 0) 
               {  fprintf(stderr, "Error inserting entry in heap file.\n");
                  HP_CloseFile(heapFile);
                  exit(EXIT_FAILURE);
               }
           }
      }
/*************************************************************************************************************************/
   int value;
   value = 3;
   if (filetype)
     {
        /* Get records from hash index */
        HT_GetAllEntries(hashId, NULL);
     }
   else
     {
        /* Get records from heap file */
        HP_GetAllEntries(heapFile, "name", "Georgios");
     }
/*************************************************************************************************************************/
   if (filetype)
     {
        /* Close id hash index */
        if (HT_CloseIndex(hashId) < 0) 
          {  fprintf(stderr, "Error closing id hash index.\n");
             exit(EXIT_FAILURE);
          }
     }
   else
     {
        /* Close heap file */
        if (HP_CloseFile(heapFile) < 0) 
          {  fprintf(stderr, "Error closing heap file.\n");
             exit(EXIT_FAILURE);
          }
     }
/*************************************************************************************************************************/

}
