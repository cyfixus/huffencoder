#Huffman Encoder/decoder

1.This huffman encoder works by reading through each byte of the file and assigning
its decimal value as the index of an unsigned long[257] and incrementing the
lu contained with each occurance. The array has space for each char and an 
additional lu for the totalBytes of the file.

2.The unsigned long[] a.k.a symbolMap is then transformed into a priority queue.
The symbol index from the lu[] is assigned as the struct Node->symbol and lu is 
assigned as the Node->freq. 
  a. the lu[] is passed into fillQueue, which iterates through each symbol and 
     creates the node if it has a valid frequency.
  b. once the node is created, it is passed through insertQueue which recursively
     goes through all the nodes to put the new one in the right spot of the queue
     based on its frequency and symbol if needed.
  c. when all the nodes are in the queue, the head pointer is returned and then
     passed into the builHTree function. BuildHTree creates a new node based on
     the combined value of head and head->next, it then inserts this new node,
     children attached, into the queue to be sorted until all nodes have been
     converted into leaves.
  d. after building the tree, buildCodes is called. it takes in an array of struct
     CodeMap used to store the codes for each symbol, and then performs a task similar
     to print leaves to iterate through the tree and assemble the codes as it traverses
     through to each leaf. When a leaf is reached, the unsigned char [] is memcpyd into
     the struct array.
3. To enocde, the generated codes are then referenced to extract each byte of the 
   input file and write the byte in terms of its code. The codes are written into a 
   new byte using masking and bit shifting, and that byte is written to file when full.
   As of 07DEC, I am still leaking and having trouble with the math for the last byte.
4. Decode extracts the symbolMap from the header of the input file and builds the tree
   and codes like encoding did. It then iterates through each byte of the input file,
   following each bit along the tree until it reaches a leaf. The leaf is written to file
   and the iteration returns to the root. 

5. I am having trouble writing the last byte while encoding. I can get it to work for one
   file, at most two. I've tried using the byteCount, codeLength
   every single one of those variables in some sort or fashion.  I'm fairly certain I need
   to extract the exact number of remaining bits, by the position from the byteCount
   and codeCount but I can't figure out the parameters or how to quantize/standardize(what
   is the word? ) I want to input data, and figure out how many bits actually need written
   and make them zeros. 
