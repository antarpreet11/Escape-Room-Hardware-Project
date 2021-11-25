// Header file for ece198.c

// Written by Bernie Roehl, July 2021

#include "stm32f4xx_hal.h"

void InitializePin(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate);

HAL_StatusTypeDef SerialSetup(uint32_t baudrate);

char SerialGetc();
void SerialGets(char *buff, int maxlen);

void SerialPutc(char c);
void SerialPuts(char *ptr);

// macro for reading an entire port (we provide this so students don't need to know about structs)
#define ReadPort(port) (port->IDR)

void InitializeKeypad();
int ReadKeypad();
