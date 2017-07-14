/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

/********************************
 * createNode makes a tree node based on freq and symbol passed */
struct Node *createNode(unsigned long freq, unsigned long symbol)
{
  struct Node *node = malloc(sizeof(struct Node));
  node->freq = freq;
  node->symbol = symbol;
  node->next = NULL;
  node->left = NULL;
  node->right = NULL;
  return node;
}

/*******************************
 * codeLength pretty much exactly strlen pg 99*/
unsigned char codeLength(unsigned char *code)
{
  unsigned char i;
  for(i = 0; *code != '\0'; code++)
  {
    i++;
  }
  return i;
}

/*************************************
 * buildCodes traveres the tree to each leaf, gathering its
 * code along the way and printing it and assigning the code
 * to the struct at the end*/
void buildCodes(struct Node *root, struct CodeMap *codeMap, 
  unsigned char *code, int index, int newCode)
{
  unsigned char length;
  if(newCode == 0)
  {
    index = 0;
    newCode =  1;
  }
  if(root->left)
  {
    code[index] = '0';
    buildCodes(root->left, codeMap, code, index + 1, newCode);
  }
  if(root->right)
  {
    code[index] = '1';
    buildCodes(root->right, codeMap, code, index + 1, newCode);
  }
  if(isLeaf(root))
  {
    code[index] = '\0';
    if(root->freq > 0)
    {
      memcpy(codeMap[root->symbol].code, code, 256);
      length = codeLength(code);
      codeMap[root->symbol].codeLength = length;
      codeMap[root->symbol].symbol = root->symbol;
      codeMap[root->symbol].freq = root->freq;
      newCode = 0;
    }
  }
  index = 0;
}
/**************************************************************/
/* encodeFile is the brain. it takes the files opened in main and
 * creates the symbol map then uses the symbol map to create the queue
 * then develops the tree from the queue, from all of this data it
 * writes the header then encodes the file 
 * I've realized having to debug the same issues both ways, because
 * the methods are literally the same minus the call to deal with
 * either gathering and writing the header array, or reading it in
 * and building the tree from the header rather than the raw data
 * and 256 correction and the call to encode or decode. I'm not 
 * entirely sure how to consolidate this into one method safely... 
 * so many parameters are being passed into encode, I'm not sure 
 * how to extract them all from this method, unless I can create
 * some sort of flag in this method to denote whether or not we
 * are encoding or decoding... omg so esy ><*/

void initFiles(FILE *in, FILE *out, char enCode)
{
  unsigned long symbolMap[257] = {0};
  unsigned long totalBytes;
  int uniqueSymbols; 
  struct Node *head;
  struct Node *root;
  struct CodeMap codeMap[256];
  unsigned char code[256] = {0};
  int index; 
  /*********************************************
   * greatest method known to man. < withouth this
   * thine reins uponth memry be at the mercy of
   * me memry leaks!... and with it. my freq table
   * makes it all the way to 255 without any bogus
   * jazz in the way! */
  /***** my code was working perfectly for generating the codes etc
   * for every file except ralphBW and the evil one. for some reason
   * 255 was making its way through everything, and working perfectly
   * until the codes were assigned, then funky stuff made its way in
   * and printData never made it to 255 for ralphBW... after some
   * detective work, I realized maybe I need to initialize the struct
   * array, cause nowhere in the chain of creation did =160 ever have
   * a freq.... I tried a zillion ways I thought would work to init
   * the struct[] and many failures later reailzed structs occupy 
   * chunks of memory, so theres probably some special method to do that
   * and I was probably taught this, but never had to absolutely use it
   * so I turned to the trusty pages 247-250 and saw that I could make 
   * stuff whatever I wanted with memset, so I set all the codeMaps to 0
   * and now it works!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
  memset(codeMap, 0, 256*sizeof(struct CodeMap));

  index = 0;
  if(enCode)
  {
    symbolCount(in, symbolMap);
  }
  else
  {
    gatherHeader(in, symbolMap);
  }
  totalBytes = symbolMap[256];
  head = fillQueue(symbolMap);
  uniqueSymbols = queueLength(head);
  if(uniqueSymbols == 0)
  {
    uniqueSymbols = 256;
  }
  else if (uniqueSymbols == 256)
  {
    uniqueSymbols = 0;
  }
  root = buildHTree(head);
  buildCodes(root, codeMap, code, index, 1);
  printData(codeMap, uniqueSymbols, totalBytes);
  if(enCode)
  {
    rewind(in);
    writeHeader(out, symbolMap, uniqueSymbols);
    lastByte(out, totalBytes);
    encode(in, out, codeMap, totalBytes);
  }
  else
  {
    decode(in, out, root, totalBytes);
  }
  freeTree(root);
}

/**********************************************************************/
/*****************ENCODING FUNCTIONS***********************************/
/**********************************************************************/


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
void symbolCount(FILE *in, unsigned long *symbolMap)
{
  unsigned char inputC;
  unsigned long total;

  while(!feof(in))
  {/** I  can't figure out how to read beyond 160 >< I had the same code
      working with fgetc and thought changing to fread would help... not
      so, then reading up on f stuff I got the bright idea of trying to
      use ftell instead of ++ through the while loop to inc the total
      one issue I can't figure out without an extra line of code, during
      the loop it somehow ends up reading the last byte twice, so to work
      around that I -- the last count, I didn't have this issue with fgetc*/
    fread(&inputC, sizeof(unsigned char), 1, in);
    ++symbolMap[inputC];
  }
  symbolMap[inputC]--;
  total = ftell(in);
  rewind(in);
  symbolMap[256] = total;
}
/*****************************
 * writeHeader takes the symbolMap and uniqueSymbols to 
 * write the uniqueSymbols in the first byte of the file
 * followed by a byte for each symbol and bytes for the frequency
 * it loops through each symbol and only writes it if it has
 * a frequency, the loop breaks when it has written all of the
 * symbols*/
void writeHeader(FILE *out, unsigned long *symbolMap, 
  unsigned char uniqueSymbols)
{
  int i, symbolCount;
  unsigned char symbol;

  symbolCount = 0;

  fwrite(&uniqueSymbols, sizeof(unsigned char), 1, out);
  
  for(i = 0; i < 256; i++)
  {
    if(symbolMap[i] != 0)
    {
      symbol = i;
      symbolCount++;
      fwrite(&symbol, sizeof(unsigned char), 1, out);
      fwrite(&symbolMap[i], sizeof(unsigned long), 1, out);
    }
    else
    {
      continue;
    }
    if(symbolCount == uniqueSymbols)
    {
      break;
    }
  }
}

/**************************************
 * encode takes in the files, the codeMap and totalBytes
 * several temp variables are created to keep track of the 
 * byte being read, written. The code, which bit, count for 
 * totalByte condition, and the codeLength and mask to write
 * byteCount is initialized to 7 for bitshifting intially at 
 * 7(128) the method loops until feof(in)
 * when on a new code, a byte is read from the input file, and
 * its code is assigned to codeIn and length coorespondingly
 * each 1 and 0 is looped through and written into the byte
 * that will be written once it has be been filled with 8 bits
 * from however many codes are required to fill it. If at any
 * point a code has reached its length, regardless of the current bit
 * position within the byte, a new code is gathered. Similarly, 
 * a byte is collected bit by bit until it is full and then the 
 * byte is written to the output file.
 * the encoding works by assigning a 1 or 0 to a position, then shifting
 * it along a 0 and exposing the 1s with | 
 * I'm still having trouble with padding with zeros on the last byte. I've
 * tried shifting with they bytes that remain, with numbers, every 
 * which way, and then though, maybe I need the difference of what remains
 * and it works for small .... but is off by one , or some weird thing
 * in the case of MobyDick*/
void encode(FILE *in, FILE *out, struct CodeMap *codeMap, 
  unsigned long totalBytes)
{
  unsigned char readByte;
  unsigned char writeByte;
  unsigned char codeCount;
  unsigned char byteCount;
  unsigned char mask;
  unsigned char codeLength;
  unsigned char *codeIn;
  unsigned char codeBit;
  unsigned long count;
  unsigned char write;
  unsigned char bitRemain;
  unsigned char bitOrig;

  mask = writeByte = codeCount = count = write = bitRemain = 0;

  byteCount = 7;
  bitOrig = 0;
  while(!feof(in))
  {
    /******************
     * read in byte, set the code from that byte, and set codeLength
     * ++count to track read bytes*/
    if(codeCount == 0)
    {
      fread(&readByte, sizeof(unsigned char), 1, in);
      codeIn = codeMap[readByte].code;
      codeLength = codeMap[readByte].codeLength;
      count++;
    }
    if(codeIn[codeCount] == '0')
    {
      codeBit = 0;
    }
    else if(codeIn[codeCount] == '1')
    {
      codeBit = 1;
    }
    mask = codeBit << byteCount;
    writeByte |= mask;
    codeCount++;
    if(codeCount == codeLength)
    {
      codeCount = 0;
    }
    if(byteCount == 0)
    {
      fwrite(&writeByte, sizeof(unsigned char), 1, out);
      writeByte = 0;
      byteCount = 7;
      bitOrig = 0;
      mask = 0;
      write = 1;
    }
    else{
      byteCount--;
      bitOrig++;
      write = 0;
    }
    if(ftell(in) == totalBytes)
    {
      break;
    }
  }
  if(!write){
    while(byteCount != bitOrig)
    byteCount--;
  }
  bitRemain = abs(((codeLength % 8) - byteCount));
  writeByte |= bitRemain;
  fwrite(&writeByte, sizeof(unsigned char), 1, out);
}
/**************************
 * lastByte writes the last bytes to file (the totalBytes)*/
void lastByte(FILE *out, unsigned long totalBytes)
{
  fwrite(&totalBytes, sizeof(unsigned long), 1, out);
}

/**********************************************************************/
/*****************DECODING FUNCTIONS***********************************/
/**********************************************************************/

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
void gatherHeader(FILE *in, unsigned long *symbolMap)
{
  unsigned char uniqueSymbols;
  unsigned char symbol;
  unsigned char i;
  unsigned long frequency;
  unsigned long total;

  fread(&uniqueSymbols, sizeof(unsigned char), 1, in);
  for(i = 0; i < uniqueSymbols; i++)
  {
    fread(&symbol, sizeof(unsigned char), 1, in);
    fread(&frequency, sizeof(unsigned long), 1, in);
    symbolMap[symbol] = frequency;
  }
  fread(&total, sizeof(unsigned long), 1, in);
  symbolMap[256] = total;
}



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
  unsigned long totalBytes)
{
  struct Node *current = root;
  unsigned char readByte;
  unsigned char bit;
  unsigned char mask;
  unsigned char byteShift;

  mask = 1 << 7;

  while(!feof(in))
  {
    fread(&readByte, sizeof(unsigned char), 1, in);
    for(byteShift = 0; byteShift < 8; byteShift++)
    {
      bit = readByte & mask;
      readByte <<= 1;
      if(bit == 0)
      {
        current = current->left;
        if(isLeaf(current))
        {
          fwrite(&current->symbol, sizeof(unsigned char), 1, out);
          current = root;
        }
      }
      else
      {
        current = current->right;
        if(isLeaf(current))
        {
          fwrite(&current->symbol, sizeof(unsigned char), 1, out);
          current = root;
        }
      }
      /**
       * this breaks out of the byteShifting loop if all original bytes
       * have been written. without this, it writes whatever symbol is 
       * coded to 0 because of the padding in the huff files */
      if(ftell(out) == totalBytes)
      {
        break;
      }
    }
  }
}