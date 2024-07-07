#include "process.h"
#include "./../shared/string.h"
#include "./../shared/debug.h"
#include "cpu.h"
#include "./../shared/malloc.c"

struct processus* proc_elu; 
struct processus* table_processus[TAILLE_TABLE_PROCESSUS];
struct processus* head_activable;
struct processus* queue_activable;

struct processus processus0;
struct processus processus1;

char *mon_nom() {
    return proc_elu->nom;
}

int mon_pid() {
    return proc_elu->pid;
}

int32_t add_proc_activable(struct processus *proc) {
    if (proc->etat != ACTIVABLE) {
        return -1;
    }
    if (queue_activable) {
        queue_activable->next = proc;
    } else {
        head_activable = proc;
    }
    queue_activable = proc;
    return queue_activable->pid;
}

void rm_head_activable() {
    struct processus *tmp = head_activable;
    head_activable = head_activable->next;
    tmp->next = NULL;
}

int32_t cree_processus(void (*code)(void), char *nom) {
    if (!proc_elu) {
        table_processus[0] = malloc(sizeof(struct processus));
        proc_elu = table_processus[0];
        proc_elu->pid = 0;
        strcpy(proc_elu->nom, nom);
        proc_elu->etat = ELU;
        return proc_elu->pid;
    } else if (proc_elu && !head_activable) {
        table_processus[1] = malloc(sizeof(struct processus));
        head_activable = table_processus[1];
        queue_activable = table_processus[1];
        queue_activable->pid = 1;
    } else {
        int new_pid = queue_activable->pid + 1;

        if (new_pid >= TAILLE_TABLE_PROCESSUS) {
            return -1;
        }

        table_processus[new_pid] = malloc(sizeof(struct processus));
        queue_activable->next = table_processus[new_pid];
        queue_activable = table_processus[new_pid];
        queue_activable->pid = new_pid;
    }
    strcpy(queue_activable->nom, nom);
    queue_activable->etat = ACTIVABLE;

    queue_activable->pile[TAILLE_PILE_EXEC - 1] = (int) code;
    queue_activable->contexte[1] = (int) &(queue_activable->pile[TAILLE_PILE_EXEC - 1]);
    return queue_activable->pid;
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
    queue_activable = NULL;
    head_activable = NULL;
    cree_processus(idle, "idle large name");
    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");

    printf("TABLE : \n");

    for (int i = 0; i < 3; i++) {
        printf("name : %s\npid : %i\n", table_processus[i]->nom, table_processus[i]->pid);
    }
}

void ordonnance(void) {
    struct processus *next_proc;

    if (head_activable) {
        //défensif
        if (head_activable->etat != ACTIVABLE) {
            printf("Tête des activables non activable");
            return;
        }
        next_proc = head_activable;
        proc_elu->etat = ACTIVABLE;
        next_proc->etat = ELU;
        add_proc_activable(proc_elu);
        rm_head_activable();

        struct processus* tmp = proc_elu; 
        proc_elu = next_proc;

        ctx_sw(&(tmp->contexte[0]), &(next_proc->contexte[0]));
    } 
}

