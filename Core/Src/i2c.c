#include "i2c.h"
#include "stm32f407xx.h"

#define I2C1_EN         (1U << 21)
#define OTP_EN          (1U << 1)
#define GPIOB_EN        (1U << 1)
#define I2C1_100Hz      80
#define I2C1_TRISE      17
#define I2C1_PE         (1U << 0)

#define SR2_BUSY        (1U << 1)
#define CR1_START       (1U << 8)
#define SR1_SB          (1U << 0)
#define SR1_ADDR        (1U << 1)
#define SR1_TXE         (1U << 7)
#define CR1_ACK         (1U << 10)
#define SR1_RXNE        (1U << 6)
#define SR1_BTF         (1U << 2)

void i2c1_init(){
    //Enable APB1 clock frequency
    RCC->APB1ENR |= I2C1_EN;

    //Enable clock access to GPIOB
    RCC->AHB1ENR |= GPIOB_EN;

    //Configure PB8 and PB9 output to alternate function
    GPIOB->MODER &= ~(1U << 16)
    GPIOB->MODER |= (1U << 17)
    GPIOB->MODER &= ~(1U << 18)
    GPIOB->MODER |= (1U << 19)

    //Configure open-drain setting 
    GPIOB->OTYPER |= (1U << 8)
    GPIOB->OTYPER |= (1U << 9)

    //Enable Pull-Up
    GPIOB->PUPDR |= (1U << 16)
    GPIOB->PUPDR &= ~(1U <<17)

    GPIOB->PUPDR |= (1U << 18)
    GPIOB->PUPDR &= ~(1U << 19)
 
    //Enter reset mode
    I2C1->CR1 |= (1U << 15)
    
    //Come out of reset mode 
    I2C1->CR1 &= ~ (1U << 15)
    
    //Set Peripheral clock frequency 
    I2C->CR2 |= (1U << 4)

    //Set I2C to standard mode, 100kHz clock
    I2C->CCR = I2C1_100Hz

    I2C1->TRISE = I2C1_TRISE;

    //Enable I2C1 module
    I2C1->CR = I2C1_PE;
}

void I2C_byteRead(char sAddr, char mAddr, char* data){
    volatile int tmp;

    // Wait until bus not busy SR2
    while(I2C1->SR2 & SR2_BUSY){}

    // Start the communication by set START bit to high. CR1
    I2C1->CR1 |=  CR1_START;
    
    // - Wait for Start bit to be 1 SR1
    while (!(I2C1->SR1 & SR1_SB)){}

    // - Transmit the ADRRESS +Write
    I2C1->DR = sAddr << 1;
    
    // - Wait FOR THE ADDRESS FLAG TO BE SET
    while (!(I2C1->SR1 & SR1_ADDR)){}
    
    // - clear the address flag
    //NOTE: Rading SR2 after reading SR1 will clear ADDR flag
    tmp = I2C1->SR2; 

    //Send memory address
    I2C1->DR = mAddr

    //Wait until transmitter empty. EMPTY = 0
    while (!(I2C1->SR1 & SR1_TXE)){} 
    
    //Generate restart
    I2C1->CR1 |= CR1_START;
    
    //Wait until start flag is set
    while (!(I2C1->SR1 & SR1)){}
    
    //Transmit slave address + Read
    I2C1->DR = sAddr << 1 | 1;
    
    //Wait until addr flag is set 
    while (!(I2C1->SR1 & SR1_ADDR)){}

    //Disable Acknowledge 
    I2C1->CR1 &= ~CR1_ACK;

    tmp = I2C1->SR2; //Read the SR2 again to clear ADDR flag

    I2C1->CR1 |= I2C_CR1_STOP;

    while (!(I2C1->SR1 & SR1_RXNE)){}

    *data++ = I2C1->DR;
    
}
void I2C_burstRead(char sAddr, char mAddr, int n, char* data){
    volatile int tmp;

    // Wait until bus not busy SR2
    while(I2C1->SR2 & SR2_BUSY){}

    // Start the communication by set START bit to high. CR1
    I2C1->CR1 |=  CR1_START;
    
    // - Wait for Start bit to be 1 SR1
    while (!(I2C1->SR1 & SR1_SB)){}

    // - Transmit the ADRRESS +Write
    I2C1->DR = sAddr << 1;
    
    // - Wait FOR THE ADDRESS FLAG TO BE SET
    while (!(I2C1->SR1 & SR1_ADDR)){}
    
    // - clear the address flag
    //NOTE: Rading SR2 after reading SR1 will clear ADDR flag
    tmp = I2C1->SR2; 

    //Send memory address
    I2C1->DR = mAddr

    //Wait until transmitter empty. EMPTY = 0
    while (!(I2C1->SR1 & SR1_TXE)){} 
    
    //Generate restart
    I2C1->CR1 |= CR1_START;
    
    //Wait until start flag is set
    while (!(I2C1->SR1 & SR1)){}
    
    //Transmit slave address + Read
    I2C1->DR = sAddr << 1 | 1;
    
    //Wait until addr flag is set 
    while (!(I2C1->SR1 & SR1_ADDR)){}

    tmp = I2C1->SR2; //Read the SR2 again to clear ADDR flag
    
    I2C1->CR1 |= CR1_ACK;

    while (n > 0U)
    {
        if (n == 1U){

            //Disable ACK
            I2C1->CR1 &= ~(CR1_ACK);

            //Generate Stop
            I2C1->CR1 |= I2C_CR1_STOP

            //Wait for RXNE 
            while (I2C1->SR1 & SR1_RXNE){}

            //Get data
            *data++ = I2C1->DR;
            break;
        }
        else{
            //Wait for RXNE 
            while (I2C1->SR1 & SR1_RXNE){}

            //Get data
            *data++ = I2C1->DR;

            n--;
        }
    }
}

void I2C1_burstWrite(char sAddr, char mAddr, int n, char* data){
    volatile int tmp;
    
    //Wait until bus is not busy
    while (I2C1->SR2 & SR2_BUSY){}
    
    //Start the communication by set START bit to high.
    I2C1->CR1 |= CR1_START;

    //Wait for START bit to be set.
    while (!(I2C1->SR & SR1_SB)){}

    //Send the slave address + write
    I2C1->DR = (sAddr << 1) | 1; 

    //Wait for the address to be sent
    while (!(I2C1->SR1 & SR1_ADDR)){}

    //CLEAR addr flag
    tmp = I2C1->SR2;

    while (!(I2C1->SR1 & SR1_TXE)){}

    I2C1->DR = mAddr;

    for (int i = 0; i < n; i++)
    {
        while (!(I2C1->SR1 & SR1_TXE)){}
        I2C1->DR = *data++;
    }
    while (!(I2C1->SR1 & SR1_BTF)){}

    I2C1->CR1 |= I2C_CR1_STOP;
}
