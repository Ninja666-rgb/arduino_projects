#include <Wire.h>
//MPU9255 Register Map
#define    _DEVICE_Addr  0x68
#define   _MAG_Addr 0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

#define   SMPLRT_DIV  0x19
#define   CONFIG    0x1A
#define   GYRO_CONFIG 0x1B
#define   ACC_CONFIG  0x1C
#define   INT_PIN_CFG 0x37
#define   PWR_MGMT_1  0x6B
#define   PWR_MGMT_2  0x6C
#define   MAG_CNTL1 0x0A
#define   MAG_ST1   0x02
#define   MAG_ST2   0x09

//Accelerometer register to access
#define   ACC_DATA_REG    0x3B

//Gyroscope register to access
#define   GYRO_DATA_REG   0x43

//Magnetometer register to access
#define   MAG_DATA_REG    0x03

//Temperature registers
#define   TEMP_H          0x41
#define   TEMP_L          0x42

#define    FIFO_ENABLE       0x23

// Resolution for GYRO_FULL_SCALE_2000_DPS
#define GYRO_RESOLUTION   0.0610351

// Resolution for ACC_FULL_SCALE_2_G
#define ACC_RESOLUTION    0.0000610351

// Resolution for MAGNETOMETER (-4800, 4800) microT
#define MAG_RESOLUTION    1.4994
#define SAMPLING_RATE 50   // samples per second

static float accData[3], gyroData[3], magData[3];
static float *ACC, *GYRO, *MAG;

void i2c_writebyte(uint8_t Addr, uint8_t Reg, uint8_t Data){
  Wire.beginTransmission(Addr);
  Wire.write(Reg);
  Wire.write(Data);
  Wire.endTransmission();
}


byte i2c_readbyte( uint8_t Addr, uint8_t Reg){
  Wire.beginTransmission(Addr);
  Wire.write(Reg);
  Wire.endTransmission();
  delay(1); // may not be needed
  Wire.requestFrom(Addr, 1);
  if (Wire.available()) {
    uint8_t Var = Wire.read();
    return Var;
  }
}


void i2c_init()
{
  // Init gyro.
  i2c_writebyte(_DEVICE_Addr, GYRO_CONFIG, GYRO_FULL_SCALE_2000_DPS);

  // Init accelerometer.
  i2c_writebyte(_DEVICE_Addr, ACC_CONFIG, ACC_FULL_SCALE_2_G);
  i2c_writebyte(_DEVICE_Addr, CONFIG, 7);

  // Enable bypass mode. --------------------------------
  i2c_writebyte(_DEVICE_Addr, INT_PIN_CFG, 0x2);

  // Set the device to be active, as it could be in sleep mode.
  i2c_writebyte(_MAG_Addr, PWR_MGMT_1, 0x0);
  i2c_readbyte(_MAG_Addr, PWR_MGMT_2);
}

float *read_acc()
{
  int accOutput[6];
  int16_t tempAcc[3];

  accOutput[0] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG);
  accOutput[1] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG + 1);
  accOutput[2] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG + 2);
  accOutput[3] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG + 3);
  accOutput[4] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG + 4);
  accOutput[5] = i2c_readbyte(_DEVICE_Addr, ACC_DATA_REG + 5);


  tempAcc[0] = (((int16_t)accOutput[0] << 8) | accOutput[1]);
  tempAcc[1] = (((int16_t)accOutput[2] << 8) | accOutput[3]);
  tempAcc[2] = (((int16_t)accOutput[4] << 8) | accOutput[5]);

  accData[0] = (float) tempAcc[0] * ACC_RESOLUTION;
  accData[1] = (float) tempAcc[1] * ACC_RESOLUTION;
  accData[2] = (float) tempAcc[2] * ACC_RESOLUTION;

  return accData;
}

float *read_gyro()
{
  int gyroOutput[6];
  int16_t tempGyro[3];

  gyroOutput[0] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG);
  gyroOutput[1] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG + 1);
  gyroOutput[2] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG + 2);
  gyroOutput[3] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG + 3);
  gyroOutput[4] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG + 4);
  gyroOutput[5] = i2c_readbyte(_DEVICE_Addr, GYRO_DATA_REG + 5);


  tempGyro[0] = (((int16_t)gyroOutput[0] << 8) | gyroOutput[1]);
  tempGyro[1] = (((int16_t)gyroOutput[2] << 8) | gyroOutput[3]);
  tempGyro[2] = (((int16_t)gyroOutput[4] << 8) | gyroOutput[5]);

  gyroData[0] = (float) tempGyro[0] * GYRO_RESOLUTION;
  gyroData[1] = (float) tempGyro[1] * GYRO_RESOLUTION;
  gyroData[2] = (float) tempGyro[2] * GYRO_RESOLUTION;

  return gyroData;
}

float *read_mag()
{
  int magOutput[6], x;
  int16_t tempMag[3];

  i2c_writebyte(_MAG_Addr, MAG_CNTL1, 0x1);

  do {
    x = i2c_readbyte(_MAG_Addr, MAG_ST1);
  } while ((x & 1) != 1);

  magOutput[0] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG);
  magOutput[1] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG + 1);
  magOutput[2] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG + 2);
  magOutput[3] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG + 3);
  magOutput[4] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG + 4);
  magOutput[5] = i2c_readbyte(_MAG_Addr, MAG_DATA_REG + 5);

  tempMag[0] = (((int16_t)magOutput[1] << 8) | magOutput[0]);
  tempMag[1] = (((int16_t)magOutput[3] << 8) | magOutput[2]);
  tempMag[2] = (((int16_t)magOutput[5] << 8) | magOutput[4]);

  magData[0] = (float) tempMag[0] * MAG_RESOLUTION;
  magData[1] = (float) tempMag[1] * MAG_RESOLUTION;
  magData[2] = (float) tempMag[2] * MAG_RESOLUTION;

  return magData;
}