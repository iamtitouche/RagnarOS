#ifndef ___HORLOGE_H___
#define ___HORLOGE_H___
#include "./../shared/stdint.h"
#include "./../shared/stdbool.h"
#include "cst_sys.h"


// extern int nb_min;
// extern int nb_heure;

void ecrit_temps(char* c, int length);

void tic_PIT(void);

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

void masque_IRQ(uint32_t num_IRQ, bool masque);

void traitant_IT_32(void);

void init_frequency(void);
#endif