#include "process.h"
#include "./../shared/string.h"
#include "./../shared/debug.h"
#include "cpu.h"
#include "./../shared/malloc.c"

struct processus* proc_elu; 
struct processus* table_processus[TAILLE_TABLE_PROCESSUS];
link tete_activable = LIST_HEAD_INIT(tete_activable);

struct processus processus0;
struct processus processus1;

char *mon_nom() {
    return proc_elu->nom;
}

int mon_pid() {
    return proc_elu->pid;
}

int32_t cree_processus(void (*code)(void), char *nom) {
    int new_pid;
    if (!proc_elu) {
        table_processus[0] = malloc(sizeof(struct processus));
        proc_elu = table_processus[0];
        proc_elu->pid = 0;
        strcpy(proc_elu->nom, nom);
        proc_elu->etat = ELU;
        return proc_elu->pid;
    } else if (proc_elu && queue_empty(&tete_activable)) {
        table_processus[1] = malloc(sizeof(struct processus));
        queue_add(table_processus[1], &tete_activable, struct processus, lien, prio);
        table_processus[1]->pid = 1;
        new_pid = 1;
    } else {
        struct processus* queue_activables = queue_bottom(&tete_activable, struct processus, lien);
        new_pid = queue_activables->pid + 1;

        if (new_pid >= TAILLE_TABLE_PROCESSUS) {
            return -1;
        }

        table_processus[new_pid] = malloc(sizeof(struct processus));
        queue_add(table_processus[new_pid], &tete_activable, struct processus, lien, prio);
        table_processus[new_pid]->pid = new_pid;
    }
    strcpy(table_processus[new_pid]->nom, nom);
    table_processus[new_pid]->etat = ACTIVABLE;

    table_processus[new_pid]->pile[TAILLE_PILE_EXEC - 1] = (int) code;
    table_processus[new_pid]->contexte[1] = (int) &(table_processus[new_pid]->pile[TAILLE_PILE_EXEC - 1]);
    return table_processus[new_pid]->pid;
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

void proc2(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        ordonnance();
    }
}


void init_processus(){
    cree_processus(idle, "idle large name");
    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");

    printf("TABLE : \n");

    for (int i = 0; i < 3; i++) {
        printf("name : %s\npid : %i\n", table_processus[i]->nom, table_processus[i]->pid);
    }
}

void ordonnance(void) {
    if (!queue_empty(&tete_activable)) {
        struct processus* tete = queue_out(&tete_activable, struct processus, lien);

        //défensif
        if (tete->etat != ACTIVABLE) {
            panic("Tête des activables non activable");
            return;
        }

        proc_elu->etat = ACTIVABLE;
        tete->etat = ELU;
        queue_add(proc_elu, &tete_activable, struct processus, lien, prio);

        struct processus* ancien_elu = proc_elu; 
        proc_elu = tete;

        ctx_sw(&(ancien_elu->contexte[0]), &(proc_elu->contexte[0]));
    } 
}

