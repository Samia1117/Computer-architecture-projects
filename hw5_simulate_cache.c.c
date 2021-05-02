
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

struct block{
    int validbit = 0;
    int dirtybit = 0;
    int tag = 0;
    char data[64];       // max size = 64B = 2^6 * 2^3
    int LRUcount = 100;
};

int main(int argc, char* argv[]) {
    char* filename = argv[1];
    int capacity = atoi(argv[2]) * 1024;
    int assc = atoi(argv[3]);
    char* writetype = argv[4];
    int blocksize = atoi(argv[5]);

    int sets = capacity/(blocksize*assc);
    struct block cache[sets][assc];               // initialized cache;

    FILE* fp;
    char instr[20];
    char data [64];
    int address, bytes;
    char load[10] = "load";
    char type[10] = "wt";

    char memory[65536];
    memset(memory, '\0', 65536);
    fp = fopen (filename, "r");
    while(1){
        int scanfVal = fscanf(fp, "%s", instr);
        if (scanfVal <1){
            break;
        }
        int sameloop = 1;
        if (strcmp(instr, load) ==0){
            fscanf(fp, "%x", &address);
            fscanf(fp, "%d", &bytes);
            int tag = address/(sets*blocksize);              // sets*blocksize = all bytes in a cache set
            int index = (address/blocksize)%sets;
            int offset = address%blocksize;
            //printf("tag, index, offset: %d %d %d\n", tag, index, offset);
            for (int i=0; i<assc; i++){  // handle load hit
                if (cache[index][i].tag == tag && cache[index][i].validbit == 1){
                    char buffer[bytes];
                    for (int j=0; j<bytes; j++){
                        memcpy(buffer+j, (cache[index][i].data)+offset+j, 1); //load the bytes of data onto buffer
                    }
                    printf("load %04x hit ", address); // this printing is all that reading means
                    for (int i=0; i<bytes; i++){
                        printf("%02hhx", *(buffer+i));   // Yes, works
                    }printf("\n");
                    cache[index][i].LRUcount = 0;   // how long ago accessed? 0 calls ago
                    sameloop = 0;
                    break;
                }
            }
            if(sameloop ==0){ //update LRU count
                for (int i=0; i<sets; i++){
                    for (int j=0; j<assc; j++){
                        cache[i][j].LRUcount +=1;
                    }
                }
                continue;
            }
            int start_addr = (index*blocksize) + (tag*sets*blocksize); 
            for (int i=0; i<assc; i++){ 
                if (cache[index][i].validbit ==0){                 //set not full - found invlaid block
                    char buffer[bytes];
                    for (int j=0; j<bytes; j++){ // first write to memory, 
                        memcpy(buffer+j, memory+start_addr+offset+j, 1); // print only say, 67 to 71 - 4 bytes requested 
                    }
                    memcpy(cache[index][i].data, memory+start_addr, blocksize); // ,then copy blocksize data, e.g. 64-127 from memory
                    printf("load %04x miss ", address); // print data that was there in memory
                    for (int i=0; i<bytes; i++){
                        printf("%02hhx", *(buffer+i));   // Yes, works
                    }printf("\n");
                    cache[index][i].dirtybit = 0;     // update fresh block
                    cache[index][i].validbit = 1;
                    cache[index][i].LRUcount= 0;    // last accessed 0 calls ago
                    cache[index][i].tag = tag;
                    sameloop = 0;
                    break;   
                }
            }
            if(sameloop ==0){
                for (int i=0; i<sets; i++){
                    for (int j=0; j<assc; j++){
                        cache[i][j].LRUcount +=1;
                    }
                }
                continue;
            }
            // else, set is full! Choose a block to evict!
            int leastIdx = 0;
            int leastVal = 0;
            for (int i=0; i<assc; i++){
                if (cache[index][i].LRUcount > leastVal){
                    leastIdx = i;
                    leastVal = cache[index][i].LRUcount;
                }
            }
            int LRUblockidx = leastIdx;
            if(strcmp(writetype, type) !=0){ // write back
                if (cache[index][LRUblockidx].dirtybit ==1){ // copy about-to-be-evicted block to memory
                    int evict_start_addr = (index*blocksize) + ((cache[index][LRUblockidx].tag)*sets*blocksize);
                    memcpy(memory+evict_start_addr, cache[index][LRUblockidx].data, blocksize);
                    // cache[index][LRUblockidx].validbit = 0;
                }
            }
            // Now safely overwrite this block in cache if write back
            memcpy(cache[index][LRUblockidx].data, memory+start_addr, blocksize); //fill LRUblock w blocksize data from memory for this start adderess
            cache[index][LRUblockidx].dirtybit = 0;
            cache[index][LRUblockidx].validbit = 1;  //unnecessary
            cache[index][LRUblockidx].LRUcount = 0;
            cache[index][LRUblockidx].tag = tag;
            char buffer[bytes];
            for (int j=0; j<bytes; j++){
                memcpy(buffer+j, memory+start_addr+offset+j, 1);
            }
            printf("load %04x miss ", address);  // last case for read
            for (int i=0; i<bytes; i++){
                printf("%02hhx", *(buffer+i));   // Yes, works
            }printf("\n");
            
            for (int i=0; i<sets; i++){  // update LRU count for all blocks on load
                for (int j=0; j<assc; j++){
                    cache[i][j].LRUcount +=1;
                }
            }
            continue;
            
        }// time to write, now!
        fscanf(fp, "%x", &address);
        fscanf(fp, "%d", &bytes);
        int tag = address/(sets*blocksize);              // sets*blocksize = all bytes in a cache set
        int index = (address/blocksize)%sets;
        int offset = address%blocksize;
        char* storedata = (char*)malloc(bytes);
        for (int i=0; i<bytes; i++){
            fscanf(fp, "%02hhx", storedata+i);
            //printf("2-bytes of data: %02hhx\n", *(storedata+i));   // Yes, works
        }
        int start_addr = (index*blocksize) + (tag*sets*blocksize); 
        if (strcmp(writetype, type) ==0){ // wt
            for (int i=0; i<assc; i++){  
                if (cache[index][i].tag == tag && cache[index][i].validbit == 1){ // wt hit
                    for(int j=0; j<bytes; j++){
                        memcpy((cache[index][i].data)+offset+j, storedata+j, 1);
                        memcpy(memory+start_addr+offset+j, storedata+j, 1); // store in cache and memory, since wt
                    }
                    printf("store %04x hit\n", address); 
                    cache[index][i].LRUcount = 0;   // how long ago accessed? 0 calls ago; 
                    cache[index][i].dirtybit = 1;
                    sameloop = 0;
                    break;
                }
            } 
            if (sameloop ==0){ //updated already  // increment LRU again!
                for (int i=0; i<sets; i++){
                    for (int j=0; j<assc; j++){
                        cache[i][j].LRUcount +=1;
                    }
                }
                continue;
            }               // else store miss and wt: cache block unchanged, just write to memory
            for (int j=0; j<bytes; j++){
                memcpy(memory+start_addr+offset+j, storedata+j, 1); // only write to memory
            }
            printf("store %04x miss\n", address);
            for (int i=0; i<sets; i++){
                for (int j=0; j<assc; j++){
                    cache[i][j].LRUcount +=1;
                }
            }
            continue;
        }
        // write back cache!
        for (int i=0; i<assc; i++){  // writeback hit
            if (cache[index][i].tag == tag && cache[index][i].validbit == 1){ 
                printf("store %04x hit\n", address); 
                for(int j=0; j<bytes; j++){       
                    memcpy((cache[index][i].data)+offset+j, storedata+j, 1);  //copy only to cache block since wb
                }  
                sameloop = 0;
                cache[index][i].dirtybit = 1;
                cache[index][i].LRUcount = 0;   // time since last hit 
                break;
            }
        }
        if (sameloop ==0){ //updated already  // increment LRU again!
            for (int i=0; i<sets; i++){
                for (int j=0; j<assc; j++){
                    cache[i][j].LRUcount +=1;
                }
            }
            continue;
        } 
        for (int i=0; i<assc; i++){            // wb miss without evicting block
            if(cache[index][i].validbit == 0){          
                memcpy(cache[index][i].data, memory+start_addr, blocksize);  // bring in block from memory - write allocate
                for (int j=0; j<bytes; j++){
                    memcpy((cache[index][i].data)+offset+j, storedata+j, 1); // update memory corresponding to this address
                }
                printf("store %04x miss\n", address);
                cache[index][i].dirtybit = 1;
                cache[index][i].validbit = 1;
                cache[index][i].LRUcount = 0;       // how long ago accessed? 0 calls ago; 
                cache[index][i].tag = tag;
                sameloop = 0;
                break;
            }
        }        // else need to evict block, LRU
        if (sameloop ==0){ //updated already  // increment LRU again!
            for (int i=0; i<sets; i++){
                for (int j=0; j<assc; j++){
                    cache[i][j].LRUcount +=1;
                }
            }
            continue;
        } 
        int leastIdx = 0;
        int leastVal = 0;
        for (int i=0; i<assc; i++){
            if (cache[index][i].LRUcount > leastVal){
                leastIdx = i;
                leastVal = cache[index][i].LRUcount;
            }
        }
        int LRUblockidx = leastIdx;
        
        if (cache[index][LRUblockidx].dirtybit ==1){
            // copy about-to-be-evicted block to memory
            int evict_start_addr = (index*blocksize) + ((cache[index][LRUblockidx].tag)*sets*blocksize);
            memcpy(memory+evict_start_addr, cache[index][LRUblockidx].data, blocksize);
        }
        // now safely bring up a block and overwrite to that block to cache
        memcpy(cache[index][LRUblockidx].data, memory+start_addr, blocksize);
        for (int j=0; j<bytes; j++){
            memcpy((cache[index][LRUblockidx].data)+offset+j, storedata+j, 1);        // update memory corresponding to this address
        }
        cache[index][LRUblockidx].dirtybit = 1;
        cache[index][LRUblockidx].validbit = 1;
        cache[index][LRUblockidx].LRUcount = 0;       // how long ago accessed? 0 calls ago; 
        cache[index][LRUblockidx].tag = tag;
        
        for (int i=0; i<sets; i++){ // increment LRU as usual
            for (int j=0; j<assc; j++){
                cache[i][j].LRUcount +=1;
            }
        }
        continue;
    }
    return 0;
}

    

    