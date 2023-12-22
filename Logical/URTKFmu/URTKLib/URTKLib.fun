
FUNCTION_BLOCK URTKView
	VAR_INPUT
		Enable : BOOL;
		InstRef : REFERENCE TO URTKInternalType;
	END_VAR
	VAR_OUTPUT
		ObjPos : ARRAY[0..8] OF REAL;
	END_VAR
	VAR
		vInternal : ARRAY[0..8] OF LREAL;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK URTK (* *) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*) (*Fmu.Type="URTK",Fmu.ModelName="URTK_try",Fmu.GUID="{05105756-48c1-2b34-6e1c-9a6048640e26}",Fmu.LocalPath="C:\Users\User\Desktop\URTK_try.fmu",Fmu.Updated="2021-01-08 16:43:25"*)
	VAR_INPUT
		Enable : BOOL;
		Xinputvelocity : LREAL; (*Xinputvelocity*)
		Yimputvelocity : LREAL; (*Yimputvelocity*)
		Zimputevelocity : LREAL; (*Zimputevelocity*)
	END_VAR
	VAR_OUTPUT
		InstRef : UDINT;
		internal_axisY1 : LREAL; (*internal_axisY[1]*)
		internal_axisY2 : LREAL; (*internal_axisY[2]*)
		internal_axisY3 : LREAL; (*internal_axisY[3]*)
		internal_axisZ1 : LREAL; (*internal_axisZ[1]*)
		internal_axisZ2 : LREAL; (*internal_axisZ[2]*)
		internal_axisZ3 : LREAL; (*internal_axisZ[3]*)
		internal_movementX1 : LREAL; (*internal_movementX[1]*)
		internal_movementX2 : LREAL; (*internal_movementX[2]*)
		internal_movementX3 : LREAL; (*internal_movementX[3]*)
		internal_movementY1 : LREAL; (*internal_movementY[1]*)
		internal_movementY2 : LREAL; (*internal_movementY[2]*)
		internal_movementY3 : LREAL; (*internal_movementY[3]*)
		internal_movementZ1 : LREAL; (*internal_movementZ[1]*)
		internal_movementZ2 : LREAL; (*internal_movementZ[2]*)
		internal_movementZ3 : LREAL; (*internal_movementZ[3]*)
		internal_Ground1 : LREAL; (*internal_Ground[1]*)
		internal_Ground2 : LREAL; (*internal_Ground[2]*)
		internal_Ground3 : LREAL; (*internal_Ground[3]*)
	END_VAR
	VAR
		Internal : URTKInternalType;
	END_VAR
END_FUNCTION_BLOCK
