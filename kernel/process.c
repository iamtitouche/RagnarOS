#include "process.h"
#include "./../shared/string.h"
#include "./../shared/debug.h"
#include "cpu.h"
#include "./../shared/malloc.c"
#include "horloge.h"

struct processus* proc_elu; 
struct processus* table_processus[TAILLE_TABLE_PROCESSUS];
link tete_activable = LIST_HEAD_INIT(tete_activable);
link tete_endormis = LIST_HEAD_INIT(tete_endormis);

char *mon_nom() {
    return proc_elu->nom;
}

int mon_pid() {
    return proc_elu->pid;
}

int find_new_pid(){
    int new_pid = 0;
    
    for (; new_pid < TAILLE_TABLE_PROCESSUS; new_pid++) {
        if (table_processus[new_pid] == NULL) {
            return new_pid;
        }
    }
        
    return -1;
}

int32_t cree_processus(void (*code)(void), char *nom) {
    if (!proc_elu) {
        table_processus[0] = malloc(sizeof(struct processus));
        proc_elu = table_processus[0];
        proc_elu->pid = 0;
        strcpy(proc_elu->nom, nom);
        proc_elu->etat = ELU;
        return proc_elu->pid;
    }

    int new_pid = find_new_pid();
    if (new_pid == -1) {
        panic("Plus de pid disponibles");
        return -1;
    }
    
    struct processus* new_proc = malloc(sizeof(struct processus));

    table_processus[new_pid] = new_proc;
    queue_add(new_proc, &tete_activable, struct processus, lien, prio);
    new_proc->pid = new_pid;
    strcpy(new_proc->nom, nom);
    new_proc->etat = ACTIVABLE;
    new_proc->sec_reveil = -1;
    new_proc->pile[TAILLE_PILE_EXEC - 1] = (int) code;
    new_proc->contexte[1] = (int) &(new_proc->pile[TAILLE_PILE_EXEC - 1]);
    return new_proc->pid;
}

void idle() {
    for (;;) {
        sti();
        hlt();
        cli();
    }
}

void proc1(void) {
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nb_sec,
        mon_nom(), mon_pid());
        dors(2);
    }
}

void proc2(void) {
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nb_sec,
        mon_nom(), mon_pid());
        dors(3);
    }
}

void proc3(void) {
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nb_sec,
        mon_nom(), mon_pid());
        dors(5);
    }
}

void init_processus(){
    for (int i = 0; i < TAILLE_TABLE_PROCESSUS; i++) {
        table_processus[i] = NULL;
    }
    
    cree_processus(idle, "idle large name");
    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");
    cree_processus(proc3, "proc3");

    printf("TABLE : \n");

    for (int i = 0; i < 3; i++) {
        printf("name : %s\npid : %i\n\n\n", table_processus[i]->nom, table_processus[i]->pid);
    }
}

void reveil_processus() {
    struct processus* ptr_proc;
    int nb_proc_a_reveiller = 0;
    //printf("penis\n");

    if(queue_empty(&tete_endormis) || ((struct processus*) (queue_bottom(&tete_endormis, struct processus, lien)))->sec_reveil > nb_sec) return;
    
    // Parcours de la liste de l'élément le moins prioritaire (queue_bottom, la tête de liste) vers le plus prioritaire 
    // (queue_top, queue_out, la queue de liste)
    queue_for_each(ptr_proc, &tete_endormis, struct processus, lien){
        if(ptr_proc->sec_reveil > nb_sec) break;

        nb_proc_a_reveiller++;

        ptr_proc->etat = ACTIVABLE;
        ptr_proc->sec_reveil = -1;
    }
    
    for (int i = 0; i < nb_proc_a_reveiller; i++){
        struct processus* proc_a_reveiller = queue_bottom(&tete_endormis, struct processus, lien);
        queue_del(proc_a_reveiller, lien);

        queue_add(proc_a_reveiller, &tete_activable, struct processus, lien, prio);
    }
}

void ordonnance(void) {
    reveil_processus();
    if (!queue_empty(&tete_activable)) {
        if (proc_elu->etat == ENDORMI) {
            queue_add(proc_elu, &tete_endormis, struct processus, lien, sec_reveil);
        } else {
            proc_elu->etat = ACTIVABLE;
            queue_add(proc_elu, &tete_activable, struct processus, lien, prio);
        }
        struct processus* tete = queue_out(&tete_activable, struct processus, lien);

        //défensif
        if (tete->etat != ACTIVABLE) {
            panic("Tête des activables non activable");
            return;
        }

        tete->etat = ELU;

        struct processus* ancien_elu = proc_elu; 
        proc_elu = tete;

        ctx_sw(&(ancien_elu->contexte[0]), &(proc_elu->contexte[0]));
    } 
}

void dors(uint32_t nbr_secs) {
    if (proc_elu->pid == 0) {
        panic("ç(majuscule)a va pas fait trop chaud");
        return;
    }
    proc_elu->etat = ENDORMI;
    proc_elu->sec_reveil = nb_sec + nbr_secs;

    ordonnance();
}

// 0 1 2 3 0 1 2 1 3 1 2 1 2 1 3 1 2

// 1: 2 4 6 8 10 12
// 2: 3 6 9 12
// 3: 5 10 