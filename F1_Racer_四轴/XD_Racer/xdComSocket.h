//Defines the exported functions for the DLL application.
//

#ifndef XDCOMSOCKET_H
#define XDCOMSOCKET_H 1



extern "C"
{

	extern BOOL isInitialized;

	__declspec(dllimport) int Clear_All_Axis();

	__declspec(dllimport) int cgIsInitialized();


	//__declspec(dllimport) int cgZeroReturn();
	__declspec(dllimport) int cgZeroReturn(int Port_Cpu_Num);

	//__declspec(dllimport) int cgCloseServoSystem();
	__declspec(dllimport) int cgCloseServoSystem(int Port_Cpu_Num);

	//__declspec(dllimport) int cgOpenServoSystem();
	__declspec(dllimport) int cgOpenServoSystem(int Port_Cpu_Num,int Axis_Num_In_Port);

	//__declspec(dllimport) int cgPositioning(int Axis_1_Data, int Axis_2_Data,int Acc_Data);
	//__declspec(dllimport) int cgPositioning(int Port_Cpu_Num, int Axis_0_Data, int Axis_1_Data, int Acc_Data,int iSeat_Belt_States);
	__declspec(dllexport) int cgPositioning(int Port_Cpu_Num, int Axis_0_Data, int Axis_1_Data, int Acc_Data,int Vel_Data_Input,int iSeat_Belt_States);
	//__declspec(dllimport) int jzPositioning(int Port_Cpu_Num, int Axis_0_Data, int Axis_1_Data, int Acc_Data,int Vel_Data_Input,int iSeat_Belt_States);

	//__declspec(dllimport) int cgGetRegester(void);
	__declspec(dllimport) int cgGetBeltRegester(int Port_Cpu_Num,int Axis_Num_In_Port);

	__declspec(dllimport) int OpenSystem(int *pError_Code_Array,int Start_Port,int End_Port);
	
	__declspec(dllimport) int CloseSystem(int *pError_Code_Array,int Start_Port,int End_Port);
	
	__declspec(dllimport) int cgGetSetRegester(int Port_Cpu_Num,WORD Optn_Type,WORD Para_Type,DWORD ParaNum,DWORD NorBum);
	
	__declspec(dllimport) int cgClearAlarm(int Start_Port,int End_Port);

	__declspec(dllexport) int CloseAllServo(int CtlNum);

}

#endif