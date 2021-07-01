#include "sdk/hardware.h"
#include "sdk/vram.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include <string.h>

ASSET(tiles, "tiles.2bpp");

void main()
{
    //Load our initial vram (this is slow, but always works, even outside of vblank)
    vram_memcpy((void*)0x8000, tiles, tiles_end - tiles);
    //Setup the OAM for sprite drawing
    oam_init();

    shadow_oam[0].y = 0x20;
    shadow_oam[0].x = 0x20;
    shadow_oam[0].tile = 0x04;
    shadow_oam[0].attr = 0x00;
    shadow_oam[1].y = 0x24;
    shadow_oam[1].x = 0x24;
    shadow_oam[1].tile = 0x02;
    shadow_oam[1].attr = 0x00;
    rBGP = 0b11100100;
    rOBP0 = 0b11100100;
    rOBP1 = 0b11100100;

    //Make sure sprites and the background are drawn
    rLCDC = LCDCF_ON | LCDCF_OBJON | LCDCF_BGON;

    //Setup the VBLANK interrupt, but we don't actually enable interrupt handling.
    // We only do this, so HALT waits for VBLANK.
    rIF = 0;
    rIE = IEF_VBLANK;

    while(1) {
        if (joypad_state & PAD_LEFT) shadow_oam[0].x --;
        if (joypad_state & PAD_RIGHT) shadow_oam[0].x ++;

        //Wait for VBLANK
        HALT();
        rIF = 0;    //As global interrupts are not enabled, we need to clear the interrupt flag.

        //Copy the sprites into OAM memory, and get the new joypad state.
        oam_dma_copy();
        joypad_update();
    }
}
