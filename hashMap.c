#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"



/*the first hashing function you can use*/
int stringHash1(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += str[i];
	return r;
}

/*the second hashing function you can use*/
int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i]; /*the difference between stringHash1 and stringHash2 is on this line*/
	return r;
}

/* initialize the supplied hashMap struct*/
void _initMap (struct hashMap * ht, int tableSize)
{
	int index;
	if(ht == NULL)
		return;
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	ht->tableSize = tableSize;
	ht->count = 0;
	for(index = 0; index < tableSize; index++)
		ht->table[index] = NULL;
}

/* allocate memory and initialize a hash map*/
hashMap *createMap(int tableSize) {
	assert(tableSize > 0);
	hashMap *ht;
	ht = malloc(sizeof(hashMap));
	assert(ht != 0);
	_initMap(ht, tableSize);
	return ht;
}

/*
 Free all memory used by the buckets.
 Note: Before freeing up a hashLink, free the memory occupied by key and value
 */
void _freeMap (struct hashMap * ht)
{
	/*write this*/
	assert(ht != NULL);
	int i;
	struct hashLink * temp;

	for(i = 0; i < ht->tableSize; i++)      //iterate through each bucket
    {
        if(ht->table[i] == NULL)            //if no nodes in current bucket iterate to next bucket
            continue;

        //Nodes exist in current bucket
        while(ht->table[i]->next != NULL)   //set all nodes in current bucket to null
        {
            ht->table[i]->key = '\0';       //set key to null
            ht->table[i]->value = 0;        //set value to 0

            //Free nodes
            temp = ht->table[i]->next;      //set temp to current node
            ht->table[i]->next = ht->table[i]->next->next;      //next node in current bucket
            free(temp);                     //free current node
            ht->count--;                    //current bucket's node decreased by 1
        }
        free(ht->table[i]);                 //free current bucket
        ht->tableSize--;                    //table's bucket size decreased by 1
    }
}

/* Deallocate buckets and the hash map.*/
void deleteMap(hashMap *ht) {
	assert(ht!= 0);
	/* Free all memory used by the buckets */
	_freeMap(ht);
	/* free the hashMap struct */
	free(ht);
}

/*
Resizes the hash table to be the size newTableSize
*/
void _setTableSize(struct hashMap * ht, int newTableSize)
{
	/*write this*/
	assert(ht != NULL);
	assert(newTableSize > 0);

	struct hashMap *tempMap = malloc(sizeof(struct hashMap));   //pointer to new table
	assert(tempMap = 0);
	_initMap(tempMap, (newTableSize * 2));   //initialize new table with twice the size of current

    int i;
	for(i = 0; i < ht->tableSize; i++)      //iterate through each bucket
    {
        if(ht->table[i] == NULL)            //if no nodes in current bucket iterate to next bucket
            continue;

        //Nodes exist in current bucket
        while(ht->table[i]->next != NULL)   //iterate through all nodes in current bucket
        {
            tempMap->table[i]->key = ht->table[i]->key;
            tempMap->table[i]->value = ht->table[i]->value;
            tempMap->table[i]->next = ht->table[i]->next;

            ht->table[i]->next = ht->table[i]->next->next;      //next node in current bucket
        }
    }
    tempMap->count = ht->count;
    tempMap->tableSize = ht->tableSize;
    free(ht);
    ht = tempMap;
}

/*
 insert the following values into a hashLink, you must create this hashLink but
 only after you confirm that this key does not already exist in the table. For example, you
 cannot have two hashLinks for the word "taco".

 if a hashLink already exists in the table for the key provided you should
 replace that hashLink--this requires freeing up the old memory pointed by hashLink->value
 and then pointing hashLink->value to value v.

 also, you must monitor the load factor and resize when the load factor is greater than
 or equal LOAD_FACTOR_THRESHOLD (defined in hashMap.h).
 */
void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{
	/*write this*/
	assert(ht != NULL);

    int key = stringHash1(k) % ht->tableSize;          //convert key to int

	//Confirm key does not exist in table
    if(containsKey(ht, k) == 1)             //key exists in table
    {
        if(atMap(ht, k) == NULL)            //value does not exist in table
        {
            //create new node
            struct hashLink *newNode = malloc(sizeof(struct hashLink));
            newNode->key = k;
            newNode->value = v;

            //Insert node as 1st element in bucket
            newNode->next = ht->table[key]->next;
            ht->table[key]->next = newNode;

            ht->count++;    //increase nodes by 1
        }
        else    //value exists in table
        {
            int i;
            for(i = key; i < ht->tableSize; i++)    //iterate through table starting from key
            {
                int tableKey = stringHash1(ht->table[i]->key) % ht->tableSize;
                if(tableKey == key)        //key found
                {
                    ht->table[i]->value = v;        //replace value of key with new value
                }
            }
        }
    }
    else
    {
        //Key not in table
    }

    //Resize if load factor is >= LOAD_FACTOR_THRESHOLD
    if(tableLoad(ht) >= LOAD_FACTOR_THRESHOLD)
    {
        _setTableSize(ht, ht->tableSize * 2);
    }
}

/*
 this returns the value (which is void*) stored in a hashLink specified by the key k.

 if the user supplies the key "taco" you should find taco in the hashTable, then
 return the value member of the hashLink that represents taco.

 if the supplied key is not in the hashtable return NULL.
 */
ValueType* atMap (struct hashMap * ht, KeyType k)
{
	/*write this*/
	assert(ht != NULL);
	int key = stringHash1(k);      //convert key to int
	int i;

	for(i = 0; i < ht->tableSize; i++)
    {
        int tableKey = stringHash1(ht->table[i]->key) % ht->tableSize;
        if(tableKey == key)        //key found
        {
            return (ValueType *) (ht->table[i]->value);
        }
        else
        {
            return NULL;   //value not found
        }
    }
    return NULL;
}

/*
 a simple yes/no if the key is in the hashtable.
 0 is no, all other values are yes.
 */
int containsKey (struct hashMap * ht, KeyType k)
{
	/*write this*/
	assert(ht != NULL);
	int i;

	for(i = 0; i < ht->tableSize; i++)
    {
        if(ht->table[i] == NULL)            //if no nodes in current bucket iterate to next bucket
            continue;

        else if(ht->table[i]->key == k)
        {
            return 1;     //key found
        }
        else
        {
            return 0;   //key not found
        }
    }
    return 0;
}

/*
 find the hashlink for the supplied key and remove it, also freeing the memory
 for that hashlink. it is not an error to be unable to find the hashlink, if it
 cannot be found do nothing (or print a message) but do not use an assert which
 will end your program.
 */
void removeKey (struct hashMap * ht, KeyType k)
{
	/*write this*/
	assert(ht != NULL);

	if(containsKey(ht, k) == 1)     //key exists in table
    {
        int i;
        for(i = 0; i < ht->tableSize; i++)
        {
            if(ht->table[i] == NULL)            //if no nodes in current bucket iterate to next bucket
                continue;

            else if(ht->table[i]->key == k)
            {
                //Nodes exist in current bucket
                while(ht->table[i]->next != NULL)   //set all nodes in current bucket to null
                {
                    ht->table[i]->key = '\0';       //set key to null
                    ht->table[i]->value = 0;        //set value to 0

                    //Free nodes
                    struct hashLink * temp = ht->table[i]->next;
                    ht->table[i]->next = ht->table[i]->next->next;      //next node in current bucket
                    free(temp);                     //free current node
                    ht->count--;                    //current bucket's node decreased by 1
                    break;
                }
            }
        }
    }
    else
    {
        //Key does not exist in table
    }
}

/*
 returns the number of hashLinks in the table
 */
int size (struct hashMap *ht)
{
	/*write this*/
    return ht->count;

}

/*
 returns the number of buckets in the table
 */
int capacity(struct hashMap *ht)
{
	/*write this*/
    return ht->tableSize;
}

/*
 returns the number of empty buckets in the table, these are buckets which have
 no hashlinks hanging off of them.
 */
int emptyBuckets(struct hashMap *ht)
{
	/*write this*/
	assert(ht != NULL);
	int numOfEmpty = 0;             //empty hashlinks found
	int i;

	for(i = 0; i < ht->tableSize; i++)
    {
        if(ht->table[i] == NULL)            //if no nodes in current bucket iterate to next bucket
        {
            numOfEmpty++;
            continue;
        }
    }
    return numOfEmpty;
}

/*
 returns the ratio of: (number of hashlinks) / (number of buckets)

 this value can range anywhere from zero (an empty table) to more then 1, which
 would mean that there are more hashlinks then buckets (but remember hashlinks
 are like linked list nodes so they can hang from each other)
 */
float tableLoad(struct hashMap *ht)
{
	/*write this*/
	assert(ht != NULL);
	assert(ht->tableSize != 0);

	return ht->count / ht->tableSize;
}
void printMap (struct hashMap * ht)
{
	int i;
	struct hashLink *temp;
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		if(temp != 0) {
			printf("\nBucket Index %d -> ", i);
		}
		while(temp != 0){
			printf("Key:%s|", temp->key);
			printValue(temp->value);
			printf(" -> ");
			temp=temp->next;
		}
	}
}


