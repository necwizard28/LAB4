/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************

 ********************************************************************
 * Basic fmu/fub implementation
 ********************************************************************/


#include <bur/plctypes.h>
#include <sys_lib.h>
#include <fmi2Functions.h>

#ifdef __cplusplus
	extern "C"
	{
#endif
#include "URTKLib.h"
#ifdef __cplusplus
	};
#endif

/**********************************************************************/
/* FUB interface declarations */
#define  FMU_Enable 			fubInstance->Enable

#define  FMU_Instance 			fubInstance->Internal.Fmi2.Instance
#define  FMU_Enabled 			fubInstance->Internal.Fmi2.Enabled
#define  FMU_Error 			fubInstance->Internal.Fmi2.Error
#define  FMU_EndTime    		fubInstance->Internal.Fmi2.EndTime
#define  FMU_StartTime    		fubInstance->Internal.Fmi2.StartTime
#define  FMU_CurrentTime   		fubInstance->Internal.Fmi2.CurrentTime
#define  FMU_StepSize    		fubInstance->Internal.Fmi2.StepSize
#define  FMU_Callbacks   		fubInstance->Internal.Fmi2.Callbacks


/**********************************************************************/
/* Method delcarations */
void URTK_try_fmuLogger(fmi2Component c, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...);
void* URTK_try_AllocateMemory(UDINT numElements, UDINT elementSize);
void URTK_try_FreeMemory(void* pMemory);


void URTK_try_initFMU(struct URTK* fubInstance);

void URTK_try_initFMUCallbacks(struct URTK* fubInstance);
void URTK_try_initFMUCoSimParameters(struct URTK* fubInstance);

void URTK_try_stepFMU(struct URTK* fubInstance);
void URTK_try_resetFMU (struct URTK* fubInstance);

void URTK_try_copyOutputsFromFmu(struct URTK* fubInstance);
void URTK_try_copyInputsToFmu(struct URTK* fubInstance);
void URTK_try_clearFmuOutputs(struct URTK* fubInstance);


/**********************************************************************/
/* FUB implementation */
void URTK(struct URTK* fubInstance)
{
	URTK_try_ProcessFmuState(fubInstance);	
}

/* Processes the fmu states */
void  URTK_try_ProcessFmuState(struct URTK* fubInstance)
{
	if (FMU_Enable)
	{
		if (!FMU_Enabled)
		{
			URTK_try_initFMU(fubInstance);
			FMU_Enabled = fmi2True;
		}

		if(FMU_Error == 0)
		{
			URTK_try_stepFMU(fubInstance);
		}			
	}else
	{
		if (FMU_Enabled)
		{
			URTK_try_resetFMU(fubInstance);
			FMU_Enabled = fmi2False;
		}		
	}
	fubInstance->InstRef = &(fubInstance->Internal);
}

/* Initialises the FMU */
void URTK_try_initFMU(struct URTK* fubInstance)
{	
	fmi2Status retStatus = fmi2OK;
	FMU_Error = 0;
	
	URTK_try_initFMUCoSimParameters(fubInstance);
	
	URTK_try_initFMUCallbacks(fubInstance);

	FMU_Instance = URTK_try_fmi2Instantiate("URTK_try",fmi2CoSimulation,"{05105756-48c1-2b34-6e1c-9a6048640e26}",NULL,&FMU_Callbacks,fmi2False,fmi2False);
	
	if (!FMU_Instance)
	{
		FMU_Error = fmi2True;
	}else
	{
		retStatus = URTK_try_fmi2SetupExperiment(FMU_Instance,0 ,0.0, FMU_StartTime, 0, FMU_EndTime);

		if (retStatus > fmi2Warning) 
		{
			FMU_Error = fmi2True;
			return;
		}

		retStatus = URTK_try_fmi2EnterInitializationMode(FMU_Instance);
		if (retStatus > fmi2Warning) 
		{
			FMU_Error = fmi2True;
			return;
		}

		retStatus = URTK_try_fmi2ExitInitializationMode(FMU_Instance);
		if (retStatus > fmi2Warning) 
		{
			FMU_Error = fmi2True;
			return;
		}
		
		URTK_try_copyOutputsFromFmu(fubInstance);
	}	
}

/* Initialize CoSimulation parmeters */
void URTK_try_initFMUCoSimParameters(struct URTK* fubInstance)
{
	FMU_StartTime = 0.00000000000000000e+00;
	FMU_EndTime = 0.0;
	FMU_StepSize = 1.00000000000000000e-03;
	
	FMU_CurrentTime = 0.0;	
}

/* Initialize FMU callbacks */
void  URTK_try_initFMUCallbacks(struct URTK* fubInstance)
{
	FMU_Callbacks.Logger = URTK_try_fmuLogger;
	FMU_Callbacks.AllocateMemory = URTK_try_AllocateMemory;
	FMU_Callbacks.FreeMemory = URTK_try_FreeMemory;	
	FMU_Callbacks.StepFinished = NULL;
	FMU_Callbacks.ComponentEnvironment = NULL;
}

/* Execute FMU steps */
void URTK_try_stepFMU(struct URTK* fubInstance)
{
	
	if (FMU_Error == 0)
	{		
		URTK_try_copyInputsToFmu(fubInstance);
		
		fmi2Status retStatus = URTK_try_fmi2DoStep(FMU_Instance, FMU_CurrentTime, FMU_StepSize, fmi2True);

		if(retStatus <= fmi2Warning)
		{
			URTK_try_copyOutputsFromFmu(fubInstance);
		
			FMU_CurrentTime += FMU_StepSize;
		}else
		{
			FMU_Error = fmi2True;
		}	
	}
}


/* Reset the FMU */
void URTK_try_resetFMU (struct URTK* fubInstance)
{
	fmi2Status retStatus = URTK_try_fmi2Reset(FMU_Instance);

	if(retStatus <= fmi2Warning)
	{
		URTK_try_clearFmuOutputs(fubInstance);
		FMU_CurrentTime = 0;
	}else
	{
		FMU_Error = fmi2True;
	}
}

/* Log FMU calls */
void URTK_try_fmuLogger(void *componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...) 
{
	
}

/* Allocate memory */
void* URTK_try_AllocateMemory(UDINT numElements, UDINT elementSize)
{
	void* address;
	UDINT status = 0;
	
	status = TMP_alloc(numElements * elementSize, &address);
	
	
	if (status == 0)
	{
		return address;
	}
	else
	{
		return NULL;
	}
}

/* Free allocated memory */
void URTK_try_FreeMemory(void* pMemory)
{
	free(pMemory);
}

/* Clear the output values of the FMU */
void URTK_try_clearFmuOutputs(struct URTK* fubInstance)
{
	/*Clear fmu output internal_axisY1*/
	fubInstance->internal_axisY1 = 0.0;
	/*Clear fmu output internal_axisY2*/
	fubInstance->internal_axisY2 = 0.0;
	/*Clear fmu output internal_axisY3*/
	fubInstance->internal_axisY3 = 0.0;
	/*Clear fmu output internal_axisZ1*/
	fubInstance->internal_axisZ1 = 0.0;
	/*Clear fmu output internal_axisZ2*/
	fubInstance->internal_axisZ2 = 0.0;
	/*Clear fmu output internal_axisZ3*/
	fubInstance->internal_axisZ3 = 0.0;
	/*Clear fmu output internal_movementX1*/
	fubInstance->internal_movementX1 = 0.0;
	/*Clear fmu output internal_movementX2*/
	fubInstance->internal_movementX2 = 0.0;
	/*Clear fmu output internal_movementX3*/
	fubInstance->internal_movementX3 = 0.0;
	/*Clear fmu output internal_movementY1*/
	fubInstance->internal_movementY1 = 0.0;
	/*Clear fmu output internal_movementY2*/
	fubInstance->internal_movementY2 = 0.0;
	/*Clear fmu output internal_movementY3*/
	fubInstance->internal_movementY3 = 0.0;
	/*Clear fmu output internal_movementZ1*/
	fubInstance->internal_movementZ1 = 0.0;
	/*Clear fmu output internal_movementZ2*/
	fubInstance->internal_movementZ2 = 0.0;
	/*Clear fmu output internal_movementZ3*/
	fubInstance->internal_movementZ3 = 0.0;
	/*Clear fmu output internal_Ground1*/
	fubInstance->internal_Ground1 = 0.0;
	/*Clear fmu output internal_Ground2*/
	fubInstance->internal_Ground2 = 0.0;
	/*Clear fmu output internal_Ground3*/
	fubInstance->internal_Ground3 = 0.0;
	
}


/* Copy output values from the FMU */
void URTK_try_copyOutputsFromFmu(struct URTK* fubInstance)
{
	fmi2ValueReference valueReference;
	fmi2Status retStatus = fmi2OK;

	/*Copy fmu output internal_axisY1 to internal_axisY1 */
	valueReference = 16;
	fmi2Real internal_axisY1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisY1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisY1 = internal_axisY1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_axisY2 to internal_axisY2 */
	valueReference = 17;
	fmi2Real internal_axisY2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisY2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisY2 = internal_axisY2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_axisY3 to internal_axisY3 */
	valueReference = 18;
	fmi2Real internal_axisY3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisY3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisY3 = internal_axisY3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_axisZ1 to internal_axisZ1 */
	valueReference = 19;
	fmi2Real internal_axisZ1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisZ1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisZ1 = internal_axisZ1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_axisZ2 to internal_axisZ2 */
	valueReference = 20;
	fmi2Real internal_axisZ2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisZ2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisZ2 = internal_axisZ2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_axisZ3 to internal_axisZ3 */
	valueReference = 21;
	fmi2Real internal_axisZ3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_axisZ3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_axisZ3 = internal_axisZ3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementX1 to internal_movementX1 */
	valueReference = 22;
	fmi2Real internal_movementX1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementX1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementX1 = internal_movementX1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementX2 to internal_movementX2 */
	valueReference = 23;
	fmi2Real internal_movementX2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementX2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementX2 = internal_movementX2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementX3 to internal_movementX3 */
	valueReference = 24;
	fmi2Real internal_movementX3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementX3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementX3 = internal_movementX3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementY1 to internal_movementY1 */
	valueReference = 25;
	fmi2Real internal_movementY1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementY1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementY1 = internal_movementY1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementY2 to internal_movementY2 */
	valueReference = 26;
	fmi2Real internal_movementY2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementY2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementY2 = internal_movementY2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementY3 to internal_movementY3 */
	valueReference = 27;
	fmi2Real internal_movementY3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementY3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementY3 = internal_movementY3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementZ1 to internal_movementZ1 */
	valueReference = 28;
	fmi2Real internal_movementZ1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementZ1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementZ1 = internal_movementZ1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementZ2 to internal_movementZ2 */
	valueReference = 29;
	fmi2Real internal_movementZ2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementZ2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementZ2 = internal_movementZ2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_movementZ3 to internal_movementZ3 */
	valueReference = 30;
	fmi2Real internal_movementZ3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_movementZ3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_movementZ3 = internal_movementZ3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_Ground1 to internal_Ground1 */
	valueReference = 13;
	fmi2Real internal_Ground1;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_Ground1);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_Ground1 = internal_Ground1;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_Ground2 to internal_Ground2 */
	valueReference = 14;
	fmi2Real internal_Ground2;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_Ground2);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_Ground2 = internal_Ground2;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu output internal_Ground3 to internal_Ground3 */
	valueReference = 15;
	fmi2Real internal_Ground3;
	retStatus = URTK_try_fmi2GetReal(FMU_Instance, &valueReference, 1, &internal_Ground3);

	if(retStatus <= fmi2Warning)
	{
		fubInstance->internal_Ground3 = internal_Ground3;
	}else
	{
		FMU_Error = fmi2True;
		return;
	}	
}

/* Copy input values to the FMU */
void URTK_try_copyInputsToFmu(struct URTK* fubInstance)
{
	fmi2ValueReference valueReference;
	fmi2Status retStatus = fmi2OK;

	/*Copy fmu input Xinputvelocity to Xinputvelocity */
	valueReference = 31;
	fmi2Real Xinputvelocity;
	Xinputvelocity = fubInstance->Xinputvelocity;
	retStatus = URTK_try_fmi2SetReal(FMU_Instance, &valueReference, 1, &Xinputvelocity);
	if(retStatus > fmi2Warning)
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu input Yimputvelocity to Yimputvelocity */
	valueReference = 32;
	fmi2Real Yimputvelocity;
	Yimputvelocity = fubInstance->Yimputvelocity;
	retStatus = URTK_try_fmi2SetReal(FMU_Instance, &valueReference, 1, &Yimputvelocity);
	if(retStatus > fmi2Warning)
	{
		FMU_Error = fmi2True;
		return;
	}
	/*Copy fmu input Zimputevelocity to Zimputevelocity */
	valueReference = 33;
	fmi2Real Zimputevelocity;
	Zimputevelocity = fubInstance->Zimputevelocity;
	retStatus = URTK_try_fmi2SetReal(FMU_Instance, &valueReference, 1, &Zimputevelocity);
	if(retStatus > fmi2Warning)
	{
		FMU_Error = fmi2True;
		return;
	}	
}



