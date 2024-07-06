#include "./../shared/console.h"
#include "cpu.h"
#include "string.h"
#define VIDEO_MEMORY_BASE 0xB8000

uint8_t lig_cur = 0;
uint8_t col_cur = 0;

uint16_t *ptr_mem(uint32_t lig, uint32_t col){
    return (uint16_t*) (VIDEO_MEMORY_BASE + 2 * ( lig * 80 + col));
}

void ecrit_car(uint32_t lig, uint32_t col, char c){
    uint16_t *ptr = ptr_mem(lig, col);
    *ptr = ((0b00001111) << 8) + c;
}

void efface_ecran(void) {
    uint16_t* ptr = (uint16_t*) VIDEO_MEMORY_BASE;

    for (uint32_t i = 0;  i < 2000; i++) {
        *ptr = 0x20;
        ptr++;
    }
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t pos = col + 80 * lig;
    
    outb(0x0F, 0x3D4);
    outb((uint8_t) pos, 0x3D5);
    outb(0x0E, 0x3D4);
    outb((uint8_t) (pos / 256), 0x3D5);
}

void defilement() {
    memmove(ptr_mem(0, 0), ptr_mem(1, 0), 16 * 24 * 80);
    
    if (lig_cur > 0){
        lig_cur--;
    }
} 

void traite_car(char c){
    if (32 <= c && c <= 126){
        ecrit_car(lig_cur, col_cur, c);
        col_cur++;
    }
    else if (c == 8){
        if (col_cur > 0) col_cur--;
    }
    else if (c == 9){
        col_cur = ((col_cur + 16) / 16) * 16;
        if (col_cur > 79) col_cur = 79;
    }
    else if (c == 10){
        lig_cur++;
        col_cur = 0;
    }
    else if (c == 12){
        efface_ecran();
        lig_cur = 0;
        col_cur = 0;
    }
    else if (c == 13){
        col_cur = 0;
    }

    if (col_cur == 80){
        col_cur = 0;
        lig_cur++;
    }
    if (lig_cur == 25){
        defilement();
    }
}

void console_putbytes(const char *s, int len){
    for (int i = 0; i < len; i++){
        traite_car(*(s+i));
    }
}
