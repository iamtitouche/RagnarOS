#ifndef __PROCESS_H__
#define __PROCESS_H__

#define TAILLE_TABLE_PROCESSUS 5
#define TAILLE_PILE_EXEC 256

#include "./../shared/stdint.h"
#include "./../shared/queue.h"

enum Etat {
    ELU,
    ACTIVABLE,
    ENDORMI
};

struct processus {
    int pid;
    char nom[50];
    enum Etat etat;
    int contexte[5];
    int pile[TAILLE_PILE_EXEC];
    struct processus* next;
    link lien;
    int prio;
};

extern struct processus* proc_elu;

void init_processus(void);
void idle(void);

void ctx_sw(int* contexte_ancien, int* contexte_nouveau);

void ordonnance(void);

#endif
