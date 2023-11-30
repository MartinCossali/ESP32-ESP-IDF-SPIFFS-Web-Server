#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "station_info.h"
#include "esp_log.h"

infoStation_t               infoStation;
hwFlags_t                   hwFlags;
scu_addresses_t             scuAddresses;
additional_station_info_t   addStationInfo;
static const char *TAG = "STATION"; // TAG for debug

/*
*********************************** SCAME ************************************
**                                                                          **
**                           Functions Definition                           **
**                                                                          **
******************************************************************************
*/

/**
  * @brief  init station info structure  
  *         
  * @param  
  * @param  
  * 
  * @retval none
  */
void initStationInfo() {
  char* scuSn = getScuSerialNumber();
  char* prodSn = getProductSerialNumber();
  uint16_t typCurr = getTypicalCurrent();
  uint16_t simpCurr = getSimplifiedCurrent();
  uint16_t scuAddr = getScuAddress();

  if(strcmp(scuSn, "") == 0) {
     setScuSerialNumber("02:80:E1");
     ESP_LOGI(TAG, "SCU serial number initialized: %s", getScuSerialNumber());
  }
  if(strcmp(prodSn, "") == 0) {
    setProductSerialNumber("100xxxxxx");
    ESP_LOGI(TAG, "Product serial number initialized: %s", getProductSerialNumber());
  }
  if(typCurr == 0) {
    setTypicalCurrent(32);
    ESP_LOGI(TAG, "SCU typical current initialized: %lu", getTypicalCurrent());
  }
  if(simpCurr == 0) {
    setSimplifiedCurrent(16);
    ESP_LOGI(TAG, "SCU simplified current initialized: %lu", getSimplifiedCurrent());
  }
  if(scuAddr == 0) {
    setScuAddress(1);
    ESP_LOGI(TAG, "SCU modbus addres initialized: %d", getScuAddress());
  }

}

/**
  * @brief  set SCU serial number  
  *         
  * @param  char*: product serial number
  * 
  * @retval none
  */
void setScuSerialNumber(char* scuSerial) {
  strncpy((char *)&infoStation.serial, scuSerial, sizeof(infoStation.serial));
}

/**
  * @brief  get product serial number  
  *         
  * @param  none
  * 
  * @retval char*: pointer string product serial number
  */
char*  getScuSerialNumber(void) {
  return((char*)infoStation.serial);
}

/**
  * @brief  set product serial number  
  *         
  * @param  char*: product serial number
  * 
  * @retval none
  */
void setProductSerialNumber(char* prodSerial) {
  strncpy((char *)&infoStation.productSn, prodSerial, sizeof(infoStation.productSn));
}

/**
  * @brief  get product serial number  
  *         
  * @param  none
  * 
  * @retval char*: pointer string product serial number
  */
char*  getProductSerialNumber(void) {
  return((char*)infoStation.productSn);
}

/**
  * @brief  set hardware checks1 
  *         
  * @param  checks1Reg_e: hardware checks1
  * 
  * @retval none
  */
void setHardwareChecks1(checks1Reg_e hwChecks) {
  hwFlags.checks1 = hwChecks;
}

/**
  * @brief  get hardware checks1 
  *         
  * @param  none
  * 
  * @retval checks1Reg_e*: pointer string product serial number
  */
checks1Reg_e  getHardwareChecks1(void) {
  return hwFlags.checks1;
}

/**
  * @brief  set hardware checks2  
  *         
  * @param  checks2Reg_e: hardware checks2
  * 
  * @retval none
  */
void setHardwareChecks2(checks2Reg_e hwChecks) {
  hwFlags.checks2 = hwChecks;
}

/**
  * @brief  get hardware checks2 
  *         
  * @param  none
  * 
  * @retval checks2Reg_e*: pointer string product serial number
  */
checks2Reg_e  getHardwareChecks2(void) {
  return hwFlags.checks2;
}

/**
  * @brief  set hardware actuators  
  *         
  * @param  uint16_t: hardware actuators
  * 
  * @retval none
  */
void setHardwareActuators(uint16_t hwActuators) {
  hwFlags.actuators = hwActuators;
}

/**
  * @brief  get hardware actuators 
  *         
  * @param  none
  * 
  * @retval char*: pointer string product serial number
  */
uint16_t  getHardwareActuators(void) {
  return hwFlags.actuators;
}

/**
  * @brief  set socket type
  *         
  * @param  sck_wiring_e: socket type
  * 
  * @retval none
  */
void setSocketType(sck_wiring_e socket) {
  infoStation.socketType = socket;
}

/**
  * @brief  get socket type
  *         
  * @param  none
  * 
  * @retval sck_wiring_e: socket type of the station
  */
sck_wiring_e  getSocketType(void) {
  return infoStation.socketType;
}

/**
  * @brief  set energy meter type
  *         
  * @param  ener: energy meter type
  * 
  * @retval none
  */
void setEnergyMeterType(energy_meter_e energyMeter) {
  infoStation.emTypeInt = energyMeter;
}

/**
  * @brief  get socket type
  *         
  * @param  none
  * 
  * @retval energy_meter_e: energy meter type of the station
  */
energy_meter_e  getEnergyMeterType(void) {
  return infoStation.emTypeInt;
}

/**
  * @brief  set typical current  
  *         
  * @param  uint32_t: typical current
  * 
  * @retval none
  */
void setTypicalCurrent(uint32_t typCurrent) {
  infoStation.max_current = typCurrent;
}

/**
  * @brief  get typical current
  *         
  * @param  none
  * 
  * @retval uint32_t: typical current
  */
uint32_t  getTypicalCurrent(void) {
  return infoStation.max_current;
}

/**
  * @brief  set simplified current  
  *         
  * @param  uint32_t: simplified current
  * 
  * @retval none
  */
void setSimplifiedCurrent(uint32_t simCurrent) {
  infoStation.max_currentSemp = simCurrent;
}

/**
  * @brief  get simplified current
  *         
  * @param  none
  * 
  * @retval uint32_t: simplified current
  */
uint32_t  getSimplifiedCurrent(void) {
  return infoStation.max_currentSemp;
}

/**
  * @brief  set battery backup status
  *         
  * @param  batteryBackup_e: battery backup on off
  * 
  * @retval none
  */
void setBatteryBackup(batteryBackup_e battery) {
  infoStation.batteryBackup = battery;
}

/**
  * @brief  get battery backup status
  *         
  * @param  none
  * 
  * @retval batteryBackup_e: battery backup on/off
  */
batteryBackup_e  getBatteryBackup(void) {
  return infoStation.batteryBackup;
}

/**
  * @brief  set scu address
  *         
  * @param  uint16_t: scu address
  * 
  * @retval none
  */
void setScuAddress(uint16_t address) {
  scuAddresses.addressRs485 = address;
}

/**
  * @brief  get scu address
  *         
  * @param  none
  * 
  * @retval uint16_t: scu address
  */
uint16_t  getScuAddress(void) {
  return scuAddresses.addressRs485;
}

/**
  * @brief  set led strip code
  *         
  * @param  uint16_t: led strip code
  * 
  * @retval none
  */
void setLedStripCode(uint16_t ledStrip) {
  addStationInfo.ledStripCode = ledStrip;
}

/**
  * @brief  get led strip code
  *         
  * @param  none
  * 
  * @retval uint16_t: led strip code
  */
uint16_t  getLedStripCode(void) {
  return addStationInfo.ledStripCode;
}

/**
  * @brief  set scu operative mode
  *         
  * @param  uint16_t: operative mode EMUMAX0 or SEM
  * 
  * @retval none
  */
void setOperativeMode(uint16_t operativeMode) {
  addStationInfo.operativeMode = operativeMode;
}

/**
  * @brief  get scu operative mode
  *         
  * @param  none
  * 
  * @retval uint16_t: scu operative mode EMUMAX0 or SEM
  */
uint16_t  getOperativeMode(void) {
  return addStationInfo.operativeMode;
}