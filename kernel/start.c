#include "debugger.h"
#include "cpu.h"
#include "./../shared/debug.h"
#include "./../shared/console.h"
#include "horloge.h"


void kernel_start(void)
{
	// initialisations
	efface_ecran();
	init_frequency();
	init_traitant_IT(32, traitant_IT_32);
	masque_IRQ(0, 0);

	// démasquage des interruptions externes
	sti();
	
	// boucle d’attente
	while (1) hlt();

	return;
}