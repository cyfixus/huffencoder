/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
/**********************************ADT****************************************/
/*****************************************************************************/

/*********************************
 * Node hold lu freq c symbol and *next * < && > 
 * Node operates as a queue node and transforms into a tree node and gets lost
 * but also freed from memory along the way... but still lost*/
struct Node
{
  unsigned long freq;
  unsigned char symbol;
  struct Node *next;
  struct Node *left;
  struct Node *right;
};
/********************************
 * CodeMap is my solution to not being able to figure out
 * how to work with a code by traversing the tree to a char
 * CodeMap is used in an array, to mirror that of the symbolMap
 * gathered from the input file. codes are assigned to each map key
 * with memcpy <<<< took waaay too long to find that guy
 * the map allows for easy access to the code to a given symbol*/
struct CodeMap
{
  unsigned long freq;
  unsigned char symbol;
  unsigned char codeLength;
  unsigned char code[257];
};

/*****************************************************************************/
/*********************************huffman.c***********************************/
/*****************************************************************************/

/**************************************
 * initFiles takes the in and out files and a flag to denote whether or
 * not encoding is being done(else decode). The method then has the pleasure
 * of building the unsigned long symbolMap[257] and filling it with the char
 * frequency counts if encoding, or refilling it from the header of an encoded
 * file. symbolMap is then transformed into a linked list/priority queue, and 
 * then into a tree, from which the codes are generated, and the file is either
 * encoded or decoded accordingly... I originally had this code without the if
 * statements pretty much duplicated in the huffencode and decode files, but 
 * realized I could probably put them together and make a flag.  I thought
 * I would then be able to keep the helpers in the files, but realized that
 * the separate main functions made that a no go, so now the helpers for 
 * encoding and decoding are both in huffman.c so its a little longer than
 * I was anticipating >< but it saves me from duplicate code!*/ 
void initFiles(FILE *in, FILE *out, char enCode);

/********************************
 * createNode makes a tree node based on freq and symbol passed */
struct Node *createNode(unsigned long freq, unsigned long symbol);

/*******************************
 * codeLength pretty much exactly strlen pg 99*/
unsigned char codeLength(unsigned char *code);

/*************************************
 * printCodes traveres the tree to each leaf, gathering its
 * code along the way and printing it and assigning the code
 * to the struct at the end*/
void buildCodes(struct Node *root, struct CodeMap *codeMap, 
  unsigned char *code, int index, int newCode);


/*****************************************************************************/
/***********************************queue.c***********************************/
/*****************************************************************************/

/************************************
 * insertQueue puts passed nodes into a priority queue based on freq and symbol
 * lowest freq, highest priority. tie freq, higher symbol has priority 
 * I decided to maintain a Queue pointer and also leaf pointers*/
struct Node *insertQueue(struct Node *head, struct Node *newNode);

/******************************
 * fillQueue takes the awesome array and turns it into nodes
 * and puts it into the queue, saving space by only creating
 * nodes for symbols with a freq > 0
 *   >< >< I tried to do this... w/o the lu* looping through
 * each symbol and creating a node with the freqs... */ 
struct Node *fillQueue(unsigned long *chaChars);

/******************
 * queueLength returns the length of the queue by iterating through from head*/
int queueLength(struct Node *head);

/******************************
 * freeQueue takes Node* and frees the Queue..*/
void freeQueue(struct Node *head);

/*****************************************************************************/
/************************************tree.c***********************************/
/*****************************************************************************/

/***************************************
 * buildHTree takes in a pointer to the root of the queue
 * if the root of the queue pointer to next isn't NULL
 * a new node is created based on the freq's of the head
 * and root->next and creates a node with that freq
 * and the root's symbol(since this will be the higher
 * of the two symbols) the new node left and right pointers
 * are set to the root and root->next and the newRoot is set
 * to the third item in the queue, then the next pointers of
 * the leaves are set to NULL then the node is inserted to
 * the queue and buildHTree is called until the tree is done*/
struct Node *buildHTree(struct Node *root);

/***********
 * isLeaf sees if the node has any kids*/
int isLeaf(struct Node *root);

/*****
 * returns rightmost symbol*/
unsigned char maxSymbol(struct Node *root);

/****
 * returns leftmost symbol*/
unsigned char minSymbol(struct Node *root);

/******************************
 * freeTree takes Node* and frees the Tree.*/
void freeTree(struct Node *head);


/*****************************************************************************/
/********************************printers.c***********************************/
/*****************************************************************************/

/********************************************************
 * printData is called when a file is encoded or decoded
 * it takes in the codeMap, and the # of symbols and totalBytes
 * it then displays this data. symbol  freq   code per the required format*/
void printData(struct CodeMap *codeMap, int uniqueSymbols, 
  unsigned long totalBytes);

/*****************************************************************************/
/******The remaining print methods are used for debugging*********************/
/*****************************************************************************/

/**********************
 * prints the whole tree, the combo nodes and all*/
void printTreeHelper(struct Node *root);

/********************
 * newLine after printing tree*/
void printTree(struct Node *root);

/************************
 * takes the root, traverses the tree going leftmost
 * first and going from there to print each node w/o kids*/
void printLeavesHelper(struct Node *root);

/**************
 * newLine after printing the leaves*/
void printLeaves(struct Node *root);

/******************************
 * printHead takes Node* and prints the head/root..*/
void printHead(struct Node *head);

/***************************
 * printQueue takes Node* and prints the formatted Queue */
void printQueue(struct Node *head);

/********************************
 * print takes the lu* and prints it formatted accordingly */
void print(unsigned long chaChars[]);

/*****************************************************************************/
/********************************huffencode.c*********************************/
/*****************************************************************************/

/*************************
 * symbolCount goes through each byte of input and ++ each symbolCount
 * returns lu* to the data gathered. last byte is saved as total bytes
 * symbolCount makes a static lu[257] because... we only need one
 * and I tried to pass this thing around for two days before typing
 * static...symbolCount goes through each byte of the FILE *in and 
 * ++ the appropriate byte in the lu[] to represent that symbol's freq
 * symbolCount was my saving grace... I wish I would have thought to
 * fill an array on my own... such lol >< waaaaaaaaaaaaaaay easier
 * than looping through the file over each symbol creating a Node*/
void symbolCount(FILE *in,  unsigned long *symbolMap);

/*****************************
 * writeHeader takes the symbolMap and uniqueSymbols to 
 * write the uniqueSymbols in the first byte of the file
 * followed by a byte for each symbol and bytes for the frequency
 * it loops through each symbol and only writes it if it has
 * a frequency, the loop breaks when it has written all of the
 * symbols*/
void writeHeader(FILE *out, unsigned long *symbolMap, 
  unsigned char uniqueSymbols);

/**************************************
 * encode actually encodes the file... whenever I figure out 
 * how to manage the codes correctly... right now it just writes
 * the file with the correct # of encoded bits... which are encoded
 * but all with the same bits */
void encode(FILE *in, FILE *out, struct CodeMap *codeMap, 
  unsigned long totalBytes);

/**************************
 * lastByte writes the last bytes to file (the totalBytes)*/
void lastByte(FILE *out, unsigned long totalBytes);

/*****************************************************************************/
/********************************huffdecode.c*********************************/
/*****************************************************************************/

/*****************************************************
 * gatherHeader takes the input file and returns an 
 * array identical to the array gathered in symbolCount() from encode
 * it reads in the first byte as the # of unique symbols and continues
 * by iterating through by the number of uniqueSymbols, gathering 
 * the symbol (unsigned char) and its frequency (unsigned long) when
 * the last symbol is colleced, an additional unsigned long is colleed
 * representative of the totalBytes of the file. This value is then assigned
 * to the last index of the array and the method returns the array to be
 * used in building the queue, tree, and further extrapolating the codes
 * from the encoded file to rewrite the original
 */ 
void gatherHeader(FILE *in, unsigned long *symbolMap);

/**************************************
 * decode file takes in the files, the root of the tree, and the
 * total # of bytes for breaking the loop when the original file
 * has been written, so extra bits are ignored. I tried with much
 * failure to write this recursively... until I reailzed I was 
 * essentially writing a forLoop to iterate through the byte
 * bit by bit to determine whether it was a 0 or something else
 * and follow the code along until a leaf is reached, at which 
 * point the symbol is written to the file and the root is returned
 * to initiate the search for the next code along the upcoming bitstream */
void decode(FILE *in, FILE *out, struct Node *root, 
  unsigned long totalBytes);

#endif