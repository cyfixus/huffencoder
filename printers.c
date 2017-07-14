/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

/********************************************************
 * printData is called when a file is encoded or decoded
 * it takes in the codeMap, and the # of symbols and totalBytes
 * it then displays this data. symbol  freq   code per the required format*/
void printData(struct CodeMap *codeMap, int uniqueSymbols, 
  unsigned long totalBytes)
{
  int count = 0;
  unsigned char current;
  printf("Symbol\tFreq\tCode\n");
  for(current = 0; current < 257; current++)
  {
    if(count == uniqueSymbols)
    {
      break;
    }
    if(codeMap[current].freq != 0)
    {
      count++;
      if(codeMap[current].symbol > 32 && codeMap[current].symbol < 127)
      {
        printf("%c\t%lu\t%s\n", codeMap[current].symbol, codeMap[current].freq,
          codeMap[current].code);
      }
      else
      {
        printf("=%d\t%lu\t%s\n", codeMap[current].symbol, codeMap[current].freq,
          codeMap[current].code);
      }
    }
  }
  printf("Total chars = %lu\n", totalBytes);
}

/**********************
 * prints the whole tree, the combo nodes and all*/
void printTreeHelper(struct Node *root)
{
  if(root)
  {
    printTreeHelper(root->left);
      if(root->symbol > 32 && root->symbol < 127)
      {
        printf("%c-%lu\t", root->symbol, root->freq);
      }
      else
      {
        printf("=%d-%lu\t", root->symbol, root->freq);
      }
    printTreeHelper(root->right);
  }
}

/********************
 * newLine after printing tree*/
void printTree(struct Node *root)
{
  printTreeHelper(root);
  printf("\n");
}

/************************
 * takes the root, traverses the tree going leftmost
 * first and going from there to print each node w/o kids*/
void printLeavesHelper(struct Node *root)
{
  if(root)
  {
    printLeavesHelper(root->left);
    printLeavesHelper(root->right);
    if(isLeaf(root))
    {
      if(root->symbol > 32 && root->symbol < 127)
      {
        printf("%c-%lu\t\n", root->symbol, root->freq);
      }
      else
      {
        printf("=%d-%lu\t\n", root->symbol, root->freq);
      }
    }
  }
}
/**************
 * newLine after printing the leaves*/
void printLeaves(struct Node *root)
{
  printLeavesHelper(root);
  printf("\n");
}

/******************************
 * printHead takes Node* and prints the head/root..*/
void printHead(struct Node *head)
{
  if(head->symbol > 32 && head->symbol < 127)
  {
    printf("Head: %c\t%lu\n", head->symbol, head->freq);
  }
  else
  {
    printf("Head: =%d\t%lu\n", head->symbol, head->freq);
  }
}

/***************************
 * printQueue takes Node* and prints the formatted Queue */
void printQueue(struct Node *head)
{
  struct Node *current = head;
  while(current)
  {
    if(current->symbol > 32 && current->symbol < 127)
    {
      printf("%c\t%lu\n", current->symbol, current->freq);
    }
    else
    {
      printf("=%d\t%lu\n", current->symbol, current->freq);
    }
    current = current->next;
  }
}

/********************************
 * print takes the lu* and prints it formatted accordingly */
void print(unsigned long *chaChars)
{
  unsigned int i;
  i = 0;
  while(i < 256)
  {
    if(chaChars[i] != 0)
    {
      if( i > 32 && i < 127)
      {
        printf("%c\t%lu\n", i, chaChars[i]);
      }
      else
      {
        printf("=%d\t%lu\n", i, chaChars[i]);
      }
    }
    i++;
  }
  printf("total bytes: %lu\n", chaChars[256]);
}