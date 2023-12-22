
TYPE
	URTKLibFmi2CbType : 	STRUCT 
		Logger : UDINT;
		AllocateMemory : UDINT;
		FreeMemory : UDINT;
		StepFinished : UDINT;
		ComponentEnvironment : UDINT;
	END_STRUCT;
	URTKLibFmi2IfType : 	STRUCT 
		Enabled : BOOL;
		StartTime : LREAL;
		EndTime : LREAL;
		CurrentTime : LREAL;
		StepSize : LREAL;
		Error : DINT;
		Instance : UDINT;
		Callbacks : URTKLibFmi2CbType;
	END_STRUCT;
	URTKInternalType : 	STRUCT 
		Fmi2 : URTKLibFmi2IfType;
	END_STRUCT;
END_TYPE
