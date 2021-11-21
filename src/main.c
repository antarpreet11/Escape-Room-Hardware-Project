// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains code for a number of different examples.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines:

// #define BUTTON_BLINK
// #define LIGHT_SCHEDULER
// #define TIME_RAND
// #define KEYPAD
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
// #define COLOR_LED

void Initialize_Attempt();
int keypad_control(int ans);
void lcd_printing();

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





    lcd_printing();

#ifdef LIGHT_SCHEDULER
    // Turn on the LED five seconds after reset, and turn it off again five seconds later.

    while (true) {
        uint32_t now = HAL_GetTick();
        if (now > 5000 && now < 10000)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
        else
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);  // turn off LED
    }
#endif

#ifdef KEYPAD
    // Read buttons on the keypad and display them on the console.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    char str_input[100];
    int i = 0;
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    while (true)
    {   
        while (ReadKeypad() < 0);   // wait for a valid key
        SerialPutc(keypad_symbols[ReadKeypad()]);
        str_input[i] = keypad_symbols[ReadKeypad()]; // look up its ASCII symbol and send it to the hsot
        if(str_input[i] == 'A')
        {
            SerialPutc('S');
            SerialPutc('S');
        }
        i++;
          
        while (ReadKeypad() >= 0);  // wait until key is released
    }
#endif

#ifdef SEVEN_SEGMENT
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)
    
    Initialize7Segment();
    while (true)
        for (int i = 0; i < 10; ++i)
        {
            Display7Segment(i);
            HAL_Delay(1000);  // 1000 milliseconds == 1 second
        }
#endif

#ifdef KEYPAD_SEVEN_SEGMENT
    // Combines the previous two examples, displaying numbers from the keypad on the 7-segment display.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    Initialize7Segment();
    while (true)
    {
        int key = ReadKeypad();
        if (key >= 0)
            Display7Segment(keypad_symbols[key]-'0');  // tricky code to convert ASCII digit to a number
    }
#endif

#ifdef COLOR_LED
    // Cycle through all 8 possible colors (including off and white) as the on-board button is pressed.
    // This example assumes that the color LED is connected to pins D11, D12 and D13.

    // Remember that each of those three pins must go through a 220 ohm current-limiting resistor!
    // Also remember that the longest pin on the LED should be hooked up to GND.

    InitializePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    while (true) {
        for (int color = 0; color < 8; ++color) {
            // bottom three bits indicate which of the three LEDs should be on (eight possible combinations)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, color & 0x01);  // blue  (hex 1 == 0001 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, color & 0x02);  // green (hex 2 == 0010 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x04);  // red   (hex 4 == 0100 binary)

            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));   // wait for button press 
            while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button release
        }
    }
#endif
    return 0;
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}

void lcd_printing() //function designed to print to the LCD and execute the question/answers part of the project
{
    int answers[3] = {3 , 15, 7, 11};
    LiquidCrystal(GPIOB, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6);
    setCursor(0,0);
    print("Welcome to Escape Room Games!");
    clear();
    setCursor(0,0);
    for(int k = 0; k<4; k++){
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
            print("How many factors does 218 have?") // Displays fourth question on LCD
            clear(); // Clears question
            setCursor(0,0);
            print("A(12) B(7) C(16) D(10)") // Displays options to choose from (Correct Answer: C)
            keypad_control(answers[k]); // Access Keypad_Control function, which allows for 3 attempts to select the correct response
        }
        clear();
        setCursor(0,0);
        HAL_Delay(5000);
    }
}

int keypad_control(int ans)
{
    InitializeKeypad(); 
    int xxx = 0;
    int right = 0;
    while (xxx < 4) // Allows for up to 3 attempts to select the right answer
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        int input = ReadKeypad();
        int j = 0;
        if (input == ans)  // top-right key in a 4x4 keypad, usually 'A'
        {    while(j < 20)   // toggle LED on or off
            {
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	            HAL_Delay(50);
	            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	            HAL_Delay(50);
                j++;
            }
            right = 1;
        }
        else if(input != ans)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	        HAL_Delay(1000);
	        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
