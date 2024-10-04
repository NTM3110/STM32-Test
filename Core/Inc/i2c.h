#ifdef I2C_H
#define I2C_H

void i2c1_init();
void I2C_byteRead(char sAddr, char mAddr, char* data);
void I2C_burstRead(char sAddr, char mAddr, int n, char* data);
void I2C1_burstWrite(char sAddr, char mAddr, int n, char* data); `

#endif