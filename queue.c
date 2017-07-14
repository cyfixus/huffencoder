/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

/************************************
 * insertQueue puts passed nodes into a priority queue based on freq and symbol
 * lowest freq, highest priority. tie freq, higher symbol has priority 
 * I decided to maintain a Queue pointer and also leaf pointers*/
struct Node *insertQueue(struct Node *head, struct Node *newNode)
{
  if(head == NULL || newNode->freq < head->freq)
  {
    newNode->next = head;
    return newNode;
  }
  else if(newNode->freq == head->freq)
  {
    if(newNode->symbol > head->symbol)
    {
      newNode->next = head;
      return newNode;
    }
    else if(newNode->symbol == head->symbol)
    {
        if(newNode->left->symbol > head->left->symbol)
        {
          newNode->next = head;
          return newNode;
        }
        else
        {
          head->next = insertQueue(head->next, newNode);
          return head;
        }
    }
    else
    {
      head->next = insertQueue(head->next, newNode);
      return head;
    }
  }
  else
  {
    head->next = insertQueue(head->next, newNode);
    return head;
  }
  return head;
}


/******************************
 * fillQueue takes the awesome array and turns it into nodes
 * and puts it into the queue, saving space by only creating
 * nodes for symbols with a freq > 0
 *   >< >< I tried to do this... w/o the lu* looping through
 * each symbol and creating a node with the freqs... */ 
struct Node *fillQueue(unsigned long *chaChars)
{
  unsigned int i; 
  unsigned long freq;
  unsigned char symbol;
  struct Node *head = NULL;
  struct Node *newNode = NULL;
  for(i = 0; i < 256; i++)
  {
    symbol = i;
    freq = chaChars[i];
    newNode = createNode(freq, symbol);
    if(freq != 0)
    {
      head = insertQueue(head, newNode);
    }
  }
  return head;
}
/******************
 * queueLength returns the length of the queue by iterating through from head*/
int queueLength(struct Node *head)
{
  struct Node *current = head;
  int length = 0;
  while(current)
  {
    length++;
    current = current->next;
  }
  return length;
}

/******************************
 * freeQueue takes Node* and frees the Queue..*/
void freeQueue(struct Node *head)
{
  struct Node *current = head;
  if(current)
  {
    freeQueue(current->next);
    current->next = NULL;
    free(current);
  }
}