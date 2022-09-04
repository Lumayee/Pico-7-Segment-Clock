//
// Created by Paul Weber on 04.09.22.
//

#include <stdio.h>
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"

int main(void);
void display(int digit, int number, int FIRST_GPIO);
void display_time(datetime_t *time);



int Button = 0; //Button to set the Time (which is also used as a interrupt)


int main(void){

    // Start on Friday 5th of June 2020 15:45:00
    datetime_t time = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
    };


    //Configuring RTC ----------------------------------------------------------------

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&time);

    while(1){
        display_time(&time);
    }

    return 0;
}



void display(int digit, int number, int FIRST_GPIO){
/*

    Structure

    --A--
    F   B
    --G--
    E   C
    --D--   H

    Pin 0 is First_GPIO

    Pin 0 = A
    Pin 1 = B
    Pin 2 = C
    Pin 3 = D
    Pin 4 = E
    Pin 5 = F
    Pin 6 = G
    Pin 7 = H

    Pins fit Perfectly into a single Byte, i created a Bitmask for all the Numbers

    */

// This array converts a number 0-9 to a bit pattern to send to the GPIOs
    int number_bits[10] = {
            0x3f,  // 0
            0x06,  // 1
            0x5b,  // 2
            0x4f,  // 3
            0x66,  // 4
            0x6d,  // 5
            0x7d,  // 6
            0x07,  // 7
            0x7f,  // 8
            0x67   // 9
    };

    int digit_bits[10] = {
            0xE,  // 0
            0xD,  // 1
            0xB,  // 2
            0x7,  // 3
    };

    int number_mask = 0xFF << FIRST_GPIO; //get the right position for the pins

    gpio_put_masked(number_mask, number_bits[number]); //Put the bit mask on GPIO

    int digit_mask = 0xF << FIRST_GPIO+8;

    gpio_put_masked(digit_mask, digit_bits[digit]); //Put the bit mask on GPIO
}

void display_time(datetime_t *time){

    int digit[4]; //create an array for all the numbers

    rtc_get_datetime(time);

    //Calculate the Digits with modulo
    digit[1]= time->hour%10;
    // If the number is smaller than 1, only 1 Digit exists, the other one will be set to 0
    if(time->hour > 9){
        digit[0] = (time->hour - digit[1])/10;
    }
    else{
        digit[0] = 0;
    }

    //Calculate the Digits with modulo
    digit[3]= time->min%10;
    if(time->min > 9){
        // If the number is smaller than 1, only 1 Digit exists, the other one will be set to 0
        digit[2] = (time->min - digit[1])/10;
    }
    else{
        digit[2] = 0;
    }

    //Go through the 4 Digits and Display them
    for(int i; i<4; i++){
        display(i, digit[i], 1);
        sleep_ms(25);
    }
}