/*******************************************************************************
Header File	: HT_functions.c
********************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "header_f.h"

void getRecord(Record *data, FILE *file)  // prints data of every record in txt file
{   fprintf(file, "%d  %s  %s  %s  %s  ", data->id, data->name, data->surname, data->status, data->dateOfBirth);
	fprintf(file, "%d  %s  %d  %d\n", data->salary, data->section, data->daysOff, data->prevYears);
}

int power_function(int base, int power)
{
   int i, p_base = base;
   for (i=1;i<power;i++)
     {  base = base*p_base;
     }
   return base;
}

int hash_function(void *key, int attrLength, int depth)
{
   unsigned char *map = key;
   unsigned hash = 2166136261u;
   int i;
   for ( i = 0; i < attrLength; i++ )
     {  hash = hash ^ map[i];
        hash = hash * 16777619;
     }
   i = hash & depth;
   return i;
}

int get_overflow_buckets(HT_info* header_info, int blocknumber, Record *data, FILE *entries)
{
    int j, readed = 0;
    char bitmap[BLOCK_SIZE/sizeof(Record)+1];
    void *block;

    if (BF_ReadBlock(header_info->fileDesc, blocknumber, &block) == 0)
      {  memcpy(bitmap, block+(BLOCK_SIZE-sizeof(bitmap)-sizeof(int)), sizeof(int));
/* Reads bitmap and prints the records of the block, that blocknumber shows */
         for (j=0;j<BLOCK_SIZE/sizeof(Record);j++)
           {  if (bitmap[j] == '1')
                {  memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                   getRecord(data,entries);
                }
           }
         memcpy(&blocknumber, block+(BLOCK_SIZE-sizeof(int)), sizeof(int));
/* If block has already overflowed then read block number of the bucket and call recursively the get_overflow_buckets function */
         if (blocknumber != -1)
           {  readed = get_overflow_buckets(header_info, blocknumber, data, entries) + readed;
           }
         readed = readed + 1;
      }
    return readed;
}

int hash_id_search(HT_info* header_info, char *bitmap, Record *data, FILE *entries, int hash_id)
{
   int found, k, counter, blocknumber, i, readed = 0, error = 0;
   void *block;
   found = 0;
   k = 0;
   do{
      if (BF_ReadBlock(header_info->fileDesc, 1, &block) == 0)
        {  counter = power_function(2, header_info->depth);
           readed = 1;
           if (counter > hash_id)
             {
/* Check records with hash_id. if no record is found then print error */
                if (BF_ReadBlock(header_info->fileDesc, hash_id, &block) == 0)
                  {  memcpy(&bitmap, block+(BLOCK_SIZE-sizeof(int)-sizeof(bitmap)), sizeof(int));
                     for (i=0;i<BLOCK_SIZE/sizeof(Record);i++)
                       {  if (bitmap[i])
                            {  memcpy(data, block+(i)*sizeof(Record), sizeof(Record));
                               getRecord(data,entries);
                            }
                        }
                     memcpy(&blocknumber, block+(BLOCK_SIZE-sizeof(int)), sizeof(int));
                     if (blocknumber != -1)
                       {  readed = readed + get_overflow_buckets(header_info, blocknumber, data, entries);
                       }
                     found = 1;
                     readed = readed + 1;
                  }
                else
                  {  BF_PrintError("Error reading block");
                     error = -1;
                  }
             }
           else
             {  printf(" To stoixeio pros anazhthsh, poy dothike, den yparxei. \n");
                found = -1;
             }
        }
      else
        {  BF_PrintError("Error reading block");
           error = -1;
        }
      }while(found == 0 && error != -1);
}

/*########################################################################################################################*/

int HT_CreateIndex( char *filename, char attrType, char* attrName, int attrLength, int depth)
{  
   int fileDesc, error = 0, i, j, next, counter = 0, blocknumber;
   HT_info info;
   info_block i_block;
   void *block;
   char bitmap[BLOCK_SIZE/sizeof(Record)+1];

   for (j=0;j<BLOCK_SIZE/sizeof(Record);j++)
     {  bitmap[j] = '0';
     }
   bitmap[BLOCK_SIZE/sizeof(Record)+1] = '\0';

   BF_Init();

   if (BF_CreateFile(filename) == 0)
     {  fileDesc = BF_OpenFile(filename);
        if (fileDesc >= 0)
          {  for (i=0;i<(2+power_function(2,depth));i++)
               {  if (BF_AllocateBlock(fileDesc) == 0)
                    {  if (i == 0)
                         {
/* If i=0 then create block with special info that the file is a hash file and the structure HT_info */
                            if (BF_ReadBlock(fileDesc, i, &block) == 0)
                              {  i_block.filetype = 1;
                                 memcpy(block,&i_block,sizeof(info_block));
                                 info.fileDesc = fileDesc;
                                 info.attrType = attrType;
                                 info.attrName = attrName;
                                 info.attrLength = attrLength;
                                 info.depth = depth;
                                 memcpy(block + sizeof(info_block),&info,sizeof(HT_info));
                                 if (BF_WriteBlock(fileDesc, i) == 0)
                                   {  error = 0;
                                   }
                                 else
                                   {  BF_PrintError("Error writing block");
                                     error = -1;
                                   }
                              }
                            else
                              {  BF_PrintError("Error reading block");
                                 error = -1;
                              }
                         }
                       if (i == 1)
                         {
/* If i=1 then create block that contains the index */
                            if (BF_ReadBlock(fileDesc, i, &block) == 0)
                              {  if (BF_WriteBlock(fileDesc, i) == 0)
                                   {  error = 0;
                                   }
                                 else
                                   {  BF_PrintError("Error writing block");
                                     error = -1;
                                   }
                              }
                            else
                              {  BF_PrintError("Error reading block");
                                 error = -1;
                              }
                         }
                       if (i > 1)
                         {
/* If i > 1 then create a bucket and update the index */
                            if (BF_ReadBlock(fileDesc, 1, &block) == 0)
                              {  //counter = power_function(2,depth);
                                 if (counter <= BLOCK_SIZE/sizeof(int))
                                   {  
                                     
                                      
                                      if ((blocknumber = BF_GetBlockCounter(fileDesc)) > 0)
                                        {  blocknumber = blocknumber - 1;
                                           memcpy(block+sizeof(int)*counter,&blocknumber,sizeof(int));
                                           counter = counter + 1;
                                           if (BF_WriteBlock(fileDesc, 1) == 0)
                                             {  error = 0;
                                             }
                                           else
                                             {  BF_PrintError("Error writing block");
                                                error = -1;
                                             }
                                           if (BF_ReadBlock(fileDesc, blocknumber, &block) == 0)
                                             {  next = -1;
                                                memcpy(block+(BLOCK_SIZE-sizeof(int)),&next,sizeof(int));
                                                memcpy(block+(BLOCK_SIZE-sizeof(bitmap)-sizeof(int)),bitmap,sizeof(bitmap));
                                                if (BF_WriteBlock(fileDesc, blocknumber) == 0)
                                                  {  error = 0;
                                                  }
                                                else
                                                  {  BF_PrintError("Error writing block");
                                                     error = -1;
                                                  }
                                             }
                                           else
                                             {  BF_PrintError("Error reading block");
                                                error = -1;
                                             }
                                        }
                                      else
                                        {  BF_PrintError("Error getting block counter1");
                                           error = -1;
                                        }
                                   }
                                 else
                                   {  printf(" To block evrethriou gemise. \n");
                                      error = -1;
                                   }
                              }
                            else
                              {  BF_PrintError("Error reading block");
                                 error = -1;
                              }
                         }
                    }
                  else
                    {  BF_PrintError("Error allocating block");
                       error = -1;
                    }
               }
          }
        else
          {  BF_PrintError("Error opening hash index\n");
             error = -1;
          }
     }
   else
     {  BF_PrintError("Error creating hash index.\n");
	exit(EXIT_FAILURE);
        error = -1;
     }
   return error;
}

/*########################################################################################################################*/

HT_info* HT_OpenIndex( char * filename)
{
    int filedesc;
    void* block;
    HT_info *HT;
    info_block info;
    HT = malloc(sizeof(HT_info));

	if ((filedesc = BF_OpenFile(filename)) > 0)
	{   if (BF_ReadBlock(filedesc, 0, &block) == 0)
	    {   memcpy(&info, block,sizeof(info_block));  // read recognition data
			if (info.filetype == 1)    // register hash file identifier
			{   memcpy(HT, block + sizeof(info_block), sizeof(HT_info)); 
		            return HT;
			}
			else
			{   printf("Not hash file\n");
			    BF_CloseFile(filedesc);
			    return NULL;
			}
		}
		else
		{   BF_PrintError("Error reading block\n");
		    return NULL;
		}
	}
	else
	{   BF_PrintError("Error opening file\n");
	    return NULL;
	}
}

/*########################################################################################################################*/

int HT_CloseIndex( HT_info* header_info)
{
   int fileDesc,error;
   fileDesc = header_info->fileDesc;
   if (BF_CloseFile(fileDesc) < 0)
     {  BF_PrintError("Error closing file");
        error = -1;
     }
   else
     {  free(header_info);
        error = 0;
     }
   return error;
}

/*########################################################################################################################*/

int HT_InsertEntry(HT_info* header_info, Record record)
{	void* block;
	int i,j, metritis=0, temp, temp1, temp2;
	bool pimp = 0;	// used to show empty space in block	
	int counter = power_function(2, header_info->depth);    // hash-array size in index
	int pointer;    // block pointer
	int index, pindex;  // bucket pointer in index
	char bitMap[(BLOCK_SIZE/sizeof(Record)) + 1];	
	char TBitMap[(BLOCK_SIZE/sizeof(Record)) + 1];	// auxiliary bitMap 

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	int InFunction(void* key, char* field)
	{	void* key2;
    	Record tr;	// prosorinh egrafh (temp record)
			index = hash_function(/**/key/**/, header_info->attrLength, header_info->depth);    // hash_function returns the position of the key in index 
    	printf("mphke 1 \n");
   		memcpy(&pointer, block + index*sizeof(int), sizeof(int)); // pointer to data block
		printf("index = %d \n", index);
  		printf("pointer = %d \n", pointer);
		if (BF_ReadBlock(header_info->fileDesc, pointer, &block) < 0)// open block that hash function returned its index
		{	BF_PrintError("Error reading block");
			return -1;
		}

		memcpy(&temp1, block + BLOCK_SIZE - sizeof(int), sizeof(int));	// read pointer of overflow bucket
printf("mphke 1 - temp1\n");
  printf("temp1 = %d \n", temp1);
		  if (temp1 == -1) // if pointer is NULL
		  {

			memcpy(bitMap, block + BLOCK_SIZE-(sizeof(int) + sizeof(bitMap)), sizeof(bitMap));	// recall bitMap
			for (i=0; i < BLOCK_SIZE/sizeof(Record); i++) // scan bitMap
			{	if (bitMap[i] == '0')
				{	memcpy(block + i*sizeof(Record), &record, sizeof(Record));	// register record	
					bitMap[i] = '1';	// bind position of the record
					memcpy(block + BLOCK_SIZE-(sizeof(int) + sizeof(bitMap)), bitMap, sizeof(bitMap));	// update bitMap
					pimp = 1;	//show that empty space exists in block
					if (BF_WriteBlock(header_info->fileDesc, pointer) < 0)
					{	BF_PrintError("Error writing block");
						return -1;
					}
					break;
				}
        printf("mphke 1 \n");
			}
	printf("mphke 1 - for\n");
			if (pimp == 0)	// block is full
			{
				if(BF_AllocateBlock(header_info->fileDesc) < 0)	// create new block
			  	{	BF_PrintError("Error allocating block");
					return -1;
				}	
				if ((temp = BF_GetBlockCounter(header_info->fileDesc)) < 0)	// find number of blocks (the last one is the new)
				{	BF_PrintError("Error getting block counter");
					return -1;
				} 
				temp--;	// o deikths tou neou block 
				if (BF_ReadBlock(header_info->fileDesc, 1, &block) < 0)	// read index block
				{	BF_PrintError("Error reading block");
					return -1;
				}
				for (i=0; i < counter; i++)
				{	memcpy(&temp1, block + i*sizeof(int), sizeof(int)); // temp1 --> temporary pointer to record block
					if (temp1 == pointer)
					{	metritis++;	// number of times pointer exists in hash table
					}
				}
				metritis = metritis/2;	// cut in half the value found
				if (metritis > 0)	// if more than one pointer exist to record block 
				{	for(i=0; i < metritis; i++)
					{	memcpy(&temp1, block + i*sizeof(int), sizeof(int));	//temp1 --> temporary pointer to record block
						if(temp1 == pointer)	
						{// split indexes to the old and new bucket
							memcpy(block + i*sizeof(int), &temp, sizeof(int));
						}
					}
				}
				else	// den iparxei parapano apo enas deikths ston pointer...ara diplasiase ton pinaka katakermatismu
				{	counter *= 2;	// diplasiasmos tou counter 
					//memcpy(block + (BLOCK_SIZE - 2*sizeof(int)), &counter, sizeof(int));// emimerosh tou counter
					if(counter <= (BLOCK_SIZE/sizeof(int)))	// an o pinakas katakermatismu xoraei se ena block(block evretiriou)
					{	for(i=0; i < counter/2; i++)	// arxikopoihsh ton neon theseon me ton paleon
						{	
							memcpy(block + (i+counter/2)*sizeof(int), block + i*sizeof(int), sizeof(int));
							 
						}
					}
					else	// o diplasios pinakas katakermatismu den xoraei se ena block
					{	if(pindex > 1)
						{	;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						// prospelash tou epomenou block evretiriou
						}
						else
						{	;	//desmefsh neou block evrethriou 
						}
							
					}
						
					header_info->depth++;// afksano kata 1 to bathos tou evretiriou	
				}

				if(BF_WriteBlock(header_info->fileDesc, 1) < 0)// enimerosh tu block evretiriou
				{	BF_PrintError("Error writing block");
					return -1;
				}

//////////////////////////////////////// enimerosh tou HT_info //////////////////////////////////////////////////////////////
			if(BF_ReadBlock(header_info->fileDesc, 0, &block) == 0)
			{	memcpy(block + sizeof(info_block), header_info, sizeof(HT_info));	// enimerosh tou HT_info
				if(BF_WriteBlock(header_info->fileDesc, 0) < 0)
				{	BF_PrintError("Error writing block");
					return -1;
				}	
			}
			else
			{	BF_PrintError("Error reading block");
				return -1;
			}
///////////////////////////////////////// enimerosh tou HT_info //////////////////////////////////////////////////////////////


///////////////////////////////// diaxorismos ton egrafon sto neo kai palaio block ///////////////////////////////////////////	
				for(/**/i=0,j=0/**/; i < (BLOCK_SIZE - (sizeof(bitMap) + sizeof(int)))/sizeof(Record)/*egrafes sto block*/; i++)
				{	if (BF_ReadBlock(header_info->fileDesc, pointer, &block) < 0)	// diavasma tou palaiou block
					{	BF_PrintError("Error reading block");
						return -1;
					}
					memcpy(&tr, block + i*sizeof(Record), sizeof(Record));	// diavasma egrafhs aftu tou block
					memcpy(TBitMap, block + BLOCK_SIZE - (sizeof(int) + sizeof(TBitMap)), sizeof(TBitMap));	//bitMap tou palaiou block	
					
					if (strcmp(field, "name") == 0)
						strcpy(key2, tr.name);
					else if (strcmp(field, "surname") == 0)
						strcpy(key2, tr.surname);  
					else if (strcmp(field, "status") == 0)
						strcpy(key2, tr.status);
					else if (strcmp(field, "dateOfBirth") == 0)
						strcpy(key2, tr.dateOfBirth);
					else if (strcmp(field, "section") == 0)
						strcpy(key2, tr.section);
					else if (strcmp(field, "id") == 0)
						key2 = &tr.id;
					else if (strcmp(field, "salary") == 0)
						key2 = &tr.salary;
      				else if (strcmp(field, "daysOff") == 0)
						key2 = &tr.daysOff;
					else if (strcmp(field, "prevYears") == 0)
						key2 = &tr.prevYears;		

					temp1 = hash_function(key2, header_info->attrLength, header_info->depth);

					if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0)	// anigma evretiriou
					{	BF_PrintError("Error reading block");
						return -1;
					}
					memcpy(&temp1, block + temp1*sizeof(int), sizeof(int));
					if (temp1 == temp) // oi egrafh pu pira apo to palaio block odos bainei sto neo...
					{	TBitMap[i] = '0'; //...ara th diagrafo apo to palaio block
						memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(TBitMap)), TBitMap, sizeof(TBitMap));	// enimerosh tu bitMap tu palaiu block
						if(BF_WriteBlock(header_info->fileDesc, pointer) < 0)	// enimerosh tu palaiou block
						{	BF_PrintError("Error writing block");
							return -1;
						}
						if(BF_ReadBlock(header_info->fileDesc, temp, &block) < 0) // diavasma tou neou block
						{	BF_PrintError("Error reading block");
							return -1;
						}
						memcpy(bitMap, block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), sizeof(bitMap)); // diavasma tou bit map tu neou block
						memcpy(block + j*sizeof(Record), &tr, sizeof(Record));	// h egrafh pige apo to palaio sto neo block
						bitMap[j] = '1';
						memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), bitMap, sizeof(bitMap));	// enimerosh tou bitMap tou neou block
							
						j++;
		
						if(BF_WriteBlock(header_info->fileDesc, temp) < 0)	// enimerosh tou neou block
						{	BF_PrintError("Error writing block");
							return -1;
						}
					}
				}			
				
				temp1 = hash_function(key, header_info->attrLength, header_info->depth);	// thesh pinaka
				memcpy(&temp1, block + temp1*sizeof(int), sizeof(int));	// o temp1 tora einai deikths sto block pu einai apothikevmeno sth thesh tou pinaka pu edixne prin o temp1
				if(BF_ReadBlock(header_info->fileDesc, temp1, &block) < 0)	// anigo to block pu prepei na bei h egrafh record
				{	BF_PrintError("Error reading block");					// ################ TEMP1 ###################
					return -1;
				}
				pimp = 0;
				memcpy(bitMap, block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), sizeof(bitMap)); // anagnosh tu bitMap
				for(i=0; i < BLOCK_SIZE/sizeof(Record); i++)
				{	if (bitMap[i] == '0')
					{	pimp = 1;
						bitMap[i] = '1';
						memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), bitMap, sizeof(bitMap));	// enimerosh tou bitMap
						memcpy(block + i*sizeof(Record), &record, sizeof(Record));	// kataxorish egrafhs
						break;
					}
				}
				if (pimp == 0)	// den ipirxe pali xoros gia na bei h record
				{	// dimiourgia kadou iperxilishs
					if(BF_AllocateBlock(header_info->fileDesc) < 0)	// kataskevh kadu iperxilishs
					{	BF_PrintError("Error allocating block");
						return -1;
					}
					if((temp2 = BF_GetBlockCounter(header_info->fileDesc) < 0))
					{	BF_PrintError("Error getting block counter");
						return -1;
					}
					temp2--;	// aritmos tou kadu iperxilishs pu molis dimiurgisa	

					memcpy(block + BLOCK_SIZE - sizeof(int), &temp2, sizeof(int));// sindesh tu block(bucket) me to kado iperxilishs
					if(BF_WriteBlock(header_info->fileDesc, temp1) < 0 )	// enimerosh tu block(bucket)
					{	BF_PrintError("Error writing block");
						return -1;
					}

					if(BF_ReadBlock(header_info->fileDesc, temp2, &block) < 0)	// anigma tou kadu iperxilishs 
					{	BF_PrintError("Error reading block");
						return -1;
					}

					memcpy(block, &record, sizeof(Record));	// kataxorish ths egrafhs ston kado iperxilishs
					TBitMap[0] = '1';
					for (i=1; i < BLOCK_SIZE/sizeof(Record); i++)	// arxikopihsh bitMap tou kadu iperxilishs
					{	TBitMap[i] = '0';
					}			
					memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(TBitMap)), TBitMap, sizeof(TBitMap));	// enimerosh tou bitMap
					temp1 = -1;					
					memcpy(block + BLOCK_SIZE - sizeof(int), &temp1, sizeof(int));	// deikths se alo kado iperxilishs = -1 = NULL
			
					if(BF_WriteBlock(header_info->fileDesc, temp2) < 0)	// enimerosh tou kadu iperxilishs
					{	BF_PrintError("Error writing block");
						return -1;
					}
			
				}
			 }	
			
			}
			else	// iparxei kados iperxilish
			{	// o temp1 exei to dikth ston kado iperxilish 
			  temp2 = temp1;
			  do				
			  {	if(BF_ReadBlock(header_info->fileDesc, temp2, &block) < 0)	// anoigma tu kadu iperxilishs
				{	BF_PrintError("Error reading block");
					return -1;
				}
				temp1 = temp2;
				memcpy(&temp2, block + BLOCK_SIZE - sizeof(int), sizeof(int));	// anagnosh tou deikth se epomeno kado iperxilishs

			  }	while(temp2 != -1); 
			
				memcpy(bitMap, block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), sizeof(bitMap));	// bitMap kadu iperxilishs
				pimp = 0;
				for (i=0; i < BLOCK_SIZE/sizeof(Record); i++)
				{	if (bitMap[i] == '0')
					{	pimp = 1;
						bitMap[i] = '1';
						memcpy(block + i*sizeof(Record), &record, sizeof(Record));	// eisagogh ths egrafhs
						break;
					}
				}
				if (pimp == 1)	// h egrafh vrike xoro na bei
				{	memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), bitMap, sizeof(bitMap));	// vale thn tote mesa
				}
				else	// h egrafh den vrike xoro na bei
				{	if(BF_AllocateBlock(header_info->fileDesc) < 0)	// dimiurgise neo kado iperxilishs
					{	BF_PrintError("Error allocating block");
						return -1;
					}
					if((temp2 = BF_GetBlockCounter(header_info->fileDesc)) < 0)
					{	BF_PrintError("Error getting block counter");
						return -1;
					}
					temp2--;	// o deikths tou neou kadu iperxilishs
					memcpy(block + BLOCK_SIZE - sizeof(int), &temp2, sizeof(int));	// o deikths ston neo kado iperxilishs
				}
				if (BF_WriteBlock(header_info->fileDesc, temp1) < 0)	// enimerosh tu palaiou kadu iperxilishs
				{	BF_PrintError("Error writing block");
					return -1;
				}
				if(BF_ReadBlock(header_info->fileDesc, temp2, &block) < 0)	// anoigma tou neou kadu iperxilishs
				{	BF_PrintError("Error reading block");
					return -1;
				}
				
				memcpy(block, &record, sizeof(Record));
				bitMap[0] = '1';
				for(i=1; i < BLOCK_SIZE/sizeof(Record); i++)
				{	bitMap[i] = '0';
				}
				i =-1;
				memcpy(block + BLOCK_SIZE - (sizeof(int) + sizeof(bitMap)), bitMap, sizeof(bitMap));	// enimerosh tu bitMap
				memcpy(block + BLOCK_SIZE - sizeof(int), &i, sizeof(int));	// o deikths tou neou kadu iperxilishs se kado iperxilishs einai -1 diladh NULL
				if(BF_WriteBlock(header_info->fileDesc, temp2) < 0)	// enimerosh tou neou kadu iperxilishs
				{	BF_PrintError("Error writing block");
					return -1;
				}
 
			}

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		//eksagogh dedomenon apo to evretirio	
if (BF_ReadBlock(header_info->fileDesc, 1, &block) == 0)	// anoigma tou block evretiriou	
{	
	
		    
/**********************************************************************************************************
 evrethrio exei: ("eggrafes"......................)
 sta bucket: (eggrafes .......)(bitmap)(deikths sto bucket yperxeilhshs)
***********************************************************************************************************/


	if (header_info->attrType == 'c')
	{	if (strcmp(header_info->attrName, "name") == 0)
		{   printf("NAME\n");  // pedio name
		    
			InFunction(&(record.name), "name");
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else if (strcmp(header_info->attrName, "surname") == 0)
		{   printf("SURNAME\n");   // pedio surname
          	   
			InFunction(&(record.surname), "surname");
		}
		else if (strcmp(header_info->attrName, "status") == 0)
		{   printf("STATUS\n");    // pedio status
           	    
		  InFunction(&(record.status), "status");
		}
		else if (strcmp(header_info->attrName, "dateOfBirth") == 0)
		{   printf("DATE OF BIRTH\n"); // pedio dateOfBirth
            	    
			InFunction(&(record.dateOfBirth), "dateOfBirth");
		}
		else if (strcmp(header_info->attrName, "section") == 0)
		{   printf("SECTION\n");   // pedio section

      		InFunction(&(record.section), "section");
		}
	}
	else if (header_info->attrType == 'i')
	{   	if (strcmp(header_info->attrName, "id") == 0)
		{   printf("ID\n");    // pedio id
        	    
			InFunction(&(record.id), "id");
	  	}
	  	else if (strcmp(header_info->attrName, "salary") == 0)
		{   printf("SALARY\n");    // pedio salary
            	  
			InFunction(&(record.salary), "salary");  
		}
		else if (strcmp(header_info->attrName, "daysOff") == 0)
		{   printf("DAYS OFF\n");  // pedio daysOff
            	    
			InFunction(&(record.daysOff), "daysOff");
		}
		else if (strcmp(header_info->attrName, "prevYears") == 0)
		{   printf("PREVIOUS YEARS\n");    // pedio prevYears
            	    
			InFunction(&(record.prevYears), "prevYears");
		}
   		else
		{ BF_PrintError("Error getting block counter\n");
		    return -1;
		}
	}
}
else
{	BF_PrintError("Error reading block");
	return -1;
}
     
}

/*########################################################################################################################*/

void HT_GetAllEntries( HT_info* header_info, void *value)
{
   int blocknumber, error = 0, j, i, blockcounter, metrhth, counter, k, kk, new, hash_id, *bucket, readed = 0;
   void *block;
   Record *data;
   data = malloc(sizeof(Record));
   FILE *entries;
   char *bitmap;//[BLOCK_SIZE/sizeof(Record)+1];
   bucket = malloc(power_function(2, header_info->depth)*sizeof(int));

   if ((entries = fopen("./Entries.txt", "w")) == NULL) /* Open Entries File */
     { perror("fopen Entries-file");
     }

   if (value == NULL)
     {  fprintf(entries, "ALL ENTRIES\n");
        k = 0;
        if (BF_ReadBlock(header_info->fileDesc, 1, &block) == 0)
          {  counter = power_function(2, header_info->depth); /* to megethos toy pinaka katakermatismoy sto evrethrio */
             readed = 1;
             for (i=0;i<counter;i++)
               {  memcpy(&blocknumber, block+(i)*sizeof(int), sizeof(int));
                  new = 1;
                  kk = 0;
                  while (new != 0 && kk < k && k != 0)
                    {
/* Elegxos gia to an to bucket poy eimaste tora exei eidh diabastei kai exoun ektypo8ei oi eggrafes toy */
                       if (blocknumber == bucket[kk])
                         {  new = 0;
                         }
                       kk = kk + 1;
                    }
/* An to bucket poy eimaste tora den exei ksanadiabastei tote ektyponontai oi eggrafes toy */
                  if (new)
                    {  bucket[k] = blocknumber;
                       k = k + 1;
                       if (BF_ReadBlock(header_info->fileDesc, blocknumber, &block) == 0)
                         {  memcpy(bitmap, block+(BLOCK_SIZE-sizeof(bitmap)-sizeof(int)), sizeof(int));
                            for (j=0;j<BLOCK_SIZE/sizeof(Record);j++)
                              {  if (bitmap[j] == '1')
                                   {  memcpy(data, block+sizeof(int)+(j)*sizeof(Record), sizeof(Record));
                                      getRecord(data,entries);
                                   }
                              }
                            memcpy(&blocknumber, block+(BLOCK_SIZE-sizeof(int)), sizeof(int));
                            if (blocknumber != -1)
                              {  readed = readed + get_overflow_buckets(header_info, blocknumber, data, entries);
                              }
                         }
                       else
                         {  BF_PrintError("Error reading block");
                            error = -1;
                         }
                    }
               }
          }
        else
          {  BF_PrintError("Error reading block");
             error = -1;
          }
     }
/********************************************************************************************************/
/********************************************************************************************************/
   else
     {  hash_id = hash_function(value, header_info->attrLength, header_info->depth);
        if (header_info->attrType == 'c')
          {  if (strcmp(header_info->attrName,"name") == 0)
                {  fprintf(entries, "NAME\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"surname") == 0)
                {  fprintf(entries, "SURNAME\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"status") == 0)
                {  fprintf(entries, "STATUS\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"dateOfBirth") == 0)
                {  fprintf(entries, "DATE OF BIRTH\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"section") == 0)
                {  fprintf(entries, "SECTON\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
          }
/********************************************************************************************************/
        if (header_info->attrType == 'i')
          {  if (strcmp(header_info->attrName,"id") == 0)
                {  fprintf(entries, "ID\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"salary") == 0)
                {  fprintf(entries, "SALARY\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"daysOff") == 0)
                {  fprintf(entries, "DAYS OFF\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
              if (strcmp(header_info->attrName,"prevYears") == 0)
                {  fprintf(entries, "Previous Years\n");
                   readed = readed + hash_id_search(header_info, bitmap, data, entries, hash_id);
                }
/********************************************************************************************************/
          }
     }
   printf("Plh8os blocks poy diabastikan = %d \n", readed);
   free(bucket);
   free(data);
   fclose(entries);
}
