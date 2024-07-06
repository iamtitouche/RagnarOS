#include "horloge.h"
#include "./../shared/console.h"
#include "./../shared/debug.h"
#include "cpu.h"
#include "segment.h"

int compteur = 0;
int nb_sec = 0;
int nb_min = 0;
int nb_heure = 0;

void ecrit_temps(char* c, int length) {
    int col = 79;
    for (int i = length - 1; i >= 0; i--){
        ecrit_car(0, col, *(c + i));
        col--;
    }
}

void init_frequency() {
    outb(0x34, 0x43);
    outb((uint8_t) (QUARTZ / CLOCKFREQ), 0x40);
    outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}

void tic_PIT(void){
    outb(0x20, 0x20);

    compteur++;
    if (compteur == 50){
        compteur = 0;
        nb_sec++;
    }

    if (nb_sec == 60){
        nb_min++;
        nb_sec = 0;
    }
    if (nb_min == 60){
        nb_heure++;
        nb_min = 0;
    }
    if (nb_heure == 100){
        nb_heure = 0;
    }

    char str[9];
    sprintf(str, "%02d:%02d:%02d", nb_heure, nb_min, nb_sec);

    ecrit_temps(str, 8);
}

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)){
    uint32_t *ptr = (uint32_t*) (ADDR_TABLE_INTERRUPTIONS + 8 * num_IT);

    uint32_t mot1 = ((((uint32_t) KERNEL_CS) << 16)) + ((((uint32_t) traitant) << 16) >> 16);
    uint32_t mot2 = ((((uint32_t) traitant) >> 16) << 16) + 0x8E00;

    *ptr = mot1;
    ptr++;
    *ptr = mot2;
}

void masque_IRQ(uint32_t num_IRQ, bool masque){
    uint8_t tab_masque = inb(0x21);

    if (!masque)
    {
        tab_masque &= (uint8_t) ~(1 << num_IRQ);
    }
    else
    {
        tab_masque |= (uint8_t)1 << num_IRQ;
    }
    
    outb(tab_masque, 0x21);
}