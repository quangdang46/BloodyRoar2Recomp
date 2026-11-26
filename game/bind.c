/* bind.c — standalone binding layer for the unified source.
 *
 * The guest-faithful modules declare guest RAM symbols as extern
 * pointers (g_DAT_x, g__DAT_x, g_PTR_x). In the recomp runtime these
 * bind into the emulated address space; for host-side builds/tests
 * this file backs them with a flat arena indexed by guest address:
 *   0x80xxxxxx -> guest_arena[addr & 0x1fffff]   (2MB guest RAM)
 *   0x1f800xxx -> guest_scratch[addr & 0xffff]  (scratch mirror)
 * This is a TEST binding — semantics match the recomp convention
 * (raw guest offsets), not a substitute for the runtime. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cpu_state.h"

static uint8_t  guest_arena[0x200000];
static uint8_t  guest_scratch[0x10000];
void *guest_at(uint32_t addr);
uint32_t guest_of(const void *p);
static uint8_t *guest_for(const char *name)
{
    char hexbuf[64]; int ndx=0; unsigned long best_addr=0; int have_addr=0;
    for (const char *p=name; *p; p++) {
        if ((*p>='0'&&*p<='9')||(*p>='a'&&*p<='f')) { if(ndx<63)hexbuf[ndx++]=*p; }
        else { if(ndx>=6){ hexbuf[ndx]=0; char *endp; unsigned long val=strtoul(hexbuf,&endp,16); if(val>best_addr)best_addr=val; have_addr=1;} ndx=0; }
    }
    if(ndx>=6){ hexbuf[ndx]=0; char *endp; unsigned long val=strtoul(hexbuf,&endp,16); if(val>best_addr)best_addr=val; have_addr=1; }
    if(!have_addr) return guest_arena;
    return (uint8_t *)guest_at((uint32_t)best_addr);
}

/* guest_at — address-based twin of guest_for for sites that carry
 * a raw guest address value rather than a named symbol. */
__attribute__((weak,noinline))
void *guest_at(uint32_t addr)
{
    if ((addr & 0xfffff000UL) == 0x1f800000UL) return guest_scratch + (addr & 0xffff);
    return guest_arena + (addr & 0x1fffffUL);
}

/* guest_sp — current guest stack pointer (KSEG0 top-of-RAM by default).
 * The runtime dispatch adapter sets this from cpu->gpr[29] before each
 * clean-function call so sp-relative guest buffers resolve to real guest
 * RAM. Standalone builds keep the arena-top default. */
static uint32_t g_guest_sp = 0x801ff000;
uint32_t guest_sp(void) { return g_guest_sp; }
void guest_sp_set(uint32_t sp) { g_guest_sp = sp; }

uint8_t * _g_pad_buffer;
uint8_t * g_DAT_1f800015;
uint8_t * g_DAT_1f80006c;
uint8_t * g_DAT_1f800098;
uint8_t * g_DAT_1f8000b1;
uint8_t * g_DAT_1f8000dc8;
uint8_t * g_DAT_1f8000e8;
uint8_t * g_DAT_1f8000ea;
uint8_t * g_DAT_1f8000eb;
uint8_t * g_DAT_1f8000ec;
uint8_t * g_DAT_1f8000ee;
uint8_t * g_DAT_1f800108;
uint8_t * g_DAT_1f800148;
uint8_t * g_DAT_1f8001f0;
uint8_t * g_DAT_1f8001f2;
uint8_t * g_DAT_1f8001f3;
uint8_t * g_DAT_1f8001f6;
uint8_t * g_DAT_1f8002f8;
uint8_t * g_DAT_1f80030c;
uint8_t * g_DAT_1f80038c;
uint8_t * g_DAT_1f800394;
uint8_t * g_DAT_1f800398;
uint8_t * g_DAT_1f80039c;
uint8_t * g_DAT_1f8003a0;
uint8_t * g_DAT_1f8003a4;
uint8_t * g_DAT_1f8003a8;
uint8_t * g_DAT_1f8003b0;
uint8_t * g_DAT_1f8003b4;
uint8_t * g_DAT_1f8003c0;
uint8_t * g_DAT_1f8003e0;
uint8_t * g_DAT_1f8003f8;
uint8_t * g_DAT_8011ec40;
uint8_t * g_DAT_8011ed80;
uint8_t * g_DAT_8011ede0;
uint8_t * g_DAT_8011ee40;
uint8_t * g_DAT_8011ee5b;
uint8_t * g_DAT_8011ee74;
uint8_t * g_DAT_8011ee8f;
uint8_t * g_DAT_8011eea4;
uint8_t * g_DAT_8011eed4;
uint8_t * g_DAT_8011ef04;
uint8_t * g_DAT_8011ef64;
uint8_t * g_DAT_8011ef75;
uint8_t * g_DAT_8011f944;
uint8_t * g_DAT_8011f946;
uint8_t * g_DAT_8011f948;
uint8_t * g_DAT_8011f9c4;
uint8_t * g_DAT_8011f9c6;
uint8_t * g_DAT_8011f9c8;
uint8_t * g_DAT_8011fa04;
uint8_t * g_DAT_8011fa0c;
uint8_t * g_DAT_8011fd78;
uint8_t * g_DAT_8011fd7a;
uint8_t * g_DAT_8011fd7a2;
uint8_t * g_DAT_8011fd98;
uint8_t * g_DAT_8011fea0;
uint8_t * g_DAT_8011fea1;
uint8_t * g_DAT_8011fea2;
uint8_t * g_DAT_8011feac;
uint8_t * g_DAT_8011fead;
uint8_t * g_DAT_8011feae;
uint8_t * g_DAT_801237e0;
uint8_t * g_DAT_80123844;
uint8_t * g_DAT_80124048;
uint8_t * g_DAT_801247a8;
uint8_t * g_DAT_801b140c;
uint8_t * g_DAT_801b144c;
uint8_t * g_DAT_801b1528;
uint8_t * g_DAT_801b152a;
uint8_t * g_DAT_801b1590;
uint8_t * g_DAT_801b1592;
uint8_t * g_DAT_801b17f4;
uint8_t * g_DAT_801b180c;
uint8_t * g_DAT_801b1820;
uint8_t * g_DAT_801b1824;
uint8_t * g_DAT_801b1828;
uint8_t * g_DAT_801b182c;
uint8_t * g_DAT_801b1830;
uint8_t * g_DAT_801b1840;
uint8_t * g_DAT_801b1844;
uint8_t * g_DAT_801b1848;
uint8_t * g_DAT_801b184c;
uint8_t * g_DAT_801b1850;
uint8_t * g_DAT_801b1854;
uint8_t * g_DAT_801b1858;
uint8_t * g_DAT_801b185c;
uint8_t * g_DAT_801b1880;
uint8_t * g_DAT_801b18a8;
uint8_t * g_DAT_801b18d0;
uint8_t * g_DAT_801b18f8;
uint8_t * g_DAT_801b1904;
uint8_t * g_DAT_801b192c;
uint8_t * g_DAT_801b1a18;
uint8_t * g_DAT_801b1a2c;
uint8_t * g_DAT_801b1a3c;
uint8_t * g_DAT_801b1a50;
uint8_t * g_DAT_801b1a74;
uint8_t * g_DAT_801b1a94;
uint8_t * g_DAT_801b1ab8;
uint8_t * g_DAT_801b1ad8;
uint8_t * g_DAT_801b1b1c;
uint8_t * g_DAT_801b1b5c;
uint8_t * g_DAT_801b1b78;
uint8_t * g_DAT_801b1b90;
uint8_t * g_DAT_801b1bb4;
uint8_t * g_DAT_801b1bd4;
uint8_t * g_DAT_801b1be8;
uint8_t * g_DAT_801b1bfc;
uint8_t * g_DAT_801b1c30;
uint8_t * g_DAT_801b1c60;
uint8_t * g_DAT_801b1c74;
uint8_t * g_DAT_801b1cbc;
uint8_t * g_DAT_801b1ccc;
uint8_t * g_DAT_801b1d48;
uint8_t * g_DAT_801b1d5c;
uint8_t * g_DAT_801b1d70;
uint8_t * g_DAT_801b1d78;
uint8_t * g_DAT_801b1ea4;
uint8_t * g_DAT_801b1eac;
uint8_t * g_DAT_801b22ec;
uint8_t * g_DAT_801b2308;
uint8_t * g_DAT_801b2314;
uint8_t * g_DAT_801b2330;
uint8_t * g_DAT_801b2332;
uint8_t * g_DAT_801b2334;
uint8_t * g_DAT_801b2338;
uint8_t * g_DAT_801b2340;
uint8_t * g_DAT_801b2342;
uint8_t * g_DAT_801b2344;
uint8_t * g_DAT_801b2348;
uint8_t * g_DAT_801b4a30;
uint8_t * g_DAT_801b4a48;
uint8_t * g_DAT_801b4ac8;
uint8_t * g_DAT_801b4b48;
uint8_t * g_DAT_801b4b54;
uint8_t * g_DAT_801b4b56;
uint8_t * g_DAT_801b4b5c;
uint8_t * g_DAT_801b4b6c;
uint8_t * g_DAT_801b4b6d;
uint8_t * g_DAT_801b4b6e;
uint8_t * g_DAT_801b4b6f;
uint8_t * g_DAT_801b4b94;
uint8_t * g_DAT_801b4b95;
uint8_t * g_DAT_801b4b96;
uint8_t * g_DAT_801b4b97;
uint8_t * g_DAT_801b4be4;
uint8_t * g_DAT_801b4c24;
uint8_t * g_DAT_801b4c25;
uint8_t * g_DAT_801b4c26;
uint8_t * g_DAT_801b4c27;
uint8_t * g_DAT_801b4c28;
uint8_t * g_DAT_801b4c29;
uint8_t * g_DAT_801b4c2a;
uint8_t * g_DAT_801b4c2b;
uint8_t * g_DAT_801b4c30;
uint8_t * g_DAT_801b4c32;
uint8_t * g_DAT_801b4c34;
uint8_t * g_DAT_801b4c36;
uint8_t * g_DAT_801b4c40;
uint8_t * g_DAT_801b4c42;
uint8_t * g_DAT_801b4c44;
uint8_t * g_DAT_801b4c46;
uint8_t * g_DAT_801b4c50;
uint8_t * g_DAT_801b4c52;
uint8_t * g_DAT_801b4c54;
uint8_t * g_DAT_801b4c56;
uint8_t * g_DAT_801b4c88;
uint8_t * g_DAT_801b4c8a;
uint8_t * g_DAT_801b4c8c;
uint8_t * g_DAT_801b4c8e;
uint8_t * g_DAT_801b4c9c;
uint8_t * g_DAT_801b4c9e;
uint8_t * g_DAT_801b4cbc;
uint8_t * g_DAT_801b4ccc;
uint8_t * g_DAT_801b4cd2;
uint8_t * g_DAT_801b4cdc;
uint8_t * g_DAT_801b4cec;
uint8_t * g_DAT_801b4cf2;
uint8_t * g_DAT_801b4d28;
uint8_t * g_DAT_801b4d34;
uint8_t * g_DAT_801b4d3c;
uint8_t * g_DAT_801b4d50;
uint8_t * g_DAT_801b4d58;
uint8_t * g_DAT_801b4d6c;
uint8_t * g_DAT_801b4d7c;
uint8_t * g_DAT_801b4d90;
uint8_t * g_DAT_801b4d98;
uint8_t * g_DAT_801b4dac;
uint8_t * g_DAT_801b4dbc;
uint8_t * g_DAT_801b4dd4;
uint8_t * g_DAT_801b4de8;
uint8_t * g_DAT_801b4e08;
uint8_t * g_DAT_801b4e20;
uint8_t * g_DAT_801b4e28;
uint8_t * g_DAT_801b4e2c;
uint8_t * g_DAT_801b4e38;
uint8_t * g_DAT_801b4e44;
uint8_t * g_DAT_801b5288;
uint8_t * g_DAT_801b5298;
uint8_t * g_DAT_801b52a8;
uint8_t * g_DAT_801b52b8;
uint8_t * g_DAT_801b5370;
uint8_t * g_DAT_801b55d0;
uint8_t * g_DAT_801b67a0;
uint8_t * g_DAT_801b67a4;
uint8_t * g_DAT_801b67a8;
uint8_t * g_DAT_801b8614;
uint8_t * g_DAT_801be838;
uint8_t * g_DAT_801bef20;
uint8_t * g_DAT_801c43a8;
uint8_t * g_DAT_801cf88c;
uint8_t * g_DAT_801e40b9;
uint8_t * g_DAT_801e40ba;
uint8_t * g_DAT_801e40bb;
uint8_t * g_DAT_801e83aa;
uint8_t * g_DAT_801e83b0;
uint8_t * g_DAT_801f0228;
uint8_t * g_DAT_801f0234;
uint8_t * g_DAT_801f0250;
uint8_t * g_DAT_801f0300;
uint8_t * g_DAT_801f0304;
uint8_t * g_DAT_801f0328;
uint8_t * g_DAT_801f04a8;
uint8_t * g_DAT_801f04c8;
uint8_t * g_DAT_801f04c9;
uint8_t * g_DAT_801f04e4;
uint8_t * g_DAT_801f04ec;
uint8_t * g_DAT_801f04ee;
uint8_t * g_DAT_801f04f0;
uint8_t * g_DAT_801f0664;
uint8_t * g_DAT_801f066e;
uint8_t * g_DAT_801f0740;
uint8_t * g_DAT_801f0750;
uint8_t * g_DAT_801f0760;
uint8_t * g_DAT_801f0780;
uint8_t * g_DAT_801f0790;
uint8_t * g_DAT_801f0940;
uint8_t * g_DAT_801f0ad8;
uint8_t * g_DAT_801f0ad9;
uint8_t * g_DAT_801f0ae0;
uint8_t * g_DAT_801f0ae4;
uint8_t * g_DAT_801f0db0;
uint8_t * g_DAT_801f0db4;
uint8_t * g_DAT_801f0fb0;
uint8_t * g_DAT_801f35f4;
uint8_t * g_DAT_801f35f4i;
uint8_t * g_DAT_801f3600;
uint8_t * g_DAT_801f36ba;
uint8_t * g_DAT_801f4cbc;
uint8_t * g_DAT_801f4d70;
uint8_t * g_DAT_801f4d72;
uint8_t * g_DAT_801f4d74;
uint8_t * g_DAT_801f4d76;
uint8_t * g_DAT_801f4d78;
uint8_t * g_DAT_801f4f28;
uint8_t * g_DAT_801f51d0;
uint8_t * g_DAT_801f51d8;
uint8_t * g_DAT_801f5210;
uint8_t * g_DAT_801f5218;
uint8_t * g_DAT_801f596c;
uint8_t * g_DAT_801f596d;
uint8_t * g_DAT_801f5970;
uint8_t * g_DAT_801f5971;
uint8_t * g_PTR_DAT_801b19d4;
uint8_t * g_PTR_DAT_801b1d44;
uint8_t * g_PTR_DAT_801b1e8c;
uint8_t * g_PTR_DAT_801b1ec0;
uint8_t * g_PTR_DAT_801b47ec;
uint8_t * g_PTR_DAT_801b4e10;
uint8_t * g_PTR_DAT_801b4e14;
uint8_t * g_PTR_DAT_801be84c;
uint8_t * g_PTR_FUN_801b1ea8;
uint8_t * g_PTR_FUN_801b1ec0;
uint32_t * g_PTR_PTR_801be894;
uint8_t * g__DAT_1f800008;
uint8_t * g__DAT_1f80000c;
uint8_t * g__DAT_1f800024;
uint8_t * g__DAT_1f800082;
uint8_t * g__DAT_1f8000c4;
uint8_t * g__DAT_1f8000dc;
uint8_t * g__DAT_1f800100;
uint8_t * g__DAT_1f800104;
uint8_t * g__DAT_1f800110;
uint8_t * g__DAT_1f800148;
uint8_t * g__DAT_1f800154;
uint8_t * g__DAT_1f800208;
uint8_t * g__DAT_1f80020c;
uint8_t * g__DAT_1f800218;
uint8_t * g__DAT_1f800250;
uint8_t * g__DAT_1f80025c;
uint8_t * g__DAT_1f80037c;
uint8_t * g__DAT_1f80037e;
uint8_t * g__DAT_1f800380;
uint8_t * g__DAT_1f800388;
uint8_t * g__DAT_1f80038c;
uint8_t * g__DAT_1f80038e;
uint8_t * g__DAT_1f800390;
uint8_t * g__DAT_1f800392;
uint8_t * g__DAT_1f800394;
uint8_t * g__DAT_1f800396;
uint8_t * g__DAT_1f800398;
uint8_t * g__DAT_1f80039a;
uint8_t * g__DAT_1f80039c;
uint8_t * g__DAT_1f80039e;
uint8_t * g__DAT_1f8003a0;
uint8_t * g__DAT_1f8003a2;
uint8_t * g__DAT_1f8003a4;
uint8_t * g__DAT_1f8003a8;
uint8_t * g__DAT_1f8003ac;
uint8_t * g__DAT_1f8003b0;
uint8_t * g__DAT_1f8003b2;
uint8_t * g__DAT_1f8003b4;
uint8_t * g__DAT_1f8003b6;
uint8_t * g__DAT_1f8003b8;
uint8_t * g__DAT_1f8003ba;
uint8_t * g__DAT_1f8003bc;
uint8_t * g__DAT_1f8003be;
uint8_t * g__DAT_1f8003c0;
uint8_t * g__DAT_1f8003cc;
uint8_t * g__DAT_1f8003d0;
uint8_t * g__DAT_1f8003d4;
uint8_t * g__DAT_1f8003d8;
uint8_t * g__DAT_1f8003dc;
uint8_t * g__DAT_1f8003e0;
uint8_t * g__DAT_1f8003e4;
uint8_t * g__DAT_1f8003e8;
uint8_t * g__DAT_1f8003ec;
uint8_t * g__DAT_1f8003f0;
uint8_t * g__DAT_1f8003f4;
uint8_t * g__DAT_1f8003f8;
uint8_t * g__DAT_1f8003fc;
uint8_t * g__DAT_801b22f8;
uint8_t * g__DAT_801b22fc;
uint8_t * g__DAT_801b2304;
uint8_t * g__DAT_801c4494;
uint8_t * g__DAT_801c44e4;
uint8_t * g__DAT_801c4508;
uint8_t * g__DAT_801c4578;
uint8_t * g__DAT_801f0228;
uint8_t * g__DAT_801f022c;
uint8_t * g__DAT_801f0292;
uint8_t * g__DAT_801f0298;
uint8_t * g__DAT_801f029a;
uint8_t * g__DAT_801f02f6;
uint8_t * g__DAT_801f02f8;
uint8_t * g__DAT_801f04a0;
int32_t * g__DAT_801f04a4;
uint8_t * g__DAT_801f04b0;
uint8_t * g__DAT_801f04b8;
uint8_t * g__DAT_801f04bc;
uint8_t * g__DAT_801f04c0;
uint8_t * g__DAT_801f04c6;
uint8_t * g__DAT_801f04e4;
uint8_t * g__DAT_801f04e6;
uint8_t * g__DAT_801f04e8;
uint8_t * g__DAT_801f04ea;
uint8_t * g__DAT_801f04ec;
uint8_t * g__DAT_801f04ee;
uint8_t * g__DAT_801f04f0;
uint8_t * g__DAT_801f04f2;
uint8_t * g__DAT_801f04f4;
uint8_t * g__DAT_801f04f6;
uint8_t * g__DAT_801f04f8;
uint8_t * g__DAT_801f04fa;
uint8_t * g__DAT_801f0654;
uint8_t * g__DAT_801f0656;
uint8_t * g__DAT_801f0730;
uint8_t * g__DAT_801f0734;
uint8_t * g__DAT_801f0738;
uint8_t * g__DAT_801f0740;
uint8_t * g__DAT_801f0744;
uint8_t * g__DAT_801f0748;
uint8_t * g__DAT_801f0758;
uint8_t * g__DAT_801f075c;
uint8_t * g__DAT_801f0760;
uint8_t * g__DAT_801f0764;
uint8_t * g__DAT_801f0768;
uint8_t * g__DAT_801f076c;
uint8_t * g__DAT_801f0770;
uint8_t * g__DAT_801f0780;
uint8_t * g__DAT_801f0784;
uint8_t * g__DAT_801f0788;
uint8_t * g__DAT_801f0790;
uint8_t * g__DAT_801f0794;
uint8_t * g__DAT_801f0798;
uint8_t * g__DAT_801f0930;
uint8_t * g__DAT_801f0938;
uint8_t * g__DAT_801f093a;
uint8_t * g__DAT_801f093c;
uint8_t * g__DAT_801f093e;
uint8_t * g__DAT_801f0940;
uint8_t * g__DAT_801f0944;
uint8_t * g__DAT_801f4cb2;
uint8_t * g__DAT_801f4cb4;
uint8_t * g__DAT_801f4cb6;
uint8_t * g__DAT_801f4cb8;
uint8_t * g__DAT_801f4cba;
uint8_t * g__DAT_801f4f20;
uint8_t * g__DAT_801f50c0;
uint8_t * g__DAT_801f50d0;
uint8_t * g__DAT_801f50e4;
uint8_t * g__DAT_801f50f4;
uint8_t * g__DAT_801f5150;
uint8_t * g__DAT_801f5160;
uint8_t * g__DAT_801f5174;
uint8_t * g__DAT_801f5184;
uint8_t * g__DAT_801f5210;
uint8_t * g__DAT_801f5214;
uint8_t * g__DAT_801f68e0;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
#endif
static void bind_globals_1(void)
{
    _g_pad_buffer = (uint8_t *)guest_for("_g_pad_buffer");
    g_DAT_1f800015 = (uint8_t *)guest_for("g_DAT_1f800015");
    g_DAT_1f80006c = (uint8_t *)guest_for("g_DAT_1f80006c");
    g_DAT_1f800098 = (uint8_t *)guest_for("g_DAT_1f800098");
    g_DAT_1f8000b1 = (uint8_t *)guest_for("g_DAT_1f8000b1");
    g_DAT_1f8000dc8 = (uint8_t *)guest_for("g_DAT_1f8000dc8");
    g_DAT_1f8000e8 = (uint8_t *)guest_for("g_DAT_1f8000e8");
    g_DAT_1f8000ea = (uint8_t *)guest_for("g_DAT_1f8000ea");
    g_DAT_1f8000eb = (uint8_t *)guest_for("g_DAT_1f8000eb");
    g_DAT_1f8000ec = (uint8_t *)guest_for("g_DAT_1f8000ec");
    g_DAT_1f8000ee = (uint8_t *)guest_for("g_DAT_1f8000ee");
    g_DAT_1f800108 = (uint8_t *)guest_for("g_DAT_1f800108");
    g_DAT_1f800148 = (uint8_t *)guest_for("g_DAT_1f800148");
    g_DAT_1f8001f0 = (uint8_t *)guest_for("g_DAT_1f8001f0");
    g_DAT_1f8001f2 = (uint8_t *)guest_for("g_DAT_1f8001f2");
    g_DAT_1f8001f3 = (uint8_t *)guest_for("g_DAT_1f8001f3");
    g_DAT_1f8001f6 = (uint8_t *)guest_for("g_DAT_1f8001f6");
    g_DAT_1f8002f8 = (uint8_t *)guest_for("g_DAT_1f8002f8");
    g_DAT_1f80030c = (uint8_t *)guest_for("g_DAT_1f80030c");
    g_DAT_1f80038c = (uint8_t *)guest_for("g_DAT_1f80038c");
    g_DAT_1f800394 = (uint8_t *)guest_for("g_DAT_1f800394");
    g_DAT_1f800398 = (uint8_t *)guest_for("g_DAT_1f800398");
    g_DAT_1f80039c = (uint8_t *)guest_for("g_DAT_1f80039c");
    g_DAT_1f8003a0 = (uint8_t *)guest_for("g_DAT_1f8003a0");
    g_DAT_1f8003a4 = (uint8_t *)guest_for("g_DAT_1f8003a4");
    g_DAT_1f8003a8 = (uint8_t *)guest_for("g_DAT_1f8003a8");
    g_DAT_1f8003b0 = (uint8_t *)guest_for("g_DAT_1f8003b0");
    g_DAT_1f8003b4 = (uint8_t *)guest_for("g_DAT_1f8003b4");
    g_DAT_1f8003c0 = (uint8_t *)guest_for("g_DAT_1f8003c0");
    g_DAT_1f8003e0 = (uint8_t *)guest_for("g_DAT_1f8003e0");
    g_DAT_1f8003f8 = (uint8_t *)guest_for("g_DAT_1f8003f8");
    g_DAT_8011ec40 = (uint8_t *)guest_for("g_DAT_8011ec40");
    g_DAT_8011ed80 = (uint8_t *)guest_for("g_DAT_8011ed80");
    g_DAT_8011ede0 = (uint8_t *)guest_for("g_DAT_8011ede0");
    g_DAT_8011ee40 = (uint8_t *)guest_for("g_DAT_8011ee40");
    g_DAT_8011ee5b = (uint8_t *)guest_for("g_DAT_8011ee5b");
    g_DAT_8011ee74 = (uint8_t *)guest_for("g_DAT_8011ee74");
    g_DAT_8011ee8f = (uint8_t *)guest_for("g_DAT_8011ee8f");
    g_DAT_8011eea4 = (uint8_t *)guest_for("g_DAT_8011eea4");
    g_DAT_8011eed4 = (uint8_t *)guest_for("g_DAT_8011eed4");
    g_DAT_8011ef04 = (uint8_t *)guest_for("g_DAT_8011ef04");
    g_DAT_8011ef64 = (uint8_t *)guest_for("g_DAT_8011ef64");
    g_DAT_8011ef75 = (uint8_t *)guest_for("g_DAT_8011ef75");
    g_DAT_8011f944 = (uint8_t *)guest_for("g_DAT_8011f944");
    g_DAT_8011f946 = (uint8_t *)guest_for("g_DAT_8011f946");
    g_DAT_8011f948 = (uint8_t *)guest_for("g_DAT_8011f948");
    g_DAT_8011f9c4 = (uint8_t *)guest_for("g_DAT_8011f9c4");
    g_DAT_8011f9c6 = (uint8_t *)guest_for("g_DAT_8011f9c6");
    g_DAT_8011f9c8 = (uint8_t *)guest_for("g_DAT_8011f9c8");
    g_DAT_8011fa04 = (uint8_t *)guest_for("g_DAT_8011fa04");
    g_DAT_8011fa0c = (uint8_t *)guest_for("g_DAT_8011fa0c");
    g_DAT_8011fd78 = (uint8_t *)guest_for("g_DAT_8011fd78");
    g_DAT_8011fd7a = (uint8_t *)guest_for("g_DAT_8011fd7a");
    g_DAT_8011fd7a2 = (uint8_t *)guest_for("g_DAT_8011fd7a2");
    g_DAT_8011fd98 = (uint8_t *)guest_for("g_DAT_8011fd98");
    g_DAT_8011fea0 = (uint8_t *)guest_for("g_DAT_8011fea0");
    g_DAT_8011fea1 = (uint8_t *)guest_for("g_DAT_8011fea1");
    g_DAT_8011fea2 = (uint8_t *)guest_for("g_DAT_8011fea2");
    g_DAT_8011feac = (uint8_t *)guest_for("g_DAT_8011feac");
    g_DAT_8011fead = (uint8_t *)guest_for("g_DAT_8011fead");
    g_DAT_8011feae = (uint8_t *)guest_for("g_DAT_8011feae");
    g_DAT_801237e0 = (uint8_t *)guest_for("g_DAT_801237e0");
    g_DAT_80123844 = (uint8_t *)guest_for("g_DAT_80123844");
    g_DAT_80124048 = (uint8_t *)guest_for("g_DAT_80124048");
    g_DAT_801247a8 = (uint8_t *)guest_for("g_DAT_801247a8");
    g_DAT_801b140c = (uint8_t *)guest_for("g_DAT_801b140c");
    g_DAT_801b144c = (uint8_t *)guest_for("g_DAT_801b144c");
    g_DAT_801b1528 = (uint8_t *)guest_for("g_DAT_801b1528");
    g_DAT_801b152a = (uint8_t *)guest_for("g_DAT_801b152a");
    g_DAT_801b1590 = (uint8_t *)guest_for("g_DAT_801b1590");
    g_DAT_801b1592 = (uint8_t *)guest_for("g_DAT_801b1592");
    g_DAT_801b17f4 = (uint8_t *)guest_for("g_DAT_801b17f4");
    g_DAT_801b180c = (uint8_t *)guest_for("g_DAT_801b180c");
    g_DAT_801b1820 = (uint8_t *)guest_for("g_DAT_801b1820");
    g_DAT_801b1824 = (uint8_t *)guest_for("g_DAT_801b1824");
    g_DAT_801b1828 = (uint8_t *)guest_for("g_DAT_801b1828");
    g_DAT_801b182c = (uint8_t *)guest_for("g_DAT_801b182c");
    g_DAT_801b1830 = (uint8_t *)guest_for("g_DAT_801b1830");
    g_DAT_801b1840 = (uint8_t *)guest_for("g_DAT_801b1840");
    g_DAT_801b1844 = (uint8_t *)guest_for("g_DAT_801b1844");
    g_DAT_801b1848 = (uint8_t *)guest_for("g_DAT_801b1848");
    g_DAT_801b184c = (uint8_t *)guest_for("g_DAT_801b184c");
    g_DAT_801b1850 = (uint8_t *)guest_for("g_DAT_801b1850");
    g_DAT_801b1854 = (uint8_t *)guest_for("g_DAT_801b1854");
    g_DAT_801b1858 = (uint8_t *)guest_for("g_DAT_801b1858");
    g_DAT_801b185c = (uint8_t *)guest_for("g_DAT_801b185c");
    g_DAT_801b1880 = (uint8_t *)guest_for("g_DAT_801b1880");
    g_DAT_801b18a8 = (uint8_t *)guest_for("g_DAT_801b18a8");
    g_DAT_801b18d0 = (uint8_t *)guest_for("g_DAT_801b18d0");
    g_DAT_801b18f8 = (uint8_t *)guest_for("g_DAT_801b18f8");
    g_DAT_801b1904 = (uint8_t *)guest_for("g_DAT_801b1904");
    g_DAT_801b192c = (uint8_t *)guest_for("g_DAT_801b192c");
    g_DAT_801b1a18 = (uint8_t *)guest_for("g_DAT_801b1a18");
    g_DAT_801b1a2c = (uint8_t *)guest_for("g_DAT_801b1a2c");
    g_DAT_801b1a3c = (uint8_t *)guest_for("g_DAT_801b1a3c");
    g_DAT_801b1a50 = (uint8_t *)guest_for("g_DAT_801b1a50");
    g_DAT_801b1a74 = (uint8_t *)guest_for("g_DAT_801b1a74");
    g_DAT_801b1a94 = (uint8_t *)guest_for("g_DAT_801b1a94");
    g_DAT_801b1ab8 = (uint8_t *)guest_for("g_DAT_801b1ab8");
    g_DAT_801b1ad8 = (uint8_t *)guest_for("g_DAT_801b1ad8");
    g_DAT_801b1b1c = (uint8_t *)guest_for("g_DAT_801b1b1c");
    g_DAT_801b1b5c = (uint8_t *)guest_for("g_DAT_801b1b5c");
    g_DAT_801b1b78 = (uint8_t *)guest_for("g_DAT_801b1b78");
    g_DAT_801b1b90 = (uint8_t *)guest_for("g_DAT_801b1b90");
    g_DAT_801b1bb4 = (uint8_t *)guest_for("g_DAT_801b1bb4");
    g_DAT_801b1bd4 = (uint8_t *)guest_for("g_DAT_801b1bd4");
    g_DAT_801b1be8 = (uint8_t *)guest_for("g_DAT_801b1be8");
    g_DAT_801b1bfc = (uint8_t *)guest_for("g_DAT_801b1bfc");
    g_DAT_801b1c30 = (uint8_t *)guest_for("g_DAT_801b1c30");
    g_DAT_801b1c60 = (uint8_t *)guest_for("g_DAT_801b1c60");
    g_DAT_801b1c74 = (uint8_t *)guest_for("g_DAT_801b1c74");
    g_DAT_801b1cbc = (uint8_t *)guest_for("g_DAT_801b1cbc");
    g_DAT_801b1ccc = (uint8_t *)guest_for("g_DAT_801b1ccc");
    g_DAT_801b1d48 = (uint8_t *)guest_for("g_DAT_801b1d48");
    g_DAT_801b1d5c = (uint8_t *)guest_for("g_DAT_801b1d5c");
    g_DAT_801b1d70 = (uint8_t *)guest_for("g_DAT_801b1d70");
    g_DAT_801b1d78 = (uint8_t *)guest_for("g_DAT_801b1d78");
    g_DAT_801b1ea4 = (uint8_t *)guest_for("g_DAT_801b1ea4");
    g_DAT_801b1eac = (uint8_t *)guest_for("g_DAT_801b1eac");
    g_DAT_801b22ec = (uint8_t *)guest_for("g_DAT_801b22ec");
    g_DAT_801b2308 = (uint8_t *)guest_for("g_DAT_801b2308");
    g_DAT_801b2314 = (uint8_t *)guest_for("g_DAT_801b2314");
    g_DAT_801b2330 = (uint8_t *)guest_for("g_DAT_801b2330");
    g_DAT_801b2332 = (uint8_t *)guest_for("g_DAT_801b2332");
    g_DAT_801b2334 = (uint8_t *)guest_for("g_DAT_801b2334");
    g_DAT_801b2338 = (uint8_t *)guest_for("g_DAT_801b2338");
    g_DAT_801b2340 = (uint8_t *)guest_for("g_DAT_801b2340");
    g_DAT_801b2342 = (uint8_t *)guest_for("g_DAT_801b2342");
    g_DAT_801b2344 = (uint8_t *)guest_for("g_DAT_801b2344");
    g_DAT_801b2348 = (uint8_t *)guest_for("g_DAT_801b2348");
    g_DAT_801b4a30 = (uint8_t *)guest_for("g_DAT_801b4a30");
    g_DAT_801b4a48 = (uint8_t *)guest_for("g_DAT_801b4a48");
    g_DAT_801b4ac8 = (uint8_t *)guest_for("g_DAT_801b4ac8");
    g_DAT_801b4b48 = (uint8_t *)guest_for("g_DAT_801b4b48");
    g_DAT_801b4b54 = (uint8_t *)guest_for("g_DAT_801b4b54");
    g_DAT_801b4b56 = (uint8_t *)guest_for("g_DAT_801b4b56");
    g_DAT_801b4b5c = (uint8_t *)guest_for("g_DAT_801b4b5c");
    g_DAT_801b4b6c = (uint8_t *)guest_for("g_DAT_801b4b6c");
    g_DAT_801b4b6d = (uint8_t *)guest_for("g_DAT_801b4b6d");
    g_DAT_801b4b6e = (uint8_t *)guest_for("g_DAT_801b4b6e");
    g_DAT_801b4b6f = (uint8_t *)guest_for("g_DAT_801b4b6f");
    g_DAT_801b4b94 = (uint8_t *)guest_for("g_DAT_801b4b94");
    g_DAT_801b4b95 = (uint8_t *)guest_for("g_DAT_801b4b95");
    g_DAT_801b4b96 = (uint8_t *)guest_for("g_DAT_801b4b96");
    g_DAT_801b4b97 = (uint8_t *)guest_for("g_DAT_801b4b97");
    g_DAT_801b4be4 = (uint8_t *)guest_for("g_DAT_801b4be4");
    g_DAT_801b4c24 = (uint8_t *)guest_for("g_DAT_801b4c24");
    g_DAT_801b4c25 = (uint8_t *)guest_for("g_DAT_801b4c25");
    g_DAT_801b4c26 = (uint8_t *)guest_for("g_DAT_801b4c26");
    g_DAT_801b4c27 = (uint8_t *)guest_for("g_DAT_801b4c27");
    g_DAT_801b4c28 = (uint8_t *)guest_for("g_DAT_801b4c28");
    g_DAT_801b4c29 = (uint8_t *)guest_for("g_DAT_801b4c29");
    g_DAT_801b4c2a = (uint8_t *)guest_for("g_DAT_801b4c2a");
    g_DAT_801b4c2b = (uint8_t *)guest_for("g_DAT_801b4c2b");
    g_DAT_801b4c30 = (uint8_t *)guest_for("g_DAT_801b4c30");
    g_DAT_801b4c32 = (uint8_t *)guest_for("g_DAT_801b4c32");
    g_DAT_801b4c34 = (uint8_t *)guest_for("g_DAT_801b4c34");
    g_DAT_801b4c36 = (uint8_t *)guest_for("g_DAT_801b4c36");
    g_DAT_801b4c40 = (uint8_t *)guest_for("g_DAT_801b4c40");
    g_DAT_801b4c42 = (uint8_t *)guest_for("g_DAT_801b4c42");
    g_DAT_801b4c44 = (uint8_t *)guest_for("g_DAT_801b4c44");
    g_DAT_801b4c46 = (uint8_t *)guest_for("g_DAT_801b4c46");
    g_DAT_801b4c50 = (uint8_t *)guest_for("g_DAT_801b4c50");
    g_DAT_801b4c52 = (uint8_t *)guest_for("g_DAT_801b4c52");
    g_DAT_801b4c54 = (uint8_t *)guest_for("g_DAT_801b4c54");
    g_DAT_801b4c56 = (uint8_t *)guest_for("g_DAT_801b4c56");
    g_DAT_801b4c88 = (uint8_t *)guest_for("g_DAT_801b4c88");
    g_DAT_801b4c8a = (uint8_t *)guest_for("g_DAT_801b4c8a");
    g_DAT_801b4c8c = (uint8_t *)guest_for("g_DAT_801b4c8c");
    g_DAT_801b4c8e = (uint8_t *)guest_for("g_DAT_801b4c8e");
    g_DAT_801b4c9c = (uint8_t *)guest_for("g_DAT_801b4c9c");
    g_DAT_801b4c9e = (uint8_t *)guest_for("g_DAT_801b4c9e");
    g_DAT_801b4cbc = (uint8_t *)guest_for("g_DAT_801b4cbc");
    g_DAT_801b4ccc = (uint8_t *)guest_for("g_DAT_801b4ccc");
    g_DAT_801b4cd2 = (uint8_t *)guest_for("g_DAT_801b4cd2");
    g_DAT_801b4cdc = (uint8_t *)guest_for("g_DAT_801b4cdc");
    g_DAT_801b4cec = (uint8_t *)guest_for("g_DAT_801b4cec");
    g_DAT_801b4cf2 = (uint8_t *)guest_for("g_DAT_801b4cf2");
    g_DAT_801b4d28 = (uint8_t *)guest_for("g_DAT_801b4d28");
    g_DAT_801b4d34 = (uint8_t *)guest_for("g_DAT_801b4d34");
    g_DAT_801b4d3c = (uint8_t *)guest_for("g_DAT_801b4d3c");
    g_DAT_801b4d50 = (uint8_t *)guest_for("g_DAT_801b4d50");
    g_DAT_801b4d58 = (uint8_t *)guest_for("g_DAT_801b4d58");
    g_DAT_801b4d6c = (uint8_t *)guest_for("g_DAT_801b4d6c");
    g_DAT_801b4d7c = (uint8_t *)guest_for("g_DAT_801b4d7c");
    g_DAT_801b4d90 = (uint8_t *)guest_for("g_DAT_801b4d90");
    g_DAT_801b4d98 = (uint8_t *)guest_for("g_DAT_801b4d98");
    g_DAT_801b4dac = (uint8_t *)guest_for("g_DAT_801b4dac");
    g_DAT_801b4dbc = (uint8_t *)guest_for("g_DAT_801b4dbc");
    g_DAT_801b4dd4 = (uint8_t *)guest_for("g_DAT_801b4dd4");
    g_DAT_801b4de8 = (uint8_t *)guest_for("g_DAT_801b4de8");
    g_DAT_801b4e08 = (uint8_t *)guest_for("g_DAT_801b4e08");
    g_DAT_801b4e20 = (uint8_t *)guest_for("g_DAT_801b4e20");
    g_DAT_801b4e28 = (uint8_t *)guest_for("g_DAT_801b4e28");
    g_DAT_801b4e2c = (uint8_t *)guest_for("g_DAT_801b4e2c");
    g_DAT_801b4e38 = (uint8_t *)guest_for("g_DAT_801b4e38");
    g_DAT_801b4e44 = (uint8_t *)guest_for("g_DAT_801b4e44");
    g_DAT_801b5288 = (uint8_t *)guest_for("g_DAT_801b5288");
    g_DAT_801b5298 = (uint8_t *)guest_for("g_DAT_801b5298");
    g_DAT_801b52a8 = (uint8_t *)guest_for("g_DAT_801b52a8");
    g_DAT_801b52b8 = (uint8_t *)guest_for("g_DAT_801b52b8");
    g_DAT_801b5370 = (uint8_t *)guest_for("g_DAT_801b5370");
    g_DAT_801b55d0 = (uint8_t *)guest_for("g_DAT_801b55d0");
    g_DAT_801b67a0 = (uint8_t *)guest_for("g_DAT_801b67a0");
    g_DAT_801b67a4 = (uint8_t *)guest_for("g_DAT_801b67a4");
    g_DAT_801b67a8 = (uint8_t *)guest_for("g_DAT_801b67a8");
    g_DAT_801b8614 = (uint8_t *)guest_for("g_DAT_801b8614");
    g_DAT_801be838 = (uint8_t *)guest_for("g_DAT_801be838");
    g_DAT_801bef20 = (uint8_t *)guest_for("g_DAT_801bef20");
    g_DAT_801c43a8 = (uint8_t *)guest_for("g_DAT_801c43a8");
    g_DAT_801cf88c = (uint8_t *)guest_for("g_DAT_801cf88c");
    g_DAT_801e40b9 = (uint8_t *)guest_for("g_DAT_801e40b9");
    g_DAT_801e40ba = (uint8_t *)guest_for("g_DAT_801e40ba");
    g_DAT_801e40bb = (uint8_t *)guest_for("g_DAT_801e40bb");
    g_DAT_801e83aa = (uint8_t *)guest_for("g_DAT_801e83aa");
    g_DAT_801e83b0 = (uint8_t *)guest_for("g_DAT_801e83b0");
    g_DAT_801f0228 = (uint8_t *)guest_for("g_DAT_801f0228");
    g_DAT_801f0234 = (uint8_t *)guest_for("g_DAT_801f0234");
    g_DAT_801f0250 = (uint8_t *)guest_for("g_DAT_801f0250");
    g_DAT_801f0300 = (uint8_t *)guest_for("g_DAT_801f0300");
    g_DAT_801f0304 = (uint8_t *)guest_for("g_DAT_801f0304");
    g_DAT_801f0328 = (uint8_t *)guest_for("g_DAT_801f0328");
    g_DAT_801f04a8 = (uint8_t *)guest_for("g_DAT_801f04a8");
    g_DAT_801f04c8 = (uint8_t *)guest_for("g_DAT_801f04c8");
    g_DAT_801f04c9 = (uint8_t *)guest_for("g_DAT_801f04c9");
    g_DAT_801f04e4 = (uint8_t *)guest_for("g_DAT_801f04e4");
    g_DAT_801f04ec = (uint8_t *)guest_for("g_DAT_801f04ec");
    g_DAT_801f04ee = (uint8_t *)guest_for("g_DAT_801f04ee");
    g_DAT_801f04f0 = (uint8_t *)guest_for("g_DAT_801f04f0");
    g_DAT_801f0664 = (uint8_t *)guest_for("g_DAT_801f0664");
    g_DAT_801f066e = (uint8_t *)guest_for("g_DAT_801f066e");
    g_DAT_801f0740 = (uint8_t *)guest_for("g_DAT_801f0740");
    g_DAT_801f0750 = (uint8_t *)guest_for("g_DAT_801f0750");
    g_DAT_801f0760 = (uint8_t *)guest_for("g_DAT_801f0760");
    g_DAT_801f0780 = (uint8_t *)guest_for("g_DAT_801f0780");
    g_DAT_801f0790 = (uint8_t *)guest_for("g_DAT_801f0790");
    g_DAT_801f0940 = (uint8_t *)guest_for("g_DAT_801f0940");
    g_DAT_801f0ad8 = (uint8_t *)guest_for("g_DAT_801f0ad8");
    g_DAT_801f0ad9 = (uint8_t *)guest_for("g_DAT_801f0ad9");
    g_DAT_801f0ae0 = (uint8_t *)guest_for("g_DAT_801f0ae0");
    g_DAT_801f0ae4 = (uint8_t *)guest_for("g_DAT_801f0ae4");
    g_DAT_801f0db0 = (uint8_t *)guest_for("g_DAT_801f0db0");
    g_DAT_801f0db4 = (uint8_t *)guest_for("g_DAT_801f0db4");
    g_DAT_801f0fb0 = (uint8_t *)guest_for("g_DAT_801f0fb0");
    g_DAT_801f35f4i = (uint8_t *)guest_for("g_DAT_801f35f4i");
    g_DAT_801f3600 = (uint8_t *)guest_for("g_DAT_801f3600");
    g_DAT_801f36ba = (uint8_t *)guest_for("g_DAT_801f36ba");
    g_DAT_801f4cbc = (uint8_t *)guest_for("g_DAT_801f4cbc");
    g_DAT_801f4d70 = (uint8_t *)guest_for("g_DAT_801f4d70");
    g_DAT_801f4d72 = (uint8_t *)guest_for("g_DAT_801f4d72");
    g_DAT_801f4d74 = (uint8_t *)guest_for("g_DAT_801f4d74");
    g_DAT_801f4d76 = (uint8_t *)guest_for("g_DAT_801f4d76");
    g_DAT_801f4d78 = (uint8_t *)guest_for("g_DAT_801f4d78");
    g_DAT_801f4f28 = (uint8_t *)guest_for("g_DAT_801f4f28");
    g_DAT_801f51d0 = (uint8_t *)guest_for("g_DAT_801f51d0");
    g_DAT_801f51d8 = (uint8_t *)guest_for("g_DAT_801f51d8");
    g_DAT_801f5210 = (uint8_t *)guest_for("g_DAT_801f5210");
    g_DAT_801f5218 = (uint8_t *)guest_for("g_DAT_801f5218");
    g_DAT_801f596c = (uint8_t *)guest_for("g_DAT_801f596c");
    g_DAT_801f596d = (uint8_t *)guest_for("g_DAT_801f596d");
    g_DAT_801f5970 = (uint8_t *)guest_for("g_DAT_801f5970");
    g_DAT_801f5971 = (uint8_t *)guest_for("g_DAT_801f5971");
    g_PTR_DAT_801b19d4 = (uint8_t *)guest_for("g_PTR_DAT_801b19d4");
    g_PTR_DAT_801b1d44 = (uint8_t *)guest_for("g_PTR_DAT_801b1d44");
    g_PTR_DAT_801b1e8c = (uint8_t *)guest_for("g_PTR_DAT_801b1e8c");
    g_PTR_DAT_801b1ec0 = (uint8_t *)guest_for("g_PTR_DAT_801b1ec0");
    g_PTR_DAT_801b47ec = (uint8_t *)guest_for("g_PTR_DAT_801b47ec");
    g_PTR_DAT_801b4e10 = (uint8_t *)guest_for("g_PTR_DAT_801b4e10");
    g_PTR_DAT_801b4e14 = (uint8_t *)guest_for("g_PTR_DAT_801b4e14");
    g_PTR_DAT_801be84c = (uint8_t *)guest_for("g_PTR_DAT_801be84c");
    g_PTR_FUN_801b1ea8 = (uint8_t *)guest_for("g_PTR_FUN_801b1ea8");
    g_PTR_FUN_801b1ec0 = (uint8_t *)guest_for("g_PTR_FUN_801b1ec0");
    g_PTR_PTR_801be894 = (uint32_t *)guest_for("g_PTR_PTR_801be894");
    g__DAT_1f800008 = (uint8_t *)guest_for("g__DAT_1f800008");
    g__DAT_1f80000c = (uint8_t *)guest_for("g__DAT_1f80000c");
    g__DAT_1f800024 = (uint8_t *)guest_for("g__DAT_1f800024");
    g__DAT_1f800082 = (uint8_t *)guest_for("g__DAT_1f800082");
    g__DAT_1f8000c4 = (uint8_t *)guest_for("g__DAT_1f8000c4");
    g__DAT_1f8000dc = (uint8_t *)guest_for("g__DAT_1f8000dc");
    g__DAT_1f800100 = (uint8_t *)guest_for("g__DAT_1f800100");
    g__DAT_1f800104 = (uint8_t *)guest_for("g__DAT_1f800104");
    g__DAT_1f800110 = (uint8_t *)guest_for("g__DAT_1f800110");
    g__DAT_1f800148 = (uint8_t *)guest_for("g__DAT_1f800148");
    g__DAT_1f800154 = (uint8_t *)guest_for("g__DAT_1f800154");
    g__DAT_1f800208 = (uint8_t *)guest_for("g__DAT_1f800208");
    g__DAT_1f80020c = (uint8_t *)guest_for("g__DAT_1f80020c");
    g__DAT_1f800218 = (uint8_t *)guest_for("g__DAT_1f800218");
    g__DAT_1f800250 = (uint8_t *)guest_for("g__DAT_1f800250");
    g__DAT_1f80025c = (uint8_t *)guest_for("g__DAT_1f80025c");
    g__DAT_1f80037c = (uint8_t *)guest_for("g__DAT_1f80037c");
    g__DAT_1f80037e = (uint8_t *)guest_for("g__DAT_1f80037e");
    g__DAT_1f800380 = (uint8_t *)guest_for("g__DAT_1f800380");
    g__DAT_1f800388 = (uint8_t *)guest_for("g__DAT_1f800388");
    g__DAT_1f80038c = (uint8_t *)guest_for("g__DAT_1f80038c");
    g__DAT_1f80038e = (uint8_t *)guest_for("g__DAT_1f80038e");
    g__DAT_1f800390 = (uint8_t *)guest_for("g__DAT_1f800390");
    g__DAT_1f800392 = (uint8_t *)guest_for("g__DAT_1f800392");
    g__DAT_1f800394 = (uint8_t *)guest_for("g__DAT_1f800394");
    g__DAT_1f800396 = (uint8_t *)guest_for("g__DAT_1f800396");
    g__DAT_1f800398 = (uint8_t *)guest_for("g__DAT_1f800398");
    g__DAT_1f80039a = (uint8_t *)guest_for("g__DAT_1f80039a");
    g__DAT_1f80039c = (uint8_t *)guest_for("g__DAT_1f80039c");
    g__DAT_1f80039e = (uint8_t *)guest_for("g__DAT_1f80039e");
    g__DAT_1f8003a0 = (uint8_t *)guest_for("g__DAT_1f8003a0");
    g__DAT_1f8003a2 = (uint8_t *)guest_for("g__DAT_1f8003a2");
    g__DAT_1f8003a4 = (uint8_t *)guest_for("g__DAT_1f8003a4");
    g__DAT_1f8003a8 = (uint8_t *)guest_for("g__DAT_1f8003a8");
    g__DAT_1f8003ac = (uint8_t *)guest_for("g__DAT_1f8003ac");
    g__DAT_1f8003b0 = (uint8_t *)guest_for("g__DAT_1f8003b0");
    g__DAT_1f8003b2 = (uint8_t *)guest_for("g__DAT_1f8003b2");
    g__DAT_1f8003b4 = (uint8_t *)guest_for("g__DAT_1f8003b4");
    g__DAT_1f8003b6 = (uint8_t *)guest_for("g__DAT_1f8003b6");
    g__DAT_1f8003b8 = (uint8_t *)guest_for("g__DAT_1f8003b8");
    g__DAT_1f8003ba = (uint8_t *)guest_for("g__DAT_1f8003ba");
    g__DAT_1f8003bc = (uint8_t *)guest_for("g__DAT_1f8003bc");
    g__DAT_1f8003be = (uint8_t *)guest_for("g__DAT_1f8003be");
    g__DAT_1f8003c0 = (uint8_t *)guest_for("g__DAT_1f8003c0");
    g__DAT_1f8003cc = (uint8_t *)guest_for("g__DAT_1f8003cc");
    g__DAT_1f8003d0 = (uint8_t *)guest_for("g__DAT_1f8003d0");
    g__DAT_1f8003d4 = (uint8_t *)guest_for("g__DAT_1f8003d4");
    g__DAT_1f8003d8 = (uint8_t *)guest_for("g__DAT_1f8003d8");
    g__DAT_1f8003dc = (uint8_t *)guest_for("g__DAT_1f8003dc");
    g__DAT_1f8003e0 = (uint8_t *)guest_for("g__DAT_1f8003e0");
    g__DAT_1f8003e4 = (uint8_t *)guest_for("g__DAT_1f8003e4");
    g__DAT_1f8003e8 = (uint8_t *)guest_for("g__DAT_1f8003e8");
    g__DAT_1f8003ec = (uint8_t *)guest_for("g__DAT_1f8003ec");
    g__DAT_1f8003f0 = (uint8_t *)guest_for("g__DAT_1f8003f0");
    g__DAT_1f8003f4 = (uint8_t *)guest_for("g__DAT_1f8003f4");
    g__DAT_1f8003f8 = (uint8_t *)guest_for("g__DAT_1f8003f8");
    g__DAT_1f8003fc = (uint8_t *)guest_for("g__DAT_1f8003fc");
    g__DAT_801b22f8 = (uint8_t *)guest_for("g__DAT_801b22f8");
    g__DAT_801b22fc = (uint8_t *)guest_for("g__DAT_801b22fc");
    g__DAT_801b2304 = (uint8_t *)guest_for("g__DAT_801b2304");
    g__DAT_801c4494 = (uint8_t *)guest_for("g__DAT_801c4494");
    g__DAT_801c44e4 = (uint8_t *)guest_for("g__DAT_801c44e4");
    g__DAT_801c4508 = (uint8_t *)guest_for("g__DAT_801c4508");
    g__DAT_801c4578 = (uint8_t *)guest_for("g__DAT_801c4578");
    g__DAT_801f0228 = (uint8_t *)guest_for("g__DAT_801f0228");
    g__DAT_801f022c = (uint8_t *)guest_for("g__DAT_801f022c");
    g__DAT_801f0292 = (uint8_t *)guest_for("g__DAT_801f0292");
    g__DAT_801f0298 = (uint8_t *)guest_for("g__DAT_801f0298");
    g__DAT_801f029a = (uint8_t *)guest_for("g__DAT_801f029a");
    g__DAT_801f02f6 = (uint8_t *)guest_for("g__DAT_801f02f6");
    g__DAT_801f02f8 = (uint8_t *)guest_for("g__DAT_801f02f8");
    g__DAT_801f04a0 = (uint8_t *)guest_for("g__DAT_801f04a0");
    g__DAT_801f04a4 = (int32_t *)guest_for("g__DAT_801f04a4");
    g__DAT_801f04b0 = (uint8_t *)guest_for("g__DAT_801f04b0");
    g__DAT_801f04b8 = (uint8_t *)guest_for("g__DAT_801f04b8");
    g__DAT_801f04bc = (uint8_t *)guest_for("g__DAT_801f04bc");
    g__DAT_801f04c0 = (uint8_t *)guest_for("g__DAT_801f04c0");
    g__DAT_801f04c6 = (uint8_t *)guest_for("g__DAT_801f04c6");
    g__DAT_801f04e4 = (uint8_t *)guest_for("g__DAT_801f04e4");
    g__DAT_801f04e6 = (uint8_t *)guest_for("g__DAT_801f04e6");
    g__DAT_801f04e8 = (uint8_t *)guest_for("g__DAT_801f04e8");
    g__DAT_801f04ea = (uint8_t *)guest_for("g__DAT_801f04ea");
    g__DAT_801f04ec = (uint8_t *)guest_for("g__DAT_801f04ec");
    g__DAT_801f04ee = (uint8_t *)guest_for("g__DAT_801f04ee");
    g__DAT_801f04f0 = (uint8_t *)guest_for("g__DAT_801f04f0");
    g__DAT_801f04f2 = (uint8_t *)guest_for("g__DAT_801f04f2");
    g__DAT_801f04f4 = (uint8_t *)guest_for("g__DAT_801f04f4");
    g__DAT_801f04f6 = (uint8_t *)guest_for("g__DAT_801f04f6");
    g__DAT_801f04f8 = (uint8_t *)guest_for("g__DAT_801f04f8");
    g__DAT_801f04fa = (uint8_t *)guest_for("g__DAT_801f04fa");
    g__DAT_801f0654 = (uint8_t *)guest_for("g__DAT_801f0654");
    g__DAT_801f0656 = (uint8_t *)guest_for("g__DAT_801f0656");
    g__DAT_801f0730 = (uint8_t *)guest_for("g__DAT_801f0730");
    g__DAT_801f0734 = (uint8_t *)guest_for("g__DAT_801f0734");
    g__DAT_801f0738 = (uint8_t *)guest_for("g__DAT_801f0738");
    g__DAT_801f0740 = (uint8_t *)guest_for("g__DAT_801f0740");
    g__DAT_801f0744 = (uint8_t *)guest_for("g__DAT_801f0744");
    g__DAT_801f0748 = (uint8_t *)guest_for("g__DAT_801f0748");
    g__DAT_801f0758 = (uint8_t *)guest_for("g__DAT_801f0758");
    g__DAT_801f075c = (uint8_t *)guest_for("g__DAT_801f075c");
    g__DAT_801f0760 = (uint8_t *)guest_for("g__DAT_801f0760");
    g__DAT_801f0764 = (uint8_t *)guest_for("g__DAT_801f0764");
    g__DAT_801f0768 = (uint8_t *)guest_for("g__DAT_801f0768");
    g__DAT_801f076c = (uint8_t *)guest_for("g__DAT_801f076c");
    g__DAT_801f0770 = (uint8_t *)guest_for("g__DAT_801f0770");
    g__DAT_801f0780 = (uint8_t *)guest_for("g__DAT_801f0780");
    g__DAT_801f0784 = (uint8_t *)guest_for("g__DAT_801f0784");
    g__DAT_801f0788 = (uint8_t *)guest_for("g__DAT_801f0788");
    g__DAT_801f0790 = (uint8_t *)guest_for("g__DAT_801f0790");
    g__DAT_801f0794 = (uint8_t *)guest_for("g__DAT_801f0794");
    g__DAT_801f0798 = (uint8_t *)guest_for("g__DAT_801f0798");
    g__DAT_801f0930 = (uint8_t *)guest_for("g__DAT_801f0930");
    g__DAT_801f0938 = (uint8_t *)guest_for("g__DAT_801f0938");
    g__DAT_801f093a = (uint8_t *)guest_for("g__DAT_801f093a");
    g__DAT_801f093c = (uint8_t *)guest_for("g__DAT_801f093c");
    g__DAT_801f093e = (uint8_t *)guest_for("g__DAT_801f093e");
    g__DAT_801f0940 = (uint8_t *)guest_for("g__DAT_801f0940");
    g__DAT_801f0944 = (uint8_t *)guest_for("g__DAT_801f0944");
    g__DAT_801f4cb2 = (uint8_t *)guest_for("g__DAT_801f4cb2");
    g__DAT_801f4cb4 = (uint8_t *)guest_for("g__DAT_801f4cb4");
    g__DAT_801f4cb6 = (uint8_t *)guest_for("g__DAT_801f4cb6");
    g__DAT_801f4cb8 = (uint8_t *)guest_for("g__DAT_801f4cb8");
    g__DAT_801f4cba = (uint8_t *)guest_for("g__DAT_801f4cba");
    g__DAT_801f4f20 = (uint8_t *)guest_for("g__DAT_801f4f20");
    g__DAT_801f50c0 = (uint8_t *)guest_for("g__DAT_801f50c0");
    g__DAT_801f50d0 = (uint8_t *)guest_for("g__DAT_801f50d0");
    g__DAT_801f50e4 = (uint8_t *)guest_for("g__DAT_801f50e4");
    g__DAT_801f50f4 = (uint8_t *)guest_for("g__DAT_801f50f4");
    g__DAT_801f5150 = (uint8_t *)guest_for("g__DAT_801f5150");
    g__DAT_801f5160 = (uint8_t *)guest_for("g__DAT_801f5160");
    g__DAT_801f5174 = (uint8_t *)guest_for("g__DAT_801f5174");
    g__DAT_801f5184 = (uint8_t *)guest_for("g__DAT_801f5184");
    g__DAT_801f5210 = (uint8_t *)guest_for("g__DAT_801f5210");
    g__DAT_801f5214 = (uint8_t *)guest_for("g__DAT_801f5214");
    g__DAT_801f68e0 = (uint8_t *)guest_for("g__DAT_801f68e0");
}

int host_rand(void) { return rand() % 100; }
/* action_select — select the event stream for action `idx` on `ctrl`.
 * Only reachable from the legacy ev_step EV5 path in ev.c (dead code —
 * the live dispatch is ev_stream_dispatch -> PTR_FUN_801b4e68 ->
 * ev5_action_select @0x8017ae18, which calls action_switch_guest
 * @0x8017a6e8). Faithful equivalent of that guest tail: motion obj is
 * ctrl+0x38. */
extern void action_switch_guest(int,int*,int);
void action_select(uint8_t *ctrl,int idx)
{
    uint32_t motion_g;
    if (!ctrl) return;
    motion_g = *(const uint32_t *)(ctrl + 0x38);   /* ctrl -> motion obj */
    /* action_switch_guest has no zero test; guest_at(0) is still a
     * valid low-memory object, so do not skip on motion_g==0. */
    action_switch_guest((int)guest_of(ctrl),(int *)guest_at(motion_g),idx);
}
/* reaction_load removed: the guest tail at 0x8017b090 calls
 * fighter_anim_advance(ctrl,motion) directly; ev.c calls it now. */
void trace_op(int op){ (void)op; }
extern CPUState *clean_cpu_state(void);

/* COP2/GTE register operand decode.  The clean port carries the COP2 move's
 * low-16 instruction bits for wide accesses (rd field in bits[15:11], so the
 * value is >= 0x800) and a bare register index for narrow ones; both resolve
 * to a GTE register 0-31.  Verified against the recompiled MIPS: e.g.
 * 0x4884F000 (MTC2 rd=30) -> arg 0xF000, 0x4802F800 (MFC2 rd=31) -> 0xF800. */
static int cop2_reg_index(int reg){ return (reg>=0x800)?((reg>>11)&0x1f):(reg&0x1f); }

void setCopReg(int fmt,int reg,uint32_t v){ (void)fmt; CPUState*c=clean_cpu_state(); if(c) gte_write_data(c,(uint8_t)cop2_reg_index(reg),v); }
int  getCopReg(int fmt,int reg){ (void)fmt; CPUState*c=clean_cpu_state(); return c?(int)gte_read_data(c,(uint8_t)cop2_reg_index(reg)):0; }
void copFunction(int fmt,uint32_t op){ (void)fmt; CPUState*c=clean_cpu_state(); if(c) gte_execute(c,op); }
void setCopControlWord(int fmt,int reg,uint32_t v){ (void)fmt; CPUState*c=clean_cpu_state(); if(c) gte_write_ctrl(c,(uint8_t)cop2_reg_index(reg),v); }
int  getCopControlWord(int fmt,int reg){ (void)fmt; CPUState*c=clean_cpu_state(); return c?(int)gte_read_ctrl(c,(uint8_t)cop2_reg_index(reg)):0; }



/* ---- second-pass bindings (arrays / scalars / fn-ptr tables) ---- */
uint8_t  FUN_801b0db8[0x400];            /* guest data table @0x801b0db8 */
uint8_t  LAB_8013ce68[0x100];
uint8_t  LAB_801aae08[0x100];
uint8_t  s_0123456789ABCDEF_80125208[17];
uint8_t  s_0123456789abcdef_8012521c[17];
uint8_t  s__1___02d__1x_0_80123264[64];
uint32_t PTR_DAT_801b5574[64];
uint32_t PTR_entry_0x80182f3c_801b5528[64];
void    *PTR_FUN_801b1734[64];
void    *PTR_LAB_801200b8[64];
void    *g_PTR_FUN_801b55c0[64];

uint8_t *g__DAT_1f8003c4, *g__DAT_1f8003c8;
uint8_t *g_DAT_801b4e50, *g_DAT_801b4e5c, *g_DAT_801e40b6;
uint8_t *g_DAT_801f04f2, *g_DAT_801f04f4, *g_DAT_801f04f6, *g_DAT_801f04f8, *g_DAT_801f04fa;
uint8_t *g_DAT_801f4d7a, *g_DAT_801f4d7c;

void trap(int code) { (void)code; }

/* pending_action_gate == pending_gate (fight.c), guest arg order:
 * (motion, ctrl, reqA, reqB) — a0 is a guest address; resolve it
 * before handing pending_gate its host pointer. */
int32_t pending_gate(uint8_t *motion, uint8_t *ctrl,
                         uint32_t req_a, uint32_t req_b);
int32_t pending_action_gate(uint32_t motion, uint8_t *ctrl,
                     uint32_t req_a, uint32_t req_b)
{
    return pending_gate((uint8_t *)guest_at(motion), ctrl,
                            req_a, req_b);
}

/* Delegation bridge to the recompiled guest code. br2_call_generated runs
 * the real br2eu_func_* body on the live CPUState (args in $a0-$a3,
 * result in $v0); src/clean_bridge.c owns the strong definition. The weak
 * br2_guest_frame/free pair below is a standalone fallback — the runtime
 * build uses clean_bridge's version, which also mirrors the reservation
 * into cpu->gpr[29]. */
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t br2_call_clean(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void br2_guest_frame_free(void);
void br2_st32(uint32_t addr, uint32_t val);
static uint32_t g_bind_frm_sp[32];
static int g_bind_frm_depth;
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t br2_guest_frame(uint32_t size)
{
    uint32_t sp = guest_sp();
    uint32_t ns = (sp - ((size + 15u) & ~15u)) & ~15u;
    if (g_bind_frm_depth < 32) g_bind_frm_sp[g_bind_frm_depth++] = sp;
    guest_sp_set(ns);
    return ns;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_guest_frame_free(void)
{
    if (g_bind_frm_depth <= 0) return;
    guest_sp_set(g_bind_frm_sp[--g_bind_frm_depth]);
}

/* COP2 register-ABI GTE functions — clean ports of the guest leaf
 * routines (pure GTE math, no IRQ/sp dependence). The ani.c/gfx.c
 * ports own the algorithm; these FUN_ wrappers keep the caller-facing
 * C signatures (host pointers in/out) and the guest $v0 contract. */
extern void anim_vec3_normalize(void);
extern void gte_vec3_normalize(void);
extern uint32_t fixed_rsqrt_q12(uint32_t);
extern uint32_t fixed_recip_lut(uint32_t);
extern void gte_ofs_set(int32_t,int32_t);
extern uint32_t gte_screen_init_defer(void);
extern uint32_t gte_sqr_vec3_b(uint32_t,uint32_t);
extern uint32_t bios_svc6(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t * gte_xform3_t_ip(uint32_t *xf, uint16_t *vec);
extern void br2_bios_tramp(uint32_t,uint32_t);
extern void gpu_state_patch_defer(void);
extern void FUN_8018e300(void);
extern void FUN_8018e310(uint32_t);
extern int psx_syscall(CPUState *cpu, uint32_t code);
uint32_t br2_ld32(uint32_t addr);
uint16_t br2_ld16(uint32_t addr);
uint8_t  br2_ld8 (uint32_t addr);
void     br2_st8 (uint32_t addr, uint8_t  val);
void     br2_st16(uint32_t addr, uint16_t val);

/* COP2 register field encoding shared with ani.c/gfx.c: reg n -> n*0x800. */
#define BR2_GTE_REG(n) ((n) * 0x800)

/* bind_gte_vec3_mag2 — issue SQR on the live $t0-$t2 vector (gpr[8..10])
 * and return MAC1+MAC2+MAC3, the magnitude-squared the guest leaves in
 * $v0 across the 0x80194690 normalize core. The core re-writes IR1..3
 * and re-issues SQR itself, so this pre-pass leaves no divergent GTE
 * state. */
static uint32_t bind_gte_vec3_mag2(CPUState *c)
{
    uint32_t mag2;
    setCopReg(2, BR2_GTE_REG(9),  c->gpr[8]);
    setCopReg(2, BR2_GTE_REG(10), c->gpr[9]);
    setCopReg(2, BR2_GTE_REG(11), c->gpr[10]);
    copFunction(2, 0x0A00428);              /* SQR */
    mag2  = (uint32_t)getCopReg(2, BR2_GTE_REG(25));
    mag2 += (uint32_t)getCopReg(2, BR2_GTE_REG(26));
    mag2 += (uint32_t)getCopReg(2, BR2_GTE_REG(27));
    return mag2;
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801851d8(void) { anim_vec3_normalize(); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194500(uint32_t val) { return (int32_t)fixed_rsqrt_q12(val); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194630(const void *in, void *out)
{
    /* guest 0x80194630: lw t0-t2 = in[0..2] (3 words), move a3,ra,
     * jal 0x80194690 (GTE normalize core; $v0 = x^2+y^2+z^2),
     * sw t0-t2 -> out[0..2], restore ra, jr ra.
     * Callers pass host pointers; the ra save/restore is the C call. */
    const uint32_t *iv = (const uint32_t *)in;
    uint32_t *ov = (uint32_t *)out;
    CPUState *c = clean_cpu_state();
    uint32_t mag2;
    if (!c) return 0;
    c->gpr[8]  = iv[0];
    c->gpr[9]  = iv[1];
    c->gpr[10] = iv[2];
    mag2 = bind_gte_vec3_mag2(c);
    gte_vec3_normalize();
    ov[0] = c->gpr[8];
    ov[1] = c->gpr[9];
    ov[2] = c->gpr[10];
    return (int32_t)mag2;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194660(const void *in, void *out)
{
    /* guest 0x80194660 (+ split tail 0x8019466C): lh t0-t2 =
     * in[0..2] (3 s16, sign-extended), jal 0x80194690,
     * sh t0-t2 -> out[0..2], $v0 = magnitude-squared. */
    const int16_t *iv = (const int16_t *)in;
    uint16_t *ov = (uint16_t *)out;
    CPUState *c = clean_cpu_state();
    uint32_t mag2;
    if (!c) return 0;
    c->gpr[8]  = (uint32_t)(int32_t)iv[0];
    c->gpr[9]  = (uint32_t)(int32_t)iv[1];
    c->gpr[10] = (uint32_t)(int32_t)iv[2];
    mag2 = bind_gte_vec3_mag2(c);
    gte_vec3_normalize();
    ov[0] = (uint16_t)c->gpr[8];
    ov[1] = (uint16_t)c->gpr[9];
    ov[2] = (uint16_t)c->gpr[10];
    return (int32_t)mag2;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194690(void)
{
    /* guest 0x80194690: GTE vec3 normalize core on the live
     * $t0-$t2 (gpr[8..10]); $v0 = MAC1+MAC2+MAC3 magnitude-squared. */
    CPUState *c = clean_cpu_state();
    uint32_t mag2;
    if (!c) return 0;
    mag2 = bind_gte_vec3_mag2(c);
    gte_vec3_normalize();
    return (int32_t)mag2;
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
#endif
static void bind_globals_2(void)
{
    /* Route through guest_at() so these globals land in the REAL guest RAM
     * (memory_get_ram_ptr()/scratchpad) in the runtime build, not the private
     * standalone guest_arena/guest_scratch. The weak guest_at() fallback still
     * resolves to the arena for standalone builds, so both stay correct. */
    g__DAT_1f8003c4 = (uint8_t *)guest_at(0x1f8003c4);
    g__DAT_1f8003c8 = (uint8_t *)guest_at(0x1f8003c8);
    g_DAT_801b4e50 = (uint8_t *)guest_at(0x801b4e50);
    g_DAT_801b4e5c = (uint8_t *)guest_at(0x801b4e5c);
    g_DAT_801e40b6 = (uint8_t *)guest_at(0x801e40b6);
    g_DAT_801f04f2 = (uint8_t *)guest_at(0x801f04f2);
    g_DAT_801f04f4 = (uint8_t *)guest_at(0x801f04f4);
    g_DAT_801f04f6 = (uint8_t *)guest_at(0x801f04f6);
    g_DAT_801f04f8 = (uint8_t *)guest_at(0x801f04f8);
    g_DAT_801f04fa = (uint8_t *)guest_at(0x801f04fa);
    g_DAT_801f4d7a = (uint8_t *)guest_at(0x801f4d7a);
    g_DAT_801f4d7c = (uint8_t *)guest_at(0x801f4d7c);
}

/* ---- game.c globals ---- */
uint8_t *_g_system_config_ptr;
uint8_t *g_DAT_1f800000;
uint8_t *g_DAT_1f800001;
uint8_t *g_DAT_1f800002;
uint8_t *g_DAT_1f800003;
uint8_t *g_DAT_1f800004;
uint8_t *g_DAT_1f800008;
uint8_t *g_DAT_1f80000c;
uint8_t *g_DAT_1f800010;
uint8_t *g_DAT_1f800011;
uint8_t *g_DAT_1f800012;
uint8_t *g_DAT_1f800013;
uint8_t *g_DAT_1f80001a;
uint8_t *g_DAT_1f80001c;
uint8_t *g_DAT_1f800020;
uint8_t *g_DAT_1f800022;
uint8_t *g_DAT_1f800024;
uint8_t *g_DAT_1f800070;
uint8_t *g_DAT_1f800078;
uint8_t *g_DAT_1f80007c;
uint8_t *g_DAT_1f800080;
uint8_t *g_DAT_1f800082;
uint8_t *g_DAT_1f800084;
uint8_t *g_DAT_1f800088;
uint8_t *g_DAT_1f80008c;
uint8_t *g_DAT_1f80008e;
uint8_t *g_DAT_1f800092;
uint8_t *g_DAT_1f80009b;
uint8_t *g_DAT_1f80009c;
uint8_t *g_DAT_1f80009e;
uint8_t *g_DAT_1f80009f;
uint8_t *g_DAT_1f8000a2;
uint8_t *g_DAT_1f8000a4;
uint8_t *g_DAT_1f8000a8;
uint8_t *g_DAT_1f8000ac;
uint8_t *g_DAT_1f8000b0;
uint8_t *g_DAT_1f8000b2;
uint8_t *g_DAT_1f8000b8;
uint8_t *g_DAT_1f8000bc;
uint8_t *g_DAT_1f8000cc;
uint8_t *g_DAT_1f8000d0;
uint8_t *g_DAT_1f8000d2;
uint8_t *g_DAT_1f8000d8;
uint8_t *g_DAT_1f8000dc;
uint8_t *g_DAT_1f8000e0;
uint8_t *g_DAT_1f8000e4;
uint8_t *g_DAT_1f80013;
uint8_t *g_DAT_1f80015;
uint8_t *g_DAT_801202e4;
uint8_t *g_DAT_801202e8;
uint8_t *g_DAT_801202ec;
uint8_t *g_DAT_801202f0;
uint8_t *g_DAT_801202f4;
uint8_t *g_DAT_80121e3c;
uint8_t *g_DAT_801b1280;
uint8_t *g_DAT_801b12e0;
uint8_t *g_DAT_801b12e6;
uint8_t *g_DAT_801b3ed8;
uint8_t *g_DAT_801bcca8;
uint8_t *g_DAT_801bccac;
uint8_t *g_DAT_801bccb0;
uint8_t *g_DAT_801bccb4;
uint8_t *g_DAT_801bccb8;
uint8_t *g_DAT_801bdde0;
uint8_t *g_DAT_801bef38;
uint8_t *g_DAT_801bef90;
uint8_t *g_DAT_801befa6;
uint8_t *g_DAT_801befa8;
uint8_t *g_DAT_801befca;
uint8_t *g_DAT_801befec;
uint8_t *g_DAT_801c11d8;
uint8_t *g_DAT_801c1200;
uint8_t *g_DAT_801c1444;
uint8_t *g_DAT_801c1446;
uint8_t *g_DAT_801c1448;
uint8_t *g_DAT_801c144a;
uint8_t *g_DAT_801c144c;
uint8_t *g_DAT_801c1450;
uint8_t *g_DAT_801c15f0;
uint8_t *g_DAT_801c15f4;
uint8_t *g_DAT_801c15f8;
uint8_t *g_DAT_801c43a0;
uint8_t *g_DAT_801c4578;
uint8_t *g_DAT_801c457c;
uint8_t *g_DAT_801c4580;
uint8_t *g_DAT_801c4581;
uint8_t *g_DAT_801c4582;
uint8_t *g_DAT_801c47b8;
uint8_t *g_DAT_801c47c0;
uint8_t *g_DAT_801e3cfc;
uint8_t *g_DAT_801e3d00;
uint8_t *g_DAT_801e40a0;
uint8_t *g_DAT_801e40fc;
uint8_t *g_DAT_801e4104;
uint8_t *g_DAT_801e4106;
uint8_t *g_DAT_801e4108;
uint8_t *g_DAT_801e410a;
uint8_t *g_DAT_801e410c;
uint8_t *g_DAT_801e410e;
uint8_t *g_DAT_801e4110;
uint8_t *g_DAT_801e61f0;
uint8_t *g_DAT_801e624c;
uint8_t *g_DAT_801e6254;
uint8_t *g_DAT_801e6256;
uint8_t *g_DAT_801e6258;
uint8_t *g_DAT_801e625a;
uint8_t *g_DAT_801e625c;
uint8_t *g_DAT_801e625e;
uint8_t *g_DAT_801e8500;
uint8_t *g_DAT_801f0270;
uint8_t *g_DAT_801f0668;
uint8_t *g_DAT_801f066a;
uint8_t *g_DAT_801f4700;
uint8_t *g_DAT_801f4708;
uint8_t *g_DAT_801f4714;
uint8_t *g_DAT_801f4730;
uint8_t *g_DAT_801f4732;
uint8_t *g_DAT_801f473c;
uint8_t *g_DAT_801f4740;
uint8_t *g_DAT_801f4744;
uint8_t *g_DAT_801f4748;
uint8_t *g_DAT_801f474c;
uint8_t *g_DAT_801f4750;
uint8_t *g_DAT_801f4754;
uint8_t *g_DAT_801f4758;
uint32_t *g_PTR_DAT_801b5b94;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
#endif
static void bind_globals_3(void)
{
    _g_system_config_ptr = (uint8_t *)guest_for("_g_system_config_ptr");
    g_DAT_1f800000 = (uint8_t *)guest_for("g_DAT_1f800000");
    g_DAT_1f800001 = (uint8_t *)guest_for("g_DAT_1f800001");
    g_DAT_1f800002 = (uint8_t *)guest_for("g_DAT_1f800002");
    g_DAT_1f800003 = (uint8_t *)guest_for("g_DAT_1f800003");
    g_DAT_1f800004 = (uint8_t *)guest_for("g_DAT_1f800004");
    g_DAT_1f800008 = (uint8_t *)guest_for("g_DAT_1f800008");
    g_DAT_1f80000c = (uint8_t *)guest_for("g_DAT_1f80000c");
    g_DAT_1f800010 = (uint8_t *)guest_for("g_DAT_1f800010");
    g_DAT_1f800011 = (uint8_t *)guest_for("g_DAT_1f800011");
    g_DAT_1f800012 = (uint8_t *)guest_for("g_DAT_1f800012");
    g_DAT_1f800013 = (uint8_t *)guest_for("g_DAT_1f800013");
    g_DAT_1f80001a = (uint8_t *)guest_for("g_DAT_1f80001a");
    g_DAT_1f80001c = (uint8_t *)guest_for("g_DAT_1f80001c");
    g_DAT_1f800020 = (uint8_t *)guest_for("g_DAT_1f800020");
    g_DAT_1f800022 = (uint8_t *)guest_for("g_DAT_1f800022");
    g_DAT_1f800024 = (uint8_t *)guest_for("g_DAT_1f800024");
    g_DAT_1f800070 = (uint8_t *)guest_for("g_DAT_1f800070");
    g_DAT_1f800078 = (uint8_t *)guest_for("g_DAT_1f800078");
    g_DAT_1f80007c = (uint8_t *)guest_for("g_DAT_1f80007c");
    g_DAT_1f800080 = (uint8_t *)guest_for("g_DAT_1f800080");
    g_DAT_1f800082 = (uint8_t *)guest_for("g_DAT_1f800082");
    g_DAT_1f800084 = (uint8_t *)guest_for("g_DAT_1f800084");
    g_DAT_1f800088 = (uint8_t *)guest_for("g_DAT_1f800088");
    g_DAT_1f80008c = (uint8_t *)guest_for("g_DAT_1f80008c");
    g_DAT_1f80008e = (uint8_t *)guest_for("g_DAT_1f80008e");
    g_DAT_1f800092 = (uint8_t *)guest_for("g_DAT_1f800092");
    g_DAT_1f80009b = (uint8_t *)guest_for("g_DAT_1f80009b");
    g_DAT_1f80009c = (uint8_t *)guest_for("g_DAT_1f80009c");
    g_DAT_1f80009e = (uint8_t *)guest_for("g_DAT_1f80009e");
    g_DAT_1f80009f = (uint8_t *)guest_for("g_DAT_1f80009f");
    g_DAT_1f8000a2 = (uint8_t *)guest_for("g_DAT_1f8000a2");
    g_DAT_1f8000a4 = (uint8_t *)guest_for("g_DAT_1f8000a4");
    g_DAT_1f8000a8 = (uint8_t *)guest_for("g_DAT_1f8000a8");
    g_DAT_1f8000ac = (uint8_t *)guest_for("g_DAT_1f8000ac");
    g_DAT_1f8000b0 = (uint8_t *)guest_for("g_DAT_1f8000b0");
    g_DAT_1f8000b2 = (uint8_t *)guest_for("g_DAT_1f8000b2");
    g_DAT_1f8000b8 = (uint8_t *)guest_for("g_DAT_1f8000b8");
    g_DAT_1f8000bc = (uint8_t *)guest_for("g_DAT_1f8000bc");
    g_DAT_1f8000cc = (uint8_t *)guest_for("g_DAT_1f8000cc");
    g_DAT_1f8000d0 = (uint8_t *)guest_for("g_DAT_1f8000d0");
    g_DAT_1f8000d2 = (uint8_t *)guest_for("g_DAT_1f8000d2");
    g_DAT_1f8000d8 = (uint8_t *)guest_for("g_DAT_1f8000d8");
    g_DAT_1f8000dc = (uint8_t *)guest_for("g_DAT_1f8000dc");
    g_DAT_1f8000e0 = (uint8_t *)guest_for("g_DAT_1f8000e0");
    g_DAT_1f8000e4 = (uint8_t *)guest_for("g_DAT_1f8000e4");
    g_DAT_1f80013 = (uint8_t *)guest_for("g_DAT_1f80013");
    g_DAT_1f80015 = (uint8_t *)guest_for("g_DAT_1f80015");
    g_DAT_801202e4 = (uint8_t *)guest_for("g_DAT_801202e4");
    g_DAT_801202e8 = (uint8_t *)guest_for("g_DAT_801202e8");
    g_DAT_801202ec = (uint8_t *)guest_for("g_DAT_801202ec");
    g_DAT_801202f0 = (uint8_t *)guest_for("g_DAT_801202f0");
    g_DAT_801202f4 = (uint8_t *)guest_for("g_DAT_801202f4");
    g_DAT_80121e3c = (uint8_t *)guest_for("g_DAT_80121e3c");
    g_DAT_801b1280 = (uint8_t *)guest_for("g_DAT_801b1280");
    g_DAT_801b12e0 = (uint8_t *)guest_for("g_DAT_801b12e0");
    g_DAT_801b12e6 = (uint8_t *)guest_for("g_DAT_801b12e6");
    g_DAT_801b3ed8 = (uint8_t *)guest_for("g_DAT_801b3ed8");
    g_DAT_801bcca8 = (uint8_t *)guest_for("g_DAT_801bcca8");
    g_DAT_801bccac = (uint8_t *)guest_for("g_DAT_801bccac");
    g_DAT_801bccb0 = (uint8_t *)guest_for("g_DAT_801bccb0");
    g_DAT_801bccb4 = (uint8_t *)guest_for("g_DAT_801bccb4");
    g_DAT_801bccb8 = (uint8_t *)guest_for("g_DAT_801bccb8");
    g_DAT_801bdde0 = (uint8_t *)guest_for("g_DAT_801bdde0");
    g_DAT_801bef38 = (uint8_t *)guest_for("g_DAT_801bef38");
    g_DAT_801bef90 = (uint8_t *)guest_for("g_DAT_801bef90");
    g_DAT_801befa6 = (uint8_t *)guest_for("g_DAT_801befa6");
    g_DAT_801befa8 = (uint8_t *)guest_for("g_DAT_801befa8");
    g_DAT_801befca = (uint8_t *)guest_for("g_DAT_801befca");
    g_DAT_801befec = (uint8_t *)guest_for("g_DAT_801befec");
    g_DAT_801c11d8 = (uint8_t *)guest_for("g_DAT_801c11d8");
    g_DAT_801c1200 = (uint8_t *)guest_for("g_DAT_801c1200");
    g_DAT_801c1444 = (uint8_t *)guest_for("g_DAT_801c1444");
    g_DAT_801c1446 = (uint8_t *)guest_for("g_DAT_801c1446");
    g_DAT_801c1448 = (uint8_t *)guest_for("g_DAT_801c1448");
    g_DAT_801c144a = (uint8_t *)guest_for("g_DAT_801c144a");
    g_DAT_801c144c = (uint8_t *)guest_for("g_DAT_801c144c");
    g_DAT_801c1450 = (uint8_t *)guest_for("g_DAT_801c1450");
    g_DAT_801c15f0 = (uint8_t *)guest_for("g_DAT_801c15f0");
    g_DAT_801c15f4 = (uint8_t *)guest_for("g_DAT_801c15f4");
    g_DAT_801c15f8 = (uint8_t *)guest_for("g_DAT_801c15f8");
    g_DAT_801c43a0 = (uint8_t *)guest_for("g_DAT_801c43a0");
    g_DAT_801c4578 = (uint8_t *)guest_for("g_DAT_801c4578");
    g_DAT_801c457c = (uint8_t *)guest_for("g_DAT_801c457c");
    g_DAT_801c4580 = (uint8_t *)guest_for("g_DAT_801c4580");
    g_DAT_801c4581 = (uint8_t *)guest_for("g_DAT_801c4581");
    g_DAT_801c4582 = (uint8_t *)guest_for("g_DAT_801c4582");
    g_DAT_801c47b8 = (uint8_t *)guest_for("g_DAT_801c47b8");
    g_DAT_801c47c0 = (uint8_t *)guest_for("g_DAT_801c47c0");
    g_DAT_801e3cfc = (uint8_t *)guest_for("g_DAT_801e3cfc");
    g_DAT_801e3d00 = (uint8_t *)guest_for("g_DAT_801e3d00");
    g_DAT_801e40a0 = (uint8_t *)guest_for("g_DAT_801e40a0");
    g_DAT_801e40fc = (uint8_t *)guest_for("g_DAT_801e40fc");
    g_DAT_801e4104 = (uint8_t *)guest_for("g_DAT_801e4104");
    g_DAT_801e4106 = (uint8_t *)guest_for("g_DAT_801e4106");
    g_DAT_801e4108 = (uint8_t *)guest_for("g_DAT_801e4108");
    g_DAT_801e410a = (uint8_t *)guest_for("g_DAT_801e410a");
    g_DAT_801e410c = (uint8_t *)guest_for("g_DAT_801e410c");
    g_DAT_801e410e = (uint8_t *)guest_for("g_DAT_801e410e");
    g_DAT_801e4110 = (uint8_t *)guest_for("g_DAT_801e4110");
    g_DAT_801e61f0 = (uint8_t *)guest_for("g_DAT_801e61f0");
    g_DAT_801e624c = (uint8_t *)guest_for("g_DAT_801e624c");
    g_DAT_801e6254 = (uint8_t *)guest_for("g_DAT_801e6254");
    g_DAT_801e6256 = (uint8_t *)guest_for("g_DAT_801e6256");
    g_DAT_801e6258 = (uint8_t *)guest_for("g_DAT_801e6258");
    g_DAT_801e625a = (uint8_t *)guest_for("g_DAT_801e625a");
    g_DAT_801e625c = (uint8_t *)guest_for("g_DAT_801e625c");
    g_DAT_801e625e = (uint8_t *)guest_for("g_DAT_801e625e");
    g_DAT_801e8500 = (uint8_t *)guest_for("g_DAT_801e8500");
    g_DAT_801f0270 = (uint8_t *)guest_for("g_DAT_801f0270");
    g_DAT_801f0668 = (uint8_t *)guest_for("g_DAT_801f0668");
    g_DAT_801f066a = (uint8_t *)guest_for("g_DAT_801f066a");
    g_DAT_801f4700 = (uint8_t *)guest_for("g_DAT_801f4700");
    g_DAT_801f4708 = (uint8_t *)guest_for("g_DAT_801f4708");
    g_DAT_801f4714 = (uint8_t *)guest_for("g_DAT_801f4714");
    g_DAT_801f4730 = (uint8_t *)guest_for("g_DAT_801f4730");
    g_DAT_801f4732 = (uint8_t *)guest_for("g_DAT_801f4732");
    g_DAT_801f473c = (uint8_t *)guest_for("g_DAT_801f473c");
    g_DAT_801f4740 = (uint8_t *)guest_for("g_DAT_801f4740");
    g_DAT_801f4744 = (uint8_t *)guest_for("g_DAT_801f4744");
    g_DAT_801f4748 = (uint8_t *)guest_for("g_DAT_801f4748");
    g_DAT_801f474c = (uint8_t *)guest_for("g_DAT_801f474c");
    g_DAT_801f4750 = (uint8_t *)guest_for("g_DAT_801f4750");
    g_DAT_801f4754 = (uint8_t *)guest_for("g_DAT_801f4754");
    g_DAT_801f4758 = (uint8_t *)guest_for("g_DAT_801f4758");
    g_PTR_DAT_801b5b94 = (uint32_t *)guest_for("g_PTR_DAT_801b5b94");
}

/* _start guest symbols (entry_0x80126178) — standalone u32s;
 * in the runtime these alias guest RAM 0x801beef0/0x801bxxxx. */
uint32_t uRam801beef0, iRam80126228, iRam801be720;
void halt_baddata(void) { for(;;) { } }

/* ---- UNPORTED weak stubs (game.c callees) ----
 * Guest functions not yet ported. Weak defs keep the tree
 * linkable; strong ports override. Return values are fake. */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_80194478(void) { gte_screen_init_defer(); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_80195290(int32_t ofs_x,int32_t ofs_y) { gte_ofs_set(ofs_x,ofs_y); }

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif

uint8_t *g_DAT_801e40b7, *g_DAT_801e40b8;
uint8_t *g_DAT_801e6206, *g_DAT_801e6207, *g_DAT_801e6208;
uint8_t *g_DAT_801e6209, *g_DAT_801e620a, *g_DAT_801e620b;
uint8_t *g_DAT_801e6254, *g_DAT_801e6256, *g_DAT_801e6258;
uint8_t *g_DAT_801e625a, *g_DAT_801e625c, *g_DAT_801e625e;
uint8_t *g_DAT_801e4104, *g_DAT_801e4106, *g_DAT_801e4108;
uint8_t *g_DAT_801e410a, *g_DAT_801e410c, *g_DAT_801e410e, *g_DAT_801e4110;
uint8_t *g_DAT_801e40a0, *g_DAT_801e40b6, *g_DAT_801e40b9, *g_DAT_801e40ba, *g_DAT_801e40bb;
uint8_t *g_DAT_801e40fc, *g_DAT_801e61f0, *g_DAT_801e624c;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
#endif
static void bind_globals_4(void)
{
    const char *names[] = {
        "g_DAT_801e40b7","g_DAT_801e40b8","g_DAT_801e6206","g_DAT_801e6207","g_DAT_801e6208",
        "g_DAT_801e6209","g_DAT_801e620a","g_DAT_801e620b","g_DAT_801e6254","g_DAT_801e6256",
        "g_DAT_801e6258","g_DAT_801e625a","g_DAT_801e625c","g_DAT_801e625e","g_DAT_801e4104",
        "g_DAT_801e4106","g_DAT_801e4108","g_DAT_801e410a","g_DAT_801e410c","g_DAT_801e410e",
        "g_DAT_801e4110","g_DAT_801e40a0","g_DAT_801e40b6","g_DAT_801e40b9","g_DAT_801e40ba",
        "g_DAT_801e40bb","g_DAT_801e40fc","g_DAT_801e61f0","g_DAT_801e624c"
    };
    uint8_t **vars[] = {
        &g_DAT_801e40b7,&g_DAT_801e40b8,&g_DAT_801e6206,&g_DAT_801e6207,&g_DAT_801e6208,
        &g_DAT_801e6209,&g_DAT_801e620a,&g_DAT_801e620b,&g_DAT_801e6254,&g_DAT_801e6256,
        &g_DAT_801e6258,&g_DAT_801e625a,&g_DAT_801e625c,&g_DAT_801e625e,&g_DAT_801e4104,
        &g_DAT_801e4106,&g_DAT_801e4108,&g_DAT_801e410a,&g_DAT_801e410c,&g_DAT_801e410e,
        &g_DAT_801e4110,&g_DAT_801e40a0,&g_DAT_801e40b6,&g_DAT_801e40b9,&g_DAT_801e40ba,
        &g_DAT_801e40bb,&g_DAT_801e40fc,&g_DAT_801e61f0,&g_DAT_801e624c
    };
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        *vars[i] = guest_for(names[i]);
}

/* load_payload — optional: back the arena with the guest EXE image
 * so bound DAT_/string/table globals carry real disc bytes in
 * standalone builds. psx_payload.bin is the user's own image; if it is
 * absent the arena simply stays zeroed (previous behaviour). The EXE
 * loads at guest 0x8011e800 size 0xa0800 -> arena offset 0x11e800. */
__attribute__((constructor))
static void load_payload(void)
{
    FILE *f = fopen("psx_payload.bin","rb");
    if (!f) f = fopen("../psx_payload.bin","rb");
    if (!f) f = fopen("bloody-roar-2/psx_payload.bin","rb");
    if (!f) return;
    if (fseek(f, 0x800, SEEK_SET) != 0) { fclose(f); return; }
    (void)fread(guest_arena + 0x11e800, 1, 0xa0800, f);
    fclose(f);
}

/* ---- save.c globals ---- */
uint8_t *g_DAT_1f800014;
uint8_t *g_DAT_1f800017;
uint8_t *g_DAT_1f800018;
uint8_t *g_DAT_1f800019;
uint8_t *g_DAT_8012181c;
uint8_t *g_DAT_80121820;
uint8_t *g_DAT_801b3f78;
uint8_t *g_DAT_801b45f4;
uint8_t *g_DAT_801b45f8;
uint8_t *g_DAT_801b45fc;
uint8_t *g_DAT_801b4600;
uint8_t *g_DAT_801b4604;
uint8_t *g_DAT_801b4608;
uint8_t *g_DAT_801b460c;
uint8_t *g_DAT_801b4610;
uint8_t *g_DAT_801b4614;
uint8_t *g_DAT_801bef60;
uint8_t *g_DAT_801bef64;
uint8_t *g_DAT_801bef67;
uint8_t *g_DAT_801bef68;
uint8_t *g_DAT_801bef69;
uint8_t *g_DAT_801bef6c;
uint8_t *g_DAT_801bef6d;
uint8_t *g_DAT_801bef6e;
uint8_t *g_DAT_801bef6f;
uint8_t *g_DAT_801bef72;
uint8_t *g_DAT_801bef74;
uint8_t *g_DAT_801bef76;
uint8_t *g_DAT_801bef78;
uint8_t *g_DAT_801bef80;
uint8_t *g_DAT_801bef84;
uint8_t *g_DAT_801bef88;
uint8_t *g_DAT_801bef8c;
uint8_t *g_DAT_801c11e0;
uint8_t *g_DAT_801c11e4;
uint8_t *g_DAT_801c11e8;
uint8_t *g_DAT_801c11ec;
uint8_t *g_DAT_801c11f4;
uint8_t *g_DAT_801c1264;
uint8_t *g_DAT_801c1304;
uint8_t *g_DAT_801c13a4;
uint8_t *g_DAT_801e8400;
uint8_t *g_DAT_801f09b;
uint8_t *g_DAT_801f24d0;
uint8_t *g_DAT_801f24d1;
uint8_t *g_DAT_801f3602;
uint8_t *g_DAT_801f5968;
uint8_t *g_DAT_801f5969;
uint8_t *g_DAT_801f596e;
uint8_t *g_DAT_801f596f;
uint8_t *g_DAT_801f5972;
uint8_t *g_DAT_801f5973;
uint8_t *g_PTR_DAT_801b4618;
uint8_t *g__DAT_1f80007c;
uint8_t *g__DAT_1f800084;
uint8_t *g__DAT_1f80008c;
uint8_t *g__DAT_1f8000b8;
uint8_t *g__DAT_801c15f0;
uint8_t *g__DAT_801e8508;
uint8_t *g__DAT_801e8518;
uint8_t *g__DAT_801f2650;
uint8_t *g__DAT_801f2654;
uint8_t *g__DAT_801f2658;
uint8_t *g__DAT_801f265a;
uint8_t *g__DAT_801f2666;
uint8_t *g__DAT_801f2668;
uint8_t *g__DAT_801f266c;
uint8_t *g__DAT_801f268a;
uint8_t *g__DAT_801f268c;
uint8_t *g__DAT_801f2690;
uint8_t *g__DAT_801f2694;
uint8_t *g__DAT_801f2696;
uint8_t *g__DAT_801f26a0;
uint8_t *g__DAT_801f26a2;
uint8_t *g__DAT_801f26a8;
uint8_t *g__DAT_801f36c0;
uint8_t *g_DAT_801b43f0;
uint8_t *g_DAT_801f2650;
char *s_1_BLOCK_IS_REQUIRED_TO_SAVE__80121824;
char *s_BLOCKS_AND_1_BLOCK_IS_REQUIRED_801219b8;
char *s_CHECKING_MEMORY_CARD__s__80121938;
char *s_COMPLETED___80121954;
char *s_DATA_ERROR____80121a24;
char *s_DO_NOT_REMOVE_MEMORY_CARD___801218d0;
char *s_ERROR___80121a34;
char *s_FORMAT_MEMORY_CARD__s__80121920;
char *s_FORMAT_MEMORY_CARD__s___801217d8;
char *s_LOAD_FROM_MEMORY_CARD__s__801218ec;
char *s_LOAD_FROM_MEMORY_CARD__s___80121788;
char *s_MEMORY_CARD_CHANGED__801217a4;
char *s_MEMORY_CARD__s___80121980;
char *s_NOW_CHECKING_MEMORY_CARD__s____801218b0;
char *s_NOW_FORMATTING_MEMORY_CARD__s____8012188c;
char *s_NOW_LOADING_FROM_MEMORY_CARD_80121844;
char *s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
char *s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
char *s_NO_MEMORY_CARD_IS_INSERTED_IN_80121960;
char *s_OVER_WRITE_OK___8012180c;
char *s_SAVE_TO_MEMORY_CARD__s__80121908;
char *s_SAVE_TO_MEMORY_CARD__s___801217bc;
char *s_SAVING_TO_MEMORY_CARD__s__801217f0;
char *s_THERE_IS_INSUFFICIENT_MEMORY_CAR_80121994;
char *s_TO_SAVE__801219d8;
char *s_UNFORMATTED_MEMORY_CARD___80121a08;
char *s_VSync__timeout_80125b38;
char *s__s____80121864;
uint32_t *g_PTR_u32_801b4160;

__attribute__((constructor))
static void bind_globals_5(void)
{
    const char *names[] = {"g_DAT_1f800014","g_DAT_1f800017","g_DAT_1f800018","g_DAT_1f800019","g_DAT_8012181c","g_DAT_80121820","g_DAT_801b3f78","g_DAT_801b45f4","g_DAT_801b45f8","g_DAT_801b45fc","g_DAT_801b4600","g_DAT_801b4604","g_DAT_801b4608","g_DAT_801b460c","g_DAT_801b4610","g_DAT_801b4614","g_DAT_801bef60","g_DAT_801bef64","g_DAT_801bef67","g_DAT_801bef68","g_DAT_801bef69","g_DAT_801bef6c","g_DAT_801bef6d","g_DAT_801bef6e","g_DAT_801bef6f","g_DAT_801bef72","g_DAT_801bef74","g_DAT_801bef76","g_DAT_801bef78","g_DAT_801bef80","g_DAT_801bef84","g_DAT_801bef88","g_DAT_801bef8c","g_DAT_801c11e0","g_DAT_801c11e4","g_DAT_801c11e8","g_DAT_801c11ec","g_DAT_801c11f4","g_DAT_801c1264","g_DAT_801c1304","g_DAT_801c13a4","g_DAT_801e8400","g_DAT_801f09b","g_DAT_801f24d0","g_DAT_801f24d1","g_DAT_801f35f4","g_DAT_801f3602","g_DAT_801f5968","g_DAT_801f5969","g_DAT_801f596e","g_DAT_801f596f","g_DAT_801f5972","g_DAT_801f5973","g_PTR_DAT_801b4618","g__DAT_1f80007c","g__DAT_1f800084","g__DAT_1f80008c","g__DAT_1f8000b8","g__DAT_801c15f0","g__DAT_801e8508","g__DAT_801e8518","g__DAT_801f2650","g__DAT_801f2654","g__DAT_801f2658","g__DAT_801f265a","g__DAT_801f2666","g__DAT_801f2668","g__DAT_801f266c","g__DAT_801f268a","g__DAT_801f268c","g__DAT_801f2690","g__DAT_801f2694","g__DAT_801f2696","g__DAT_801f26a0","g__DAT_801f26a2","g__DAT_801f26a8","g__DAT_801f36c0","g_DAT_801b43f0","g_DAT_801f2650","g_DAT_801f35f4"};
    uint8_t **vars[] = {&g_DAT_1f800014,&g_DAT_1f800017,&g_DAT_1f800018,&g_DAT_1f800019,&g_DAT_8012181c,&g_DAT_80121820,&g_DAT_801b3f78,&g_DAT_801b45f4,&g_DAT_801b45f8,&g_DAT_801b45fc,&g_DAT_801b4600,&g_DAT_801b4604,&g_DAT_801b4608,&g_DAT_801b460c,&g_DAT_801b4610,&g_DAT_801b4614,&g_DAT_801bef60,&g_DAT_801bef64,&g_DAT_801bef67,&g_DAT_801bef68,&g_DAT_801bef69,&g_DAT_801bef6c,&g_DAT_801bef6d,&g_DAT_801bef6e,&g_DAT_801bef6f,&g_DAT_801bef72,&g_DAT_801bef74,&g_DAT_801bef76,&g_DAT_801bef78,&g_DAT_801bef80,&g_DAT_801bef84,&g_DAT_801bef88,&g_DAT_801bef8c,&g_DAT_801c11e0,&g_DAT_801c11e4,&g_DAT_801c11e8,&g_DAT_801c11ec,&g_DAT_801c11f4,&g_DAT_801c1264,&g_DAT_801c1304,&g_DAT_801c13a4,&g_DAT_801e8400,&g_DAT_801f09b,&g_DAT_801f24d0,&g_DAT_801f24d1,&g_DAT_801f35f4,&g_DAT_801f3602,&g_DAT_801f5968,&g_DAT_801f5969,&g_DAT_801f596e,&g_DAT_801f596f,&g_DAT_801f5972,&g_DAT_801f5973,&g_PTR_DAT_801b4618,&g__DAT_1f80007c,&g__DAT_1f800084,&g__DAT_1f80008c,&g__DAT_1f8000b8,&g__DAT_801c15f0,&g__DAT_801e8508,&g__DAT_801e8518,&g__DAT_801f2650,&g__DAT_801f2654,&g__DAT_801f2658,&g__DAT_801f265a,&g__DAT_801f2666,&g__DAT_801f2668,&g__DAT_801f266c,&g__DAT_801f268a,&g__DAT_801f268c,&g__DAT_801f2690,&g__DAT_801f2694,&g__DAT_801f2696,&g__DAT_801f26a0,&g__DAT_801f26a2,&g__DAT_801f26a8,&g__DAT_801f36c0,&g_DAT_801b43f0,&g_DAT_801f2650,&g_DAT_801f35f4};
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        *vars[i] = guest_for(names[i]);
    s_1_BLOCK_IS_REQUIRED_TO_SAVE__80121824 = (char *)guest_at(0x80121824);
    s_BLOCKS_AND_1_BLOCK_IS_REQUIRED_801219b8 = (char *)guest_at(0x801219b8);
    s_CHECKING_MEMORY_CARD__s__80121938 = (char *)guest_at(0x80121938);
    s_COMPLETED___80121954 = (char *)guest_at(0x80121954);
    s_DATA_ERROR____80121a24 = (char *)guest_at(0x80121a24);
    s_DO_NOT_REMOVE_MEMORY_CARD___801218d0 = (char *)guest_at(0x801218d0);
    s_ERROR___80121a34 = (char *)guest_at(0x80121a34);
    s_FORMAT_MEMORY_CARD__s__80121920 = (char *)guest_at(0x80121920);
    s_FORMAT_MEMORY_CARD__s___801217d8 = (char *)guest_at(0x801217d8);
    s_LOAD_FROM_MEMORY_CARD__s__801218ec = (char *)guest_at(0x801218ec);
    s_LOAD_FROM_MEMORY_CARD__s___80121788 = (char *)guest_at(0x80121788);
    s_MEMORY_CARD_CHANGED__801217a4 = (char *)guest_at(0x801217a4);
    s_MEMORY_CARD__s___80121980 = (char *)guest_at(0x80121980);
    s_NOW_CHECKING_MEMORY_CARD__s____801218b0 = (char *)guest_at(0x801218b0);
    s_NOW_FORMATTING_MEMORY_CARD__s____8012188c = (char *)guest_at(0x8012188c);
    s_NOW_LOADING_FROM_MEMORY_CARD_80121844 = (char *)guest_at(0x80121844);
    s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c = (char *)guest_at(0x8012186c);
    s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4 = (char *)guest_at(0x801219e4);
    s_NO_MEMORY_CARD_IS_INSERTED_IN_80121960 = (char *)guest_at(0x80121960);
    s_OVER_WRITE_OK___8012180c = (char *)guest_at(0x8012180c);
    s_SAVE_TO_MEMORY_CARD__s__80121908 = (char *)guest_at(0x80121908);
    s_SAVE_TO_MEMORY_CARD__s___801217bc = (char *)guest_at(0x801217bc);
    s_SAVING_TO_MEMORY_CARD__s__801217f0 = (char *)guest_at(0x801217f0);
    s_THERE_IS_INSUFFICIENT_MEMORY_CAR_80121994 = (char *)guest_at(0x80121994);
    s_TO_SAVE__801219d8 = (char *)guest_at(0x801219d8);
    s_UNFORMATTED_MEMORY_CARD___80121a08 = (char *)guest_at(0x80121a08);
    s_VSync__timeout_80125b38 = (char *)guest_at(0x80125b38);
    s__s____80121864 = (char *)guest_at(0x80121864);
    g_PTR_u32_801b4160 = (uint32_t *)guest_at(0x801b4160);
}

/* ---- save.c callees ---- */
uint8_t *g_DAT_1f800015;
uint8_t *g_DAT_1f800068;
uint8_t *g_DAT_1f800074;
uint8_t *g_DAT_1f800098;
uint8_t *g_DAT_1f800099;
uint8_t *g_DAT_1f80009a;
uint8_t *g_DAT_1f80009d;
uint8_t *g_DAT_1f8000b1;
uint8_t *g_DAT_1f8000e8;
uint8_t *g_DAT_1f8000e9;
uint8_t *g_DAT_1f8000eb;
uint8_t *g_DAT_1f8000ec;
uint8_t *g_DAT_1f8000ee;
uint8_t *g_DAT_1f8001f0;
uint8_t *g_DAT_1f8001f1;
uint8_t *g_DAT_1f8001f3;
uint8_t *g_DAT_1f8001f4;
uint8_t *g_DAT_1f8001f6;
uint8_t *g_DAT_80121f30;
uint8_t *g_DAT_80121fe0;
uint8_t *g_DAT_80122074;
uint8_t *g_DAT_80122075;
uint8_t *g_DAT_80122076;
uint8_t *g_DAT_80122078;
uint8_t *g_DAT_80122079;
uint8_t *g_DAT_8012207c;
uint8_t *g_DAT_8012207d;
uint8_t *g_DAT_801b2253;
uint8_t *g_DAT_801b2350;
uint8_t *g_DAT_801b33dc;
uint8_t *g_DAT_801b3400;
uint8_t *g_DAT_801b346c;
uint8_t *g_DAT_801b35a0;
uint8_t *g_DAT_801b35b8;
uint8_t *g_DAT_801b35cc;
uint8_t *g_DAT_801b3758;
uint8_t *g_DAT_801b377c;
uint8_t *g_DAT_801b37dc;
uint8_t *g_DAT_801b37f4;
uint8_t *g_DAT_801b3814;
uint8_t *g_DAT_801b3834;
uint8_t *g_DAT_801b3854;
uint8_t *g_DAT_801b3868;
uint8_t *g_DAT_801b388c;
uint8_t *g_DAT_801b38ac;
uint8_t *g_DAT_801b38d0;
uint8_t *g_DAT_801b3968;
uint8_t *g_DAT_801b3a00;
uint8_t *g_DAT_801b3a84;
uint8_t *g_DAT_801b3b08;
uint8_t *g_DAT_801b3b94;
uint8_t *g_DAT_801b3c24;
uint8_t *g_DAT_801b3ca8;
uint8_t *g_DAT_801b3d34;
uint8_t *g_DAT_801b3db4;
uint8_t *g_DAT_801b3e44;
uint8_t *g_DAT_801b3eb0;
uint8_t *g_DAT_801b3ebc;
uint8_t *g_DAT_801b3f64;
uint8_t *g_DAT_801f24d8;
uint8_t *g_PTR_DAT_801b4254;
uint8_t *g__DAT_1f800004;
uint8_t *g__DAT_1f800008;
uint8_t *g__DAT_1f80000c;
uint8_t *g__DAT_1f800020;
uint8_t *g__DAT_1f800022;
uint8_t *g__DAT_1f800080;
uint8_t *g__DAT_1f800082;
uint8_t *g__DAT_1f800088;
uint8_t *g__DAT_1f80008e;
uint8_t *g__DAT_1f800090;
uint8_t *g__DAT_1f8000a4;
uint8_t *g__DAT_1f8000b4;
uint8_t *g__DAT_1f8000b6;
uint8_t *g__DAT_1f8000bc;
uint8_t *g__DAT_1f8000c4;
uint8_t *g__DAT_1f8000dc;
uint8_t *g__DAT_1f800100;
uint8_t *g__DAT_1f800104;
uint8_t *g__DAT_1f800110;
uint8_t *g__DAT_1f800148;
uint8_t *g__DAT_1f800154;
uint8_t *g__DAT_1f800208;
uint8_t *g__DAT_1f80020c;
uint8_t *g__DAT_1f800218;
uint8_t *g__DAT_1f800250;
uint8_t *g__DAT_1f80025c;
uint8_t *g__DAT_801c43a0;
uint8_t *g__DAT_801c4578;
uint8_t *g__DAT_801c47b8;
uint8_t *g__DAT_801e3cfc;
uint8_t *g__DAT_801e850c;
uint8_t *g__DAT_801e851c;
uint8_t *g__DAT_801f265c;
uint8_t *g__DAT_801f265e;
uint8_t *g__DAT_801f2664;
uint8_t *g__DAT_801f2674;
uint8_t *g__DAT_801f2678;
uint8_t *g__DAT_801f267c;
uint8_t *g__DAT_801f36c2;
uint8_t *g__DAT_801f4714;
uint8_t *g__DAT_801f4728;
uint8_t *g__DAT_801f472c;
uint8_t *g__DAT_801f4730;
uint8_t *g__DAT_801f4732;
uint8_t *g__DAT_801f4734;
uint8_t *g__DAT_801f4738;
uint8_t *g__DAT_801f473c;
uint8_t *g__DAT_801f4744;
uint8_t *g__DAT_801f4748;
uint8_t *g__DAT_801f474c;
uint8_t *g__DAT_801f4750;
uint8_t *g__DAT_801f4754;
uint8_t *g__DAT_801f4cb2;
uint8_t *g__DAT_801f4cb6;
uint8_t *g__DAT_801fc68c;


__attribute__((constructor))
static void bind_globals_6(void)
{
    const char *names[] = {"g_DAT_1f800015","g_DAT_1f800068","g_DAT_1f800074","g_DAT_1f800098","g_DAT_1f800099","g_DAT_1f80009a","g_DAT_1f80009d","g_DAT_1f8000b1","g_DAT_1f8000e8","g_DAT_1f8000e9","g_DAT_1f8000eb","g_DAT_1f8000ec","g_DAT_1f8000ee","g_DAT_1f8001f0","g_DAT_1f8001f1","g_DAT_1f8001f3","g_DAT_1f8001f4","g_DAT_1f8001f6","g_DAT_80121f30","g_DAT_80121fe0","g_DAT_80122074","g_DAT_80122075","g_DAT_80122076","g_DAT_80122078","g_DAT_80122079","g_DAT_8012207c","g_DAT_8012207d","g_DAT_801b2253","g_DAT_801b2350","g_DAT_801b33dc","g_DAT_801b3400","g_DAT_801b346c","g_DAT_801b35a0","g_DAT_801b35b8","g_DAT_801b35cc","g_DAT_801b3758","g_DAT_801b377c","g_DAT_801b37dc","g_DAT_801b37f4","g_DAT_801b3814","g_DAT_801b3834","g_DAT_801b3854","g_DAT_801b3868","g_DAT_801b388c","g_DAT_801b38ac","g_DAT_801b38d0","g_DAT_801b3968","g_DAT_801b3a00","g_DAT_801b3a84","g_DAT_801b3b08","g_DAT_801b3b94","g_DAT_801b3c24","g_DAT_801b3ca8","g_DAT_801b3d34","g_DAT_801b3db4","g_DAT_801b3e44","g_DAT_801b3eb0","g_DAT_801b3ebc","g_DAT_801b3f64","g_DAT_801f24d8","g_PTR_DAT_801b4254","g__DAT_1f800004","g__DAT_1f800008","g__DAT_1f80000c","g__DAT_1f800020","g__DAT_1f800022","g__DAT_1f800080","g__DAT_1f800082","g__DAT_1f800088","g__DAT_1f80008e","g__DAT_1f800090","g__DAT_1f8000a4","g__DAT_1f8000b4","g__DAT_1f8000b6","g__DAT_1f8000bc","g__DAT_1f8000c4","g__DAT_1f8000dc","g__DAT_1f800100","g__DAT_1f800104","g__DAT_1f800110","g__DAT_1f800148","g__DAT_1f800154","g__DAT_1f800208","g__DAT_1f80020c","g__DAT_1f800218","g__DAT_1f800250","g__DAT_1f80025c","g__DAT_801c43a0","g__DAT_801c4578","g__DAT_801c47b8","g__DAT_801e3cfc","g__DAT_801e850c","g__DAT_801e851c","g__DAT_801f265c","g__DAT_801f265e","g__DAT_801f2664","g__DAT_801f2674","g__DAT_801f2678","g__DAT_801f267c","g__DAT_801f36c2","g__DAT_801f4714","g__DAT_801f4728","g__DAT_801f472c","g__DAT_801f4730","g__DAT_801f4732","g__DAT_801f4734","g__DAT_801f4738","g__DAT_801f473c","g__DAT_801f4744","g__DAT_801f4748","g__DAT_801f474c","g__DAT_801f4750","g__DAT_801f4754","g__DAT_801f4cb2","g__DAT_801f4cb6","g__DAT_801fc68c"};
    uint8_t **vars[] = {&g_DAT_1f800015,&g_DAT_1f800068,&g_DAT_1f800074,&g_DAT_1f800098,&g_DAT_1f800099,&g_DAT_1f80009a,&g_DAT_1f80009d,&g_DAT_1f8000b1,&g_DAT_1f8000e8,&g_DAT_1f8000e9,&g_DAT_1f8000eb,&g_DAT_1f8000ec,&g_DAT_1f8000ee,&g_DAT_1f8001f0,&g_DAT_1f8001f1,&g_DAT_1f8001f3,&g_DAT_1f8001f4,&g_DAT_1f8001f6,&g_DAT_80121f30,&g_DAT_80121fe0,&g_DAT_80122074,&g_DAT_80122075,&g_DAT_80122076,&g_DAT_80122078,&g_DAT_80122079,&g_DAT_8012207c,&g_DAT_8012207d,&g_DAT_801b2253,&g_DAT_801b2350,&g_DAT_801b33dc,&g_DAT_801b3400,&g_DAT_801b346c,&g_DAT_801b35a0,&g_DAT_801b35b8,&g_DAT_801b35cc,&g_DAT_801b3758,&g_DAT_801b377c,&g_DAT_801b37dc,&g_DAT_801b37f4,&g_DAT_801b3814,&g_DAT_801b3834,&g_DAT_801b3854,&g_DAT_801b3868,&g_DAT_801b388c,&g_DAT_801b38ac,&g_DAT_801b38d0,&g_DAT_801b3968,&g_DAT_801b3a00,&g_DAT_801b3a84,&g_DAT_801b3b08,&g_DAT_801b3b94,&g_DAT_801b3c24,&g_DAT_801b3ca8,&g_DAT_801b3d34,&g_DAT_801b3db4,&g_DAT_801b3e44,&g_DAT_801b3eb0,&g_DAT_801b3ebc,&g_DAT_801b3f64,&g_DAT_801f24d8,&g_PTR_DAT_801b4254,&g__DAT_1f800004,&g__DAT_1f800008,&g__DAT_1f80000c,&g__DAT_1f800020,&g__DAT_1f800022,&g__DAT_1f800080,&g__DAT_1f800082,&g__DAT_1f800088,&g__DAT_1f80008e,&g__DAT_1f800090,&g__DAT_1f8000a4,&g__DAT_1f8000b4,&g__DAT_1f8000b6,&g__DAT_1f8000bc,&g__DAT_1f8000c4,&g__DAT_1f8000dc,&g__DAT_1f800100,&g__DAT_1f800104,&g__DAT_1f800110,&g__DAT_1f800148,&g__DAT_1f800154,&g__DAT_1f800208,&g__DAT_1f80020c,&g__DAT_1f800218,&g__DAT_1f800250,&g__DAT_1f80025c,&g__DAT_801c43a0,&g__DAT_801c4578,&g__DAT_801c47b8,&g__DAT_801e3cfc,&g__DAT_801e850c,&g__DAT_801e851c,&g__DAT_801f265c,&g__DAT_801f265e,&g__DAT_801f2664,&g__DAT_801f2674,&g__DAT_801f2678,&g__DAT_801f267c,&g__DAT_801f36c2,&g__DAT_801f4714,&g__DAT_801f4728,&g__DAT_801f472c,&g__DAT_801f4730,&g__DAT_801f4732,&g__DAT_801f4734,&g__DAT_801f4738,&g__DAT_801f473c,&g__DAT_801f4744,&g__DAT_801f4748,&g__DAT_801f474c,&g__DAT_801f4750,&g__DAT_801f4754,&g__DAT_801f4cb2,&g__DAT_801f4cb6,&g__DAT_801fc68c};
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        *vars[i] = guest_for(names[i]);
}

/* ---- menu.c globals ---- */
int g_game_state;
int g_sub_state;
char *s_NAME_ENTRY_80122080;
char *s_RANKING_80122048;
char *s_SURVIVAL_8012205c;
char *s_TIME_ATTACK_80122050;
char *s_VS_RANKING_80122068;
char *s_time_out_in_decoding___80120e90;

/* ---- faithful `jal` from clean C ------------------------------------
 * Args are already live in cpu->gpr[4..7] (caller writes them explicitly);
 * caller-saved regs are left holding callee leftovers exactly like the
 * guest, $ra is set to the guest link address so BIOS/kernel services
 * inside the callee see the same caller token the generated CPS frame
 * would present, and $v0 comes back in gpr[2]. Mirrors the nested-dispatch
 * contract of call_guest/call_guest4 (clean targets via clean_try_dispatch,
 * everything else via psx_dispatch_call) minus the arg-reg restore and the
 * jalr-style $t0 target write — a guest `jal` does neither. */
extern int clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                              uint32_t return_addr);
extern int br2_bail_pending(void);
static uint32_t bind_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
}

/* menu.c callees — weak fallbacks that delegate to the generated
 * guest bodies. Args 5+ travel on the guest stack (o32), staged via
 * br2_guest_frame; pointer args arrive as host pointers and are
 * converted with guest_of. */
int FUN_801468ec(uint32_t a0,uint32_t a1,uint32_t mat,const void *ctrl,int32_t mode)
{
    /* guest 0x801468EC — fighter emit state machine. Loads the 8-word
     * matrix at mat into GTE CT0..CT7, normalizes the mode index mode
     * through the 6-entry jr table at 0x801202CC ({0,4,2,6,1,7}), then
     * runs three bit-selected emit sections (bit0/bit1/bit2 of the
     * normalized mode) that walk 4-byte record lists hanging off the
     * FighterCtrl at ctrl and jal the 8-byte jr-$ra padding stubs
     * 0x80136F70/0x80136F80 (interrupt-check waypoints; the args are
     * dead but reproduced faithfully). Faithful notes:
     *  - guest frame is exactly 0x60; incoming arg5 (mode) is already
     *    marshalled at old_sp+0x10 by the dispatch wrapper; outgoing
     *    arg5/arg6 land at fr+0x10/0x14 and the 3-word vector at
     *    fr+0x18 is passed by guest pointer — all via br2_guest_frame.
     *  - the prologue spills s0-s7/fp/ra + the a0/a1 home slots into
     *    the frame so the stack image matches generated.
     *  - the ctc2 sequence mirrors generated: direct gte_ctrl[] stores
     *    for CT0-3 and CT5-7, gte_write_ctrl() for CT4 (16-bit reg). */
    CPUState *c = clean_cpu_state();
    uint32_t fr, s4, P, L2, T, fp, s0;
    int32_t s6, s2, s3, s5v, s7, ret = 0;
    if (!c) {
        uint32_t fr0 = br2_guest_frame(0x20);
        int r;
        br2_st32(fr0 + 0x10, (uint32_t)mode);
        r = (int)br2_call_clean(0x801468ecu,a0,a1,mat,guest_of(ctrl));
        br2_guest_frame_free();
        return r;
    }
    s4 = guest_of(ctrl);
    s6 = mode;
    c->gpr[4] = a0; c->gpr[5] = a1; c->gpr[6] = mat; c->gpr[7] = s4;
    fr = br2_guest_frame(0x60);
    br2_st32(fr + 0x38, c->gpr[16]);
    br2_st32(fr + 0x3c, c->gpr[17]);
    br2_st32(fr + 0x40, c->gpr[18]);
    br2_st32(fr + 0x44, c->gpr[19]);
    br2_st32(fr + 0x48, c->gpr[20]);
    br2_st32(fr + 0x4c, c->gpr[21]);
    br2_st32(fr + 0x50, c->gpr[22]);
    br2_st32(fr + 0x54, c->gpr[23]);
    br2_st32(fr + 0x58, c->gpr[30]);
    br2_st32(fr + 0x5c, c->gpr[31]);
    br2_st32(fr + 0x60, a0);
    br2_st32(fr + 0x64, a1);
    P  = br2_ld32(s4 + 0x2c); br2_st32(fr + 0x28, P);
    L2 = br2_ld32(s4 + 0x10); br2_st32(fr + 0x2c, L2);
    T  = br2_ld32(s4 + 0x14); br2_st32(fr + 0x30, T);
    fp = br2_ld32(s4 + 0x20);
    s0 = br2_ld32(s4 + 0x0c);

    /* GTE CT0..CT7 <- 8 matrix words at mat (generated order preserved) */
    {
        uint32_t m0 = br2_ld32(mat + 0x00), m1 = br2_ld32(mat + 0x04);
        uint32_t m2 = br2_ld32(mat + 0x08), m3 = br2_ld32(mat + 0x0c);
        uint32_t m4 = br2_ld32(mat + 0x10), m5 = br2_ld32(mat + 0x14);
        uint32_t m6 = br2_ld32(mat + 0x18), m7 = br2_ld32(mat + 0x1c);
        c->gte_ctrl[0] = m0; c->gte_ctrl[1] = m1;
        c->gte_ctrl[2] = m2; c->gte_ctrl[3] = m3;
        gte_write_ctrl(c, 4, m4);
        c->gte_ctrl[5] = m5; c->gte_ctrl[6] = m6; c->gte_ctrl[7] = m7;
    }
    /* jr-table mode normalize (table at 0x801202CC): input 0..5 ->
     * {0,4,2,6,1,7}; inputs >=6 keep the raw index. */
    if ((uint32_t)s6 < 6u) {
        static const int8_t mode_map[6] = {0, 4, 2, 6, 1, 7};
        s6 = mode_map[s6];
    }

    /* 0x801469E0: s6 != 0 -> waypoint call, arg5 = 26 */
    if (s6 != 0) {
        c->gpr[4] = a0; c->gpr[5] = a1;
        c->gpr[6] = fp + 0x34; c->gpr[7] = 0;
        br2_st32(fr + 0x10, 26);
        (void)bind_jal(0x80136f70u, 0x80146a00u);
    }

    /* ---- bit0 section (0x80146A00..0x80146AE4) ---- */
    if (s6 & 1) {
        uint32_t f60 = br2_ld32(s4 + 0x60);
        if (!(f60 & 0x200000u)) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            int32_t single;
            /* flag pair on st_58, else on partner ctrl P+0x58:
             * "hit" = (f & 0x30000) && !(f & 0x8000000) */
            if ((f58 & 0x30000u) != 0 && (f58 & 0x08000000u) == 0) {
                single = 1;
            } else {
                uint32_t fP = br2_ld32(P + 0x58);
                single = (fP & 0x30000u) != 0 && (fP & 0x08000000u) == 0;
            }
            if (single) { s2 = 0; s3 = 1; }
            else        { s2 = 1; s3 = (int8_t)br2_ld8(s4 + 9); }
            while (s2 < s3) {
                uint32_t e = s0 + (uint32_t)s2 * 4u;
                int32_t idx = (int8_t)br2_ld8(e + 1);
                int32_t val = (int16_t)br2_ld16(e + 2);
                c->gpr[4] = a0; c->gpr[5] = a1;
                c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                c->gpr[7] = 2;
                br2_st32(fr + 0x10, (uint32_t)(val << 1));
                (void)bind_jal(0x80136f70u, 0x80146ad8u);
                s2++;
            }
        }
    }

    /* ---- bit1 section (0x80146AE4..0x80146C24) ---- */
    if (((uint32_t)s6 >> 1) & 1u) {
        int8_t b27 = (int8_t)br2_ld8(s4 + 0x27);
        if (b27 >= 0) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            s7 = 0;
            if (f58 & 0x40000u) {
                s7 = 1;
            } else if (f58 & 0x30000u) {
                if (!(f58 & 0x08000000u)) {
                    uint32_t f5c = br2_ld32(s4 + 0x5c);
                    if ((f5c & 0x80000080u) != 0x80u) s7 = 1;
                }
            }
            {
                uint32_t p64 = br2_ld32(P + 0x64);
                s5v = (int32_t)(p64 & 0x1000u);
                s3 = (int8_t)br2_ld8(s4 + 0x0a);
                if (s7 == 0 || s5v != 0) {
                    if (s3 > 0) {
                        uint32_t cur = L2;
                        int32_t n = 0;
                        do {
                            int32_t idx = (int8_t)br2_ld8(cur + 1);
                            int32_t val = (int16_t)br2_ld16(cur + 2);
                            cur += 4; n++;
                            c->gpr[4] = a0; c->gpr[5] = a1;
                            c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                            c->gpr[7] = 1;
                            br2_st32(fr + 0x10, (uint32_t)(val << 1));
                            (void)bind_jal(0x80136f70u, 0x80146bb8u);
                        } while (n < s3);
                    }
                }
            }
        }
        /* BC8: bit1 still set -> st_5c & 0x2000 gates the stub80 call */
        if (br2_ld32(s4 + 0x5c) & 0x2000u) {
            int32_t x = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e) - 0x400) << 20) >> 20;
            int32_t y = (((int32_t)(int16_t)br2_ld16(s4 + 0xc4) - 0x400) << 20) >> 20;
            c->gpr[4] = a0; c->gpr[5] = a1;
            c->gpr[6] = fp + 0x34; c->gpr[7] = (uint32_t)y;
            br2_st32(fr + 0x10, 0x400);
            br2_st32(fr + 0x14, (uint32_t)x);
            (void)bind_jal(0x80136f80u, 0x80146c24u);
            ret = x;
        } else {
            ret = 0;
        }
    }

    /* ---- bit2 section (0x80146C24..0x80146E00) ---- */
    if (((uint32_t)s6 >> 2) & 1u) {
        int8_t b26 = (int8_t)br2_ld8(s4 + 0x26);
        if (b26 >= 0) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            s7 = 0;
            if (f58 & 0x40000u) {
                s7 = 1;
            } else if (f58 & 0x30000u) {
                if (!(f58 & 0x08000000u)) {
                    uint32_t f5c = br2_ld32(s4 + 0x5c);
                    if ((f5c & 0x80000080u) != 0x80u) s7 = 1;
                }
            }
            /* guest s2/s3 are 0 on every path reaching the scan loop */
            {
                uint32_t p64 = br2_ld32(P + 0x64);
                s5v = (int32_t)(p64 & 0x1000u);
                uint32_t cur = T;
                int32_t n;
                for (n = 0; n < 8; n++, cur += 4) {
                    int32_t cmd = (int8_t)br2_ld8(cur);
                    int32_t idx, val;
                    if (cmd == -1) break;                    /* -> D94 */
                    if (cmd == -2) continue;                 /* -> D80 */
                    if ((s7 != 0) != (s5v != 0)) continue;   /* flag gate */
                    idx = (int8_t)br2_ld8(cur + 1);
                    val = (int16_t)br2_ld16(cur + 2);
                    c->gpr[4] = a0; c->gpr[5] = a1;
                    c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                    c->gpr[7] = 3;
                    br2_st32(fr + 0x10, (uint32_t)(val << 1));
                    (void)bind_jal(0x80136f70u, 0x80146d20u);
                    if (!(br2_ld32(s4 + 0x6c) & 0x800u)) {
                        /* second waypoint: 3-word vector at fr+0x18 */
                        uint32_t rec = br2_ld32(s4 + 0x40) +
                                       (uint32_t)((int32_t)cmd * 0x1c);
                        int32_t v2 = (int16_t)br2_ld16(cur + 2);
                        br2_st32(fr + 0x18, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x0e));
                        br2_st32(fr + 0x1c, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x10));
                        br2_st32(fr + 0x20, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x12));
                        c->gpr[4] = a0; c->gpr[5] = a1;
                        c->gpr[6] = fr + 0x18; c->gpr[7] = 5;
                        br2_st32(fr + 0x10, (uint32_t)(v2 << 1));
                        (void)bind_jal(0x80136f70u, 0x80146d80u);
                    }
                }
            }
        }
        /* D94: bit2 retest (always set on this path) + st_58 sign gate */
        {
            int32_t f58s = (int32_t)br2_ld32(s4 + 0x58);
            if (f58s < 0) {
                int32_t x = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e)
                            + (int32_t)(int16_t)br2_ld16(s4 + 0xc6) - 0x400) << 20) >> 20;
                int32_t z = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e) - 0x400) << 20) >> 20;
                c->gpr[4] = a0; c->gpr[5] = a1;
                c->gpr[6] = fp + 0x34; c->gpr[7] = (uint32_t)x;
                br2_st32(fr + 0x10, (uint32_t)((uint32_t)(uint8_t)br2_ld8(s4 + 0xdb) << 4));
                br2_st32(fr + 0x14, (uint32_t)z);
                (void)bind_jal(0x80136f80u, 0x80146e00u);
                ret = z;
            } else {
                ret = f58s;
            }
        }
    }

    /* 0x80146E00: trailing waypoint (jr-$ra stub), then epilogue */
    (void)bind_jal(0x80136f78u, 0x80146e08u);
    br2_guest_frame_free();
    return ret;
}
/* FUN_8018b100 — guest 0x8018B100 (264B, sibling of battle.c's
 * prim_list_persp_step). Dual-buffer primitive-list transform: walks
 * `count` 0x28-byte records at guest addr a0; for each record with
 * sx16(rec+0xC)==0 and lw(rec)!=0 it runs gte_xform3_t_ip twice around
 * an RTPS-class GTE cmd, then jalr's the record's 16-byte task entries.
 * Faithful port notes:
 *  - scratchpad context save/restore at 0x1F8003E0..0x3FC (caller
 *    s0-s6+ra) and the 0x1F8003C0..0x3DC block (live t4-t8 + GTE
 *    MAC1-3 results) are observable guest state — reproduced via
 *    br2_st32/live cpu->gpr reads exactly as generated.
 *  - the inner jalr loop propagates callee-clobbered $a1/$a2 across
 *    iterations (only $a0=entry[8], $a3=entry[4] are rewritten). The
 *    br2_call_generated bridge does NOT restore $a0-$a3, so reading
 *    cpu->gpr[5]/[6] before each call yields the same stale-register
 *    semantics the guest register file produces; $t0 is loaded with the
 *    jump target first, matching `lw $t0,0xC($s2); jalr $ra,$t0`.
 *  - $v0 = final record pointer (a0 advanced by 0x28*count), or a0
 *    unchanged when count <= 0 (shared epilogue `move $v0,$a0`). */
int FUN_8018b100(uint32_t recs,uint32_t a1,uint32_t a2,uint32_t count,
                 uint32_t a4,uint32_t a5)
{
    CPUState *c = clean_cpu_state();
    uint32_t s0, s1, s4, s5, s2, entry;
    int32_t remaining;
    if (!c) {
        uint32_t fr = br2_guest_frame(0x20);
        int r;
        br2_st32(fr + 0x10, a4);
        br2_st32(fr + 0x14, a5);
        r = (int)br2_call_clean(0x8018b100u,recs,a1,a2,count);
        br2_guest_frame_free();
        return r;
    }
    if ((int32_t)count <= 0) return (int)recs;      /* blez -> shared epilogue */

    /* Guest entry register state: $a2/$a3 arrive in $a2/$a3; mirror them
     * so the first xform call sees the same live $a2/$a3 the guest does. */
    c->gpr[4] = recs; c->gpr[5] = a1; c->gpr[6] = a2; c->gpr[7] = count;

    /* save caller s0-s6 + ra into scratchpad (0x1F8003E0..0x3FC) */
    br2_st32(0x1f8003fcu, c->gpr[31]);
    br2_st32(0x1f8003f8u, c->gpr[16]);
    br2_st32(0x1f8003f4u, c->gpr[17]);
    br2_st32(0x1f8003f0u, c->gpr[18]);
    br2_st32(0x1f8003ecu, c->gpr[19]);
    br2_st32(0x1f8003e8u, c->gpr[20]);
    br2_st32(0x1f8003e4u, c->gpr[21]);
    br2_st32(0x1f8003e0u, c->gpr[22]);

    s0 = recs; s1 = a1; s4 = a4; s5 = a5;
    remaining = (int32_t)count;
    do {
        int32_t t0v = (int16_t)br2_ld16(s0 + 0xcu);
        uint32_t prim = br2_ld32(s0);          /* lw $a1,0($s0) */
        c->gpr[8] = (uint32_t)t0v;              /* $t0 = lh result */
        c->gpr[4] = s5;                         /* delay-slot move $a0,$s5 */
        if (t0v == 0 && prim != 0) {
            /* jal 0x80186F5C (gte_xform3_t_ip): $a0=$s5,
             * $a1=prim+0x20, $a2/$a3 = live leftovers. */
            c->gpr[5] = prim + 0x20;
            gte_xform3_t_ip((uint32_t *)guest_at(s5),
                            (uint16_t *)guest_at(prim + 0x20));
            /* lwc2 gte[0]=rec[1], gte[1]=rec[2]; GTE cmd 0x0480012 */
            setCopReg(2, BR2_GTE_REG(0),  br2_ld32(s0 + 4));
            setCopReg(2, BR2_GTE_REG(1),  br2_ld32(s0 + 8));
            copFunction(2, 0x480012);
            /* stash live t4-t8 + MAC1-3 into scratchpad 0x1F8003C0..3DC */
            br2_st32(0x1f8003c0u, c->gpr[14]);
            br2_st32(0x1f8003c4u, c->gpr[12]);
            br2_st32(0x1f8003c8u, c->gpr[15]);
            br2_st32(0x1f8003ccu, c->gpr[24]);
            br2_st32(0x1f8003d0u, c->gpr[13]);
            br2_st32(0x1f8003d4u, (uint32_t)getCopReg(2, BR2_GTE_REG(25)));
            br2_st32(0x1f8003d8u, (uint32_t)getCopReg(2, BR2_GTE_REG(26)));
            br2_st32(0x1f8003dcu, (uint32_t)getCopReg(2, BR2_GTE_REG(27)));
            /* jal 0x80186F5C again: $a0=$s4, $a1=0x1F8003C0. */
            c->gpr[5] = 0x1f8003c0u;
            c->gpr[4] = s4;
            gte_xform3_t_ip((uint32_t *)guest_at(s4),
                            (uint16_t *)guest_at(0x1f8003c0u));
            /* task-entry loop: s2 = lw(rec+0x1C) -> [count][16B entries] */
            s2 = br2_ld32(s0 + 0x1c);
            c->gpr[5] = s1;                     /* move $a1,$s1 once */
            {
                int32_t n = (int32_t)br2_ld32(s2);
                entry = s2 + 4;
                /* guest guards with beqz/bgtz: any n != 0 runs >=1 entry */
                if (n != 0) do {
                    uint32_t a3v = br2_ld32(entry + 4);
                    uint32_t a0v = br2_ld32(entry + 8);
                    uint32_t fn  = br2_ld32(entry + 12);
                    c->gpr[7] = a3v;
                    c->gpr[8] = fn;             /* $t0 = jalr target */
                    c->gpr[4] = a0v;
                    br2_call_clean(fn, a0v, c->gpr[5], c->gpr[6], a3v);
                    n--;
                    entry += 16;
                } while (n > 0);
            }
        }
        remaining--;
        s0 += 0x28;                             /* delay-slot advance */
    } while (remaining > 0);

    /* epilogue: $a0 = final record ptr (-> $v0), restore saved regs */
    c->gpr[4] = s0;
    c->gpr[31] = br2_ld32(0x1f8003fcu);
    c->gpr[16] = br2_ld32(0x1f8003f8u);
    c->gpr[17] = br2_ld32(0x1f8003f4u);
    c->gpr[18] = br2_ld32(0x1f8003f0u);
    c->gpr[19] = br2_ld32(0x1f8003ecu);
    c->gpr[20] = br2_ld32(0x1f8003e8u);
    c->gpr[21] = br2_ld32(0x1f8003e4u);
    c->gpr[22] = br2_ld32(0x1f8003e0u);
    return (int)s0;
}

__attribute__((constructor))
static void bind_globals_7(void)
{
    s_NAME_ENTRY_80122080 = (char *)guest_at(0x80122080);
    s_RANKING_80122048 = (char *)guest_at(0x80122048);
    s_SURVIVAL_8012205c = (char *)guest_at(0x8012205c);
    s_TIME_ATTACK_80122050 = (char *)guest_at(0x80122050);
    s_VS_RANKING_80122068 = (char *)guest_at(0x80122068);
    s_time_out_in_decoding___80120e90 = (char *)guest_at(0x80120e90);
}

/* guest_of — inverse of guest_at: recover the guest address a
 * bound host pointer refers to (arena -> 0x80xxxxxx, scratch ->
 * 0x1f800xxx). Needed where guest code stores ptr & 0xffffff tags. */
__attribute__((weak,noinline))
uint32_t guest_of(const void *p)
{
    const uint8_t *b = (const uint8_t *)p;
    if (b >= guest_arena && b < guest_arena + sizeof(guest_arena))
        return 0x80000000u | (uint32_t)(b - guest_arena);
    if (b >= guest_scratch && b < guest_scratch + sizeof(guest_scratch))
        return 0x1f800000u | (uint32_t)(b - guest_scratch);
    return 0;
}
uint8_t *g_DAT_801b5b98;
uint8_t *g__DAT_801f68d8;
uint8_t *g_DAT_801b55ec;
uint8_t *g__DAT_1f8000e0;
uint8_t *g__DAT_1f8000e4;
uint8_t *g__DAT_801e8500;
uint8_t *g_DAT_801b55dc;
uint8_t *g_DAT_801b55e0;
uint8_t *g_DAT_801b55e4;
uint8_t *g_DAT_801f642c;
uint8_t *g_DAT_801f68b4;


__attribute__((constructor))
static void bind_globals_8(void)
{
    const char *names[] = {"g_DAT_801b5b98","g__DAT_801f68d8","g_DAT_801b55ec","g__DAT_1f8000e0","g__DAT_1f8000e4","g__DAT_801e8500","g_DAT_801b55dc","g_DAT_801b55e0","g_DAT_801b55e4","g_DAT_801f642c","g_DAT_801f68b4"};
    uint8_t **vars[] = {&g_DAT_801b5b98,&g__DAT_801f68d8,&g_DAT_801b55ec,&g__DAT_1f8000e0,&g__DAT_1f8000e4,&g__DAT_801e8500,&g_DAT_801b55dc,&g_DAT_801b55e0,&g_DAT_801b55e4,&g_DAT_801f642c,&g_DAT_801f68b4};
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        *vars[i] = guest_for(names[i]);
}

/* ---- game.c batch-2 callees ---- */

/* GPU syscall dispatch: wrappers in gfx.c resolve table entries
 * to guest func addrs and hand them here. Weak default = no-op until
 * the libgpu syscall cluster (0x80192xxx) is ported. */
__attribute__((weak))
uint32_t gpu_invoke(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)fn; (void)a0; (void)a1; (void)a2; (void)a3;
    return 0;
}

uint8_t *g_DAT_801b8320;
uint8_t *g_DAT_801b8321;
uint8_t *g_DAT_801b8322;
uint8_t *g_DAT_801b8323;
uint8_t *g_DAT_801b8324;
uint8_t *g_DAT_801b8326;
/* ---- cd.c + gfx.c globals ---- */
uint8_t *g_DAT_801bc7b4;
uint8_t *g_DAT_801bc7c0;
uint8_t *g_DAT_801bc848;
uint8_t *g_DAT_801bc854;
uint8_t *g_DAT_801bcb80;
uint8_t *g_DAT_801bcb94;
uint8_t *g_DAT_801bcb98;
uint8_t *g_DAT_801bcb9c;
uint8_t *g_DAT_801bcba4;
uint8_t *g_DAT_801bcba8;
uint8_t *g_DAT_801bcbac;
uint8_t *g_DAT_801bcbb0;
char *s_ClearImage2_801254f8;
char *s_ClearImage_801254ec;
char *s_ClearOTagR__08x__d_____80125540;
char *s_DrawOTag__08x_____80125558;
char *s_DrawSync__d_____801254b4;
char *s_LoadImage_80125504;
char *s_PutDispEnv__08x_____801255a0;
char *s_PutDrawEnv__08x_____8012556c;
char *s_ResetGraph__d_____80125430;
char *s_ResetGraph_jtb__08x_env__08x_80125410;
char *s_SetDispMask__d_____801254a0;

__attribute__((constructor))
static void bind_globals_9(void)
{
    const char *names[] = {"g_DAT_801bc7b4","g_DAT_801bc7c0","g_DAT_801bc848","g_DAT_801bc854","g_DAT_801bcb80","g_DAT_801bcb94","g_DAT_801bcb98","g_DAT_801bcb9c","g_DAT_801bcba4","g_DAT_801bcba8","g_DAT_801bcbac","g_DAT_801bcbb0"};
    uint8_t **vars[] = {&g_DAT_801bc7b4,&g_DAT_801bc7c0,&g_DAT_801bc848,&g_DAT_801bc854,&g_DAT_801bcb80,&g_DAT_801bcb94,&g_DAT_801bcb98,&g_DAT_801bcb9c,&g_DAT_801bcba4,&g_DAT_801bcba8,&g_DAT_801bcbac,&g_DAT_801bcbb0};
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        *vars[i] = guest_for(names[i]);
    s_ClearImage2_801254f8 = (char *)guest_at(0x801254f8);
    s_ClearImage_801254ec = (char *)guest_at(0x801254ec);
    s_ClearOTagR__08x__d_____80125540 = (char *)guest_at(0x80125540);
    s_DrawOTag__08x_____80125558 = (char *)guest_at(0x80125558);
    s_DrawSync__d_____801254b4 = (char *)guest_at(0x801254b4);
    s_LoadImage_80125504 = (char *)guest_at(0x80125504);
    s_PutDispEnv__08x_____801255a0 = (char *)guest_at(0x801255a0);
    s_PutDrawEnv__08x_____8012556c = (char *)guest_at(0x8012556c);
    s_ResetGraph__d_____80125430 = (char *)guest_at(0x80125430);
    s_ResetGraph_jtb__08x_env__08x_80125410 = (char *)guest_at(0x80125410);
    s_SetDispMask__d_____801254a0 = (char *)guest_at(0x801254a0);
}

/* ---- cd.c / gfx.c callees ---- */

__attribute__((constructor))
static void bind_globals_10(void)
{
    g_DAT_801b8320 = guest_at(0x801b8320);
    g_DAT_801b8321 = guest_at(0x801b8321);
    g_DAT_801b8322 = guest_at(0x801b8322);
    g_DAT_801b8323 = guest_at(0x801b8323);
    g_DAT_801b8324 = guest_at(0x801b8324);
    g_DAT_801b8326 = guest_at(0x801b8326);
}

/* ---- cd.c batch26 callees ---- */
__attribute__((weak))
uint32_t call_guest(uint32_t fn, uint32_t a, uint32_t b)
{
    (void)fn; (void)a; (void)b;
    return 0;
}
char *s__s____80125840;
char *s__s___s__Sync__s__Ready__s_801257b4;
char *s__s__no_param_80125848;
__attribute__((constructor))
static void bind_globals_11(void)
{
    s__s____80125840 = (char *)guest_at(0x80125840);
    s__s___s__Sync__s__Ready__s_801257b4 = (char *)guest_at(0x801257b4);
    s__s__no_param_80125848 = (char *)guest_at(0x80125848);
}

char *s_CD_sync_8012582c;
char *s_CD_ready_80125834;
char *s_CD_datasync_801258b0;
char *s_CD_timeout__801257a4;
char *s_VSync__timeout_80125b38;
char *s_CdRead__sector_error_80125ac8;
char *s_DiskError__801257d0;
char *s_com__s_code___02x__02x__801257dc;
char *s_CDROM__unknown_intr_801257f8;
char *s___d__8012580c;
__attribute__((constructor))
static void bind_globals_12(void)
{
    s_CD_sync_8012582c = (char *)guest_at(0x8012582c);
    s_CD_ready_80125834 = (char *)guest_at(0x80125834);
    s_CD_datasync_801258b0 = (char *)guest_at(0x801258b0);
    s_CD_timeout__801257a4 = (char *)guest_at(0x801257a4);
    s_VSync__timeout_80125b38 = (char *)guest_at(0x80125b38);
    s_CdRead__sector_error_80125ac8 = (char *)guest_at(0x80125ac8);
    s_DiskError__801257d0 = (char *)guest_at(0x801257d0);
    s_com__s_code___02x__02x__801257dc = (char *)guest_at(0x801257dc);
    s_CDROM__unknown_intr_801257f8 = (char *)guest_at(0x801257f8);
    s___d__8012580c = (char *)guest_at(0x8012580c);
}

__attribute__((weak))
uint32_t bios_call(uint32_t fn, uint32_t a0)
{
    (void)fn; (void)a0;
    return 0;
}
char *s__s_bad_RECT_801254c8;
char *s___d__d____d__d__801254d4;
__attribute__((constructor))
static void bind_globals_13(void)
{
    s__s_bad_RECT_801254c8 = (char *)guest_at(0x801254c8);
    s___d__d____d__d__801254d4 = (char *)guest_at(0x801254d4);
}

/* batch28 callees (not yet ported) */
__attribute__((weak)) int  text_win_init() { return 0; }
__attribute__((weak)) int  text_line_select() { return 0; }
__attribute__((weak)) uint32_t text_widget_alloc() { return 0; }

char *s_Print_mode_error_8011f140;
__attribute__((constructor))
static void bind_globals_28(void)
{
    s_Print_mode_error_8011f140 = (char *)guest_at(0x8011f140);
}

/* batch29 text-window callees */
__attribute__((weak)) void text_win_printf() { return; }
__attribute__((weak)) void guest_memset_b() { return; }
__attribute__((weak)) uint32_t gpu_sprite_rect_emit() { return 0; }
__attribute__((weak)) uint32_t gpu_sprite_rect_emit_mode() { return 0; }
__attribute__((weak)) void gpu_pkt_tag_74() { return; }
__attribute__((weak)) void gpu_pkt_tag_60() { return; }
void *PTR_FUN_801b831c;

/* batch30 callees (not yet ported) */
__attribute__((weak)) void menu_stepper_redraw() { return; }
__attribute__((weak)) void menu_stepper_input() { return; }
__attribute__((weak)) void chan_sweep_flag100() { return; }
__attribute__((weak)) void chan_sweep_flag200() { return; }
__attribute__((weak)) void datetime_confirm_a() { return; }
__attribute__((weak)) void datetime_confirm_b() { return; }
__attribute__((weak)) int  slot_display_pos() { return 0; }
__attribute__((weak)) void mdec_pump_a() { return; }
__attribute__((weak)) void mdec_pump_b() { return; }
__attribute__((weak)) void gp_20_store_default() { return; }
__attribute__((weak)) void aux_tbl_store_a() { return; }
__attribute__((weak)) void aux_tbl_store_b() { return; }

char *PTR_s_NAME_ENTRY_801b13a8[16];
char *PTR_s_MOVIES_AND_PICTURES_801b13c4;
__attribute__((constructor))
static void bind_globals_30(void)
{
    int i;
    for (i = 0; i < 16; i++)
        PTR_s_NAME_ENTRY_801b13a8[i] = (char *)guest_at(
            *(uint32_t *)guest_at(0x801b13a8 + i * 4));
    PTR_s_MOVIES_AND_PICTURES_801b13c4 = (char *)guest_at(
        *(uint32_t *)guest_at(0x801b13c4));
}
/* ret_zero_b @0x80163898: six consecutive `jr ra; move v0,zero`
 * stubs in guest — returns 0. Real impl may be a jump-table alias;
 * keep faithful return-0 until the true target is identified. */
__attribute__((weak)) void dialog_mode_mirror() { return; }
__attribute__((weak)) void mc_slot_action() { return; }
__attribute__((weak)) void print_dispatch() { return; }
__attribute__((weak)) void hud_text_a() { return; }
__attribute__((weak)) void hud_text_b() { return; }
__attribute__((weak)) void hud_text_c() { return; }
__attribute__((weak)) void slot_name_measure() { return; }
__attribute__((weak)) void hud_text_d() { return; }
__attribute__((weak)) void sprite_node_emit_a() { return; }
__attribute__((weak)) void sprite_node_emit_b() { return; }

/* printf rodata: hex-digit strings + guest ctype bitmap ctype_bitmap_801b6701 (@0x801b6701)
 * (extracted verbatim from the payload image). */
char s_0123456789abcdef_80124fe8[] = "0123456789abcdef";
char s_0123456789ABCDEF_80125004[] = "0123456789ABCDEF";
char s__null__80124ffc[] = "(null)";
uint8_t ctype_bitmap_801b6701[256] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 8, 8, 8, 8, 8, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 24, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16, 16, 16, 16, 65, 65, 65, 65, 65, 65, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 16, 16, 16, 16, 16, 66, 66, 66, 66, 66, 66, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 64, 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26, 24, 26, 27, 27, 27, 26, 26, 26, 26, 27, 27, 27, 29, 29, 29, 34, 34, 34, 29, 29, 29, 27, 27, 29, 29, 32, 32, 34, 34, 37, 38, 37, 35, 35, 34, 35, 38, 38, 40, 40, 40, 48, 48, 46, 46, 56, 56, 58, 69, 69, 83, 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26};
/* bios_tramp_b035 — guest 0x8018EF50: BIOS B035 debug-output
 * trampoline fronting the line-buffer flush. a1 arrives as a host
 * pointer (callers pass guest_at(...)); convert back to a guest
 * address for the generated body. */
__attribute__((weak)) void bios_tramp_b035(uint32_t a0, const void *buf, uint32_t a2)
{
    /* Same routing as the strong cd.c port: B0 svc 0x35 with buf
     * translated host-pointer -> guest address. */
    (void)bios_svc6(0xb0,0x35,a0,guest_of(buf),a2,0);
}
__attribute__((weak)) void battle_config_eval() { return; }
__attribute__((weak)) int32_t mode_name_announce() { return 0; }
__attribute__((weak)) int32_t bcfg_step_a() { return 0; }
__attribute__((weak)) int32_t stage_fight_announce() { return 0; }
__attribute__((weak)) int32_t fight_timer_announce() { return 0; }
__attribute__((weak)) int32_t continue_countdown() { return 0; }
__attribute__((weak)) int32_t bcfg_step_d() { return 0; }
__attribute__((weak)) int32_t bcfg_step_c() { return 0; }
__attribute__((weak)) int32_t bcfg_step_b() { return 0; }
__attribute__((weak)) int32_t battle_state_gate() { return 0; }
__attribute__((weak)) void chan_blocks_reset() { return; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8018e300(void)
{
    /* guest 0x8018E300: li a0,1; syscall 0 (EnterCriticalSection/DI,
     * $v0 = old IEc). Routed through psx_syscall like clean_bridge. */
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;
    cpu->gpr[4] = 1;
    psx_syscall(cpu, 0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8018e310(uint32_t arg)
{
    /* guest 0x8018E310: li a0,2; syscall 0 (ExitCriticalSection/EI). */
    CPUState *cpu = clean_cpu_state();
    (void)arg;
    if (!cpu) return;
    cpu->gpr[4] = 2;
    psx_syscall(cpu, 0);
}
__attribute__((weak))
uint32_t bios_svc(uint32_t table, uint32_t svc, uint32_t a0, uint32_t a1)
{
    (void)svc; (void)a1;
    return bios_call(table, a0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_8019b380(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
    /* guest 0x8019B380: B0 svc 0x08 (kernel object alloc) trampoline —
     * forwards a0-a3 to the BIOS vector. */
    return bios_svc6(0xb0,0x08,a0,a1,a2,a3);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019b390(uint32_t a0)
{
    /* guest 0x8019B390: B0 svc 0x0C (kernel object delete) trampoline. */
    (void)bios_svc6(0xb0,0x0c,a0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void cd_mode_apply_wrap(void)
{
    /* guest 0x8019E410: B0 svc 0x4B trampoline — forwards live a0-a3. */
    br2_bios_tramp(0xb0,0x4b);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void cd_mode_apply(uint32_t a0)
{
    /* guest 0x8019E400: B0 svc 0x4A trampoline — forwards live a0-a3;
     * the C caller's a0 lands in $a0, a1-a3 are live leftovers exactly
     * as the guest jal would leave them. */
    CPUState *c = clean_cpu_state();
    if (c) c->gpr[4] = a0;
    br2_bios_tramp(0xb0,0x4a);
}

/* bind_bios_call_live — B0-kernel-call trampoline semantics for the
 * 0x8019E4C8/0x8019E55C/0x8019E600 family: the guest stub only sets
 * $t1=svc/$t2=0xB0 and jr's the vector, forwarding the caller's
 * a0-a3 untouched and returning the BIOS $v0. `ret_pc` is the guest
 * return address the jal/jalr would have written to $ra (used only by
 * the svc ring recorder). */
static uint32_t bind_bios_call_live(uint32_t svc, uint32_t ret_pc)
{
    CPUState *c = clean_cpu_state();
    uint32_t ra, v;
    if (!c) return bios_svc6(0xb0,svc,0,0,0,0);
    ra = c->gpr[31];
    c->gpr[31] = ret_pc;
    v = bios_svc6(0xb0,svc,c->gpr[4],c->gpr[5],c->gpr[6],c->gpr[7]);
    c->gpr[31] = ra;
    return v;
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e5cc(void)
{
    /* guest 0x8019E5CC: v0=0xDF80; copy 0x70 bytes (28 words)
     * 0x801AE430..0x801AE4A0 -> 0x0000DF80.., jr ra. Pure copy —
     * no calls, no IRQ-sensitive state. */
    uint32_t src = 0x801ae430u, end = 0x801ae4a0u, dst = 0xdf80u;
    while (src != end) {
        br2_st32(dst, br2_ld32(src));
        src += 4;
        dst += 4;
    }
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e4c8(void)
{
    /* guest 0x8019E4C8: DI; B0:0x56 (live a0-a3) -> obj;
     * p = *(obj+0x18); dst = (*(p+0x70)&0xFFFF)<<16 + (*(p+0x74)&0xFFFF)
     * + 40; copy 5 words 0x801AE4A0..0x801AE4B4 -> dst..; then
     * *(0x0000DFFC) = dst+20; jal 0x801957F0 (gpu_state_patch_defer).
     * No EI — faithful to the guest epilogue. */
    uint32_t obj, p, dst, src, end;
    FUN_8018e300();
    obj = bind_bios_call_live(0x56, 0x8019e4e8u);
    p   = br2_ld32(obj + 0x18u);
    dst = ((br2_ld32(p + 0x70u) & 0xffffu) << 16)
        + (br2_ld32(p + 0x74u) & 0xffffu) + 40u;
    src = 0x801ae4a0u; end = 0x801ae4b4u;
    while (src != end) {
        br2_st32(dst, br2_ld32(src));
        src += 4;
        dst += 4;
    }
    br2_st32(0xdffcu, dst);              /* dst already advanced +20 */
    gpu_state_patch_defer();
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e55c(void)
{
    /* guest 0x8019E55C: DI; B0:0x57 (live a0-a3) -> obj;
     * p = *(obj+0x16C); copy 5 words 0x801AE4B4..0x801AE4C8 ->
     * p+0x9C8..; jal 0x801957F0. No EI — faithful epilogue. */
    uint32_t obj, p, src, end;
    FUN_8018e300();
    obj = bind_bios_call_live(0x57, 0x8019e57cu);
    p   = br2_ld32(obj + 0x16cu);
    src = 0x801ae4b4u; end = 0x801ae4c8u;
    while (src != end) {
        br2_st32(p + 0x9c8u, br2_ld32(src));
        src += 4;
        p   += 4;
    }
    gpu_state_patch_defer();
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t cd_seek_state_machine(uint32_t arg)
{
    /* guest 0x8019D5B0: li t2,0xB0; li t1,0x0B; jr t2 — B0 svc 0x0B
     * trampoline forwarding live a0-a3 (generated
     * SLES_017.22_full_47.c br2eu_func_8019D5B0). ra already holds the
     * caller's return address; record it as the svc return pc. */
    CPUState *c = clean_cpu_state();
    uint32_t ret = c ? c->gpr[31] : 0x8019d5bcu;
    (void)arg;
    return (int32_t)bind_bios_call_live(0x0b, ret);
}

/* ---- menu-agent callees (unported, weak until disasm-verified) ---- */

/* ---- save-agent callees (unported, weak until disasm-verified) ---- */

/* ---- round-2 unported callees (weak until disasm-verified) ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0260(const void *rec)
{
    /* guest 0x801A0260: B0 svc 0x43 CD-record read trampoline.
     * Callers pass a host pointer; translate back to a guest address. */
    return bios_svc6(0xb0,0x43,guest_of(rec),0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801a05f0(uint32_t a0)
{
    /* guest 0x801A05F0: A0 svc 0xAC trampoline. */
    (void)bios_svc6(0xa0,0xac,a0,0,0,0);
}

/* ---- round-3 unported callees ---- */

/* ---- cd-batch32 callees ---- */

/* ---- cd 99440 callees ---- */

/* ---- menu batch32 callees ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int ret_zero_e(int32_t arg)
{
    /* guest 0x80161630 — debug/service state machine. Frame is 0xa0;
     * state lives at sp+0x58 (0=init, 1=announce loop, 0xf0=table
     * reset, 0xf1=exit request, 0xff=done), flag at sp+0x50 is the
     * return value. All frame traffic goes through br2_guest_frame so
     * sprintf/announce callees see the real guest stack; guest
     * registers are host vars. bind_jal supplies the per-site link
     * address as $ra (the generated CPS `jal link` contract). */
    CPUState *c = clean_cpu_state();
    uint32_t fr;
    int ret;
    if (!c) {
        return (int)br2_call_clean(0x80161630u,(uint32_t)arg,0,0,0);
    }
    c->gpr[4] = (uint32_t)arg;
    fr = br2_guest_frame(0xa0);
    /* prologue spills + arg home slot (observable stack image) */
    br2_st32(fr + 0x78, c->gpr[16]);
    br2_st32(fr + 0x7c, c->gpr[17]);
    br2_st32(fr + 0x80, c->gpr[18]);
    br2_st32(fr + 0x84, c->gpr[19]);
    br2_st32(fr + 0x88, c->gpr[20]);
    br2_st32(fr + 0x8c, c->gpr[21]);
    br2_st32(fr + 0x90, c->gpr[22]);
    br2_st32(fr + 0x94, c->gpr[23]);
    br2_st32(fr + 0x98, c->gpr[30]);
    br2_st32(fr + 0x9c, c->gpr[31]);
    br2_st32(fr + 0xa0, (uint32_t)arg);
    br2_st32(fr + 0x50, 0);
    br2_st32(fr + 0x58, 0);
    br2_st16(0x1f80008c, 0);
    for (;;) {
        uint32_t t0 = br2_ld32(fr + 0x58);
        if (t0 == 1) goto L_announce;
        if ((int32_t)t0 < 2) {
            if (t0 == 0) goto L_init;
            goto L_tick;                    /* negative state */
        }
        if (t0 == 0xf0u) goto L_reset;
        if (t0 == 0xf1u) { br2_st32(fr + 0x58, 0xffu); goto L_check; }
        goto L_tick;                        /* unknown state */

    L_init:
        /* 0x801616B4: arg==0 -> frame_divisor gate + intro sequence */
        if (br2_ld32(fr + 0xa0) == 0) {
            bind_jal(0x8012aaa8u, 0x801616ccu);      /* frame_divisor_get */
            if ((int32_t)c->gpr[2] == 0) {
                c->gpr[4] = 2; c->gpr[5] = 0; c->gpr[6] = 0;
                bind_jal(0x8012cbc0u, 0x801616e0u);  /* print_mode_dispatch_b */
                c->gpr[4] = 0;
                bind_jal(0x8012c0b4u, 0x801616e8u);  /* intro_movie_state */
                if ((int32_t)c->gpr[2] != 0) {
                    c->gpr[4] = 0x203; c->gpr[5] = 0; c->gpr[6] = 0;
                    bind_jal(0x80156ef4u, 0x801616fcu); /* deferred_cmd_post */
                }
            }
        }
        /* 0x80161700: clear cells, build 2x8 slot-index table at fr+0x30 */
        br2_st16(fr + 0x1a, 0);
        br2_st16(fr + 0x18, 0);
        {
            int32_t s5, s1, s2;
            for (s5 = 0; s5 < 2; s5++) {
                uint32_t a0v = (uint32_t)s5 << 4;
                for (s1 = 0; s1 < 8; s1++, a0v += 2) {
                    uint32_t fid = br2_ld16(0x801c11e0u + a0v + 0x3f0u);
                    uint32_t tp = 0x801b41d4u;
                    s2 = 0;
                    for (;;) {
                        if (fid == br2_ld16(tp)) break;
                        s2++;
                        if (s2 >= 9) break;
                        tp += 2;
                    }
                    br2_st16(fr + 0x30 + a0v, (uint16_t)s2);
                }
            }
        }
        /* state = old+1 (=1); globals 0x801F8508=0, 0x801F8518=0x30 */
        br2_st32(fr + 0x58, br2_ld32(fr + 0x58) + 1u);
        br2_st32(0x801f8508u, 0);
        br2_st32(0x801f8518u, 0x30);
        /* fall through into the announce body */

    L_announce:
        /* 0x80161794: announce + banner + optional deferred post */
        c->gpr[4] = br2_ld32(0x1f800008); c->gpr[5] = br2_ld32(0x1f80000c);
        c->gpr[6] = 0x8012136c; c->gpr[7] = 0x30;
        br2_st32(fr + 0x10, 0x30); br2_st32(fr + 0x14, 0);
        bind_jal(0x8012e58cu, 0x801617b8u);             /* announce_param_block_init */
        c->gpr[4] = 5; c->gpr[5] = 0; c->gpr[6] = 4; c->gpr[7] = 7;
        bind_jal(0x8012bf0cu, 0x801617ccu);             /* banner_stripe_scroll */
        if (br2_ld16(0x1f80007c) & 0x800) {
            c->gpr[4] = 0x400; c->gpr[5] = 0xc; c->gpr[6] = 0;
            bind_jal(0x80156ef4u, 0x801617ecu);         /* deferred_cmd_post */
            br2_st32(fr + 0x58, 0xf0u);
            br2_st32(fr + 0x50, 1);
        }
        /* 0x801617FC: sprintf + announce pass, s5 = 0..1 */
        {
            uint32_t c64 = 0x300000u, c68 = 0x600000u;
            uint32_t c6c = 0x1f800000u, c70 = 0x20u;
            int32_t fpv = 0;
            int32_t s5;
            br2_st32(fr + 0x5c, fr + 0x20);             /* buf ptr cell */
            for (s5 = 0; s5 < 2; s5++) {
                int32_t s3v = 0x60;
                /* sprintf(buf, 0x80121D1C, s5+1) */
                c->gpr[4] = fr + 0x20;
                c->gpr[5] = 0x80121d1c;
                c->gpr[6] = (uint32_t)(s5 + 1);
                bind_jal(0x8018ef60u, 0x8016183cu);     /* sprintf_core_guest */
                /* announce(buf-string, ...) arg5=0x60 arg6=0 */
                c->gpr[4] = br2_ld32(0x1f800008);
                c->gpr[5] = br2_ld32(0x1f80000c);
                c->gpr[6] = fr + 0x20;
                c->gpr[7] = (uint32_t)((int32_t)c64 >> 16);
                br2_st32(fr + 0x10, 0x60); br2_st32(fr + 0x14, 0);
                bind_jal(0x8012e58cu, 0x80161860u);
                /* pad dispatch: (lhu(c6c+0x6c) & 0xc) == 0xc ? */
                {
                    uint32_t pad = br2_ld16(c6c + 0x6c);
                    int32_t s1 = 0, s2;
                    if ((pad & 0xc) == 0xc) {
                        /* 0x8016187C: map 8 entries 0x801B3F78.. via table */
                        uint32_t a0p = 0x801b3f78u, a3p = 0x801b3f88u;
                        uint32_t a2o = 0;
                        for (;;) {
                            uint32_t fid = br2_ld16(a0p);
                            uint32_t tp = 0x801b41d4u;
                            int32_t k = 0;
                            for (;;) {
                                if (fid == br2_ld16(tp)) break;
                                k++;
                                if (k >= 9) break;
                                tp += 2;
                            }
                            br2_st16(fr + 0x30 + fpv + a2o, (uint16_t)k);
                            a0p += 2;
                            if (a0p >= a3p) break;
                            a2o += 2;
                        }
                        s2 = -1;
                    } else {
                        /* 0x80161918: first set bit of pad & 8-entry mask tbl */
                        uint32_t a0p = 0x801b419cu;
                        for (; s1 < 8; s1++, a0p += 2) {
                            if (br2_ld16(a0p) & pad) {
                                uint32_t cell = fr + 0x30 + fpv + (uint32_t)(s1 * 2);
                                int32_t idx = (int16_t)br2_ld16(cell);
                                c->gpr[4] = (uint32_t)idx;
                                c->gpr[5] = 9;
                                c->gpr[6] = (uint32_t)s5;
                                c->gpr[7] = 1;
                                bind_jal(0x80129c10u, 0x80161900u); /* counter_step_pad_c */
                                br2_st16(cell, (uint16_t)c->gpr[2]);
                                br2_st32(0x801c11f0u, 1);
                                break;
                            }
                        }
                        s2 = s1;
                    }
                    /* 0x8016194C: per-column announce loop s1 = 0..7 */
                    {
                        int32_t s3w = s3v + 0x20;          /* guest s3 += 0x20 */
                        uint32_t s4v = (uint32_t)s3w << 16;/* sll s4,s3,16 */
                        uint32_t s6p = 0x801b417cu;
                        uint32_t t54 = c70 + 0x50;
                        uint32_t t60 = c68;
                        int32_t s1i;
                        for (s1i = 0; s1i < 8; s1i++) {
                            int32_t s3a;
                            uint32_t s0v;
                            if (s1i == s2) {
                                /* 0x8016197C: special announce (arg6=3) */
                                c->gpr[4] = br2_ld32(0x1f800008);
                                c->gpr[5] = br2_ld32(0x1f80000c);
                                c->gpr[6] = 0x80121d28;
                                c->gpr[7] = (uint32_t)((int32_t)t60 >> 16);
                                br2_st32(fr + 0x10, (uint32_t)((int32_t)s4v >> 16));
                                br2_st32(fr + 0x14, 3);
                                bind_jal(0x8012e58cu, 0x801619a8u);
                                s3a = 3;
                            } else {
                                s3a = 0;
                            }
                            /* 0x801619A8: announce(lw(s6p++), a3=s16(c64>>16),
                             * arg5=s4>>16, arg6=8) */
                            c->gpr[4] = br2_ld32(0x1f800008);
                            c->gpr[5] = br2_ld32(0x1f80000c);
                            c->gpr[6] = br2_ld32(s6p); s6p += 4;
                            c->gpr[7] = (uint32_t)((int32_t)c64 >> 16);
                            s0v = (uint32_t)((int32_t)s4v >> 16);
                            br2_st32(fr + 0x10, s0v);
                            br2_st32(fr + 0x14, 8);
                            bind_jal(0x8012e58cu, 0x801619d4u);
                            /* 0x801619D4: announce(string tbl[idx], a3=s16(t54),
                             * arg5=s4>>16, arg6=s3a) */
                            {
                                uint32_t cell = fr + 0x30 + fpv + (uint32_t)(s1i * 2);
                                int32_t idx = (int16_t)br2_ld16(cell);
                                c->gpr[4] = br2_ld32(0x1f800008);
                                c->gpr[5] = br2_ld32(0x1f80000c);
                                c->gpr[6] = br2_ld32(0x801b41acu + (uint32_t)idx * 4u);
                                c->gpr[7] = (uint32_t)(int32_t)(int16_t)t54;
                                br2_st32(fr + 0x10, s0v);
                                br2_st32(fr + 0x14, (uint32_t)s3a);
                                bind_jal(0x8012e58cu, 0x80161a1cu);
                            }
                            s4v += 0x180000u;
                        }
                    }
                }
                /* 0x80161A30: per-s5 cell updates (lui 0x100 = 0x01000000) */
                c64 += 0x01000000u; c68 += 0x01000000u;
                c6c += 2;           c70 += 0x01000000u;
                fpv += 0x10;
            }
        }
        /* 0x80161A74: hud_text_d + announce tail + spr submit */
        {
            int32_t lhu22 = (int32_t)br2_ld16(0x1f800022);
            int32_t s16a = (int32_t)(int16_t)(lhu22 - 0x38);
            int32_t s16b = (int32_t)(int16_t)(lhu22 - 0x3c);
            uint32_t a0p = br2_ld32(0x1f800008) + 4;
            uint32_t a1p = br2_ld32(0x1f80000c);
            c->gpr[4] = a0p; c->gpr[5] = a1p;
            c->gpr[6] = 0x80121d38; c->gpr[7] = 0x98;
            br2_st32(fr + 0x10, (uint32_t)s16a); br2_st32(fr + 0x14, 0);
            bind_jal(0x8012e7ccu, 0x80161aa8u);             /* hud_text_d */
            c->gpr[4] = a0p; c->gpr[5] = a1p;
            c->gpr[6] = 0x80121d48; c->gpr[7] = 0xdc;
            br2_st32(fr + 0x10, (uint32_t)s16b); br2_st32(fr + 0x14, 8);
            bind_jal(0x8012e58cu, 0x80161ae0u);
            c->gpr[4] = a0p; c->gpr[5] = a1p;
            c->gpr[6] = 0x80121318; c->gpr[7] = 0x120;
            br2_st32(fr + 0x10, (uint32_t)s16a); br2_st32(fr + 0x14, 0);
            bind_jal(0x8012e7ccu, 0x80161b14u);
            c->gpr[4] = a0p;
            c->gpr[5] = ((uint32_t)lhu22 << 16) + 0xffc40148u;
            c->gpr[6] = 0x0f17001eu; c->gpr[7] = 0x7a16d8e0u;
            bind_jal(0x8018c824u, 0x80161b44u);             /* spr_slot_pkt_submit */
        }
        /* 0x80161B44: saved-arg dispatch -> hud_gauge_render or tick */
        if (br2_ld32(fr + 0xa0) == 0) {
            c->gpr[4] = 0x1f800000; c->gpr[5] = br2_ld32(0x1f800008);
            c->gpr[6] = 0x24;
            bind_jal(0x8012c374u, 0x80161b70u);             /* hud_gauge_render */
        } else if (br2_ld32(fr + 0xa0) == 1) {
            c->gpr[4] = 0x1f800000; c->gpr[5] = br2_ld32(0x1f800008);
            c->gpr[6] = 0x34;
            bind_jal(0x8012c374u, 0x80161b84u);
        }
        goto L_tick;

    L_reset:
        /* 0x80161B90: state==0xf0 -> rewrite fighter-id table, state=0xff */
        {
            uint32_t v0 = br2_ld32(0x1f8000c0);
            br2_st8(v0 + 0x17, 0);
            br2_st8(v0 + 0x18, 0);
            {
                int32_t s5, s1;
                for (s5 = 0; s5 < 2; s5++) {
                    uint32_t a0v = (uint32_t)s5 << 4;
                    for (s1 = 0; s1 < 8; s1++, a0v += 2) {
                        int32_t idx = (int16_t)br2_ld16(fr + 0x30 + a0v);
                        br2_st16(0x801c11e0u + a0v + 0x3f0u,
                                 br2_ld16(0x801b41d4u + (uint32_t)idx * 2u));
                    }
                }
            }
            br2_st32(fr + 0x58, 0xffu);
            goto L_check;
        }

    L_tick:
        /* 0x80161C08: frame_tick(0x1f800000) then re-check state */
        c->gpr[4] = 0x1f800000;
        bind_jal(0x8012aeecu, 0x80161c10u);                 /* frame_tick */
    L_check:
        if (br2_ld32(fr + 0x58) != 0xffu) continue;
        break;
    }
    ret = (int)br2_ld32(fr + 0x50);
    br2_guest_frame_free();
    return ret;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_801948c0(uint32_t val) { return (int32_t)fixed_recip_lut(val); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80195370(uint32_t *in, uint32_t *out)
{
    /* guest 0x80195370: lwc2 gte[9..11] <- a0[0..2], GTE cmd
     * 0x0A80428 (SQR), swc2 gte[25..27] (MAC1..3) -> a1[0..2],
     * $v0 = a1. Callers pass host pointers that may alias; stage
     * through a real guest frame like gte_sqr_vec3_b expects. */
    uint32_t fr = br2_guest_frame(0x20);
    uint32_t in_g = fr, out_g = fr + 0x10;
    uint32_t r;
    memcpy(guest_at(in_g), in, 12);
    r = gte_sqr_vec3_b(in_g, out_g);
    memcpy(out, guest_at(out_g), 12);
    br2_guest_frame_free();
    return (int)r;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a0ddc(void)
{
    /* guest 0x801A0DDC: DI ($v0 = old IEc saved to s0); for i=0..7
     * jal 0x801A11C0 — a B0 svc 0x09 trampoline — with a0 =
     * *(0x8020EF70+4i) (the eight channel objects); EI only when the
     * saved IEc == 1. Faithful DI/EI pairing preserved. */
    CPUState *c = clean_cpu_state();
    uint32_t saved;
    int i;
    if (!c) { br2_call_clean(0x801a0ddcu,0,0,0,0); return; }
    FUN_8018e300();
    saved = c->gpr[2];
    for (i = 0; i < 8; i++) {
        uint32_t obj = br2_ld32(0x8020ef70u + (uint32_t)i * 4u);
        uint32_t ra  = c->gpr[31];
        /* jal return addresses inside the unrolled guest loop. */
        c->gpr[31] = 0x801a0dfcu + (uint32_t)i * 0x10u;
        c->gpr[4]  = obj;
        (void)bios_svc6(0xb0,0x09,obj,c->gpr[5],c->gpr[6],c->gpr[7]);
        c->gpr[31] = ra;
    }
    if (saved == 1) FUN_8018e310(0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a2f0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
    /* guest 0x8019A2F0: B0 svc 0x12 CD channel init trampoline. */
    (void)bios_svc6(0xb0,0x12,a0,a1,a2,a3);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a300(void)
{
    /* guest 0x8019A300: B0 svc 0x13 trampoline. */
    (void)bios_svc6(0xb0,0x13,0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a310(void)
{
    /* guest 0x8019A310: B0 svc 0x14 trampoline. */
    (void)bios_svc6(0xb0,0x14,0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a350(void)
{
    /* guest 0x8019A350: indirect tail-call — lw t1,0x801FEB58; jr t1.
     * The cell holds an OpenBIOS kernel-object method pointer. */
    (void)call_guest(br2_ld32(0x801feb58u),0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a364(void)
{
    /* guest 0x8019A364: indirect tail-call — lw t1,0x801FEB5C; jr t1. */
    (void)call_guest(br2_ld32(0x801feb5cu),0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad48(uint32_t a0)
{
    /* guest 0x8019AD48: A0 svc 0x72 trampoline. */
    (void)bios_svc6(0xa0,0x72,a0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad70(uint32_t a0)
{
    /* guest 0x8019AD70: B0 svc 0x18 trampoline. */
    (void)bios_svc6(0xb0,0x18,a0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad80(uint32_t a0)
{
    /* guest 0x8019AD80: B0 svc 0x19 trampoline. */
    (void)bios_svc6(0xb0,0x19,a0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e420(void)
{
    /* guest 0x8019E420: B0 svc 0x4C trampoline. */
    (void)bios_svc6(0xb0,0x4c,0,0,0,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e600(void)
{
    /* guest 0x8019E600: DI; B0:0x56 (live a0-a3) -> obj;
     * p = *(obj+0x18); copy 3 words 0x801AE670..0x801AE67C ->
     * p+112..; jal 0x801957F0 (gpu_state_patch_defer); EI;
     * restore ra, jr ra. */
    uint32_t obj, src, end;
    FUN_8018e300();
    obj = bind_bios_call_live(0x56, 0x8019e620u);
    src = 0x801ae670u; end = 0x801ae67cu;
    while (src != end) {
        br2_st32(obj + 112u, br2_ld32(src));
        src += 4;
        obj += 4;
    }
    gpu_state_patch_defer();
    FUN_8018e310(0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_801a0220(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0220: B0 svc 0x32 CD queue op (indirect via $t2). */
    return (int32_t)bios_svc6(0xb0,0x32,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0230(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0230: B0 svc 0x33 CD queue op. */
    return bios_svc6(0xb0,0x33,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0240(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0240: B0 svc 0x34 CD queue op. */
    return bios_svc6(0xb0,0x34,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801a0250(uint32_t a0)
{
    /* guest 0x801A0250: B0 svc 0x36 trampoline. */
    (void)bios_svc6(0xb0,0x36,a0,0,0,0);
}

/* ---- MMIO-aware accessors (weak selftest fallbacks) ----
 * In the standalone arena every address is direct-mapped, so these reduce
 * to plain guest_at() derefs. The runtime build overrides them in
 * src/clean_bridge.c with psx_read / psx_write routing for MMIO. */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint8_t br2_ld8(uint32_t addr) { return *(uint8_t *)guest_at(addr); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint16_t br2_ld16(uint32_t addr) { return *(uint16_t *)guest_at(addr); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t br2_ld32(uint32_t addr) { return *(uint32_t *)guest_at(addr); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st8(uint32_t addr, uint8_t val) { *(uint8_t *)guest_at(addr) = val; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st16(uint32_t addr, uint16_t val) { *(uint16_t *)guest_at(addr) = val; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st32(uint32_t addr, uint32_t val) { *(uint32_t *)guest_at(addr) = val; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint8_t br2_ld8p(uint32_t cell) { return br2_ld8(*(uint32_t *)guest_at(cell)); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint16_t br2_ld16p(uint32_t cell) { return br2_ld16(*(uint32_t *)guest_at(cell)); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t br2_ld32p(uint32_t cell) { return br2_ld32(*(uint32_t *)guest_at(cell)); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st8p(uint32_t cell, uint8_t val) { br2_st8(*(uint32_t *)guest_at(cell), val); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st16p(uint32_t cell, uint16_t val) { br2_st16(*(uint32_t *)guest_at(cell), val); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st32p(uint32_t cell, uint32_t val) { br2_st32(*(uint32_t *)guest_at(cell), val); }
