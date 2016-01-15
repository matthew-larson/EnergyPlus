// C++ Headers
#include <cmath>

// ObjexxFCL Headers
#include <ObjexxFCL/Array.functions.hh>
#include <ObjexxFCL/Fmath.hh>

// EnergyPlus Headers
#include <IntegratedHeatPump.hh>
#include <BranchNodeConnections.hh>
#include <CurveManager.hh>
#include <DataAirSystems.hh>
#include <DataContaminantBalance.hh>
#include <DataEnvironment.hh>
#include <DataLoopNode.hh>
#include <DataPlant.hh>
#include <DataPrecisionGlobals.hh>
#include <DataWater.hh>
#include <FluidProperties.hh>
#include <General.hh>
#include <GeneralRoutines.hh>
#include <GlobalNames.hh>
#include <InputProcessor.hh>
#include <NodeInputManager.hh>
#include <OutAirNodeManager.hh>
#include <OutputProcessor.hh>
#include <OutputReportPredefined.hh>
#include <PlantUtilities.hh>
#include <Psychrometrics.hh>
#include <ReportSizingManager.hh>
#include <ScheduleManager.hh>
#include <UtilityRoutines.hh>
#include <WaterManager.hh>
#include <WaterThermalTanks.hh>
#include <InputProcessor.hh>

namespace EnergyPlus {

	namespace IntegratedHeatPumps {

		// USE STATEMENTS:
		// Use statements for data only modules
		// Using/Aliasing
		using namespace DataPrecisionGlobals;
		using namespace DataLoopNode;
		using namespace DataGlobals;
		using General::RoundSigDigits;

		// Use statements for access to subroutines in other modules

		// Data
		//MODULE PARAMETER DEFINITIONS

		static std::string const BlankString;

		// DERIVED TYPE DEFINITIONS

		// MODULE VARIABLE DECLARATIONS:
		// Identifier is VarSpeedCoil
		int NumIHPs(0); // The Number of Water to Air Heat Pumps found in the Input
		bool GetCoilsInputFlag(true); 

		// SUBROUTINE SPECIFICATIONS FOR MODULE

		// Driver/Manager Routines

		// Get Input routines for module

		// Initialization routines for module

		// Update routines to check convergence and update nodes

		// Update routine

		// Utility routines
		//SHR, bypass factor routines

		// Object Data
		Array1D<IntegratedHeatPumpData> IntegratedHeatPumpUnits;

		// MODULE SUBROUTINES:
		//*************************************************************************

		// Functions
		void
			clear_state()
		{
			
			IntegratedHeatPumpUnits.deallocate();
		}


		void
			SimIHP(
			std::string const & CompName, // Coil Name
			int & CompIndex, // Index for Component name
			int const CyclingScheme, // Continuous fan OR cycling compressor
			Real64 & MaxONOFFCyclesperHour, // Maximum cycling rate of heat pump [cycles/hr]
			Real64 & HPTimeConstant, // Heat pump time constant [s]
			Real64 & FanDelayTime, // Fan delay time, time delay for the HP's fan to
			int const CompOp, // compressor on/off. 0 = off; 1= on
			Real64 const PartLoadFrac,
			int const SpeedNum, // compressor speed number
			Real64 const SpeedRatio, // compressor speed ratio
			Real64 const SensLoad, // Sensible demand load [W]
			Real64 const LatentLoad, // Latent demand load [W]
			bool const IsCallbyWH, //whether the call from the water heating loop or air loop, true = from water heating loop
			bool const FirstHVACIteration, // TRUE if First iteration of simulation
			Optional< Real64 const > OnOffAirFlowRat // ratio of comp on to comp off air flow rate
			)
		{

			//       AUTHOR         Bo Shen, ORNL
			//       DATE WRITTEN   March 2012
			//       MODIFIED       Bo Shen, 12/2014, add variable-speed HPWH
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS SUBROUTINE:
			// This subroutine manages variable-speed Water to Air Heat Pump component simulation.

			// METHODOLOGY EMPLOYED:

			// REFERENCES:
			// N/A

			// Using/Aliasing
			using InputProcessor::FindItemInList;
			using General::TrimSigDigits;
			using VariableSpeedCoils::SimVariableSpeedCoils;

			// Locals
			// SUBROUTINE ARGUMENT DEFINITIONS:

			// shut off after compressor cycle off  [s]
			// part-load ratio = load/total capacity, passed in by the parent object

			// SUBROUTINE PARAMETER DEFINITIONS:
			// na

			// INTERFACE BLOCK SPECIFICATIONS
			// na

			// DERIVED TYPE DEFINITIONS
			// na

			
			// SUBROUTINE LOCAL VARIABLE DECLARATIONS:
			int DXCoilNum(0); // The WatertoAirHP that you are currently loading input into
			int LocNum(0); 
			bool ErrorFound(false); 
			Real64 MyLoad(0.0);
			Real64 MaxCap(0.0);
			Real64 MinCap(0.0);
			Real64 OptCap(0.0);

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}

			if (CompIndex == 0) {
				DXCoilNum = FindItemInList(CompName, IntegratedHeatPumpUnits);
				if (DXCoilNum == 0) {
					ShowFatalError("Integrated Heat Pump not found=" + CompName);
				}
				CompIndex = DXCoilNum;
			}
			else {
				DXCoilNum = CompIndex;
				if (DXCoilNum > NumIHPs || DXCoilNum < 1) {
					ShowFatalError("SimIHP: Invalid CompIndex passed=" + TrimSigDigits(DXCoilNum) + 
						", Number of Integrated HPs=" + TrimSigDigits(NumIHPs) + ", IHP name=" + CompName);
				}
				if (!CompName.empty() && CompName != IntegratedHeatPumpUnits(DXCoilNum).Name) {
					ShowFatalError("SimIHP: Invalid CompIndex passed=" + TrimSigDigits(DXCoilNum) + 
						", Integrated HP name=" + CompName + ", stored Integrated HP Name for that index=" + IntegratedHeatPumpUnits(DXCoilNum).Name);
				}
			}; 


			switch (IntegratedHeatPumpUnits(DXCoilNum).CurMode)
			{
			case IdleMode:
				break; 
			case SCMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SCCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac, 
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SHMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case DWHMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).DWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SCWHMatchSCMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SCWHMatchWHMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SCDWHMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SHDWHElecHeatOffMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			case SHDWHElecHeatOnMode:
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				SimVariableSpeedCoils(BlankString, IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilIndex,
					CyclingScheme, MaxONOFFCyclesperHour, HPTimeConstant, FanDelayTime, CompOp, PartLoadFrac,
					SpeedNum, SpeedRatio, SensLoad, LatentLoad, OnOffAirFlowRat);
				break; 
			default:
				break; 
			}

		}



		void
			GetIHPInput()
		{

			// SUBROUTINE INFORMATION:
			//       AUTHOR         Bo Shen
			//       DATE WRITTEN   December, 2015
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS SUBROUTINE:
			// Obtains input data for Integrated HPs and stores it in IHP data structures

			// METHODOLOGY EMPLOYED:
			// Uses "Get" routines to read in data.

			// REFERENCES:
			// n/a

			// Using/Aliasing
			using namespace InputProcessor;
			using namespace NodeInputManager;
			using BranchNodeConnections::TestCompSet;
			using GlobalNames::VerifyUniqueCoilName;
			using namespace OutputReportPredefined;
			using General::TrimSigDigits;
			using CurveManager::GetCurveIndex;
			using CurveManager::GetCurveType;
			using CurveManager::CurveValue;
			using CurveManager::SetCurveOutputMinMaxValues;
			using OutAirNodeManager::CheckOutAirNodeNumber;
			using WaterManager::SetupTankDemandComponent;
			using WaterManager::SetupTankSupplyComponent;
			using ScheduleManager::GetScheduleIndex;
			using VariableSpeedCoils::GetCoilIndexVariableSpeed;
			using VariableSpeedCoils::SetAirNodes;
			using VariableSpeedCoils::SetWaterNodes;

			// Locals
			// SUBROUTINE ARGUMENT DEFINITIONS:
			// na

			// SUBROUTINE PARAMETER DEFINITIONS:
			static std::string const RoutineName("GGetIHPInput: "); // include trailing blank space

			// INTERFACE BLOCK SPECIFICATIONS
			// na

			// DERIVED TYPE DEFINITIONS
			// na

			// SUBROUTINE LOCAL VARIABLE DECLARATIONS:
			int DXCoilNum; //No of IHP DX system
			int IHPNum; // The Water to Air HP that you are currently loading input into
			int NumASIHPs; // Counter for air-source integrated heat pumps

			int NumAlphas; // Number of variables in String format
			int NumNums; // Number of variables in Numeric format
			int NumParams; // Total number of input fields
			static int MaxNums(0); // Maximum number of numeric input fields
			static int MaxAlphas(0); // Maximum number of alpha input fields
			std::string CoilName; // Name of the  Coil
			std::string Coiltype; // type of coil

			std::string CurrentModuleObject; // for ease in getting objects
			Array1D_string AlphArray; // Alpha input items for object
			Array1D_string cAlphaFields; // Alpha field names
			Array1D_string cNumericFields; // Numeric field names
			Array1D< Real64 > NumArray; // Numeric input items for object
			Array1D_bool lAlphaBlanks; // Logical array, alpha field input BLANK = .TRUE.
			Array1D_bool lNumericBlanks; // Logical array, numeric field input BLANK = .TRUE.

			static bool ErrorsFound(false); // If errors detected in input
	
			int CoilCounter; // Counter
			int I; // Loop index increment

			int IOStat;
			int AlfaFieldIncre; // increment number of Alfa field

			bool IsNotOK; // Flag to verify name
			bool IsBlank; // Flag for blank name
			bool errFlag;
			Real64 CurveVal; // Used to verify modifier curves equal 1 at rated conditions
			Real64 WHInletAirTemp; // Used to pass proper inlet air temp to HPWH DX coil performance curves
			Real64 WHInletWaterTemp; // Used to pass proper inlet water temp to HPWH DX coil performance curves
			int InNode(0);
			int OutNode(0);


			NumASIHPs = GetNumObjectsFound("COILSYSTEM:INTEGRATEDHEATPUMP:AIRSOURCE");
			NumIHPs = NumASIHPs;

			DXCoilNum = 0;

			if (NumIHPs <= 0) {
				ShowSevereError("No Equipment found in Integrated Heat Pumps");
				ErrorsFound = true;
			}

			// Allocate Arrays
			if (NumIHPs > 0) {
				IntegratedHeatPumpUnits.allocate(NumIHPs);
			}

			//air-source integrated heat pump
			GetObjectDefMaxArgs("COILSYSTEM:INTEGRATEDHEATPUMP:AIRSOURCE", NumParams, NumAlphas, NumNums);
			MaxNums = max(MaxNums, NumNums);
			MaxAlphas = max(MaxAlphas, NumAlphas);

			AlphArray.allocate(MaxAlphas);
			cAlphaFields.allocate(MaxAlphas);
			lAlphaBlanks.dimension(MaxAlphas, true);
			cNumericFields.allocate(MaxNums);
			lNumericBlanks.dimension(MaxNums, true);
			NumArray.dimension(MaxNums, 0.0);

			// Get the data for air-source IHPs
			CurrentModuleObject = "COILSYSTEM:INTEGRATEDHEATPUMP:AIRSOURCE"; //for reporting

			for (CoilCounter = 1; CoilCounter <= NumASIHPs; ++CoilCounter) {

				++DXCoilNum;
				AlfaFieldIncre = 1;

				GetObjectItem(CurrentModuleObject, CoilCounter, AlphArray, NumAlphas, NumArray, NumNums, IOStat, lNumericBlanks, lAlphaBlanks, cAlphaFields, cNumericFields);

				IsNotOK = false;
				IsBlank = false;

				VerifyName(AlphArray(1), IntegratedHeatPumpUnits, DXCoilNum - 1, IsNotOK, IsBlank, CurrentModuleObject + " Name");
				if (IsNotOK) {
					ErrorsFound = true;
					if (IsBlank) AlphArray(1) = "xxxxx";
				}
				VerifyUniqueCoilName(CurrentModuleObject, AlphArray(1), errFlag, CurrentModuleObject + " Name");
				if (errFlag) {
					ErrorsFound = true;
				}

				IntegratedHeatPumpUnits(DXCoilNum).NodeConnected = false;
				IntegratedHeatPumpUnits(DXCoilNum).Name = AlphArray(1);
				IntegratedHeatPumpUnits(DXCoilNum).IHPtype = "AIRSOURCE_IHP";

				IntegratedHeatPumpUnits(DXCoilNum).AirInletNodeNum = 
					GetOnlySingleNode(AlphArray(2), ErrorsFound, CurrentModuleObject, AlphArray(1), NodeType_Air, NodeConnectionType_Inlet, 1, ObjectIsNotParent);
				IntegratedHeatPumpUnits(DXCoilNum).AirOutletNodeNum = 
					GetOnlySingleNode(AlphArray(3), ErrorsFound, CurrentModuleObject, AlphArray(1), NodeType_Air, NodeConnectionType_Outlet, 1, ObjectIsNotParent);
				IntegratedHeatPumpUnits(DXCoilNum).WaterInletNodeNum = 
					GetOnlySingleNode(AlphArray(4), ErrorsFound, CurrentModuleObject, AlphArray(1), NodeType_Water, NodeConnectionType_Inlet, 2, ObjectIsNotParent);
				IntegratedHeatPumpUnits(DXCoilNum).WaterOutletNodeNum = 
					GetOnlySingleNode(AlphArray(5), ErrorsFound, CurrentModuleObject, AlphArray(1), NodeType_Water, NodeConnectionType_Outlet, 2, ObjectIsNotParent);
				IntegratedHeatPumpUnits(DXCoilNum).WaterTankoutNod =
					GetOnlySingleNode(AlphArray(6), ErrorsFound, CurrentModuleObject, AlphArray(1), NodeType_Water, NodeConnectionType_Outlet, 2, ObjectIsNotParent);

				TestCompSet(CurrentModuleObject, AlphArray(1), AlphArray(2), AlphArray(3), "Air Nodes");
				TestCompSet(CurrentModuleObject, AlphArray(1), AlphArray(4), AlphArray(5), "Water Nodes");


				IntegratedHeatPumpUnits(DXCoilNum).SCCoilType = "COIL:COOLING:DX:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SCCoilName = AlphArray(7);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SCCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SCCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SCCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}
				
				IntegratedHeatPumpUnits(DXCoilNum).SHCoilType = "COIL:HEATING:DX:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SHCoilName = AlphArray(8);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SHCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SHCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SHCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}
							
				IntegratedHeatPumpUnits(DXCoilNum).DWHCoilType = "COIL:WATERHEATING:AIRTOWATERHEATPUMP:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).DWHCoilName = AlphArray(9);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).DWHCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).DWHCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).DWHCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}


				IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilType = "COIL:WATERHEATING:AIRTOWATERHEATPUMP:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilName = AlphArray(10);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}

				IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilType = "COIL:COOLING:DX:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilName = AlphArray(11);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}


				IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilType = "COIL:WATERHEATING:AIRTOWATERHEATPUMP:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilName = AlphArray(12);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}

				
				IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilType = "COIL:HEATING:DX:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilName = AlphArray(13);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}


				IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilType = "COIL:WATERHEATING:AIRTOWATERHEATPUMP:VARIABLESPEED";
				IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilName = AlphArray(14);
				Coiltype = IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilType;
				CoilName = IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilName;

				ValidateComponent(Coiltype, CoilName, IsNotOK, CurrentModuleObject);
				if (IsNotOK) {
					ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
					ErrorsFound = true;
				}
				else {
					errFlag = false;
					IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilIndex = GetCoilIndexVariableSpeed(Coiltype, CoilName, errFlag);
					if (errFlag) {
						ShowContinueError("...specified in " + CurrentModuleObject + "=\"" + AlphArray(1) + "\".");
						ErrorsFound = true;
					}
				}



				IntegratedHeatPumpUnits(DXCoilNum).TindoorOverCoolAllow = NumArray(1);
				IntegratedHeatPumpUnits(DXCoilNum).TambientOverCoolAllow = NumArray(2);
				IntegratedHeatPumpUnits(DXCoilNum).TindoorWHHighPriority = NumArray(3);
				IntegratedHeatPumpUnits(DXCoilNum).TambientWHHighPriority = NumArray(4);
				IntegratedHeatPumpUnits(DXCoilNum).ModeMatchSCWH = int(NumArray(5));
				IntegratedHeatPumpUnits(DXCoilNum).MinSpedSCWH = int(NumArray(6));
				IntegratedHeatPumpUnits(DXCoilNum).WaterVolSCDWH = NumArray(7);
				IntegratedHeatPumpUnits(DXCoilNum).MinSpedSCDWH = int(NumArray(8));
				IntegratedHeatPumpUnits(DXCoilNum).TimeLimitSHDWH = NumArray(9);
				IntegratedHeatPumpUnits(DXCoilNum).MinSpedSHDWH = int(NumArray(10));

				if (false == IntegratedHeatPumpUnits(DXCoilNum).NodeConnected)
				{
					//air node connections
					InNode = IntegratedHeatPumpUnits(DXCoilNum).AirInletNodeNum;
					OutNode = IntegratedHeatPumpUnits(DXCoilNum).AirOutletNodeNum;
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SCCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SHCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilName, ErrorsFound, InNode, OutNode);
					SetAirNodes(IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilName, ErrorsFound, InNode, OutNode);

					//water node connections
					InNode = IntegratedHeatPumpUnits(DXCoilNum).WaterInletNodeNum;
					OutNode = IntegratedHeatPumpUnits(DXCoilNum).WaterOutletNodeNum;
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).SCWHCoilName, ErrorsFound, InNode, OutNode);
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).SCDWHCoolCoilName, ErrorsFound, InNode, OutNode);
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).SCDWHWHCoilName, ErrorsFound, InNode, OutNode);
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).SHDWHHeatCoilName, ErrorsFound, InNode, OutNode);
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).SHDWHWHCoilName, ErrorsFound, InNode, OutNode);
					SetWaterNodes(IntegratedHeatPumpUnits(DXCoilNum).DWHCoilName, ErrorsFound, InNode, OutNode);
					IntegratedHeatPumpUnits(DXCoilNum).NodeConnected = true;
				};

			}


			if (ErrorsFound) {
				ShowFatalError(RoutineName + "Errors found in getting " + CurrentModuleObject + " input.  Preceding condition(s) causes termination.");
			}


		}

		// Beginning Initialization Section of the Module
		//******************************************************************************

		void
			InitIHP(
			int const DXCoilNum, // Current DXCoilNum under simulation
			Real64 const MaxONOFFCyclesperHour, // Maximum cycling rate of heat pump [cycles/hr]
			Real64 const HPTimeConstant, // Heat pump time constant [s]
			Real64 const FanDelayTime, // Fan delay time, time delay for the HP's fan to
			Real64 const SensLoad, // Control zone sensible load[W]
			Real64 const LatentLoad, // Control zone latent load[W]
			int const CyclingScheme, // fan operating mode
			Real64 const EP_UNUSED(OnOffAirFlowRatio), // ratio of compressor on flow to average flow over time step
			Real64 const SpeedRatio, // compressor speed ratio
			int const SpeedNum // compressor speed number
			)
		{

		
		}

		void
			SizeIHP(int const DXCoilNum)
		{

		

		}

	

		void
			UpdateIHP(int const DXCoilNum)
		{
		

		}

		void
			DecideWorkMode(int const DXCoilNum,
			Real64 const SensLoad, // Sensible demand load [W]
			Real64 const LatentLoad // Latent demand load [W]
			)//shall be called from a air loop parent
		{
			using DataHVACGlobals::SmallLoad;
			using DataEnvironment::OutDryBulbTemp;
			using WaterThermalTanks::SimWaterThermalTank;
			using WaterThermalTanks::GetWaterThermalTankInput;

			Real64 MyLoad(0.0);
			Real64 MaxCap(0.0); 
			Real64 MinCap(0.0);
			Real64 OptCap(0.0);

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}
						
			//decide working mode at the first moment
			//check if there is a water heating call
			IntegratedHeatPumpUnits(DXCoilNum).IsWHCallAvail = false; 
			IntegratedHeatPumpUnits(DXCoilNum).CheckWHCall = true; 
			if (0 == IntegratedHeatPumpUnits(DXCoilNum).WHtankID)//not initialized yet
			{
				IntegratedHeatPumpUnits(DXCoilNum).IsWHCallAvail = false;
			}
			else
			{
				SimWaterThermalTank(
					IntegratedHeatPumpUnits(DXCoilNum).WHtankType, IntegratedHeatPumpUnits(DXCoilNum).WHtankName,
					IntegratedHeatPumpUnits(DXCoilNum).WHtankID,
					false, false,
					MyLoad, MaxCap, MinCap, OptCap, true // TRUE if First iteration of simulation
					);
			}

			IntegratedHeatPumpUnits(DXCoilNum).CheckWHCall = false;

			if (false == IntegratedHeatPumpUnits(DXCoilNum).IsWHCallAvail)//no water heating call
			{
				if ((SensLoad < (-1.0 * SmallLoad)) || (LatentLoad < (-1.0 * SmallLoad)))
				{
					IntegratedHeatPumpUnits(DXCoilNum).CurMode = SCMode;
				}
				else if (SensLoad > SmallLoad)
				{
					if ((IntegratedHeatPumpUnits(DXCoilNum).ControlledZoneTemp > IntegratedHeatPumpUnits(DXCoilNum).TindoorOverCoolAllow) &&
						(OutDryBulbTemp > IntegratedHeatPumpUnits(DXCoilNum).TambientOverCoolAllow))
						IntegratedHeatPumpUnits(DXCoilNum).CurMode = IdleMode;
					else
						IntegratedHeatPumpUnits(DXCoilNum).CurMode = SHMode;
				}
				else
				{
					IntegratedHeatPumpUnits(DXCoilNum).CurMode = IdleMode;
				}
			}
			//below has water heating calls
			else if ((SensLoad < (-1.0 * SmallLoad)) || (LatentLoad < (-1.0 * SmallLoad)))//simultaneous SC and WH calls
			{
				if (IntegratedHeatPumpUnits(DXCoilNum).WaterFlowAccumVol < IntegratedHeatPumpUnits(DXCoilNum).WaterVolSCDWH)
				{
					IntegratedHeatPumpUnits(DXCoilNum).CurMode = SCDWHMode;
				}
				else
				{
					if (1 == IntegratedHeatPumpUnits(DXCoilNum).ModeMatchSCWH) IntegratedHeatPumpUnits(DXCoilNum).CurMode = SCWHMatchWHMode;
					else IntegratedHeatPumpUnits(DXCoilNum).CurMode = SCWHMatchSCMode;
				};

			}
			else if ((IntegratedHeatPumpUnits(DXCoilNum).ControlledZoneTemp > IntegratedHeatPumpUnits(DXCoilNum).TindoorOverCoolAllow) &&
				(OutDryBulbTemp > IntegratedHeatPumpUnits(DXCoilNum).TambientOverCoolAllow))
			{
				IntegratedHeatPumpUnits(DXCoilNum).CurMode = SCWHMatchWHMode;
			}
			else if ((IntegratedHeatPumpUnits(DXCoilNum).ControlledZoneTemp > IntegratedHeatPumpUnits(DXCoilNum).TindoorWHHighPriority) &&
				(OutDryBulbTemp > IntegratedHeatPumpUnits(DXCoilNum).TambientWHHighPriority))
			{
				IntegratedHeatPumpUnits(DXCoilNum).CurMode = DWHMode;
			}
			else if (SensLoad > SmallLoad)
			{
				if (IntegratedHeatPumpUnits(DXCoilNum).SHDWHRunTime > IntegratedHeatPumpUnits(DXCoilNum).TimeLimitSHDWH)
				{
					IntegratedHeatPumpUnits(DXCoilNum).CurMode = SHDWHElecHeatOnMode;
				}
				else
				{
					IntegratedHeatPumpUnits(DXCoilNum).CurMode = SHDWHElecHeatOffMode;
				};
			}
			else
			{
				IntegratedHeatPumpUnits(DXCoilNum).CurMode = DWHMode;
			}
		}

		int
			GetCurWorkMode(int const DXCoilNum)
		{
			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}
			return(IntegratedHeatPumpUnits(DXCoilNum).CurMode);
		}


		int
			GetCoilIndexIHP(
			std::string const & CoilType, // must match coil types in this module
			std::string const & CoilName, // must match coil names for the coil type
			bool & ErrorsFound // set to true if problem
			)
		{

			// FUNCTION INFORMATION:
			//       AUTHOR         Bo Shen
			//       DATE WRITTEN   Jan 2015
			//       MODIFIED       na
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS FUNCTION:
			// This function looks up the coil index for the given coil and returns it.  If
			// incorrect coil type or name is given, ErrorsFound is returned as true and index is returned
			// as zero.

			// METHODOLOGY EMPLOYED:
			// na

			// REFERENCES:
			// na

			// Using/Aliasing
			using InputProcessor::FindItemInList;

			// Return value
			int IndexNum; // returned index of matched coil

			// Locals
			// FUNCTION ARGUMENT DEFINITIONS:

			// FUNCTION PARAMETER DEFINITIONS:
			// na

			// INTERFACE BLOCK SPECIFICATIONS:
			// na

			// DERIVED TYPE DEFINITIONS:
			// na

			// FUNCTION LOCAL VARIABLE DECLARATIONS:
			// na

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}

			IndexNum = FindItemInList(CoilName, IntegratedHeatPumpUnits);

			if (IndexNum == 0) {
				ShowSevereError("GetCoilIndexIHP: Could not find CoilType=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
				ErrorsFound = true;
			}

			return IndexNum;

		}

		int
			GetCoilInletNodeIHP(
			std::string const & CoilType, // must match coil types in this module
			std::string const & CoilName, // must match coil names for the coil type
			bool & ErrorsFound // set to true if problem
			)

		{

			// FUNCTION INFORMATION:
			//       AUTHOR         Bo Shen
			//       DATE WRITTEN   Jan 2016
			//       MODIFIED       na
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS FUNCTION:
			// This function looks up the given coil and returns the inlet node.  If
			// incorrect coil type or name is given, ErrorsFound is returned as true and value is returned
			// as zero.

			// METHODOLOGY EMPLOYED:
			// na

			// REFERENCES:
			// na

			// Using/Aliasing
			using InputProcessor::FindItemInList;

			// Return value
			int NodeNumber; // returned outlet node of matched coil

			// Locals
			// FUNCTION ARGUMENT DEFINITIONS:
			// FUNCTION PARAMETER DEFINITIONS:
			// na

			// INTERFACE BLOCK SPECIFICATIONS:
			// na

			// DERIVED TYPE DEFINITIONS:
			// na

			// FUNCTION LOCAL VARIABLE DECLARATIONS:
			int WhichCoil;

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}

			WhichCoil = FindItemInList(CoilName, IntegratedHeatPumpUnits);
			if (WhichCoil != 0) {
				NodeNumber = IntegratedHeatPumpUnits(WhichCoil).AirInletNodeNum;
			}

			if (WhichCoil == 0) {
				ShowSevereError("GetCoilInletNodeIHP: Could not find CoilType=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
				ErrorsFound = true;
				NodeNumber = 0;
			}

			return NodeNumber;

		}

		int
			GetIHPCoilPLFFPLR(
			std::string const & CoilType, // must match coil types in this module
			std::string const & CoilName, // must match coil names for the coil type
			int const Mode,//mode coil type
			bool & ErrorsFound // set to true if problem
			)
		{
			// FUNCTION INFORMATION:
			//       AUTHOR         Bo Shen
			//       DATE WRITTEN   Jan, 2016
			//       MODIFIED       na
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS FUNCTION:
			// This function looks up the given coil and returns PLR curve index.  If
			// incorrect coil type or name is given, ErrorsFound is returned as true and value is returned
			// as zero.

			// METHODOLOGY EMPLOYED:
			// na

			// REFERENCES:
			// na

			// Using/Aliasing
			using InputProcessor::FindItemInList;
			using VariableSpeedCoils::GetVSCoilPLFFPLR;

			// Return value
			int PLRNumber; // returned outlet node of matched coil

			// Locals
			// FUNCTION ARGUMENT DEFINITIONS:
			// FUNCTION PARAMETER DEFINITIONS:
			// na

			// INTERFACE BLOCK SPECIFICATIONS:
			// na

			// DERIVED TYPE DEFINITIONS:
			// na

			// FUNCTION LOCAL VARIABLE DECLARATIONS:
			int WhichCoil;

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}

			WhichCoil = FindItemInList(CoilName, IntegratedHeatPumpUnits);
			if (WhichCoil != 0) {

				switch (Mode)
				{
				case IdleMode:
					break;
				case SCMode:
					PLRNumber = 
						GetVSCoilPLFFPLR(IntegratedHeatPumpUnits(WhichCoil).SCCoilType, IntegratedHeatPumpUnits(WhichCoil).SCCoilName, ErrorsFound);
					break;
				case SHMode:
					PLRNumber =
						GetVSCoilPLFFPLR(IntegratedHeatPumpUnits(WhichCoil).SHCoilType, IntegratedHeatPumpUnits(WhichCoil).SHCoilName, ErrorsFound);
					break;
				case DWHMode:
					PLRNumber =
						GetVSCoilPLFFPLR(IntegratedHeatPumpUnits(WhichCoil).DWHCoilType, IntegratedHeatPumpUnits(WhichCoil).DWHCoilName, ErrorsFound);
					break;
				case SCWHMatchSCMode:
				case SCWHMatchWHMode:
					PLRNumber =
						GetVSCoilPLFFPLR(IntegratedHeatPumpUnits(WhichCoil).SCWHCoilType, IntegratedHeatPumpUnits(WhichCoil).SCWHCoilName, ErrorsFound);
					break;
				default:
					ShowSevereError("GetIHPCoilPLFFPLR: wrong mode choice=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
					ErrorsFound = true;
					PLRNumber = 0;
					break;
				}
			}
			else {
				WhichCoil = 0;
			}

			if (WhichCoil == 0) {
				ShowSevereError("GetIHPCoilPLFFPLR: Could not find CoilType=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
				ErrorsFound = true;
				PLRNumber = 0;
			}

			return PLRNumber;
		}

		

		Real64
			GetCoilCapacityIHP(
			std::string const & CoilType, // must match coil types in this module
			std::string const & CoilName, // must match coil names for the coil type
			int const Mode,//mode coil type
			bool & ErrorsFound // set to true if problem
			)
		{

			// FUNCTION INFORMATION:
			//       AUTHOR         Bo Shen
			//       DATE WRITTEN   Jan 2016
			//       MODIFIED       na
			//       RE-ENGINEERED  na

			// PURPOSE OF THIS FUNCTION:
			// This function looks up the rated coil capacity at the nominal speed level for the given coil and returns it.  If
			// incorrect coil type or name is given, ErrorsFound is returned as true and capacity is returned
			// as negative.

			// METHODOLOGY EMPLOYED:
			// na

			// REFERENCES:
			// na

			// Using/Aliasing
			using InputProcessor::FindItemInList;
			using VariableSpeedCoils::GetCoilCapacityVariableSpeed; 

			// Return value
			Real64 CoilCapacity; // returned capacity of matched coil

			// Locals
			// FUNCTION ARGUMENT DEFINITIONS:

			// FUNCTION PARAMETER DEFINITIONS:
			// na

			// INTERFACE BLOCK SPECIFICATIONS:
			// na

			// DERIVED TYPE DEFINITIONS:
			// na

			// FUNCTION LOCAL VARIABLE DECLARATIONS:
			int WhichCoil;

			// Obtains and Allocates WatertoAirHP related parameters from input file
			if (GetCoilsInputFlag) { //First time subroutine has been entered
				GetIHPInput();
				//    WaterIndex=FindGlycol('WATER') !Initialize the WaterIndex once
				GetCoilsInputFlag = false;
			}

			WhichCoil = FindItemInList(CoilName, IntegratedHeatPumpUnits);
			if (WhichCoil != 0) {

				switch (Mode)
				{
				case IdleMode:
					break;
				case SCMode:
					CoilCapacity =
						GetCoilCapacityVariableSpeed(IntegratedHeatPumpUnits(WhichCoil).SCCoilType, IntegratedHeatPumpUnits(WhichCoil).SCCoilName, ErrorsFound);
					break;
				case SHMode:
					CoilCapacity =
						GetCoilCapacityVariableSpeed(IntegratedHeatPumpUnits(WhichCoil).SHCoilType, IntegratedHeatPumpUnits(WhichCoil).SHCoilName, ErrorsFound);
					break;
				case DWHMode:
					CoilCapacity =
						GetCoilCapacityVariableSpeed(IntegratedHeatPumpUnits(WhichCoil).DWHCoilType, IntegratedHeatPumpUnits(WhichCoil).DWHCoilName, ErrorsFound);
					break;
				case SCWHMatchSCMode:
				case SCWHMatchWHMode:
					CoilCapacity =
						GetCoilCapacityVariableSpeed(IntegratedHeatPumpUnits(WhichCoil).SCWHCoilType, IntegratedHeatPumpUnits(WhichCoil).SCWHCoilName, ErrorsFound);
					break;
				default:
					ShowSevereError("GetIHPCoilPLFFPLR: wrong mode choice=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
					ErrorsFound = true;
					CoilCapacity = -1000.0;
					break;
				}
			}
			else {
				WhichCoil = 0;
			}

			if (WhichCoil == 0) {
				ShowSevereError("GetCoilCapacityVariableSpeed: Could not find CoilType=\"" + CoilType + "\" with Name=\"" + CoilName + "\"");
				ErrorsFound = true;
				CoilCapacity = -1000.0;
			}

			return CoilCapacity;

		}

		int
			GetLowSpeedNumIHP(int const DXCoilNum)
		{
			return(0); 
		}

		int
			GetMaxSpeedNumIHP(int const DXCoilNum)
		{
			return(0);
		}

		Real64
			GetAirVolFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio)
		{
			return(0.0); 
		}

		Real64
			GetWaterVolFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio)
		{
			return(0.0);
		}

		Real64
			GetAirMassFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio)
		{
			return(0.0);
		}
		

		//     NOTICE

		//     Copyright (c) 1996-2015 The Board of Trustees of the University of Illinois
		//     and The Regents of the University of California through Ernest Orlando Lawrence
		//     Berkeley National Laboratory.  All rights reserved.

		//     Portions of the EnergyPlus software package have been developed and copyrighted
		//     by other individuals, companies and institutions.  These portions have been
		//     incorporated into the EnergyPlus software package under license.   For a complete
		//     list of contributors, see "Notice" located in main.cc.

		//     NOTICE: The U.S. Government is granted for itself and others acting on its
		//     behalf a paid-up, nonexclusive, irrevocable, worldwide license in this data to
		//     reproduce, prepare derivative works, and perform publicly and display publicly.
		//     Beginning five (5) years after permission to assert copyright is granted,
		//     subject to two possible five year renewals, the U.S. Government is granted for
		//     itself and others acting on its behalf a paid-up, non-exclusive, irrevocable
		//     worldwide license in this data to reproduce, prepare derivative works,
		//     distribute copies to the public, perform publicly and display publicly, and to
		//     permit others to do so.

		//     TRADEMARKS: EnergyPlus is a trademark of the US Department of Energy.

	} // IntegratedHeatPumps

} // EnergyPlus
