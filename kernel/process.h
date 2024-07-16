#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "./../shared/stdint.h"
#include "./../shared/queue.h"
#include "cst_sys.h"

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
    int sec_reveil;
};

extern struct processus* proc_elu;

void init_processus(void);

void idle(void);

void ctx_sw(int* contexte_ancien, int* contexte_nouveau);

void ordonnance(void);

void dors(uint32_t nbr_secs);

#endif
