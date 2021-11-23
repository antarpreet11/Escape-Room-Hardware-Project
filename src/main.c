// Sample code for ECE 198

// Written by Bernie Roehl, August 2021
// Edited by Antarpreet and Rahavan, November 2021 

void Initialize_Attempt();
int keypad_control(int ans);
void lcd_printing();
void conclusion(int n);
void lcda();

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions
#include "ece198.h"
#include "LiquidCrystal.h"

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LsED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED

    SerialSetup(9600);
    lcda();
}

void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
}

void lcd_printing() //function designed to print to the LCD and execute the question/answers part of the project
{
    int answers[4] = {3 , 15, 7, 11};
    LiquidCrystal(GPIOB, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6);
    setCursor(0,0);
    print("Welcome to Escape Room Games!");
    clear();
    setCursor(0,0);
    for(int k = 0; k<4; k++)
    {
        if(k == 0){
            print("What are the prime factors of 27?"); // Displays first question on LCD
            clear(); // Clears the question
            setCursor(0,0);
            print("A(3) B(2) C(9) D(27)"); // Displays options to choose from (Correct Answer: A)
            keypad_control(answers[k]); // Access Keypad_Control function, which allows for 3 attempts to select the correct response
        }
        else if(k == 1){
            print("Square root of 729?"); // Displays second question on LCD
            clear(); // Clears the question
            setCursor(0,0);
            print("A(29) B(37) C(9) D(27)"); // Displays options to choose from (Correct Answer: D)
            keypad_control(answers[k]); // Access Keypad_Control function, which allows for 3 attempts to selct the correct response
        }
        else if(k == 2){
            print("Syllables in Dinosaur?"); // Displays third question on LCD
            clear(); // Clears the question
            setCursor(0,0);
            print("A(4) B(3) C(2) D(1)"); // Displays options to choose from (Correct Answer: B)
            keypad_control(answers[k]); // Access Keypad_Control function, which allows for 3 attempts to select the correct response
        }
        else if(k == 3){
            print("How many factors does 218 have?"); // Displays fourth question on LCD
            clear(); // Clears question
            setCursor(0,0);
            print("A(12) B(7) C(16) D(10)"); // Displays options to choose from (Correct Answer: C)
            keypad_control(answers[k]); // Access Keypad_Control function, which allows for 3 attempts to select the correct response
        }

        clear();
        setCursor(0,0);
        HAL_Delay(300); 
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        // A blinking pattern is summoned after the question is completed
    }
}

int keypad_control(int ans)
{
    InitializeKeypad(); 
    int xxx = 0;
    int right = 0;
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6); // Turns off the green LED in the beginning
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8); // Turns off the red LED in the beginning
    while (xxx < 3) // Allows for up to 3 attempts to select the right answer
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        int input = ReadKeypad();
        int j = 0;
        if (input == ans)  // top-right key in a 4x4 keypad, usually 'A'
        {   while(j < 20)   // toggle LED on or off
            {
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	            HAL_Delay(50);
	            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	            HAL_Delay(50);
                j++;
            }
            right = 1;
        }
        else if(input != ans)
        {
            while(j < 15)   // toggle LED on or off
            {
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
	            HAL_Delay(50);
	            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
	            HAL_Delay(50);
                j++;
            }
        }
        if(right == 1)
        {
            break; // If the correct answer is selected within the 3 attempts, the LED will flash and you cannot select anymore times
        }    
        
        while (ReadKeypad() >= 0);  // wait until key is released
        xxx++;
    }
    return 0;
}

void conclusion(int n)
{
    
}

void lcda()
{
    LiquidCrystal(GPIOC, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);
    setCursor(0,0);
    print("Hello");
}