#ifndef IntegratedHeatPumps_hh_INCLUDED
#define IntegratedHeatPumps_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.hh>
#include <ObjexxFCL/Optional.hh>

// EnergyPlus Headers
#include <EnergyPlus.hh>
#include <VariableSpeedCoils.hh>
#include <DataSizing.hh>
#include <DataGlobals.hh>
#include <DataHVACGlobals.hh>

namespace EnergyPlus {

namespace IntegratedHeatPumps {

	// Using/Aliasing
	// Using/Aliasing
	using VariableSpeedCoils::MaxSpedLevels;

	// Data
	//MODULE PARAMETER DEFINITIONS

	// Identifier is VarSpeedCoil
	extern int NumIHPs; //counter for all integrated heat pumps including air-source and water-source
	extern bool GetCoilsInputFlag; // Flag set to make sure you get input once

	// operation mode
	int const IdleMode(0);
	int const SCMode(1);
	int const SHMode(2);
	int const DWHMode(3);
	int const SCWHMatchSCMode(4);
	int const SCWHMatchWHMode(5);
	int const SCDWHMode(6);
	int const SHDWHElecHeatOffMode(7);
	int const SHDWHElecHeatOnMode(8);

	// SUBROUTINE SPECIFICATIONS FOR MODULE

	// Driver/Manager Routines

	// Get Input routines for module

	// Initialization routines for module

	// Update routines to check convergence and update nodes

	// Update routine

	// Utility routines
	//SHR, bypass factor routines

	// Types

	struct IntegratedHeatPumpData // variable speed coil
	{
		// Members
		std::string Name; // Name of the  Coil
		std::string IHPtype; // type of coil

		int AirInletNodeNum; // Node Number of the Air Inlet
		int AirOutletNodeNum; // Node Number of the Air Outlet
		int WaterInletNodeNum; // Node Number of the Water Onlet
		int WaterOutletNodeNum; // Node Number of the Water Outlet
		int WaterTankoutNod; // water node to monitor the supply water flow amount

		std::string SCCoilType; // Numeric Equivalent for SC Coil Type
		std::string SCCoilName;
		int SCCoilIndex; // Index to SC coil

		std::string SHCoilType; // Numeric Equivalent for SH Coil Type
		std::string SHCoilName;
		int SHCoilIndex; // Index to SH coil

		std::string SCWHCoilType; // Numeric Equivalent for SCWH Coil Type
		std::string SCWHCoilName; 
		int SCWHCoilIndex; // Index to SCWH coil

		std::string DWHCoilType; // Numeric Equivalent for DWH Coil Type
		std::string DWHCoilName;
		int DWHCoilIndex; // Index to DWH coil

		std::string SCDWHCoolCoilType; // Numeric Equivalent for SCDWH Coil Type, cooling part
		std::string SCDWHCoolCoilName;
		int SCDWHCoolCoilIndex; // Index to SCDWH coil, cooling part

		std::string SCDWHWHCoilType; // Numeric Equivalent for SCDWH Coil Type, water heating part
		std::string SCDWHWHCoilName;
		int SCDWHWHCoilIndex; // Index to SCDWH coil, water heating part

		std::string SHDWHHeatCoilType; // Numeric Equivalent for SHDWH Coil Type, heating part
		std::string SHDWHHeatCoilName;
		int SHDWHHeatCoilIndex; // Index to SHDWH coil, heating part

		std::string SHDWHWHCoilType; // Numeric Equivalent for SHDWH Coil Type, water heating part
		std::string SHDWHWHCoilName;
		int SHDWHWHCoilIndex; // Index to SHDWH coil, water heating part

		int ModeMatchSCWH;//- 0: match cooling load, 1 : match water heating load in SCWH mode
		int MinSpedSCWH; //-minimum speed level for SCWH mode
		int MinSpedSCDWH; //- minimum speed level for SCDWH mode
		int MinSpedSHDWH;//- minimum speed level for SHDWH mode
		Real64 TindoorOverCoolAllow;  //- [C], indoor temperature above which indoor overcooling is allowed
		Real64 TambientOverCoolAllow; //- [C], ambient temperature above which indoor overcooling is allowed
		Real64 TindoorWHHighPriority;  //- [C], indoor temperature above which water heating has the higher priority
		Real64 TambientWHHighPriority; //ambient temperature above which water heating has the higher priority

		Real64 WaterVolSCDWH;// limit of water volume before switching from SCDWH to SCWH
		Real64 TimeLimitSHDWH; //time limit before turning from SHDWH to electric heating 

		//flow rates of SC mode
		Array1D< Real64 > SCVolumeFlowRate; // Supply air volume flow rate during cooling operation
		Array1D< Real64 > SCMassFlowRate; // Supply air mass flow rate during cooling operation
		Array1D< Real64 > SCSpeedRatio; // Fan speed ratio in cooling mode

		//flow rates of SH mode
		Array1D< Real64 > SHVolumeFlowRate; // Supply air volume flow rate during heating operation
		Array1D< Real64 > SHMassFlowRate; // Supply air mass flow rate during heating operation
		Array1D< Real64 > SHSpeedRatio; // Fan speed ratio in heating mode

		//flow rates of SCWH mode
		Array1D< Real64 > SCWHVolumeFlowRate; // Supply air volume flow rate during SCWH operation
		Array1D< Real64 > SCWHMassFlowRate; // Supply air mass flow rate during SCWH operation
		Array1D< Real64 > SCWHSpeedRatio; // Fan speed ratio in SCWH mode

		//flow rates of SCDWH mode
		Array1D< Real64 > SCDWHVolumeFlowRate; // Supply air volume flow rate during SCDWH operation
		Array1D< Real64 > SCDWHMassFlowRate; // Supply air mass flow rate during SCDWH operation
		Array1D< Real64 > SCDWHSpeedRatio; // Fan speed ratio in SCDWH mode

		//flow rates of SHDWH mode
		Array1D< Real64 > SHDWHVolumeFlowRate; // Supply air volume flow rate during SHDWH operation
		Array1D< Real64 > SHDWHMassFlowRate; // Supply air mass flow rate during SHDWH operation
		Array1D< Real64 > SHDWHSpeedRatio; // Fan speed ratio in SHDWH mode

		//not initialized yet
		int WHtankType;
		std::string WHtankName; 
		int WHtankID; 
		bool IsWHCallAvail;//whether water heating call available
		bool CheckWHCall; 
		int CurMode; //current working mode
		Real64 ControlledZoneTemp; 
		Real64 WaterFlowAccumVol;// water flow accumulated volume
		Real64 SHDWHRunTime; 
		bool NodeConnected; 
		Real64 TotalHeatingEnergyRate; 
		
		// Default Constructor
		IntegratedHeatPumpData() :
			SCCoilIndex(0), 
			SHCoilIndex(0),
			SCWHCoilIndex(0),
			DWHCoilIndex(0),
			SCDWHCoolCoilIndex(0),
			SCDWHWHCoilIndex(0),
			SHDWHHeatCoilIndex(0),
			SHDWHWHCoilIndex(0),
			AirInletNodeNum(0),
			AirOutletNodeNum(0),
			WaterInletNodeNum(0),
			WaterOutletNodeNum(0),
			WaterTankoutNod(0),
			ModeMatchSCWH(0),
			MinSpedSCWH(1),
			MinSpedSCDWH(1),
			MinSpedSHDWH(1),
			TindoorOverCoolAllow(0.0),
			TambientOverCoolAllow(0.0),
			TindoorWHHighPriority(0.0),
			TambientWHHighPriority(0.0),
			SCVolumeFlowRate(MaxSpedLevels, 0.0),
			SCMassFlowRate(MaxSpedLevels, 0.0),
			SCSpeedRatio(MaxSpedLevels, 0.0),
			SHVolumeFlowRate(MaxSpedLevels, 0.0),
			SHMassFlowRate(MaxSpedLevels, 0.0),
			SHSpeedRatio(MaxSpedLevels, 0.0),
			SCWHVolumeFlowRate(MaxSpedLevels, 0.0),
			SCWHMassFlowRate(MaxSpedLevels, 0.0),
			SCWHSpeedRatio(MaxSpedLevels, 0.0),
			SCDWHVolumeFlowRate(MaxSpedLevels, 0.0),
			SCDWHMassFlowRate(MaxSpedLevels, 0.0),
			SCDWHSpeedRatio(MaxSpedLevels, 0.0),
			SHDWHVolumeFlowRate(MaxSpedLevels, 0.0),
			SHDWHMassFlowRate(MaxSpedLevels, 0.0),
			SHDWHSpeedRatio(MaxSpedLevels, 0.0),
			WaterVolSCDWH(0.0),
			TimeLimitSHDWH(0.0) 
		{}

		// Member Constructor
			IntegratedHeatPumpData(
			std::string const & Name, // Name of the  Coil
			std::string const & IHPCoilType, // type of coil
			std::string const &SCCoilType_Num, // Numeric Equivalent for SC Coil Type
			std::string const &SCCoilName, // Numeric Equivalent for SC Coil Type
			int const SCCoilIndex, // Index to SC coil
			std::string const &SHCoilType_Num, // Numeric Equivalent for SH Coil Type
			std::string const &SHCoilName, // Numeric Equivalent for SH Coil Type
			int const SHCoilIndex, // Index to SH coil
			std::string const &SCWHCoilType_Num, // Numeric Equivalent for SCWH Coil Type
			std::string const &SCWHCoilName, // Numeric Equivalent for SCWH Coil Type
			int const SCWHCoilIndex, // Index to SCWH coil
			std::string const &DWHCoilType_Num, // Numeric Equivalent for DWH Coil Type
			std::string const &DWHCoilName, // Numeric Equivalent for DWH Coil Type
			int const DWHCoilIndex, // Index to DWH coil
			std::string const &SCDWHCoolCoilType_Num, // Numeric Equivalent for SCDWH Coil Type, cooling part
			std::string const &SCDWHCoolCoilName, // Numeric Equivalent for SCDWH Coil Type, cooling part
			int const SCDWHCoolCoilIndex, // Index to SCDWH coil, cooling part
			std::string const &SCDWHWHCoilType_Num, // Numeric Equivalent for SCDWH Coil Type, water heating part
			std::string const &SCDWHWHCoilName, // Numeric Equivalent for SCDWH Coil Type, water heating part
			int const SCDWHWHCoilIndex, // Index to SCDWH coil, water heating part
			std::string const &SHDWHHeatCoilType_Num, // Numeric Equivalent for SHDWH Coil Type, heating part
			std::string const &SHDWHHeatCoilName, // Numeric Equivalent for SHDWH Coil Type, heating part
			int const SHDWHHeatCoilIndex, // Index to SHDWH coil, heating part
			std::string const &SHDWHWHCoilType_Num, // Numeric Equivalent for SHDWH Coil Type, water heating part
			std::string const &SHDWHWHCoilName, // Numeric Equivalent for SHDWH Coil Type, water heating part
			int const SHDWHWHCoilIndex, // Index to SHDWH coil, water heating part
			int const AirInletNodeNum, // Node Number of the Air Inlet
			int const AirOutletNodeNum, // Node Number of the Air Outlet
			int const WaterInletNodeNum, // Node Number of the Water Onlet
			int const WaterOutletNodeNum, // Node Number of the Water Outlet
			int const WaterTankoutNod, //node to monitor hot water flow rate
			int const ModeMatchSCWH,//- 0: match cooling load, 1 : match water heating load in SCWH mode
			int const MinSpedSCWH, //-minimum speed level for SCWH mode
			int const MinSpedSCDWH, //- minimum speed level for SCDWH mode
			int const MinSpedSHDWH,//- minimum speed level for SHDWH mode
			Real64 const TindoorOverCoolAllow,  //- [C], indoor temperature above which indoor overcooling is allowed
			Real64 const TambientOverCoolAllow, //- [C], ambient temperature above which indoor overcooling is allowed
			Real64 const TindoorWHHighPriority,  //- [C], indoor temperature above which water heating has the higher priority
			Real64 const TambientWHHighPriority, //ambient temperature above which water heating has the higher priority
			Array1D< Real64 >  const &SCVolumeFlowRate, // Supply air volume flow rate during cooling operation
			Array1D< Real64 >  const &SCMassFlowRate, // Supply air mass flow rate during cooling operation
			Array1D< Real64 >  const &SCSpeedRatio, // Fan speed ratio in cooling mode
			Array1D< Real64 >  const &SHVolumeFlowRate, // Supply air volume flow rate during heating operation
			Array1D< Real64 >  const &SHMassFlowRate, // Supply air mass flow rate during heating operation
			Array1D< Real64 >  const &SHSpeedRatio, // Fan speed ratio in heating mode
			Array1D< Real64 >  const &SCWHVolumeFlowRate, // Supply air volume flow rate during SCWH operation
			Array1D< Real64 >  const &SCWHMassFlowRate, // Supply air mass flow rate during SCWH operation
			Array1D< Real64 >  const &SCWHSpeedRatio, // Fan speed ratio in SCWH mode
			Array1D< Real64 >  const &SCDWHVolumeFlowRate, // Supply air volume flow rate during SCDWH operation
			Array1D< Real64 >  const &SCDWHMassFlowRate, // Supply air mass flow rate during SCDWH operation
			Array1D< Real64 >  const &SCDWHSpeedRatio, // Fan speed ratio in SCDWH mode
			Array1D< Real64 >  const &SHDWHVolumeFlowRate, // Supply air volume flow rate during SHDWH operation
			Array1D< Real64 >  const &SHDWHMassFlowRate, // Supply air mass flow rate during SHDWH operation
			Array1D< Real64 >  const &SHDWHSpeedRatio, // Fan speed ratio in SHDWH mode
			Real64 const WaterVolSCDWH,// limit of water volume before switching from SCDWH to SCWH
			Real64 const TimeLimitSHDWH //time limit before turning from SHDWH to electric heating 
		) :
			Name( Name ),
			IHPtype(IHPCoilType),
			SCCoilType(SCCoilType_Num), 
			SCCoilName(SCCoilName),
			SCCoilIndex(SCCoilIndex), 
			SHCoilType(SHCoilType_Num), 
			SHCoilName(SHCoilName),
			SHCoilIndex(SHCoilIndex), 
			SCWHCoilType(SCWHCoilType_Num), 
			SCWHCoilName(SCWHCoilName),
			SCWHCoilIndex(SCWHCoilIndex), 
			DWHCoilType(DWHCoilType_Num), 
			DWHCoilName(DWHCoilName),
			DWHCoilIndex(DWHCoilIndex), 
			SCDWHCoolCoilType(SCDWHCoolCoilType_Num), 
			SCDWHCoolCoilName(SCDWHCoolCoilName),
			SCDWHCoolCoilIndex(SCDWHCoolCoilIndex),
			SCDWHWHCoilType(SCDWHWHCoilType_Num), 
			SCDWHWHCoilName(SCDWHWHCoilName),
			SCDWHWHCoilIndex(SCDWHWHCoilIndex), 
			SHDWHHeatCoilType(SHDWHHeatCoilType_Num), 
			SHDWHHeatCoilName(SHDWHHeatCoilName),
			SHDWHHeatCoilIndex(SHDWHHeatCoilIndex),
			SHDWHWHCoilType(SHDWHWHCoilType_Num), 
			SHDWHWHCoilName(SHDWHWHCoilName),
			SHDWHWHCoilIndex(SHDWHWHCoilIndex), 
			AirInletNodeNum(AirInletNodeNum), 
			AirOutletNodeNum(AirOutletNodeNum),
			WaterInletNodeNum(WaterInletNodeNum), 
			WaterOutletNodeNum(WaterOutletNodeNum), 
			WaterTankoutNod(WaterTankoutNod),
			ModeMatchSCWH(ModeMatchSCWH),
			MinSpedSCWH(MinSpedSCWH), 
			MinSpedSCDWH(MinSpedSCDWH),
			MinSpedSHDWH(MinSpedSHDWH),
			TindoorOverCoolAllow(TindoorOverCoolAllow),  
			TambientOverCoolAllow(TambientOverCoolAllow), 
			TindoorWHHighPriority(TindoorWHHighPriority),  
			TambientWHHighPriority(TambientWHHighPriority), 
			SCVolumeFlowRate(MaxSpedLevels, SCVolumeFlowRate),
			SCMassFlowRate(MaxSpedLevels, SCMassFlowRate),
			SCSpeedRatio(MaxSpedLevels, SCSpeedRatio),
			SHVolumeFlowRate(MaxSpedLevels, SHVolumeFlowRate),
			SHMassFlowRate(MaxSpedLevels, SHMassFlowRate),
			SHSpeedRatio(MaxSpedLevels, SHSpeedRatio),
			SCWHVolumeFlowRate(MaxSpedLevels, SCWHVolumeFlowRate),
			SCWHMassFlowRate(MaxSpedLevels, SCWHMassFlowRate),
			SCWHSpeedRatio(MaxSpedLevels, SCWHSpeedRatio),
			SCDWHVolumeFlowRate(MaxSpedLevels, SCDWHVolumeFlowRate),
			SCDWHMassFlowRate(MaxSpedLevels, SCDWHMassFlowRate),
			SCDWHSpeedRatio(MaxSpedLevels, SCDWHSpeedRatio),
			SHDWHVolumeFlowRate(MaxSpedLevels, SHDWHVolumeFlowRate),
			SHDWHMassFlowRate(MaxSpedLevels, SHDWHMassFlowRate),
			SHDWHSpeedRatio(MaxSpedLevels, SHDWHSpeedRatio),
			WaterVolSCDWH(WaterVolSCDWH),
			TimeLimitSHDWH(TimeLimitSHDWH)
		{}

	};

	// Object Data
	extern Array1D< IntegratedHeatPumpData > IntegratedHeatPumpUnits;

	// Functions
	void
		clear_state();

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
		Optional< Real64 const > OnOffAirFlowRat = _ // ratio of comp on to comp off air flow rate
	);

	void
	GetIHPInput();

	// Beginning Initialization Section of the Module
	//******************************************************************************

	void
	InitIHP(
		int const CoilNum, // Current DXCoilNum under simulation
		Real64 const MaxONOFFCyclesperHour, // Maximum cycling rate of heat pump [cycles/hr]
		Real64 const HPTimeConstant, // Heat pump time constant [s]
		Real64 const FanDelayTime, // Fan delay time, time delay for the HP's fan to
		Real64 const SensLoad, // Control zone sensible load[W]
		Real64 const LatentLoad, // Control zone latent load[W]
		int const CyclingScheme, // fan operating mode
		Real64 const OnOffAirFlowRatio, // ratio of compressor on flow to average flow over time step
		Real64 const SpeedRatio, // compressor speed ratio
		int const SpeedNum // compressor speed number
	);

	void
	SizeIHP( int const CoilNum );
	
	void
	UpdateIHP( int const DXCoilNum );

	void
		DecideWorkMode(int const DXCoilNum,
		Real64 const SensLoad, // Sensible demand load [W]
		Real64 const LatentLoad // Latent demand load [W]
		);

	int 
		GetCurWorkMode(int const DXCoilNum);

	int
		GetLowSpeedNumIHP(int const DXCoilNum);
	int
		GetMaxSpeedNumIHP(int const DXCoilNum);

	Real64
		GetAirVolFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio);

	Real64
		GetWaterVolFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio);

	Real64
		GetAirMassFlowRateIHP(int const DXCoilNum, int const SpeedNum, Real64 const SpeedRatio);

	int
		GetCoilIndexIHP(
		std::string const & CoilType, // must match coil types in this module
		std::string const & CoilName, // must match coil names for the coil type
		bool & ErrorsFound // set to true if problem
		);

	int
		GetCoilInletNodeIHP(
		std::string const & CoilType, // must match coil types in this module
		std::string const & CoilName, // must match coil names for the coil type
		bool & ErrorsFound // set to true if problem
		);

	Real64
		GetCoilCapacityIHP(
		std::string const & CoilType, // must match coil types in this module
		std::string const & CoilName, // must match coil names for the coil type
		int const Mode,//mode coil type
		bool & ErrorsFound // set to true if problem
		);

	int
		GetIHPCoilPLFFPLR(
		std::string const & CoilType, // must match coil types in this module
		std::string const & CoilName, // must match coil names for the coil type
		int const Mode,//mode coil type
		bool & ErrorsFound // set to true if problem
		);
	
	//     NOTICE

	//     Copyright © 1996-2014 The Board of Trustees of the University of Illinois
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

#endif
