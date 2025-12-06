//
// Created by ambaqinejad on 3/8/25.
//

#ifndef SIDS_H
#define SIDS_H
#include <cmath>

// **************************************************** SID1
typedef struct __attribute__((__packed__)) ExtendedTeleP1
{
    uint8_t CameraPacket1[100];
}ExtendedP1;
// **************************************************** SID2
typedef struct __attribute__((__packed__)) ExtendedTeleP2
{
    uint8_t CameraPacket2[26];
    uint8_t CameraPacket3[20];
    int8_t  res;

    uint64_t  MemoryStatus;
    uint16_t  StartFrequency;
    uint16_t  StopFrequency;
    uint16_t  CFARConstant;
    uint16_t   StepTime;
    uint8_t   RxGain;
    uint32_t  SigintResponseFlag;
    uint8_t   MRAMAdcsConfigFlag;       //OBC
    uint8_t   MRAMSatParameterFlag;     //OBC
    uint8_t   MRAMTeleCounterFlag;      //OBC
    uint8_t   LastResetSource;          //OBC
    uint32_t  TeleCounterOnline;        //OBC
    uint32_t  TeleCounterOffline;       //OBC
    uint16_t  TaskResetCnt;             //OBC
    uint8_t   LastTaskResetNum;         //OBC
    uint8_t   respart2_2[4];
    uint32_t  UV_TransmitterFreq;
    uint32_t  UV_RecieverFreq;
    uint32_t  UV_TX;
    uint8_t   respart2_3;
}ExtendedP2;
// **************************************************** SID3
typedef struct __attribute__((__packed__)) ExtendedTeleP3
{
    uint8_t  UV_RX[4];
    uint8_t  UV_AES_Config[4];
    uint8_t  UV_TR_Output_Power[4];
    uint8_t  UV_BeacanRepeatTime[4];
    uint8_t  UV_Voltage[4];//20
    uint8_t  UV_RSSI[4];
    uint8_t  UV_Temperature[4];
    uint8_t  UV_Temperature_OverHeat[4];
    uint8_t  UV_Rf_Telemetry[4];


    int8_t   RF_telemetry_X[3];//39
    int8_t   TEMP_telemetry_X[4];

    uint16_t XdataRate;
    int8_t   voltage_telemetry_X[4];
    int8_t   Transmitter_Status_X;//50
    int8_t   OutputPower;
    int8_t   CAN_Status_X;
    int8_t   Transmitter_mode_X;
    uint32_t  Tx_frequency_X;//57

    uint32_t  S_TR_Scratch;
    uint32_t  S_TR_AES_Index;
    uint32_t  S_TR_Modulation;
    uint32_t  S_TR_Bitrate;
    uint32_t  S_TR_Fdiviation;//77
    uint32_t  S_TR_BandWidth;
    uint32_t  S_TR_CrcInit;
    uint32_t  S_TR_Lmx2572_Freq;
    uint32_t  S_TR_Fifo_State;
    uint32_t  S_TR_SystemFaults;//97
    uint8_t resp2[3];
}ExtendedP3;
// **************************************************** SID4
typedef struct __attribute__((__packed__)) ExtendedTeleP4
{
    uint32_t  S_TR_ReadLogAmp;
    uint32_t  S_TR_TempSens1;
    uint32_t  S_TR_TempSens2;
    uint32_t  S_TR_Power;
    uint32_t  S_TR_IntThr;//20
    uint32_t  S_TR_TransCuttOffTemp;
    uint32_t  S_RX_Scratch;
    uint32_t  S_RX_AES_Index;
    uint32_t  S_RX_Modulation;
    uint32_t  S_RX_Bitrate;//40
    uint32_t  RX_Fdiviation;
    uint32_t  S_RX_BandWidth;
    uint32_t  S_RX_CrcInit;
    uint32_t  S_RX_Lmx2572_Freq;
    uint32_t  S_RX_Fifo_State;//60
    uint32_t  S_RX_SystemFaults;
    uint32_t  S_RX_ReadLogAmp;
    uint32_t  S_RX_TempSens1;
    uint32_t  S_RX_TempSens2;
    uint32_t  S_RX_Power;//80
    uint32_t  S_RX_IntThr;
    uint32_t  S_RX_Reciever_SyncCrcErr;
    uint32_t  S_RX_RSSI_Const;
    uint32_t  S_RX_RSSI;
    uint32_t  S_RX_RS485BD;//100
}ExtendedP4;
// **************************************************** SID5
typedef struct __attribute__((__packed__)) ExtendedTeleP5
{
    //PMU---------------------------------------------------------------
    uint8_t  PMU1[100];
}ExtendedP5;
// **************************************************** SID6
typedef struct __attribute__((__packed__)) ExtendedTeleP6
{
    uint8_t  PMU2[100];
}ExtendedP6;
// **************************************************** SID7
typedef struct __attribute__((__packed__)) ExtendedTeleP7
{
    uint8_t  PMU3[100];
}ExtendedP7;
// **************************************************** SID8
typedef struct __attribute__((__packed__)) ExtendedTeleP8
{
    uint32_t FDIRReportFlags;
    uint32_t FDIROverCurrentReport;
    uint32_t OBCConfigReport;//12

    uint8_t SFResetCnt;
    uint8_t CBResetCnt;
    uint8_t IRPDUResetCnt;
    uint8_t Sun1DevStatus;
    uint8_t Sun2DevStatus;
    uint8_t Sun3DevStatus;
    uint8_t Sun4DevStatus;
    uint8_t Sun5DevStatus;//20

    uint8_t Sun6DevStatus;
    uint8_t Gyro1DevStatus;
    uint8_t Gyro2DevStatus;
    uint8_t RW1DevStatus;
    uint8_t RW2DevStatus;
    uint8_t RW3DevStatus;
    uint8_t RW4DevStatus;
    uint8_t RW5DevStatus;//28

    uint8_t StarTrackerDevStatus;
    uint8_t IRStarTrackerDevStatus;
    uint8_t CameraLCCDevStatus;
    uint8_t UVBandDevStatus;
    uint8_t MT_Isis5vDevStatus;
    uint8_t MT_Isis33vDevStatus;
    uint8_t MT_HescoDevStatus;
    uint8_t XBand5vDevStatus;//36

    uint8_t XBandVBatDevStatus;
    uint8_t MM_HMR1DevStatus;
    uint8_t MM_HMR2DevStatus;
    uint8_t SBandRxDevStatus;
    uint8_t SBandTxDevStatus;
    uint8_t SIGINTDevStatus;
    uint8_t GPS1DevStatus;
    uint8_t GPS2DevStatus; //44

    uint8_t PRP8vDevStatus;
    uint8_t PRP12vDevStatus;
    uint8_t TempSensorDevStatus;
    uint8_t BatteryHeaterDevStatus;
    uint8_t Onewire1DevStatus;
    uint8_t Onewire2DevStatus;
    uint8_t NandCommandFlag;
    uint8_t NandInitFlag;//52

    uint8_t MMC_InitFlag;
    uint8_t PRPNumberSample;
    uint8_t FreeRunningCnt1;
    uint8_t FreeRunningCnt2;
    int16_t CB1Temprature;
    int16_t CAMTemprature1; //60

    int16_t CAMTemprature2;
    int16_t CB2Temprature1;
    int16_t CB2Temprature2;
    uint16_t PRPTimeSampling; //68

    uint32_t NumberOfResetPMUConnection :4; //Number of Reset Clyde Connection
    uint32_t NumberOfResetSFConnection :4;
    uint32_t NumberOfResetCB1Connection :4;
    uint32_t NumberOfResetIRPDUConnection :4;
    uint32_t NumberOfContiniousReset :4;
    uint32_t NumberOfOBC1_CAN1Reset :4;
    uint32_t NumberOfOBC2_CAN1Reset :4;
    uint32_t NumberOfUV_CAN1Connection :4;

    uint32_t Sun1FaultCnt :4;
    uint32_t Sun2FaultCnt :4;
    uint32_t Sun3FaultCnt :4;
    uint32_t Sun4FaultCnt :4;
    uint32_t Sun5FaultCnt :4;
    uint32_t Sun6FaultCnt :4;
    uint32_t Gyro1FaultCnt :4;
    uint32_t Gyro2FaultCnt :4; //76

    uint32_t RW1FaultCnt :4;
    uint32_t RW2FaultCnt :4;
    uint32_t RW3FaultCnt :4;
    uint32_t RW4FaultCnt :4;
    uint32_t RW5FaultCnt :4;
    uint32_t StarTrackerFaultCnt :4;
    uint32_t IRStarTrackerFaultCnt :4;
    uint32_t CameraFaultCnt :4;

    uint32_t SIGINTFaultCnt :4;
    uint32_t MT_IsisFaultCnt :4;
    uint32_t MT_HescoFaultCnt :4;
    uint32_t MM_HMR1FaultCnt :4;
    uint32_t MM_HMR2FaultCnt :4;
    uint32_t SBandRxFaultCnt :4;
    uint32_t SBandTxFaultCnt :4;
    uint32_t XBandFaultCnt :4; //84

    uint32_t GPS1FaultCnt :4;
    uint32_t GPS2FaultCnt :4;
    uint32_t PRPFaultCnt :4;
    uint32_t TempSensorFaultCnt :4;
    uint32_t BatteryHeaterFaultCnt :4;
    uint32_t RWDevID5 :8;
    uint32_t EnableBatteryHeaterFDIR :1;
    uint32_t Res3 :3; //88

    uint16_t MMCBlkCnt;
    uint32_t MMC_WriteErrors;
    uint16_t processSPIDataPacketErrorCnt;

    uint8_t RWDevID[4];
}ExtendedP8;
// **************************************************** SID9
typedef struct __attribute__((__packed__)) ExtendedTeleP9
{
    uint8_t   UC1_SunReceiveCnt[3];
    uint16_t  UC2_LastGetTime;           //time of last data received
    uint16_t  UC2_TransmitCnt;         //
    uint16_t  UC2_SFReceiveCnt;
    uint8_t   UC2_SwitchCmd;
    uint8_t   UC2_SwitchRealState;
    uint8_t   UC2_SwitchFlagState;
    uint8_t   UC2_SunReceiveCnt[3];
    uint16_t  HE_durationProcess;
    uint16_t  HE_SFduration;
    // uint8_t   HE_OBCTaskRunning[MAX_TASK_RUNNING];    //32 is enough , we can reserve 4 byte
    uint8_t   HE_OBCTaskRunning[32];    //32 is enough , we can reserve 4 byte
    uint16_t  Heater33vTimer;
    uint16_t  OAPPowerDisipation;
    uint16_t  HE_SPIGetCounter;
    uint8_t   HE_MRAMCnt[3];
    uint32_t  HE_ObcCounter;
    uint8_t   HE_RTCTime1[3];
    uint8_t   HE_RTCTime2[3];
    uint8_t   HE_RTCTime3[3];
    double    HE_JulianDate;//
    uint8_t   RW1TimeSetCommand;
    uint16_t  RW1LastGetTime;
    uint8_t   RW2TimeSetCommand;
    uint16_t  RW2LastGetTime;
    uint8_t   RW3TimeSetCommand;
    uint16_t  RW3LastGetTime;
    uint8_t   RW4TimeSetCommand;
    uint16_t  SunIRLastGetTime;
    uint16_t  RWRLastGetTime;
    uint16_t  StarIRLastGetTime;
    uint16_t  HescoLastGetTime;
    uint8_t   BatteryFreeRunning;

}ExtendedP9;
// // **************************************************** SID10
typedef struct __attribute__((__packed__)) ExtendedTeleP10
{
    uint16_t    SS1LastGetTime;
    uint8_t     SS1NumberSample;
    uint16_t    SS2LastGetTime;
    uint8_t     SS2NumberSample;
    uint16_t    SS3LastGetTime;
    uint8_t     SS3NumberSample;
    uint16_t    SS4LastGetTime;
    uint8_t     SS4NumberSample;
    uint16_t    SS5LastGetTime;
    uint8_t     SS5NumberSample;
    uint16_t    Gyro1LastGetTime;
    uint16_t    Gyro1NumberSample;
    uint16_t    Gyro2LastGetTime;
    uint16_t    Gyro2NumberSample;
    uint16_t    MMLastGetTime;
    uint8_t     MMNumberSample;
    uint16_t    GPSLastGetTime;
    uint8_t     GPSNumberSample;
    uint8_t     ClydeFreeRunning;
    uint16_t    ErrorCounterClyde;
    uint16_t    ErrorCounterClydeBattery;
    uint8_t     OneWireFlagSwitch;
    uint8_t     Year;
    uint8_t     Month;
    uint8_t     Day;
    uint8_t     Hour;
    uint8_t     Minute;
    uint8_t     Second;
    uint8_t     OBC_RTC[6];//32
    int8_t      OBC_MB_Temp;
    uint8_t     OBC_ID_;
    uint16_t    BootCounter;
    uint32_t    OBC_CommandCounter;//40
    uint32_t    OBC_Counter;
    uint16_t    OBC_CpuUsage;
    uint8_t     OBC_Mode;
    uint16_t    OBC_Current;
    uint16_t    OBC_RcvdCmdCounter;//51
    uint16_t    OBC_ExecutedCmdCounter;
    uint16_t    UV_Fail_Timer;
    uint8_t     OBC_WaitingTCToRun;
    uint32_t    OBC_ParameterAdd;//60
    uint32_t    MMC_Write_BLK_ADD;
    uint32_t    MMC_Read_BLK_ADD;
    uint8_t     OBC_LINK_TYPE;//69
    uint16_t    RW4LastGetTime;
    uint16_t    STRTimeSampling;
    uint16_t    MTTimeSetCmd;
    uint16_t    MTTimeSampling;
    uint8_t     FailCRC;
    uint32_t    Flash_program_Add;
    uint8_t     ready_program;
    uint8_t     Program_Mode;
    uint8_t     ProgramVersion;
}ExtendedP10;
// // **************************************************** SID11
typedef struct __attribute__((__packed__)) ExtendedTeleP11
{
    uint8_t   KeyIndexEncrypt_UV;
    uint8_t   KeyIndexEncrypt_X;
    uint8_t   KeyIndexEncrypt_S_TX;
    uint8_t   KeyIndexEncrypt_S_RX;
    uint8_t   Num_active_Tasks;
    //uint8_t   Last_TC_ID;               // can be delete
    uint8_t   MRAMs_Usage;
    uint8_t   Nandflash_Usage;
    double      SF_JulianDate;
    float       SF_MTDipoleActuation[3];       //set by hercules
    uint32_t    SF_SysTimerCnt;                //free running timer counter
    uint32_t    SF_TransmitCnt;                //send packet counter by SF (when SPI packet trasmit)
    uint16_t    SF_MTDipoleDuaration;      //set by hercules (ms) 0: mean MT is off
    int16_t     SF_WheelRefrenceCommand[5];    //set by hercules
    uint16_t    SF_CpuUsage;         //idle counter between interrupt
    uint16_t    SF_MTDurationOffTime;             //set by hercules
    uint8_t     SF_RWInterfaceCfg;             //set by hercules |RW4|RW3|RW2|RW1|     10:UART , 01:I2C , 11:CAN , 00:None
    uint8_t     SF_PowerDeviceCfg;             //set by hercules ||PMUInterface(2Bit: 00:CAN 10:I2C)|1wire_sens1|1wire_sens2|MT|MM1|ST1|GPS||
    uint8_t     SF_AOCSFerquency;              //AOCS Frequency 4:1Hz , 2:2Hz,1:4Hz,16:.25Hz,8:0.5Hz
    uint8_t     SF_I2CStatus[2];               //error of I2C Bus Status (Bus 1 ,2)
    uint8_t     SF_CB1PowerSwitchCfg[2];       //set by hercules :UC1:ST1|RW3|RW2|RW1|Gyro1|Sun3|Sun2|Sun1|
    float       SFCmd_MTDipoleActuation[3];
    uint16_t    SFCmd_MTDipoleDuaration;
    int16_t     SFCmd_WheelRefrenceCommand[5];
    uint16_t    SFCmd_MTDurationOffTime;
    uint8_t     SFCmd_RWInterfaceCfg;
    uint8_t     SFCmd_PowerDeviceCfg;
    uint8_t     SFCmd_AOCSFerquency;
    //uint8_t     SFCmd_CB1PowerSwitchCfg[2];         //set by hercules :UC2:ST2|CAM|RW5|RW4|Gyro2|Sun6|Sun5|Sun4|
    uint16_t    UC1_LastGetTime;           //time of last data received
    uint16_t    UC1_TransmitCnt;         //
    uint16_t    UC1_SFReceiveCnt;
    uint8_t     UC1_SwitchCmd;
    uint8_t     UC1_SwitchRealState;
    uint8_t     UC1_SwitchFlagState;
    uint32_t   DisableFaultPowerFDIR;

}ExtendedP11;
// // **************************************************** SID12
typedef struct __attribute__((__packed__)) ExtendedTeleP12
{
    int16_t MMHMR1_X;
    int16_t MMHMR1_Y;
    int16_t MMHMR1_Z;
    int16_t MMHMR2_X;
    int16_t MMHMR2_Y;
    int16_t MMHMR2_Z;
    int8_t  Gyro_Num;
    int16_t Gyro1RateX;
    int16_t Gyro1RateY;
    int16_t Gyro1RateZ;
    int16_t Gyro2RateX;
    int16_t Gyro2RateY;
    int16_t Gyro2RateZ;
    int8_t   Gyro1TempX;
    int8_t   Gyro1TempY;
    int8_t   Gyro1TempZ;
    uint8_t  GPS_number;
    uint8_t  GPS_State;
    uint8_t  GPS_Temperature;
    uint32_t TOW;
    uint16_t WeakNumber;
    uint16_t UTC_Offset;
    float_t  GPSLatitude;
    float_t  GPSLongitude;
    float_t  GPSAltitude;
    uint8_t  GPS_Visible[4];
    uint8_t  GPS_Used[4];
    uint8_t  GlonassVisible[4];
    uint8_t  GlonassUsed[4];
    float_t  GPSPosX;
    float_t  GPSPosY;
    float_t  GPSPosZ;
    float_t  GPSProcVX;
    float_t  GPSProcVY;
    float_t  GPSProcVZ;
    uint8_t  GPSPDOP;
    uint8_t  GPSHDop;
    uint8_t  GPSVDop;
    uint8_t  GPSTDop;
    uint8_t  GPS1_CH11_SNR;
    uint8_t  GPS1_CH12_SNR;
    uint8_t  GPS1_CH13_SNR;
    uint8_t  GPS1_CH14_SNR;
    uint8_t  GPS1_CH15_SNR;
}ExtendedP12;
// // **************************************************** SID13
typedef struct __attribute__((__packed__)) ExtendedTeleP13
{
    uint8_t  GPS1_CH16_SNR;
    uint8_t  GPS1_CH17_SNR;
    uint8_t  GPS1_CH18_SNR;
    uint8_t  GPS1_CH19_SNR;
    uint8_t  GPS1_CH110_SNR;
    uint8_t  GPS1_CH111_SNR;
    uint8_t  GPS1_CH112_SNR;
    uint8_t  GPS1_CH21_SNR;
    uint8_t  GPS1_CH22_SNR;
    uint8_t  GPS1_CH23_SNR;
    uint8_t GPS1_CH24_SNR;
    uint8_t GPS1_CH25_SNR;
    uint8_t GPS1_CH26_SNR;
    uint8_t GPS1_CH27_SNR;
    uint8_t GPS1_CH28_SNR;
    uint8_t GPS1_CH29_SNR;
    uint8_t GPS1_CH210_SNR;
    uint8_t GPS1_CH211_SNR;
    uint8_t GPS1_CH212_SNR;
    float_t SS1ALPHA;
    float_t SS1BETA;
    float_t SS1sundetection;
    uint8_t SS1ERR;
    float_t SS2ALPHA;
    float_t SS2BETA;
    float_t SS2sundetection;
    uint8_t SS2ERR;
    float_t SS3ALPHA;
    float_t SS3BETA;
    float_t SS3sundetection;
    uint8_t SS3ERR;
    float_t SS4ALPHA;
    float_t SS4BETA;
    float_t SS4sundetection;
    uint8_t SS4ERR;
    float_t SS5ALPHA;
    float_t SS5BETA;
    float_t SS5sundetection;
    uint8_t SS5ERR;
    float_t HFP_SS_VecX;
    float_t HFP_SS_VecY;
    float_t HFP_SS_VecZ;
    int8_t HFP_qq;
    int8_t HFP_gg;
    uint16_t HFP_SS_Time;


}ExtendedP13;
// // **************************************************** SID14
typedef struct __attribute__((__packed__)) ExtendedTeleP14
{

    uint16_t HFP_SS_Temp;
    uint16_t HFP_SS_Voltage;


    int8_t Gyro2TempX;
    int8_t Gyro2TempY;
    int8_t Gyro2TempZ;
    int8_t Reserv0;
    float Reserv1;
    float Reserv2;


    int16_t IMTQ_CMDDipX;
    int16_t IMTQ_CMDDipY;
    int16_t IMTQ_CMDDipZ;
    int16_t IMTQCurX;
    int16_t IMTQCury;
    int16_t IMTQCurz;
    int8_t IMTQCoilTempX;
    int8_t IMTQCoilTempy;
    int8_t IMTQCoilTempz;
    int8_t IMTQ_MCUTemp;
    uint8_t     Status_cmd;  //*******
    uint8_t     status;       //*******
    int32_t   HesMMFilteredX;
    int32_t   HesMMFilteredY;
    int32_t   HesMMFilteredZ;
    int16_t   HesCMD_DipX;
    int16_t   HesCMD_DipY;
    int16_t   HesCMD_DipZ;
    int16_t   HesCurX;
    int16_t   HesCurY;
    int16_t   HesCurZ;
    int16_t   HesCoilTempX;
    int16_t   HesCoilTempY;
    int16_t   HesCoilTempZ;
    int16_t   HesMCUTemp;
    int16_t   RW1Speed;
    int16_t   RW1RefSpeed;
    uint16_t  RW1Cur;
    uint8_t   RW1stat_Err_flg;
    int16_t   RW2Speed;
    int16_t   RW2RefSpeed;
    uint16_t  RW2Cur;
    uint8_t   RW2stat_Err_flg;
    int16_t   RW3Speed;
    int16_t   RW3RefSpeed;
    uint16_t  RW3Cur;
    uint8_t     RW3stat_Err_flg; //*******
    int16_t   RW4Speed;
    int16_t   RW4RefSpeed;
    uint16_t  RW4Cur;
    uint8_t   RW4stat_Err_flg;
    int16_t   RWHescoSpeed;
    int16_t   RWHescoRefSpeed;
    uint16_t  RWHescoCur;
}ExtendedP14;
// // **************************************************** SID15
typedef struct __attribute__((__packed__)) ExtendedTeleP15
{
    uint8_t RWHestat_Err_flg;
    float_t TY_Star_attitudeQ1;
    float_t TY_Star_attitudeQ2;
    float_t TY_Star_attitudeQ3;
    float_t TY_Star_attitudeQ4;
    double  TY_Star_internal_time;
    int8_t  TY_StarTemp;
    uint8_t TY_StarImage_expo_value;
    uint8_t TY_StarImage_threshold;
    uint8_t TY_Star_background_value;
    uint8_t TY_StarReporting;
    uint8_t TY_NumberOfExtractedStars;
    uint8_t TY_ExternalImageSwitchStatus;
    uint8_t TY_AttitudeEffectiveSign;
    float_t IR_Star_attitudeQ1;
    float_t IR_Star_attitudeQ2;
    float_t IR_Star_attitudeQ3;
    float_t IR_Star_attitudeQ4;
    uint8_t bytes[8];//double_t   IR_Star_internal_time;
    int8_t  IR_StarTemp;
    uint8_t IR_StarImage_expo_value;
    uint8_t IR_StarImage_threshold;
    uint8_t IR_Star_background_value;
    uint8_t IR_StarReporting;
    uint8_t IR_NumberOfExtractedStars;
    uint8_t IR_ExternalImageSwitchStatus;
    uint8_t IR_AttitudeEffectiveSign;

    float_t SS_B_X;
    float_t SS_B_Y;
    float_t SS_B_Z;
    uint8_t SS_ID;
    float_t MM_B_X;
    float_t MM_B_Y;
    float_t MM_B_Z;
    float_t Gyro_B_X;
    float_t Gyro_B_Y;
    uint8_t res_p15[2];
}ExtendedP15;
// // **************************************************** SID16
typedef struct __attribute__((__packed__)) ExtendedTeleP16
{
    float_t Gyro_B_Z;
    float_t ST_Q1_IM;
    float_t ST_Q2_IM;
    float_t ST_Q3_IM;
    float_t ST_Q4_IM;
    uint8_t ST_ID;
    float_t GPS_Rx;
    float_t GPS_Ry;
    float_t GPS_Rz;
    float_t GPS_Vx;
    float_t GPS_Vy;
    float_t GPS_Vz;
    double_t GPSJD;
    float_t MT_CMD_Act_x;
    float_t MT_CMD_Act_y;
    float_t MT_CMD_Act_z;
    float_t RW_CMD_Act_x;
    float_t RW_CMD_Act_y;
    float_t RW_CMD_Act_z;
    uint8_t interfaceManagerVariablesflags[7];
    float_t q0_Estimated;
    float_t q1_Estimated;
    float_t q2_Estimated;
    float_t q3_Estimated;
}ExtendedP16;
// // **************************************************** SID17
typedef struct __attribute__((__packed__)) ExtendedTeleP17
{

    float_t W_X_Estimated;
    float_t W_Y_Estimated;
    float_t W_Z_Estimated;
    float_t q0_EKF;
    float_t q1_EKF;
    float_t q2_EKF;
    float_t q3_EKF;
    float_t W_X_EKF;
    float_t W_Y_EKF;
    float_t W_Z_EKF;
    float_t q0_EKFSG;
    float_t q1_EKFSG;
    float_t q2_EKFSG;
    float_t q3_EKFSG;
    float_t W_X_EKFSG;
    float_t W_Y_EKFSG;
    float_t W_Z_EKFSG;
    float_t q0_StarGyro;
    float_t CurrentBatteryCapacity;         //OBC
    float_t PowerInputGeneration;           //OBC
    float_t TotalPowerDisipation;           //OBC
    float_t OAPPowerDisipation;             //OBC
    float_t OAPPowerGeneration;             //OBC
    uint8_t globalStatesMRAMFlag;
    uint8_t globalConfigurationMRAMFlag;
    uint8_t globalScheduleTableMRAMFlag;
    uint8_t MRAM1Status;
    uint8_t MRAM2Status;
    uint8_t MRAM3Status;
    uint8_t res_part17[2];
}ExtendedP17;
// // **************************************************** SID18
typedef struct __attribute__((__packed__)) ExtendedTeleP18
{
    float_t q1_StarGyro;
    float_t q2_StarGyro;
    float_t q3_StarGyro;
    float_t W_X_EKFSG;
    float_t W_Y_EKFSG;
    float_t W_Z_EKFSG;
    float_t PNormEKF;
    float_t PNormEKFSG;
    float_t ConditionNumberEKF;
    float_t ConditionNumberEKFSG;
    float_t q0_Path;
    float_t q1_Path;
    float_t q2_Path;
    float_t q3_Path;
    float_t W_X_Path;
    float_t W_Y_Path;
    float_t W_Z_Path;
    float_t dt_ADCS;
    float_t T_Des_X;
    float_t T_Des_Y;
    float_t T_Des_Z;
    float_t ADCSMTDipoleX;
    float_t ADCSMTDipoleY;
    float_t ADCSMTDipoleZ;
    float_t ADCSRWTorqueX;

}ExtendedP18;
// // **************************************************** SID19
typedef struct __attribute__((__packed__)) ExtendedTeleP19
{

    float_t ADCSRWTorqueY;
    float_t ADCSRWTorqueZ;
    float_t RWRPMCommand_X;
    float_t RWRPMCommand_Y;
    float_t RWRPMCommand_Z;
    float_t KpC;
    float_t KpF;
    float_t KdC;
    float_t KdF;
    double_t ADCS_JD;
    uint8_t adcsToInterfaceManagerFlag[3];

    double_t OrbitJulianDate;//TeleOrbit
    float_t GPSJDDiff;
    float_t Orbit_dt;
    float_t R_InECI_x;
    float_t R_InECI_y;
    float_t R_InECI_z;

    float_t V_InECI_x;
    float_t V_InECI_y;
    float_t V_InECI_z;

    /////////////////////////////////////////////////

    float_t SunModelInECI_x;
    float_t SunModelInECI_y;
    float_t SunModelInECI_z;
    uint8_t res_p19;

}ExtendedP19;
// // **************************************************** SID20
typedef struct __attribute__((__packed__)) ExtendedTeleP20
{

    float_t IGRFInECI_x;
    float_t IGRFInECI_y;
    float_t IGRFInECI_z;
    uint8_t OrbitToInterfaceManagerFlag[2];//14
    float_t R_InECI_Prog_x;
    float_t R_InECI_Prog_y;
    float_t R_InECI_Prog_z;
    float_t V_InECI_Prog_x;
    float_t V_InECI_Prog_y;
    float_t V_InECI_Prog_z;

    float_t MTCustomDipole_x;//TeleSupervisor
    float_t MTCustomDipole_y;
    float_t MTCustomDipole_z;
    float_t RWCustomjRPM_x;
    float_t RWCustomjRPM_y;
    float_t RWCustomjRPM_z;
    float_t WBuffNorm;
    uint8_t CriteriaMode;
    uint32_t  CriteriaCounter; //*******
    uint8_t TaskID;
    uint8_t hardwareStatus;
    uint8_t supervisorToInterfaceManagerFlg;
    uint8_t OS_CPU_ARM_EXCEPT_RESETCnt;             //OBC
    uint8_t ARM_EXCEPT_UNDEF_INSTRCnt;              //OBC
    uint8_t OS_CPU_ARM_EXCEPT_SWICnt;               //OBC
    uint8_t OS_CPU_ARM_EXCEPT_PREFETCH_ABORTCnt;    //OBC
    uint8_t OS_CPU_ARM_EXCEPT_DATA_ABORTCnt;        //OBC
    uint8_t OS_CPU_ARM_EXCEPT_ADDR_ABORTCnt;        //OBC
    uint8_t OS_CPU_ARM_EXCEPT_FIQCnt;               //OBC
    uint8_t OS_CPU_ARM_EXCEPT_OtherCnt;             //OBC
    uint8_t MMC_Errorr_Counter;
    uint8_t MRAMOBCCriticalStatusDataFlag;
    uint8_t MRAMCPUExceptionFlag;
    uint8_t respart19[15];
}ExtendedP20;
// // **************************************************** SID21
typedef struct __attribute__((__packed__)) ExtendedTeleP21
{
    uint8_t     ID_CMD_REC_Return[100];
}ExtendedP21;
// // **************************************************** SID22
typedef struct __attribute__((__packed__)) ExtendedTeleP22
{
    uint8_t     ID_CMD_REC_Return[100];
}ExtendedP22;
// // **************************************************** SID23
typedef struct __attribute__((__packed__)) ExtendedTeleP23
{
    uint8_t     ID_CMD_REC_Return[100];
}ExtendedP23;
// // **************************************************** SID24
typedef struct __attribute__((__packed__)) ExtendedTeleP24
{
    uint8_t     ID_CMD_Executed_Return[100];
}ExtendedP24;
// // **************************************************** SID25
typedef struct __attribute__((__packed__)) ExtendedTeleP25
{
    uint8_t     ID_CMD_Executed_Return[100];
}ExtendedP25;
// // **************************************************** SID26
typedef struct __attribute__((__packed__)) ExtendedTeleP26
{
    uint8_t     ID_CMD_Executed_Return[100];
}ExtendedP26;
// // **************************************************** SID27
typedef struct __attribute__((__packed__)) ExtendedTeleP27
{
    uint8_t   SETPARAM_VAR_TYPE;
    uint32_t  SETPARAM_MEM_ADD;
    uint8_t  SETPARAM_MEM_VAL[8];
    uint8_t   Get_Param_length;
    uint32_t  Get_param_Start_Add;
    uint8_t   Get_param_Value[40];


    uint8_t IRPDU_Enable1;
    uint8_t IRPDU_Flag1;
    uint16_t IRPDU_CurrentSband1;
    uint16_t IRPDU_CurrentGps1;
    uint16_t IRPDU_CurrentProp8V1;
    uint16_t IRPDU_CurrentProp12V1;
    uint16_t IRPDU_CurrentMThesco;
    uint16_t IRPDU_TemperatureSen1;
    uint8_t IRPDU_CommandCounter1;
    uint8_t IRPDU_TelemetryCounter1;
    uint8_t IRPDU_Enable2;
    uint8_t IRPDU_Flag2;
    uint16_t IRPDU_CurrentSband2;
    uint16_t IRPDU_CurrentGps2;
    uint16_t IRPDU_CurrentProp8V2;
    uint16_t IRPDU_CurrentProp12V2;
    uint16_t IRPDU_CurrentMM_HMR;
    uint16_t IRPDU_TemperatureSen2;
    uint8_t IRPDU_CommandCounter2;
    uint8_t IRPDU_TelemetryCounter2;
    uint16_t CB_SwitchState;        //switch state for CB1
    uint16_t PmuState;              //switch state for Clyde
    uint16_t CBPmuState;            //switch state for IR PDU

    uint8_t   res[4];
}ExtendedP27;
// // **************************************************** SID28
// //TEMP------------------------------------------
typedef struct __attribute__((__packed__)) ExtendedTeleP28
{
        int16_t Analog_Sensor_1;
        int16_t Analog_Sensor_2;
        int16_t Analog_Sensor_3;
        int16_t Analog_Sensor_4;
        int16_t Analog_Sensor_5;//10
        int16_t Analog_Sensor_6;
        int16_t Analog_Sensor_7;
        int16_t Analog_Sensor_8;
        int16_t Analog_Sensor_9;
        int16_t Analog_Sensor_10;//20
        int16_t Analog_Sensor_11;
        int16_t Analog_Sensor_12;
        int16_t Analog_Sensor_13;
        int16_t Analog_Sensor_14;
        int16_t Analog_Sensor_15;//30
        int16_t Analog_Sensor_16;
        int16_t Analog_Sensor_17;
        int16_t Analog_Sensor_18;
        int16_t Analog_Sensor_19;
        int16_t Analog_Sensor_20;//40

        int8_t Temp_Sensor1;
        int8_t Temp_Sensor2;
        int8_t Temp_Sensor3;
        int8_t Temp_Sensor4;
        int8_t Temp_Sensor5;//50
        int8_t Temp_Sensor6;
        int8_t Temp_Sensor7;
        int8_t Temp_Sensor8;
        int8_t Temp_Sensor9;
        int8_t Temp_Sensor10;//60
        int8_t Temp_Sensor11;
        int8_t Temp_Sensor12;
        int8_t Temp_Sensor13;
        int8_t Temp_Sensor14;
        int8_t Temp_Sensor15;//70
        int8_t Temp_Sensor16;
        int8_t Temp_Sensor17;
        int8_t Temp_Sensor18;
        int8_t Temp_Sensor19;
        int8_t Temp_Sensor20;//80
        int8_t Temp_Sensor21;
        int8_t Temp_Sensor22;
        int8_t Temp_Sensor23;
        int8_t Temp_Sensor24;
        int8_t Temp_Sensor25;//90
        int8_t Temp_Sensor26;
        int8_t Temp_Sensor27;
        int8_t Temp_Sensor28;
        int8_t Temp_Sensor29;
        int8_t Temp_Sensor30;//100
        int8_t Temp_Sensor31;
        int8_t Temp_Sensor32;
        int8_t Temp_Sensor33;
        int8_t Temp_Sensor34;
        int8_t Temp_Sensor35;//10
        int8_t Temp_Sensor36;
        int8_t Temp_Sensor37;
        int8_t Temp_Sensor38;
        int8_t Temp_Sensor39;
        int8_t Temp_Sensor40;//20
        int8_t Temp_Sensor41;
        int8_t Temp_Sensor42;
        int8_t Temp_Sensor43;
        int8_t Temp_Sensor44;
        int8_t Temp_Sensor45;
        uint8_t   prop_Status;
        int16_t   prop_TankTemp;
        int16_t   prop_PlenumTemp;
        int16_t   prop_TankPressure;
        int16_t   prop_PlenumPressure;
        uint8_t    prop_Duration1;
        uint8_t    prop_Duration2;
        uint8_t    prop_Delay1;
        uint8_t    prop_Delay2;
        uint8_t    prop_PWM1;
        uint8_t    prop_PWM2;
}ExtendedP28;


#endif //SIDS_H
