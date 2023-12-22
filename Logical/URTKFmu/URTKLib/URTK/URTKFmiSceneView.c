#include <bur/plctypes.h>
#include <sys_lib.h>
#include "URTKLib.h"

#include <fmi2Functions.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void URTKView(struct URTKView* fubInstance)
{
	if (fubInstance->Enable == 1) {
				
		if (fubInstance->InstRef->Fmi2.Enabled) {
			URTK_try_getMapleSimAuxiliaryData(fubInstance->InstRef->Fmi2.Instance, &(fubInstance->vInternal));
		}		
		
		
		int i;
		for (i=0; i <= 8; i++) {
			fubInstance->ObjPos[i] = (float)fubInstance->vInternal[i];
		}
		
		} else {

	}

}