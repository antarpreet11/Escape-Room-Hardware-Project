// Support routines for ECE 198 (University of Waterloo)

// Written by Bernie Roehl, July 2021

#include <stdbool.h>  // for bool datatype

#include "ece198.h"

///////////////////////
// Initializing Pins //
///////////////////////

// Initialize a pin (or pins) to a particular mode, with optional pull-up or pull-down resistors
// and possible alternate function
// (we use this so students don't need to know about structs)

void InitializePin(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pins;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullups;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/////////////////
// Serial Port //
/////////////////

UART_HandleTypeDef UART_Handle;  // the serial port we're using

// initialize the serial port at a particular baud rate (PlatformIO serial monitor defaults to 9600)

HAL_StatusTypeDef SerialSetup(uint32_t baudrate)
{
    __USART2_CLK_ENABLE();        // enable clock to USART2
    __HAL_RCC_GPIOA_CLK_ENABLE(); // serial port is on GPIOA

    GPIO_InitTypeDef GPIO_InitStruct;

    // pin 2 is serial RX
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // pin 3 is serial TX (most settings same as for RX)
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // configure the serial port
    UART_Handle.Instance = USART2;
    UART_Handle.Init.BaudRate = baudrate;
    UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Handle.Init.StopBits = UART_STOPBITS_1;
    UART_Handle.Init.Parity = UART_PARITY_NONE;
    UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_Handle.Init.Mode = UART_MODE_TX_RX;

    return HAL_UART_Init(&UART_Handle);
}

// wait for a character to arrive from the serial port, then return it

char SerialGetc()
{
    while ((UART_Handle.Instance->SR & USART_SR_RXNE) == 0)
        ;                             // wait for the receiver buffer to be Not Empty
    return UART_Handle.Instance->DR;  // return the incoming character
}

// send a single character out the serial port

void SerialPutc(char c)
{
    while ((UART_Handle.Instance->SR & USART_SR_TXE) == 0)
        ;                          // wait for transmitter buffer to be empty
    UART_Handle.Instance->DR = c;  // send the character
}

// write a string of characters to the serial port

void SerialPuts(char *ptr)
{
    while (*ptr)
        SerialPutc(*ptr++);
}

// get a string of characters (up to maxlen) from the serial port into a buffer,
// collecting them until the user presses the enter key;
// also echoes the typed characters back to the user, and handles backspacing

void SerialGets(char *buff, int maxlen)
{
    int i = 0;
    while (1)
    {
        char c = SerialGetc();
        if (c == '\r') // user pressed Enter key
        {
            buff[i] = '\0';
            SerialPuts("\r\n"); // echo return and newline
            return;
        }
        else if (c == '\b') // user pressed Backspace key
        {
            if (i > 0)
            {
                --i;
                SerialPuts("\b \b"); // overwrite previous character with space
            }
        }
        else if (i < maxlen - 1) // user pressed a regular key
        {
            buff[i++] = c;  // store it in the buffer
            SerialPutc(c);  // echo it
        }
    }
}

/////////////////////
// Keypad Scanning //
/////////////////////

struct { GPIO_TypeDef *port; uint32_t pin; }
rows[] = {
    { GPIOC, GPIO_PIN_7 },
    { GPIOA, GPIO_PIN_9 },
    { GPIOA, GPIO_PIN_8 },
    { GPIOB, GPIO_PIN_10 }
},
cols[] = {
    { GPIOB, GPIO_PIN_4 },
    { GPIOB, GPIO_PIN_5 },
    { GPIOB, GPIO_PIN_3 },
    { GPIOA, GPIO_PIN_10 }
};

void InitializeKeypad() {
    // rows are outputs, columns are inputs and are pulled low so they don't "float"
    for (int i = 0; i < 4; ++i) {
        InitializePin(rows[i].port, rows[i].pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
        InitializePin(cols[i].port, cols[i].pin, GPIO_MODE_INPUT, GPIO_PULLDOWN, 0);
     }
}

int ReadKeypad() {
    // scan a 4x4 key matrix by applying a voltage to each row in succession and seeing which column is active
    // (should work with a 4x3 matrix, since last column will just return zero)
    for (int row = 0; row < 4; ++row) {
        // enable the pin for (only) this row
        for (int i = 0; i < 4; ++i)
            HAL_GPIO_WritePin(rows[i].port, rows[i].pin, i == row);  // all low except the row we care about
        for (int col = 0; col < 4; ++col)  // check all the column pins to see if any are high
            if (HAL_GPIO_ReadPin(cols[col].port, cols[col].pin))
                return row*4+col;
    }
    return -1;  // none of the keys were pressed
}

