#ifndef HOMECONTROLLIGHTING_H
#define HOMECONTROLLIGHTING_H

/*********************************************************************
    Filename:       HomeControlLighting.h
    Revised:        $Date: 2004/12/30 19:27:31 $
    Revision:       $Revision: 1.6 $
    Description:    
    Notes:          

    Home Control - Lighting

    This file contains all the consistent information defined within
    the profile.

    Copyright (c) 2003 by Figure 8 Wireless, LLC., All Rights Reserved.
    Permission to use, reproduce, copy, prepare derivative works,
    modify, distribute, perform, display or sell this software and/or
    its documentation for any purpose is prohibited without the express
    written consent of Figure 8 Wireless, LLC.
*********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
#define MAX_LIGHT_VALUE                           255
// Profile ID
#define CHL_PROFILE_ID                            0x0100

// Cluster IDs
#define CLUSTERID_OnOffDRC                        0x01
#define CLUSTERID_DimBrightDRC                    0x02
#define CLUSTERID_PresetDRC                       0x03
#define CLUSTERID_ProgramDRC                      0x04
#define CLUSTERID_AdjDRC                          0x05

#define CLUSTERID_LightLevelLSM                   0x06
#define CLUSTERID_ProgramLSM                      0x07

#define CLUSTERID_OccupancyOS                     0x08
#define CLUSTERID_ProgramOS                       0x09

#define CLUSTERID_StatusDLC                       0x11
#define CLUSTERID_ProgramDLC                      0x12

#define CLUSTERID_OnOffSRC                        0x13
#define CLUSTERID_ProgramSRC                      0x14

#define CLUSTERID_StatusSLC                       0x15
#define CLUSTERID_ProgramSLC                      0x16

// Attributes - DRC

/* OnOffDRC */

#define OnOffDRC_OnOff                            0x0000

/* Constants - OnOff */
#define OnOffDRC_ON                               0xFF
#define OnOffDRC_OFF                              0x00
#define OnOffDRC_TOGGLE                           0xF0

/* DimBrightDRC */
#define DimBrightDRC_DimBright                    0x0000

#define DimBrightDRC_DIM_ID                       0x0000
  #define Is_DimBrightDRC_DIM(Attrib) (((Attrib & 0xFF00) == DimBrightDRC_DIM_ID)? TRUE : FALSE)

#define DimBrightDRC_BRIGHT_ID                    0x0100
  #define Is_DimBrightDRC_BRIGHT(Attrib) (((Attrib & 0xFF00) == DimBrightDRC_BRIGHT_ID)? TRUE : FALSE)

/* PresetDRC */
#define PresetDRC_StorePreset_ID                  0x0000
  #define Is_PresetDRC_StorePreset(Attrib) (((Attrib & 0xFF00) == PresetDRC_StorePreset_ID)? TRUE : FALSE) 

#define PresetDRC_Preset_ID                       0x0100
  #define Is_PresetDRC_Preset(Attrib) (((Attrib & 0xFF00) == PresetDRC_Preset_ID)? TRUE : FALSE) 

/* AdjDRC */
#define AdjDRC_CurrentLevel_ID                    0x0000
  #define Is_AdjDRC_CurrentLevel(Attrib) (((Attrib & 0xFF00) == AdjDRC_CurrentLevel_ID)? TRUE : FALSE) 

#define AdjDRC_PreviousLevel_ID                   0x0100
  #define Is_AdjDRC_PreviousLevel(Attrib) (((Attrib & 0xFF00) == AdjDRC_PreviousLevel_ID)? TRUE : FALSE)

#define AdjDRC_Stop_ID                            0x0500
  #define Is_AdjDRC_Stop(Attrib) (((Attrib & 0xFF00) == AdjDRC_Stop_ID)? TRUE : FALSE)

#define AdjDRC_MinDimLevel_ID                     0x0600
  #define Is_AdjDRC_MinDimLevel(Attrib) (((Attrib & 0xFF00) == AdjDRC_MinDimLevel_ID)? TRUE : FALSE)

#define AdjDRC_MaxBrightLevel_ID                  0x0700
  #define Is_AdjDRC_MaxBrightLevel(Attrib) (((Attrib & 0xFF00) == AdjDRC_MaxBrightLevel_ID)? TRUE : FALSE)

/* ProgramDRC */
#define ProgramDRC_Override                       0x0000
#define ProgramDRC_Auto                           0x0001
#define ProgramDRC_FactoryDefault                 0x0002


// Attributes - OS
#define OccupancyOS_CurrentState                  0x0000

#define OccupancyOS_CurrentState_Occupied         0xFF
#define OccupancyOS_CurrentState_Unoccupied       0x00
#define OccupancyOS_CurrentState_Wait             0xF0

#define ProgramOS_ReportTime                      0x0000
#define ProgramOS_TimeOut                         0x0001
#define ProgramOS_CurrentStateOn                  0x0002
#define ProgramOS_CurrentStateOff                 0x0003
#define ProgramOS_Override                        0x0004
#define ProgramOS_Auto                            0x0005
#define ProgramOS_FactoryDefault                  0x0006

// Attributes - LSM
#define LightLevelLSM_CurrentLevel                0x0000

#define ProgramLSM_ReportTime                     0x0000
#define ProgramLSM_MinLevelChange                 0x0001
#define ProgramLSM_Threshold                      0x0002
#define ProgramLSM_Offset                         0x0003
#define ProgramLSM_Override                       0x0004
#define ProgramLSM_Auto                           0x0005
#define ProgramLSM_FactoryDefault                 0x0006

// Attributes - SRC

/* OnOffSRC */
#define OnOffSRC_OnOff                            0x0000
/* Constants - OnOff */
#define OnOffSRC_ON                               0xFF
#define OnOffSRC_OFF                              0x00
#define OnOffSRC_TOGGLE                           0xF0

/* ProgramSRC */
#define ProgramSRC_Override                       0x0000
#define ProgramSRC_Auto                           0x0001
#define ProgramSRC_FactoryDefault                 0x0002

// Attributes - DLC

/* ProgramDLC */
#define ProgramDLC_Override                       0x0000
#define ProgramDLC_Auto                           0x0001
#define ProgramDLC_FactoryDefault                 0x0002
#define ProgramDLC_ResetO_LSDLC                   0x0007
#define ProgramDLC_PresetO_LSDLC                  0x0008
#define ProgramDLC_BrownOutMinVolt                0x0009
#define ProgramDLC_ShutDownPkCurrent              0x000A
#define ProgramDLC_MeteringPeriod                 0x000B

#define StatusDLC_OnOff                           0x0000
#define StatusDLC_RunTime                         0x0001
#define StatusDLC_Watts                           0x0002
#define StatusDLC_TotalPower                      0x0003
#define StatusDLC_Energy                          0x0004
#define StatusDLC_Vars                            0x0005
#define StatusDLC_Voltage                         0x0006
#define StatusDLC_Current                         0x0007
#define StatusDLC_Ripple                          0x0008
#define StatusDLC_Frequency                       0x0009
#define StatusDLC_Phase                           0x000A
#define StatusDLC_PhaseAngle                      0x000B
#define StatusDLC_PF                              0x000C
#define StatusDLC_PeakWatts                       0x000D
#define StatusDLC_PeakTotalPower                  0x000E
#define StatusDLC_PeakVoltage                     0x000F
#define StatusDLC_PeakCurrent                     0x0010
#define StatusDLC_LoadType                        0x0011

// Attributes for SLC

/* ProgramSLC */
#define ProgramSLC_Override                       0x0000
#define ProgramSLC_Auto                           0x0001
#define ProgramSLC_FactoryDefault                 0x0002
#define ProgramSLC_ResetO_LSSLC                   0x0007
#define ProgramSLC_PresetO_LSSLC                  0x0008
#define ProgramSLC_BrownOutMinVolt                0x0009
#define ProgramSLC_ShutDownPkCurrent              0x000A
#define ProgramSLC_MeteringPeriod                 0x000B


#define StatusSLC_OnOff                           0x0000
#define StatusSLC_RunTime                         0x0001
#define StatusSLC_Watts                           0x0002
#define StatusSLC_TotalPower                      0x0003
#define StatusSLC_Energy                          0x0004
#define StatusSLC_Vars                            0x0005
#define StatusSLC_Voltage                         0x0006
#define StatusSLC_Current                         0x0007
#define StatusSLC_Ripple                          0x0008
#define StatusSLC_Frequency                       0x0009
#define StatusSLC_Phase                           0x000A
#define StatusSLC_PhaseAngle                      0x000B
#define StatusSLC_PF                              0x000C
#define StatusSLC_PeakWatts                       0x000D
#define StatusSLC_PeakTotalPower                  0x000E
#define StatusSLC_PeakVoltage                     0x000F
#define StatusSLC_PeakCurrent                     0x0010
#define StatusSLC_LoadType                        0x0011

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HOMECONTROLLIGHTING_H */