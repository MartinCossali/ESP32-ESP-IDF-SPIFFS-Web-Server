/*
*********************************** SCAME ************************************
**                                                                          **
**                                  DEFINES                                 **
**                                                                          **
******************************************************************************
*/
#define MAX_NAME_LENGTH 14                               /* Socket name max length                        */
#define MAX_SCHEDS 4                                     /* Schedulation max number                       */
/* Procedure parameters wifi module length */
#define   ACTIV_KEY_LENGTH              ((uint8_t)6)     /* Activation key max number of bytes.           */              
#define   USER_PIN_LENGTH               ((uint8_t)6)     /* User pin max number of bytes.                 */        
#define   MAX_ROUTER_SSID_LENGTH        ((uint8_t)32)    /* Router SSID max number of bytes               */
#define   MAX_ROUTER_PASS_LENGTH        ((uint8_t)64)    /* Router password max number of bytes allowed   */
#define   INSTALLER_PIN_LENGTH          ((uint8_t)6)     /* Installer pin max number of bytes             */        
#define   PRODUCT_SN_LENGTH             ((uint8_t)10)    /* Product serial number length.                 */    

/*
*********************************** SCAME ************************************
**                                                                          **
**           LOCAL MACROS, TYPEDEF, STRUCTURE, ENUM                         **
**                                                                          **
******************************************************************************
*/
/* Socket type enum                                                                                       */
typedef enum 
{
  PRESA_NESSUNA = 0x00,
  PRESA_TIPO_3A = 0x01,
  CONNETTORE_TETHERED_TIPO_1 = 0x02,
  CONNETTORE_TETHERED_TIPO_2 = 0x03,
  PRESA_SCHUKO = 0x04,
  PRESA_TIPO_2 = 0x05,
} sck_wiring_e;

/* Energy Meter enum                                                                                      */
typedef enum
{
  UNKNOW =       (uint8_t)0,
  NONE =         (uint8_t)0, 
  TA =           (uint8_t)1,
  MONO_GAVAZZI = (uint8_t)2,
  MONO_ALGO2,
  MONO_LOVATO,
  TRI_GAVAZZI,
  TRI_ALGO2,
  TRI_LOVATO,
  MONO_SCAME,
  TRI_SCAME,
  TA_TRI,
  MONO_SINAPSI = 0x0B
} energy_meter_e;

/* Operational mode enum                                                                                  */
typedef enum
{
  EVS_FREE_MODE = (uint8_t)(0x00),
  EVS_PERS_MODE = (uint8_t)(0x01),
  EVS_NET_MODE  = (uint8_t)(0x02),
  EVS_OCPP_MODE = (uint8_t)(0x03)
} evs_mode_en;

/* Station Operative mode enum                                                                            */
typedef enum
{
  MODE_MONO_PH_NO_PM =     ((uint8_t)0x00), /** measure: Etot, Time, Pist, L1                             */
  MODE_TRI_PH_NO_PM,                        /** measure: Etot, Time, Pist, L1, L2, L3                     */
  MODE_MONO_PH_PM,                          /** measure: Etot, Time, Pist, Pest, L1                       */
  MODE_TRI_PH_PM_BAL,                       /** measure: Etot, Time, Pist, Pest, L1, L2, L3, Pest1, Pest2, Pest3 */
  MODE_TRI_PH_PM_UMBAL                      /** measure: Etot, Time, Pist, Pest, L1, L2, L3               */
} modePwr_e;

/* Power managment umbalance enum                                                                         */
typedef enum 
{
PMNG_UNBAL_OFF  = (uint8_t)(0x00),
PMNG_UNBAL_ON   = (uint8_t)(0x01)
} power_management_unbal_en; 

/* Battery backup presence enum                                                                           */
typedef enum
{
  BATTERY_BACKUP_OFF  = ((uint8_t)0x00),
  BATTERY_BACKUP_ON   = ((uint8_t)0x01)
} batteryBackup_e;

/* 230V monitoring status flag enum                                                                       */
typedef enum
{
  DISABLED  = ((uint8_t)0x00),
  ENABLED   = ((uint8_t)0x01)
} statusFlag_e;

/* Schedulation struct                                                                                   */
typedef struct
{
  uint8_t id;
  uint8_t days;
  uint8_t start_hour;
  uint8_t start_min;
  uint8_t end_hour;
  uint8_t end_min;
  int32_t power;
  uint8_t enable;
} sck_schedule_t;

/* Scu address                                                                                           */
typedef struct
{
  uint16_t addressRs485;                         /* Serial address for master / slave mode               */ 
} scu_addresses_t;

/* Other station info additional                                                                         */
typedef struct
{
  uint16_t ledStripCode;                        /* 6, 9, 12 o 18 led                                     */               
  uint16_t operativeMode;                       /* 1 EMUMAX0, 0 SEM                                      */
} additional_station_info_t;

/* Boot event enum. Registro Modbus EVSE_BOOT_EVENTS_RO --> Address = 0x0643, 1 word                     */
typedef enum
{
  REGULAR_BOOT                   = 0x0000,       /* Boot regolare SCU                                    */
  FACTORY_RESET                  = 0x0001,       /* Reset di fabbrica SCU                                */ 
  ANOMALY_REBOOT                 = 0x0002,       /* Reboot anomalo SCU                                   */
  REGULAR_REBOOT                 = 0x0003,       /* Reboot regolare SCU                                  */
} bootReg_e;

/* REGISTRO EVSE_ERROR1_RO --> Address = 0x0404, 1 word                                                  */
typedef enum
{
  ERROR1_NONE                    = 0x0000,       /* Error 1 register, value NO error                     */
  ERROR1_RCDM                    = 0x0001,       /* Error 1 register, value RCDM error                   */ 
  ERROR1_LIDE                    = 0x0002,       /* Error 1 register, value LIDE error                   */
  ERROR1_VENT                    = 0x0004,       /* Error 1 register, value ventilation error            */
  ERROR1_BLCK                    = 0x0008,       /* Error 1 register, value socket block error           */
  ERROR1_BLEF                    = 0x0010,       /* Error 1 register, value Bluetooth error              */
  ERROR1_WIFI                    = 0x0020,       /* Error 1 register, value Wi-fi error                  */
  ERROR1_MIRR                    = 0x0040,       /* Error 1 register, value Mirror error                 */
  ERROR1_RCBO                    = 0x0080,       /* Error 1 register, value magnetothermic error         */
  ERROR1_CPSE                    = 0x0100,       /* Error 1 register, value CP short circuit error       */
  ERROR1_PPSE                    = 0x0200,       /* Error 1 register, value PP short circuit error       */
  ERROR1_CPLS                    = 0x0400,       /* Error 1 register, value CP lost error                */
  ERROR1_PPLS                    = 0x0800,       /* Error 1 register, value PP lost error                */
  ERROR1_VBUS                    = 0x1000,       /* Error 1 register, value low voltage supply error     */
  ERROR1_MFRE                    = 0x2000,       /* Error 1 register, value RFID reader error            */
  ERROR1_EMTR                    = 0x4000,       /* Error 1 register, value internal energy meter error  */
  ERROR1_OVCE                    = 0x8000,       /* Error 1 register, value overcurrent error            */
} error1Reg_e;

/* REGISTRO EVSE_ERROR2_RO --> Address = 0x0405, 1 word                                                  */
typedef enum
{
  ERROR2_NONE                    = 0x0000,       /* Error 2 register, value NO error                     */
  ERROR2_RCTE                    = 0x0001,       /* Error 2 register, value rectifier diode presence error */ 
  ERROR2_EMEX                    = 0x0002,       /* Error 2 register, value external energy meter error  */
  ERROR2_CHN2                    = 0x0004,       /* Error 2 register, value chain2  error                */
  ERROR2_PENF                    = 0x0010,       /* Error 2 register, value PEN error                    */
  ERROR2_HGTP                    = 0x0020,       /* Error 2 register, value high temperature error       */
} error2Reg_e;

/* REGISTRO EVSE_CHECK1_RW --> Address = 0x0026, 1 word                                                  */
typedef enum
{
  CHECK1_NONE                    = 0x0000,       /* Checks 1 register, value NO check                    */
  CHECK1_RCDM                    = 0x0001,       /* Checks 1 register, value RCDM check                  */ 
  CHECK1_LIDE                    = 0x0002,       /* Checks 1 register, value LIDE check                  */
  CHECK1_VENT                    = 0x0004,       /* Checks 1 register, value ventilation check           */
  CHECK1_BLCK                    = 0x0008,       /* Checks 1 register, value socket block check          */
  CHECK1_BLEF                    = 0x0010,       /* Checks 1 register, value Bluetooth check             */
  CHECK1_WIFI                    = 0x0020,       /* Checks 1 register, value Wi-fi check                 */
  CHECK1_MIRR                    = 0x0040,       /* Checks 1 register, value Mirror check                */
  CHECK1_RCBO                    = 0x0080,       /* Checks 1 register, value magnetothermic check        */
  CHECK1_CPSE                    = 0x0100,       /* Checks 1 register, value CP short circuit check      */
  CHECK1_PPSE                    = 0x0200,       /* Checks 1 register, value PP short circuit check      */
  CHECK1_CPLS                    = 0x0400,       /* Checks 1 register, value CP lost check               */
  CHECK1_PPLS                    = 0x0800,       /* Checks 1 register, value PP lost check               */
  CHECK1_VBUS                    = 0x1000,       /* Checks 1 register, value low voltage supply check    */
  CHECK1_MFRE                    = 0x2000,       /* Checks 1 register, value RFID reader check           */
  CHECK1_EMTR                    = 0x4000,       /* Checks 1 register, value internal energy meter check */
  CHECK1_OVCE                    = 0x8000,       /* Checks 1 register, value overcurrent check           */
} checks1Reg_e;

/* REGISTRO EVSE_CHECK2_RW --> Address = 0x0027, 1 word                                                  */
typedef enum
{
  CHECK2_NONE                    = 0x0000,       /* Checks 2 register, value NO check                    */
  CHECK2_RCTE                    = 0x0001,       /* Checks 2 register, value rectifier diode presence check */ 
  CHECK2_EMEX                    = 0x0002,       /* Checks 2 register, value external energy meter check */
  CHECK2_CHN2                    = 0x0004,       /* Checks 2 register, value chain2 check                */
  CHECK2_PENF                    = 0x0010,       /* Checks 2 register, value PEN check                   */
  CHECK2_HGTP                    = 0x0020,       /* Checks 2 register, value high temperature check      */
} checks2Reg_e;

/* Hardware checks and actuators struct                                                                  */
typedef struct 
{
  checks1Reg_e checks1;
  checks2Reg_e checks2;
  uint16_t actuators;
} hwFlags_t;

/* Station general parameters                                                                            */
typedef struct 
{
  char                          name[MAX_NAME_LENGTH];
  char                          serial[8];
  char                          firmware[24];
  sck_wiring_e                  socketType;
  int32_t                       max_current;
  int32_t                       max_currentSemp;
  energy_meter_e                emTypeInt;
  energy_meter_e                emTypeExt;
  evs_mode_en                   evs_mode;
  modePwr_e                     modePwr;
  uint8_t                       pmModeEn;
  power_management_unbal_en     pmUnbalEn;
  batteryBackup_e               batteryBackup;
  statusFlag_e                  v230MonFlag;
  /* Schedulation */
  sck_schedule_t                scheds[MAX_SCHEDS];
  /* Check */
  char                          key;  
  /* Added some parameters for Wi-Fi module to App communication                                        */
  char                          activKey[ACTIV_KEY_LENGTH];                     /* Activation key inserted in user manual                       */
  char                          userPin[USER_PIN_LENGTH];                       /* User Pin selected by the app user                            */
  char                          routerSsid[MAX_ROUTER_SSID_LENGTH];             /* Router SSID of a home network                                */
  char                          routerPass[MAX_ROUTER_PASS_LENGTH];             /* Router Password of a home network                            */
  char                          installerPin[INSTALLER_PIN_LENGTH];             /* Installer Pin memorized in Eeprom                            */
  uint8_t                       socketActivatedFlag;                            /* Flag to check if the socket has been already initialized     */
  bootReg_e                     bootEvent;                                      /* Enum for boot SCU event                                      */                
  char                          productSn[PRODUCT_SN_LENGTH];                   /* Product serial number 100xxxxxx                              */                        
  uint16_t                      maxRandomDelay;                                 /* Max random delay value set for UK market (1800 in future)    */
  uint8_t                       esitoUpdateFw;                                  /* 1: esito update FW OK, 0: altro                              */
  uint8_t                       antennaPresence;                                /* 1 se antenna presente da test produzione, 0 non presente     */
  /* Added some parameters for debug  */
  uint8_t                       sinapsiRS485Errors;                             /* counter error communication to SINAPSI                       */
  /* Added some parameters for restore wifi module to factory parameter                                  */
  uint8_t                       keyForRestoreModule;                            /* Key for restore module 0xA9                                  */
  uint8_t                       restoreModule;                                  /* restore default parameter wifi module when 1                 */
} infoStation_t;

/*                                                             
*********************************** SCAME ************************************
**                                                                          **
**                            External function                             **
**                                                                          **
******************************************************************************
*/
void initStationInfo(void);

/* Infostation get/set struct functions                                                                           */
char* getScuSerialNumber(void);
void setScuSerialNumber(char* scuSerial);
char* getProductSerialNumber(void);
void setProductSerialNumber(char* prodSerial);
checks1Reg_e getHardwareChecks1(void);
void setHardwareChecks1(checks1Reg_e hwChecks);
checks2Reg_e getHardwareChecks2(void);
void setHardwareChecks2(checks2Reg_e hwChecks);
uint16_t getHardwareActuators(void);
void setHardwareActuators(uint16_t hwChecks);
sck_wiring_e getSocketType(void);
void setSocketType(sck_wiring_e socket);
energy_meter_e getEnergyMeterType(void);
void setEnergyMeterType(energy_meter_e energyMeter);
uint32_t getTypicalCurrent(void);
void setTypicalCurrent(uint32_t typCurrent);
uint32_t getSimplifiedCurrent(void);
void setSimplifiedCurrent(uint32_t typCurrent);
batteryBackup_e getBatteryBackup(void);
void setBatteryBackup(batteryBackup_e battery);
uint16_t getScuAddress(void);
void setScuAddress(uint16_t address); 
uint16_t getLedStripCode(void);
void setLedStripCode(uint16_t ledStrip);
uint16_t getOperativeMode(void);
void setOperativeMode(uint16_t operativeMode);