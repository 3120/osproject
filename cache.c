
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/sem.h>
#include "cache.h"
#include "string.h"


void cache_init(int size){
    cache_table = malloc( size * sizeof(int));//allocate array space
    cfd_table = malloc( size * sizeof(int));//allocate array space
    file_table = malloc( size * sizeof(char));//allocate array for file cache
    table_size = size;
}

int cache_open(char *file){
    int i = 0;
    int k = 0;
    int j = 0;
    //look for file in cache
    for(i = 0; i < table_size;i++){
        if(cache_table[i] == file){

            return cfd_table[i];//if found, return cfd
        }
    }
    for(k = 0; k < table_size;k++){
        //look for empty space in cache to store file
        
        if(strcmp(file_table[k], file) == 0){
            return cfd_table[k];
        }
    }
    
    //if open space in cache, store file
    for(j = 0; i < table_size;j++){
        if(cfd_table[j] == 0){
            cfd_table[j] = 1; //TODO impliment proper CFD
            file_table[j] = file;
            cache_table[j] = 1;
            printf( "File of size %d cached.\n", sizeof(file));
            return cfd_table[k];
        }
    }
    //if no empty space, store file using LRU(not implimented)
    printf( "File of size %d cached.\n", sizeof(file_table[0]));
    cfd_table[0] = 1;
    file_table[0] = file;
    cache_table[0] = 1;
    
    
    return -1; //if unable to open, return -1
}

int cache_send(int cfd, int client, int n){
    /*for(i = 0; i < table_size;i++){
        if(cfd_table[i] == cfd){
            //removed code for errors
        }
    }*/ //NOT WORKING
}

int cache_filesize(int cfd){
    int i;
    
    for(i = 0; i < table_size;i++){
        if(cfd_table[i] == cfd){
            return sizeof(file_table[i]); //return size of file
        }
    }
    return -1;//if not found, return -1
}

int cache_close(int cfd){
    int i = 0;
    
    for(i = 0; i<table_size; i++){
        if(cfd_table[i] == cfd ){
            if(cache_table[i] > 0){
                printf( "File with cfd [%d] in use.", cfd);
                return -1;
            }
            cfd_table[i] = 0;//de-reference cfd
        }  
    }
    printf( "File with cfd [%d] not found.", cfd);
    return -1; //return -1 if failed
}

