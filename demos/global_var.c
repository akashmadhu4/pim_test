#include <stdio.h> 
#include <defs.h>  

int i = 0;  

int main() { 
  i = i + 1; 
  printf("tasklet %u: i = %u\n", me(), i); 
  return 0; 
} 