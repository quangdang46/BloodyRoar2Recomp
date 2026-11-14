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

static uint8_t  guest_arena[0x200000];
static uint8_t  guest_scratch[0x10000];
void *guest_at(uint32_t addr);
uint32_t guest_of(const void *p);
static uint8_t *guest_for(const char *name)
{
    char buf[64]; int n=0; unsigned long best=0; int found=0;
    for (const char *c=name; *c; c++) {
        if ((*c>='0'&&*c<='9')||(*c>='a'&&*c<='f')) { if(n<63)buf[n++]=*c; }
        else { if(n>=6){ buf[n]=0; char *e; unsigned long v=strtoul(buf,&e,16); if(v>best)best=v; found=1;} n=0; }
    }
    if(n>=6){ buf[n]=0; char *e; unsigned long v=strtoul(buf,&e,16); if(v>best)best=v; found=1; }
    if(!found) return guest_arena;
    return (uint8_t *)guest_at((uint32_t)best);
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
void action_select(uint8_t *ctrl,int idx){ (void)ctrl;(void)idx; }
void reaction_load(uint8_t *ctrl,uint32_t e){ (void)ctrl;(void)e; }
void trace_op(int op){ (void)op; }
static uint32_t cop_regs[32];
static uint32_t cop_cw[16];
void setCopReg(int f,int r,uint32_t v){ if(r<32)cop_regs[r]=v; }
int  getCopReg(int f,int r){ return r<32?(int)cop_regs[r]:0; }
void copFunction(int f,uint32_t op){ (void)f;(void)op; }
void setCopControlWord(int f,int r,uint32_t v){ if(r<16)cop_cw[r]=v; }
int  getCopControlWord(int f,int r){ return r<16?(int)cop_cw[r]:0; }



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
extern void br2_guest_frame_free(void);
void br2_st32(uint32_t a, uint32_t v);
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

/* COP2 register-ABI GTE functions — intentionally runtime-bound.
 * Weak so the recomp runtime's COP2 implementation overrides them. */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801851d8(void) { br2_call_generated(0x801851d8u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194500(uint32_t a0) { return (int32_t)br2_call_generated(0x80194500u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194630(const void *in, void *out)
{
    /* guest 0x80194630: lw t0-t2 = in[0..2] (3 words), jal 0x80194690
     * (GTE normalize), sw t0-t2 -> out[0..2], $v0 = result.
     * Callers pass host pointers; stage through a real guest frame. */
    uint32_t fr = br2_guest_frame(0x40);
    uint32_t ib = fr + 0x10, ob = fr + 0x20;
    int32_t r;
    memcpy(guest_at(ib), in, 12);
    r = (int32_t)br2_call_generated(0x80194630u, ib, ob, 0, 0);
    memcpy(out, guest_at(ob), 12);
    br2_guest_frame_free();
    return r;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194660(const void *in, void *out)
{
    /* guest 0x80194660: lhu t0-t2 = in[0..2] (3 s16), falls into
     * 0x8019466C -> jal 0x80194690, sh t0-t2 -> out[0..2]. */
    uint32_t fr = br2_guest_frame(0x40);
    uint32_t ib = fr + 0x10, ob = fr + 0x20;
    int32_t r;
    memcpy(guest_at(ib), in, 6);
    r = (int32_t)br2_call_generated(0x80194660u, ib, ob, 0, 0);
    memcpy(out, guest_at(ob), 6);
    br2_guest_frame_free();
    return r;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_80194690(void) { return (int32_t)br2_call_generated(0x80194690u,0,0,0,0); }

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
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_80194478(void) { br2_call_generated(0x80194478u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_80195290(int32_t a0,int32_t a1)
{
    br2_call_generated(0x80195290u,(uint32_t)a0,(uint32_t)a1,0,0);
}

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

/* menu.c callees — weak fallbacks that delegate to the generated
 * guest bodies. Args 5+ travel on the guest stack (o32), staged via
 * br2_guest_frame; pointer args arrive as host pointers and are
 * converted with guest_of. */
int FUN_801468ec(uint32_t a0,uint32_t a1,uint32_t a2,const void *a3,int32_t a4)
{
    uint32_t fr = br2_guest_frame(0x20);
    int r;
    br2_st32(fr + 0x10, (uint32_t)a4);
    r = (int)br2_call_generated(0x801468ecu,a0,a1,a2,guest_of(a3));
    br2_guest_frame_free();
    return r;
}
int FUN_8018b100(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,
                 uint32_t a4,uint32_t a5)
{
    uint32_t fr = br2_guest_frame(0x20);
    int r;
    br2_st32(fr + 0x10, a4);
    br2_st32(fr + 0x14, a5);
    r = (int)br2_call_generated(0x8018b100u,a0,a1,a2,a3);
    br2_guest_frame_free();
    return r;
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

/* printf rodata: hex-digit strings + guest ctype bitmap @0x801b6701
 * (extracted verbatim from the payload image). */
char s_0123456789abcdef_80124fe8[] = "0123456789abcdef";
char s_0123456789ABCDEF_80125004[] = "0123456789ABCDEF";
char s__null__80124ffc[] = "(null)";
uint8_t DAT_801b6701[256] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 8, 8, 8, 8, 8, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 24, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16, 16, 16, 16, 65, 65, 65, 65, 65, 65, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 16, 16, 16, 16, 16, 66, 66, 66, 66, 66, 66, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 64, 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26, 24, 26, 27, 27, 27, 26, 26, 26, 26, 27, 27, 27, 29, 29, 29, 34, 34, 34, 29, 29, 29, 27, 27, 29, 29, 32, 32, 34, 34, 37, 38, 37, 35, 35, 34, 35, 38, 38, 40, 40, 40, 48, 48, 46, 46, 56, 56, 58, 69, 69, 83, 2, 16, 16, 19, 16, 19, 22, 22, 22, 22, 22, 22, 26};
/* bios_tramp_b035 — guest 0x8018EF50: BIOS B035 debug-output
 * trampoline fronting the line-buffer flush. a1 arrives as a host
 * pointer (callers pass guest_at(...)); convert back to a guest
 * address for the generated body. */
__attribute__((weak)) void bios_tramp_b035(uint32_t a0, const void *a1, uint32_t a2)
{
    br2_call_generated(0x8018ef50u, a0, guest_of(a1), a2, 0);
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
void FUN_8018e300(void) { br2_call_generated(0x8018e300u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8018e310(uint32_t a0) { br2_call_generated(0x8018e310u,a0,0,0,0); }
__attribute__((weak))
uint32_t bios_svc(uint32_t table, uint32_t svc, uint32_t a0, uint32_t a1)
{
    (void)svc; (void)a1;
    return bios_call(table, a0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_8019b380(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
    /* guest 0x8019B380: B0 svc 0x08 (kernel object alloc). */
    return br2_call_generated(0x8019b380u,a0,a1,a2,a3);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019b390(uint32_t a0) { br2_call_generated(0x8019b390u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void cd_mode_apply_wrap(void) { br2_call_generated(0x8019e410u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void cd_mode_apply(uint32_t a0) { br2_call_generated(0x8019e400u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e5cc(void) { br2_call_generated(0x8019e5ccu,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e4c8(void) { br2_call_generated(0x8019e4c8u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e55c(void) { br2_call_generated(0x8019e55cu,0,0,0,0); }

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t cd_seek_state_machine() { return 0; }

/* ---- menu-agent callees (unported, weak until disasm-verified) ---- */

/* ---- save-agent callees (unported, weak until disasm-verified) ---- */

/* ---- round-2 unported callees (weak until disasm-verified) ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0260(const void *a0)
{
    /* guest 0x801A0260: B0 svc 0x43 CD-record read trampoline.
     * Callers pass a host pointer (clean_bridge guest_of's it). */
    return br2_call_generated(0x801a0260u, guest_of(a0), 0, 0, 0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801a05f0(uint32_t a0) { br2_call_generated(0x801a05f0u,a0,0,0,0); }

/* ---- round-3 unported callees ---- */

/* ---- cd-batch32 callees ---- */

/* ---- cd 99440 callees ---- */

/* ---- menu batch32 callees ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int ret_zero_e() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_801948c0(uint32_t a0) { return (int32_t)br2_call_generated(0x801948c0u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80195370(uint32_t *in, uint32_t *out)
{
    /* guest 0x80195370: lwc2 gte[9..11] <- a0[0..2], gte cmd 0x28,
     * swc2 gte[25..27] -> a1[0..2]. Callers pass host pointers;
     * stage through a real guest frame (in/out may alias). */
    uint32_t fr = br2_guest_frame(0x20);
    int32_t r;
    memcpy(guest_at(fr), in, 12);
    r = (int32_t)br2_call_generated(0x80195370u, fr, fr, 0, 0);
    memcpy(out, guest_at(fr), 12);
    br2_guest_frame_free();
    return r;
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a0ddc(void) { br2_call_generated(0x801a0ddcu,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a2f0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
    /* guest 0x8019A2F0: B0 svc 0x12 CD channel init. */
    br2_call_generated(0x8019a2f0u,a0,a1,a2,a3);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a300(void) { br2_call_generated(0x8019a300u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a310(void) { br2_call_generated(0x8019a310u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a350(void) { br2_call_generated(0x8019a350u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019a364(void) { br2_call_generated(0x8019a364u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad48(uint32_t a0) { br2_call_generated(0x8019ad48u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad70(uint32_t a0) { br2_call_generated(0x8019ad70u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019ad80(uint32_t a0) { br2_call_generated(0x8019ad80u,a0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e420(void) { br2_call_generated(0x8019e420u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_8019e600(void) { br2_call_generated(0x8019e600u,0,0,0,0); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int32_t FUN_801a0220(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0220: B0 svc 0x32 CD queue op (indirect via $t2). */
    return (int32_t)br2_call_generated(0x801a0220u,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0230(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0230: B0 svc 0x33 CD queue op. */
    return br2_call_generated(0x801a0230u,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t FUN_801a0240(uint32_t a0,uint32_t a1,uint32_t a2)
{
    /* guest 0x801A0240: B0 svc 0x34 CD queue op. */
    return br2_call_generated(0x801a0240u,a0,a1,a2,0);
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void FUN_801a0250(uint32_t a0) { br2_call_generated(0x801a0250u,a0,0,0,0); }

/* ---- MMIO-aware accessors (weak selftest fallbacks) ----
 * In the standalone arena every address is direct-mapped, so these reduce
 * to plain guest_at() derefs. The runtime build overrides them in
 * src/clean_bridge.c with psx_read / psx_write routing for MMIO. */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint8_t br2_ld8(uint32_t a) { return *(uint8_t *)guest_at(a); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint16_t br2_ld16(uint32_t a) { return *(uint16_t *)guest_at(a); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
uint32_t br2_ld32(uint32_t a) { return *(uint32_t *)guest_at(a); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st8(uint32_t a, uint8_t v) { *(uint8_t *)guest_at(a) = v; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st16(uint32_t a, uint16_t v) { *(uint16_t *)guest_at(a) = v; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st32(uint32_t a, uint32_t v) { *(uint32_t *)guest_at(a) = v; }
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
void br2_st8p(uint32_t cell, uint8_t v) { br2_st8(*(uint32_t *)guest_at(cell), v); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st16p(uint32_t cell, uint16_t v) { br2_st16(*(uint32_t *)guest_at(cell), v); }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void br2_st32p(uint32_t cell, uint32_t v) { br2_st32(*(uint32_t *)guest_at(cell), v); }
