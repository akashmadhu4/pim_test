#include <stdio.h> 
#include <defs.h> 
#include "mutex.h" 

int i = 0; 

MUTEX_INIT(my_mutex); 

int main() { 

  mutex_lock(my_mutex); 
  i = i + 1; 
  printf("tasklet %u: i = %u\n", me(), i); 
  mutex_unlock(my_mutex); 
  
  return 0; 
} 