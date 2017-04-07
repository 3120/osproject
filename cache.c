
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/sem.h>
#include "cache.h"


void cache_init(int size){
    cache_table = malloc( size * sizeof(int));//allocate array space
    cfd_table = malloc( size * sizeof(int));//allocate array space
    file_table = malloc( size * sizeof(char));//allocate array for file cache
    table_size = size;
}

int cache_open(char *file){
    int i = 0;
    int k = 0;
    
    //look for file in cache
    for(i = 0; i < table_size;i++){
        if(cache_table[i] == file){

            return cfd_table[i];//if found, return cfd
        }
    }
    for(k = 0; k < table_size;k++){
        //look for empty space in cache to store file
        
        if(file_table[k] == file){
            return cfd_table[k];
        }
    }
    
    //TODO if no empty space, store file using LRU
    
    printf( "File of size %d cached.\n", sizeof(file));
    return -1; //if unable to open, return -1
}

int cache_send(int cfd, int client, int n){
    for(i = 0; i < table_size;i++){
        if(cfd_table[i] == cfd){
            //TODO replace this method with working method from test files
        }
    }
}

int cache_filesize(int cfd){
    int i;
    
    for(i = 0; i < table_size;i++){
        if(cfd_table[i] == cfd){
            return sizeof(cache_table[i]); //return size of file
        }
    }
    return -1;//if not found, return -1
}

int cache_close(int cfd){
    
}

