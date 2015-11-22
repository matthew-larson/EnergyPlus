A New Versatile Fan Input Object
================

**B. Griffith, Energy Archmage Company, for SEI/TRANE**

 - first draft 11/10/2015
 - second draft 11/21/2015
 

## Justification for New Feature ##

The existing set of fan objects is confusing to users and a burden to interface developers.  Fan:ConstantVolume and Fan:OnOff appear to be for the same sort of applications and it has been suggested that they be combined into one.  Fan:OnOff has curve options for performance at varying flow which seems to be the domain of Fan:VariableVolume. Fan:VariableVolume has fixed input fields for curve coefficients rather than referencing an external performance curve which is current practice for other HVAC component models. There is no clarity on which fan is for two-speed applications. 

ASHRAE Standard 90.1 has new requirements that will lead to two-speed fans in many (all?) applications which makes in more important than ever that EnergyPlus has a clear and accurate way of modeling two-speed (and multi-speed) fans. Fan:OnOff was expanded circa 2009 to add curves for performance at different flow rates to improve modeling multi-speed fans in unitary systems (see NFP_OnOffFan_MultispeedPowerAdjustments.doc). However, PNNL researchers  point out that the flow-weighted average approach introduces up to 20% error in power consumption compared to a time-weighted approach (see Hart, Athalye, and Wang. 2013. Improving Simulation of Outside Air Economizers and Fan Control for Unitary Air Conditioners). The problem with the current approach is that it assumes that the fan is really running at the average flow rate when calculating power. When the average flow rate does not match one of the actual speed levels that the fan can operate at, the fan is actually cycling between speed levels with some fraction of the time spent at each speed level.  Because fan power is highly non-linear with flow, it is important to average the power results using a time-weighted approach rather than simply use the average flow in the power calculation.  It is therefore justified that fan models be improved to include input data for the discrete speed levels that the fans can actually operate at so that the current problem can be corrected. The following figure diagrams the issue.

![PNNL figure on power calculation problem]( Two-Speed_FanPowerProblem_PNNL.png)
Figure 1. PNNL graphic showing fan power calculation problem PNNL-SA-100271 

Because fans are present in the vast majority of input files, making substantial changes to existing fan objects, or outright forcing two or more to be combined, would create transition and usability burdens for users and interface developers.  It is therefore helpful to introduce an entirely new fan object that can coexist for a time with the older fan objects while gradually replacing them over one or more release cycles.  A new fan object is more valuable if designed to allow for older fan input objects to eventually be transitioned to the new object. (However transition and removal of older fans is not being proposed for the current project, and would be a separate project for a future release cycle.) 

Scalable units for sizing fan power of the form W/cfm and W/cfm-inH2O have been requested by a major interface developer because it simplifies the creation of library data and communicating with users in straightforward engineering units.  However, the existing fan input objects do not have an input for design electric power which is unlike HVAC component models elsewhere in EnergyPlus. After examining the options for adding scalable fan sizing it was concluded that an entirely new fan input object would be justified.

A new, do-all fan model that can be substituted for Fan:VariableVolume, Fan:ConstantVolume, or Fan:OnOff, and not require a separate FanPerformance:NightVentilation object, is justified because it will enable a major revamp of usability issues related to fans, a simpler solution for managing libraries, more accurate two- and multi-speed modeling, allow using separate curves and tables to describe VAV performance, etc. etc.  


## E-mail and  Conference Call Conclusions ##

An early-phase first draft design document was circulated among the sizing subgroup and discussed during a conference call on November 11, 2015. Email comments were provided by three reviewers. The overall idea of a new fan object was viewed favorably but with caution and concerns about transition and policy towards yet another fan. Follow up discussions resulted in a recommendation to move forward with a full proposal, specific suggestions to improve the design of the input object for transition, and a request to use new-style OO implementation in a separate source code file.  

A second draft of the design document was produced that incorporates comments. 

## Overview ##

This proposal is for adding an entirely new fan input object that can serve as a direct substitute for Fan:VariableVolume, Fan:ConstantVolume, or Fan:OnOff. In addition it will incorporate input fields needed for night ventilation operation mode so that a separate FanPerformance:NightVentilation is not required when using the night ventilation availability manager. The core fan model will not be significantly changed, but rather the simple (not component model) model variants will be all be available with a single input object.  The exception is that for fans with discrete speed levels, the power calculation will be done using a time-weighted method instead of a flow-weighted method. Motor losses not added to the air stream can be rejected to the mechanical room. The older fan objects will be retained for some number of release cycles with the plant that they will eventually be deprecated and transitioned to use the new fan object. 

## Approach ##

A new input object with proposed class name *Fan* will be added that has a design electric power consumption, separate performance modifier curves, scalable sizing factors, the ability to be used as a single-speed, two-speed, multi-speed, or VAV.  New input for discrete speed levels will be included and fan power calculations improved for time-weighted averaging. The underlying engineering model will be largely retained.  Sizing calculations will determine and report a Design Electric Power Consumption. Energy balance will be improved by allowing the option of entering a name of the zone where the motor is located that will receive motor heat losses that are not added to the air stream as internal gains. Parent objects that are not destined to be deprecated and contain a fan will all be modified to accept the new fan.  The new fan will be designed to allow for eventual transition of older objects. 

## Implementation ##
Fan models are currently implemented in Fans.hh and Fans.cc.  The different fan models all share the same data structure and many of the same routines.  The new fan will be implemented in new source file, called  FanObject.hh/FanObject.cc, to allow for new style programming and a clean separation to help with eventual removal of the older fan objects.  When motor efficiency and motor in air stream fractions are both less than 1.0, the motor losses and a motor zone name is input, then the losses will be registered as internal gains as is done for pumps. 
  
### OO Design###
The new fan will be implemented as a class using Object Oriented programming. The existing fan code in Fans.hh/Fans.cc will be used as guide to the engineering formulation but reimplemented using OO techniques.  The style will largely follow the pattern in HVACFourPipeBeam.cc. The fan object instances will be held in standard library smart pointers.  

### Data Structures ###
The data structure called FanEquipConditions will serve as a starting point for data in the new fan class. New variables will be added for input for scalable units sizing factors, sizing method switches, design electric power, and discrete speed levels.  

## Testing/Validation/Data Sources ##

The new fan will be compared to the older fans with equivalent input and results compared. 

## Input Output Reference Documentation ##

See changes made in actual I/O reference .md files.  The fans are described is in the file "01d-InputOutputReference.md."  However wherever fan objects are referenced and the new fan can be used the descriptions will be updated. 

### Input Description ###

The IDD object for the proposed new fan input follows 

    Fan,
         \memo Versatile simple fan that can be used in variable air volume, constant volume, or on-off cycling applications.
         \memo Performance at different flow rates, or speed levels, is determined using separate performance curve or table
         \memo or prescribed power fractions at discrete speed levels for two-speed or multi-speed fans.
         \min-fields 13
         \extensible:2
    A1 , \field Name
         \required-field
         \reference Fans
         \reference FansCV
         \reference FansCVandOnOff
         \reference FansCVandOnOffandVAV
    A2 , \field Availability Schedule Name
         \note Availability schedule name for this fan. Schedule value > 0 means the fan is available.
         \note If this field is blank, the fan is always available.
         \type object-list
         \object-list ScheduleNames
    A3 , \field Air Inlet Node Name
         \type node
         \required-field
    A4 , \field Air Outlet Node Name
         \type node
         \required-field
    N1 , \field Design Maximum Air Flow Rate
         \required-field
         \type real
         \units m3/s
         \minimum> 0.0
         \autosizable
    A5 , \field Speed Control Method
         \type choice
         \key Variable
         \key Discrete
         \default Discrete
    N2 , \field Electric Power Minimum Flow Rate Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
         \default 0.2
    N3 , \field Design Pressure Rise
         \type real
         \units Pa
         \ip-units inH2O
         \required-field
    N4 , \field Motor Efficiency
         \type real
         \default 0.9
         \minimum> 0.0
         \maximum 1.0
    N5 , \field Motor In Air Stream Fraction
         \note 0.0 means fan motor outside of air stream, 1.0 means motor inside of air stream
         \type real
         \default 1.0
         \minimum 0.0
         \maximum 1.0
    N6 , \field Design Electric Power Consumption
         \type real
         \units W
         \autosizable
         \note Fan power consumption at maximum air flow rate.  
         \note If autosized the method used to scale power is chosen in the following field
    A6 , \field Design Power Sizing Method
         \type choice
         \key PowerPerFlow
         \key PowerPerFlowPerPressure
         \key TotalEfficiency
         \default PowerPerFlowPerPressure
    N7 , \field Electric Power Per Unit Flow Rate
         \type real
         \units W/(m3/s)
         \ip-units W/(ft3/min)
         \default ???
    N8 , \field Electric Power Per Unit Flow Rate Per Unit Pressure
         \type real
         \units W/((m3/s) -Pa)
         \ip-units W/((ft3/min)-inH2O)
         \default 1.66667
    N9,  \field Fan Total Efficiency
         \type real
         \default 0.7
         \minimum>0.0
         \maximum 1.0
    A7 , \field Electric Power Function of Flow Fraction Curve Name
         \note independent variable is normalized flow rate, current flow divided by Design Maximum Flow Rate.
         \note dependent variable is modification factor multiplied by Design Power Consumption.
         \note field is required if Speed Control Method is set to Variable
         \type object-list
         \object-list UniVariateCurves
         \object-list UniVariateTables
    N10, \field Night Ventilation Mode Pressure Rise
         \note pressure rise to use when in night mode using AvailabilityManager:NightVentilation
         \type real
         \units Pa
         \ip-units inH2O
    N11, \field Night Ventilation Mode Flow Fraction
         \note Fraction of Design Maximum Air Flow Rate to use when in night mode using AvailabilityManager:NightVentilation
         \type real
         \minimum 0.0
         \maximum 1.0
    A8 , \field Motor Loss Zone Name
          \note optional, if used fan motor heat losses that not added to air stream are transfered to zone as internal gains
          \type object-list
          \object-list ZoneNames
    N12, \field Motor Loss Radiative Fraction 
          \note optional. If zone identified in previous field then this determines
          \note the split between convection and radiation for the fan motor's skin losses
          \type real
          \minimum 0.0
          \maximum 1.0
    A9 , \field End-Use Subcategory
         \type alpha
         \retaincase
         \default General
    N13, \field Number of Speeds
         \note number of different speed levels available when Speed Control Method is set to Discrete
         \note Speed need to be arranged in increasing order in remaining field sets.
         \note If set to 1, or omitted, and Speed Control Method is Discrete then constant fan speed is the design maximum.
         \type integer
         \default 1
    N14, \field Speed 1 Flow Fraction
         \begin-extensible
         \type real
         \minimum 0.0
         \maximum 1.0
    N15, \field Speed 1 Electric Power Fraction
         \note if left blank then use Electric Power Function of Flow Fraction Curve
         \type real
         \minimum 0.0
         \maximum 1.0
    N16, \field Speed 2 Flow Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
    N17, \field Speed 2 Electric Power Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
    N18, \field Speed 3 Flow Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
    N19, \field Speed 3 Electric Power Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
    N20, \field Speed n Flow Fraction
         \type real
         \minimum 0.0
         \maximum 1.0
    N21; \field Speed n Electric Power Fraction
         \type real
         \minimum 0.0
         \maximum 1.0

### Outputs Description ###

<insert text>

## Engineering Reference ##

See changes made in actual Eng Ref .md file called " "

## Example File and Transition Changes ##

No transition is planned for this project however feasibility of transition rules are taken into account with the intent of eventually being able to transition Fan:VariableVolume, Fan:OnOff, and Fan:ConstantVolume into the the Fan. 

## References ##

hart, R., Athalye, R., Wang, W.; 2013. Improving Simulation of Outside Air Economizer and Fan Control for Unitary Air Conditioners. ASHRAE Transactions. Vol 119 Issue 2, p1-8. 

