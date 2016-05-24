/************************************************************************
Header File	: HP_functions.c
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "header_f.h"

void getRecord(Record *data, FILE *file)  // ektiponei ta dedomena kathe egrafhs sto arxeio txt
{   fprintf(file, "%d  %s  %s  %s  %s  ", data->id, data->name, data->surname, data->status, data->dateOfBirth);
	fprintf(file, "%d  %s  %d  %d\n", data->salary, data->section, data->daysOff, data->prevYears);
}

/*########################################################################################################################*/

int HP_CreateFile(char *filename)
{   void *block;   // h domh me ta dedomena anagnoriseis arxeiou sto proto block tou arxeiou
	int filedesc , i=0;
        info_block i_block;
	BF_Init();

	if (BF_CreateFile(filename) == 0)
	{   filedesc = BF_OpenFile(filename);
	    if (filedesc >= 0)
                if (BF_AllocateBlock(filedesc) == 0)    // desmefsh tou protou block
	        {   if (BF_ReadBlock(filedesc, 0, &block) == 0)  
	            {   i_block.filetype = 0;
                        memcpy(block,&i_block,sizeof(info_block)); // katagrafh ton dedomenon anagnorishs arxeio, sto 1o block
		        if (BF_WriteBlock(filedesc, 0) == 0)    // egrafh tu block sto disko
			  {     if (BF_AllocateBlock(filedesc) == 0)
                                  {    if (BF_ReadBlock(filedesc, 1, &block) == 0)
                                         {   memcpy(block, &i, sizeof(int));
                                             if (BF_WriteBlock(filedesc, 1) < 0)
                                               {   return -1;
                                               }
                                         }
                                       else
                                         {   BF_PrintError("Error reading block");
			                     return -1;
                                         }
                                  }
                                else
				  {   BF_PrintError("Error allocating block");
                                      return -1;
	                           }
				}
				else
				{   BF_PrintError("Error writing block");
				    return -1;
				}
			}
			else
			{   BF_PrintError("Error reading block");
			    return -1;
			}
		}
		else
		{   BF_PrintError("Error Allocating Block");
		    return -1;
		}
	else
	{	BF_PrintError("Error openning file");
		return -1;
	}
	
 }
 else
 	BF_PrintError("Error creating file");
	
	return 0;
}

/*########################################################################################################################*/

int HP_OpenFile( char *fileName)
{
   int fileDesc, filetype;
   void *block;
   fileDesc = BF_OpenFile(fileName);
   if (fileDesc < 0)
     {  BF_PrintError("Error opening file");
        return fileDesc;
     }
   else
     {  if (BF_ReadBlock(fileDesc,0,&block) != 0)
          {  BF_PrintError("Error getting block");
             return -1;
          }
        else
          {  filetype = ((info_block*)block)->filetype;
             if (filetype == 0)
               {  return fileDesc;
               }
             else
               {  printf("Not a Heap File. \n");
                  HP_CloseFile(fileDesc);
                  return -1;
               }
          }
     }
}

/*########################################################################################################################*/

int HP_CloseFile(int fileDesc)
{
	if (BF_CloseFile(fileDesc) == 0)
	    return 0;
	else
	{   BF_PrintError("Error closing file");
	    return -1;
	}
}

/*########################################################################################################################*/

int HP_InsertEntry( int fileDesc, Record record)
{
   int blocks, error, recordspace, freespace;
   void *block;
   /* blocks : krata to plh8os ton block opos dinontai apo thn BF_GetBlockCounter
      recordspace : krata poses eggrafes exei to block
      freespace : krata posos xoros einai eley8eros sto block
   */
   blocks = BF_GetBlockCounter(fileDesc);
   if (blocks > 0)
     {  if (BF_ReadBlock(fileDesc, blocks-1, &block) < 0)
          {  BF_PrintError("Error getting block");
             return -1;
          }
        else
          {  memcpy(&recordspace,block,sizeof(int));
             freespace = BLOCK_SIZE - (recordspace*sizeof(Record) + sizeof(int));
             if (sizeof(Record) < freespace)
               {  memcpy(block+sizeof(int)+(recordspace)*sizeof(Record), &record, sizeof(Record));
                  recordspace = recordspace + 1;
                  memcpy(block, &recordspace, sizeof(int));
                  if (BF_WriteBlock(fileDesc, blocks-1) < 0)
                    {  return -1;
                    }
                  else
                    {  return 0;
                    }
/* An den yparxei la8os pernaei sto block to record pou dinetai */
               }
             else
               {  if (BF_AllocateBlock(fileDesc) == 0)
                    {  blocks = BF_GetBlockCounter(fileDesc);
                       if (blocks > 0)
                         {  if (BF_ReadBlock(fileDesc, blocks - 1, &block) == 0)
                              {  int i;
                                 i = 0;
                                memcpy(block, &i, sizeof(int));
                                if (BF_WriteBlock(fileDesc, 1) < 0)
                                  {  return -1;
                                  }
                                else
                                  {  return HP_InsertEntry(fileDesc, record);
                                  }
                              }
                         }
                       else
                         { return -1;
                         }
/* An to block einai gemato dhmiourgeitai neo block kai mpainei se ayto to record */
                    }
                  else
                    {  BF_PrintError("Error allocating block");
                       return -1;
                    }
               }
          }
     }
   else
     {  BF_PrintError("Error getting blocks");
        return -1;
     }
}

/*########################################################################################################################*/

void HP_GetAllEntries(int fileDesc, char* fieldName, void* value)
{ int counter;
  void *block;
  Record* data;
  int i=0, j, *v, metrhth;
  v = value;
  
// open the txt file
  FILE *fp;
  if ((fp = fopen("./Entries.txt", "w")) == NULL) /* Open Print_Data File */
    { perror("fopen Entries-file");
    }
  data = malloc(sizeof(Record));

  if ((counter = BF_GetBlockCounter(fileDesc)) > 0)
  {	//counter--;  // o afkodas aritmos tu telefteou block tou arxeiou
  
      if (fieldName == NULL && value == NULL)    // periptosh NULL
      {   fprintf(fp, "ALL ENTRIES\n");   // oles oi kataxoriseis
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
				    getRecord(data,fp);   //***************************
				}
			}
			else
				BF_PrintError("Error reading block");

		}
       }
       else if (fieldName != NULL && value != NULL)
       {
	if (strcmp(fieldName, "id") == 0)
	{   fprintf(fp, "ID\n");    // pedio id
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (data->id == *v)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

		}
			
	}
	else if (strcmp(fieldName, "name") == 0)
	{   fprintf(fp, "NAME\n");  // pedio name
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (strcmp(data->name, value) == 0)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");
		}
	}
	else if (strcmp(fieldName, "surname") == 0)
	{   fprintf(fp, "SURNAME\n");   // pedio surname
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (strcmp(data->surname, value) == 0)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");
		}
	}
	else if (strcmp(fieldName, "status") == 0)
	{   fprintf(fp, "STATUS\n");    // pedio status
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (strcmp(data->status, value) == 0)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");
		}

	}
	else if (strcmp(fieldName, "dateOfBirth") == 0)
	{   fprintf(fp, "DATE OF BIRTH\n"); // pedio dateOfBirth
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (strcmp(data->dateOfBirth, value) == 0)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

		}
			
	}
	else if (strcmp(fieldName, "salary") == 0)
	{   fprintf(fp, "SALARY\n");    // pedio salary
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (data->salary == *v)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

		}
	}
	else if (strcmp(fieldName, "section") == 0)
	{    fprintf(fp, "SECTION\n");   // pedio section
             for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
   	     {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (strcmp(data->section, value) == 0)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

	     }
        }
	else if (strcmp(fieldName, "daysOff") == 0)
	{   fprintf(fp, "DAYS OFF\n");  // pedio daysOff
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (data->daysOff == *v)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

		}
	}
	else if (strcmp(fieldName, "prevYears") == 0)
	{   fprintf(fp, "PREVIOUS YEARS\n");    // pedio prevYears
            for (i=1; i < counter; i++)   // vgazo ektos to proto block pu einai gia th pliroforia anagnorishs arxeiou
	    {   if (BF_ReadBlock(fileDesc, i, &block) == 0)
			{   memcpy(&metrhth, block,sizeof(int));    // fortosh tu block sth mnhmh
				for(j=0; j < metrhth/*h egrafes se kathe block dedomenon*/; j++)
				{   memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                    if (data->prevYears == *v)
				    {
						getRecord(data,fp);   //***************************
					}
				}
			}
			else
				BF_PrintError("Error reading block");

		}
	}
    }
 }
 else
 	BF_PrintError("Error getting block counter");
printf("Plh8os blocks poy diabastikan = %d \n", i-1);
fclose(fp);
free(data);
}
