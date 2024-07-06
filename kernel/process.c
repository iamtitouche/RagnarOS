#include "process.h"
#include "./../shared/string.h"
#include "./../shared/debug.h"
#include "cpu.h"

struct processus* proc_elu; 
struct processus table_processus[TAILLE_TABLE_PROCESSUS];

struct processus processus0;
struct processus processus1;

char *mon_nom() {
    return proc_elu->nom;
}

int mon_pid() {
    return proc_elu->pid;
}

void idle(void)
{
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}

void proc1(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}


void init_processus(){
    processus0.pid = 0;
    strcpy(processus0.nom, "idle");
    processus0.etat = ELU;

    processus1.pid = 1;
    strcpy(processus1.nom, "proc1");
    processus1.etat = ACTIVABLE;

    table_processus[0] = processus0;
    table_processus[1] = processus1;
    proc_elu = &(table_processus[0]);

    // Initialisation de proc1 qui n'a encore jamais été exécuté 
    //int* addr_sommet_pile = &(processus1.pile[TAILLE_PILE_EXEC - 1]);
    //processus1.pile[TAILLE_PILE_EXEC - 1] = (int) proc1; 
    //processus1.contexte[1] = (int) addr_sommet_pile;

    int* addr_sommet_pile = &(table_processus[1].pile[TAILLE_PILE_EXEC - 1]);
    table_processus[1].pile[TAILLE_PILE_EXEC - 1] = (int) proc1; 
    table_processus[1].contexte[1] = (int) addr_sommet_pile;
}

void ordonnance(void) {
    int curr_pid = proc_elu->pid;
    struct processus *next_proc;

    if (curr_pid + 1 < TAILLE_TABLE_PROCESSUS) {
        next_proc = proc_elu + 1;
    } else {
        next_proc = &(table_processus[0]);
    }

    proc_elu->etat = ACTIVABLE;
    next_proc->etat = ELU;

    struct processus* tmp = proc_elu; 
    proc_elu = next_proc;

    ctx_sw(&(tmp->contexte[0]), &(next_proc->contexte[0]));
}

