#include "debugger.h"
#include "cpu.h"
#include "./../shared/debug.h"
#include "./../shared/console.h"


void kernel_start(void)
{
	efface_ecran();
	printf("\bHello world!");
	printf("\tpénissedelout\bre\t\t\t\t\t\t\t\t\t\t\t\tb\br");

	printf("\f");
	
	while(1)
	  hlt();

	return;
}
