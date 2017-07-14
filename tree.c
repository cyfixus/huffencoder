/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

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
struct Node *buildHTree(struct Node *root)
{
  if(queueLength(root) > 1)
  {
    struct Node *newNode;
    struct Node *newRoot;
    unsigned long nodeFreq;


    newRoot = root->next->next;

    nodeFreq = root->freq + root->next->freq;
    newNode = createNode(nodeFreq, root->next->symbol);

    newNode->left = root;
    newNode->right = root->next;

    root->next->next = NULL;
    root->next = NULL;

    newRoot = insertQueue(newRoot, newNode);
    return buildHTree(newRoot);
  }
  else
  {
    return root;
  }
}

/***********
 * isLeaf sees if the node has any kids*/
int isLeaf(struct Node *root)
{
  return !(root->left) && !(root->right);
}

/*****
 * returns rightmost symbol*/
unsigned char maxSymbol(struct Node *root)
{
  if(root == NULL)
  {
    printf("MAX NODE NULL\n");
    return -1;
  }
  else if(root->right == NULL)
  {
    return root->symbol;
  }
  else
  {
    return maxSymbol(root->right);
  }
}

/****
 * returns leftmost symbol*/
unsigned char minSymbol(struct Node *root)
{
  if(root == NULL)
  {
    printf("MIN NODE NULL\n");
    return -1;
  }
  else if(root->left == NULL)
  {
    return root->symbol;
  }
  else
  {
    return minSymbol(root->left);
  }
}

/******************************
 * freeTree takes Node* and frees the Tree.*/
void freeTree(struct Node *head)
{
  if(head)
  {
    freeTree(head->left);
    freeTree(head->right);
    head->next = NULL;
    head->left = NULL;
    head->right = NULL;
    free(head);
  }
}