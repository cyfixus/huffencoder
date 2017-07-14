/******************
 * cyfixus        *
 * NOV-14-2016    *
*******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

/***************************
 * main opens the two files (in, out) and passes their pointers
 * to be encoded****robot voice**** and the closes the files*/
int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;

  if(argc != 3) 
  {
    printf("wrong number of args\n");
    return 1;
  }

  infile = argv[1];
  outfile = argv[2];

  in = fopen(infile, "rb");
  if(in == NULL)
  {
    printf("couldn't open %s for reading\n", infile);
    return 2;
  }

  out = fopen(outfile, "wb");
  if(out == NULL)
  {
    printf("couldn't open %s for writing\n", outfile);
    return 3;
  }

  initFiles(in, out, 1);

  fclose(in);
  fclose(out);

  return 0;
}