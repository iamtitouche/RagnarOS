#include "process.h"
#include "./../shared/string.h"
#include "./../shared/debug.h"
#include "cpu.h"
#include "./../shared/malloc.c"

struct processus* proc_elu; 
struct processus* table_processus[TAILLE_TABLE_PROCESSUS];
struct processus* queue_table;

struct processus processus0;
struct processus processus1;

char *mon_nom() {
    return proc_elu->nom;
}

int mon_pid() {
    return proc_elu->pid;
}

int32_t cree_processus(void (*code)(void), char *nom) {
    if (!queue_table) {
        table_processus[0] = malloc(sizeof(struct processus));
        queue_table = table_processus[0];
        queue_table->pid = 0;
        strcpy(queue_table->nom, nom);
        queue_table->etat = ELU;
        proc_elu = queue_table;
    } else {
        int new_pid = queue_table->pid + 1;

        if (new_pid >= TAILLE_TABLE_PROCESSUS) {
            return -1;
        }

        table_processus[new_pid] = malloc(sizeof(struct processus));
        queue_table->next = table_processus[new_pid];
        queue_table = table_processus[new_pid];
        queue_table->pid = new_pid;
        strcpy(queue_table->nom, nom);
        queue_table->etat = ACTIVABLE;

        queue_table->pile[TAILLE_PILE_EXEC - 1] = (int) code;
        queue_table->contexte[1] = (int) &(queue_table->pile[TAILLE_PILE_EXEC - 1]);
    }
    return queue_table->pid;
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
    queue_table = NULL;
    cree_processus(idle, "idle");
    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");

    printf("TABLE : \n");

    for (int i = 0; i < 3; i++) {
        printf("name : %s\npid : %i\n", table_processus[i]->nom, table_processus[i]->pid);
    }
}

void ordonnance(void) {
    struct processus *next_proc;

    if (proc_elu->next) {
        next_proc = proc_elu->next;
    } else {
        next_proc = table_processus[0];
    }

    proc_elu->etat = ACTIVABLE;
    next_proc->etat = ELU;

    struct processus* tmp = proc_elu; 
    proc_elu = next_proc;

    ctx_sw(&(tmp->contexte[0]), &(next_proc->contexte[0]));
}

