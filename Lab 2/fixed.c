#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "inc/tm4c123gh6pm.h"
#include "math.h"

/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 */ 
void ST7735_sDecOut2(int32_t n){
	int32_t decDigits = n % 100; //stores last 2 digits
	int32_t intDigits = n / 100; //stores first 3 digits
	if(decDigits < 10){
		//insert zero before digit
		printf("%d.0%d", intDigits, decDigits);
	}
	else{
		printf("%d.%d", intDigits, decDigits);
	}
}

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
void ST7735_uBinOut8(uint32_t n){
	if(n >= 256000){
		printf("***.**");
		
	}
	else{
		double temp = (n / 256.00);	//convert to fixed point value
		int32_t real = temp * 100;		//chop off decimals
		ST7735_sDecOut2(real);	//print to screen
	}
	/*
	int32_t temp2 = temp * 100; //gives number as ##### format
	int32_t decDigits = temp2 % 100; //stores last 2 digits
	int32_t intDigits = temp2 / 100; //stores first 3 digits
	if(decDigits < 10){
		//insert zero before digit
		printf("%d.0%d", intDigits, decDigits);
	}
	else{
		printf("%d.%d", intDigits, decDigits);
	}
	*/
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

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_PlotClear(minY, maxY);
	ST7735_FillScreen(0);
	//printf("%s", title);
	ST7735_DrawString(0, 0, title, ST7735_Color565(255, 0, 0));
	plotminX = minX;
	plotminY = minY;
	plotmaxX = maxX;
	plotmaxY = maxY;
	//draw axes, cross at 0,0 in plot
	//x axis first
	int32_t Yval = 0;
	double temp = 0.000;
	temp = (0.000 - (double)plotminY) / ((double)plotmaxY - (double)plotminY);
	Yval = (-1)*(temp * 127) + 160;
	for(int i = 0; i < 128; i++){
		ST7735_DrawPixel(i, Yval, ST7735_Color565(0, 0, 255));
	}
	//now y axis
	int32_t Xval = 0;
	temp = (0.000 - (double)plotminX) / ((double)plotmaxX - (double)plotminX);
	Xval = temp * 127;
	for(int i = 0; i < 128; i++){
		ST7735_DrawPixel(Xval, i + 32, ST7735_Color565(0, 0, 255));
	}
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
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t Xval = 0;
	int32_t Yval = 0;
	double temp = -1.000;
	for(int32_t i = 0; i < num; i++){
		//draw pixel based on max X or Y and the size of the screen
		if(bufX[i] >= plotminX && bufX[i] <= plotmaxX){
			temp = ((double)bufX[i] - (double)plotminX) / ((double)plotmaxX - (double)plotminX);
		}
		Xval = temp * 127;
		if(bufY[i] >= plotminY && bufY[i] <= plotmaxY){
			temp = ((double)bufY[i] - (double)plotminY) / ((double)plotmaxY - (double)plotminY);
		}
		Yval = (-1)*(temp * 127) + 160;
		if(Xval >= 0 && Yval >= 0){
			ST7735_DrawPixel(Xval, Yval, ST7735_Color565(255, 0, 0));
		}
	}
}











