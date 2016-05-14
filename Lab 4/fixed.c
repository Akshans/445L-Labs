// fixed.c
// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// August 5, 2015
// Possible main program to test the lab
// Feel free to edit this to match your specifications

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "string.h"
//#include "ST7735.h"
//#include "PLL.h"
#include "fixed.h"
#include "math.h"
#include "inc/tm4c123gh6pm.h"
/****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 
	void ST7735_sDecOut3(int32_t n)
	{
		if(n>9999 || n<-9999)
		{
			printf("*.***");
			return;
		}
		if(n<0)
		{
			printf("-");
			n = 0-n;
		}
		int32_t m = n;
		for(int i=3; i>=0; i--)
			{
				int num = (int)pow(10.0,(double)i);
				n = m/num; //1000/1000 = 1
				m=m % num; //1000 gets changed to 000	
				printf("%d",n);
				if(i ==3)
				{
					printf(".");
				}
			}		
	}

/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     2	  "  0.01"
    64	  "  0.25"
   100	  "  0.39"
   500	  "  1.95"
   512	  "  2.00"
  5000	  " 19.53"
 30000	  "117.19"
255997	  "999.99"
256000	  "***.**"
*/
void ST7735_uBinOut8(uint32_t n)
{
	if(n>256000 || n<0)
		{
			printf("*.***");
			return;
		}
		if(n<0)
		{
			printf("-");
			n = 0-n;
		}
		int32_t m = n;
		for(int i=3; i>=0; i--)
			{
				int num = (int)pow(10.0,(double)i);
				n = m/num; //1000/1000 = 1
				m=m % num; //1000 gets changed to 000	
				printf("%d",n);
				if(i ==3)
				{
					printf(".");
				}
			}		
	}

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
int32_t plotminX;
int32_t plotminY;
int32_t plotmaxX;
int32_t plotmaxY;

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
	{
	ST7735_PlotClear(minY, maxY);
	//printf("%s", title);
	ST7735_DrawString(10, 0, title, ST7735_Color565(255, 0, 0));
	plotminX = minX;
	plotminY = minY;
	plotmaxX = maxX;
	plotmaxY = maxY;
	
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[])
{
	int32_t Xval = 0;
	int32_t Yval = 0;
	double temp = -1.0;
	for(int32_t i = 0; i < num; i++){
		//draw pixel based on max X or Y and the size of the screen
		if(bufX[i] > plotminX && bufX[i] < plotmaxX){
			temp = (bufX[i] - plotminX) / (plotmaxX - plotminX);
		}
		Xval = temp * 127;
		if(bufY[i] > plotminY && bufY[i] < plotmaxY){
			temp = (bufY[i] - plotminY) / (plotmaxY - plotminY);
		}
		Yval = temp * 127;
		if(Xval >= 0 && Yval >= 0){
			ST7735_DrawPixel(Xval, Yval, ST7735_Color565(255, 0, 0));
		}
	}
}