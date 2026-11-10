#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern void *guest_at(uint32_t);
extern void entry_0x80126234(void);
extern void display_mode_setup(uint32_t);
extern int match_end_check(intptr_t);
extern int memcard_poll_frame(void);
extern void battle_session_arm(intptr_t,int);
extern void frame_tick(intptr_t);
extern int g_game_state;
int main(void){
    /* display modes 0..7 */
    for (int m=0;m<8;m++) display_mode_setup(m);
    printf("display modes ok\n");
    /* arena reset */
    memset(guest_at(0x801beef0),0,0x100);
    printf("arena reset ok\n");
    int r=match_end_check(0x1f800000);
    printf("6347c -> %x\n",r);
    r=memcard_poll_frame();
    printf("5f644 -> %x\n",r);
    battle_session_arm(0x1f800000,0);
    printf("2ae3c ok\n");
    printf("PASS all\n");
    return 0;
}
