#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern void *guest_at(uint32_t);
extern void entry_0x80126234(void);
extern void FUN_8012708c(uint32_t);
extern int FUN_8016347c(intptr_t);
extern int FUN_8015f644(void);
extern void FUN_8012ae3c(intptr_t,int);
extern void FUN_8012aeec(intptr_t);
extern int g_game_state;
int main(void){
    /* display modes 0..7 */
    for (int m=0;m<8;m++) FUN_8012708c(m);
    printf("display modes ok\n");
    /* arena reset */
    memset(guest_at(0x801beef0),0,0x100);
    printf("arena reset ok\n");
    int r=FUN_8016347c(0x1f800000);
    printf("6347c -> %x\n",r);
    r=FUN_8015f644();
    printf("5f644 -> %x\n",r);
    FUN_8012ae3c(0x1f800000,0);
    printf("2ae3c ok\n");
    printf("PASS all\n");
    return 0;
}
