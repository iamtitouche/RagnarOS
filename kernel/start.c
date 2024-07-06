#include "debugger.h"
#include "cpu.h"
#include "./../shared/debug.h"
#include "./../shared/console.h"
#include "horloge.h"
#include "process.h"


void kernel_start(void)
{
	// initialisations
	efface_ecran();
	init_frequency();
	init_traitant_IT(32, traitant_IT_32);
	masque_IRQ(0, 0);

	// initialisations des structures de processus
	init_processus();

	// démasquage des interruptions externes
	//sti();

	// démarrage du processus par défaut
	idle();

	return;
}