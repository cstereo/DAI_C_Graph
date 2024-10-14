//====================================================================================
// DAI computer demos to use c functions related to screen (graphic and text)
//====================================================================================
// REMARKS
// 1) Mandelbrot requires to be compiled with mbf32 math library due to insufficient precision of dai32 library
// Command line example : zcc.exe +dai -m -v -s -create-app --list --math-mbf32 -Cz--loud a.c >> a.txt
// 2) Static variables may be required for some programs due to stack limited size (128 bytes)
// This is the case for example when using printf (which requires some delay to let charracter be processed)
// If necessary, stack pointer (SP) can be move to an other memory position


//====================================================================================
// Includes
//====================================================================================
#include <stdint.h>
#include <stdio.h>


//====================================================================================
// Function declarations
//====================================================================================

// -----------------------------------------------------------------------------------
// Function for test
// -----------------------------------------------------------------------------------
void mandelbrot(void); // Mandelbrot fractal graphic, around 4 hours to run, can be stopped using a lon push on break key
void test_graphics(void); // Plot some simple graphic figures
void test_texts (void); // In text mode, change color and move cursor

// -----------------------------------------------------------------------------------
// Functions to emulate Dai basic commands
// -----------------------------------------------------------------------------------
// differences from basic commands : no error is returned 
void dai_mode(uint8_t m); // change graphic modes
void dai_colorg(uint8_t C0,uint8_t C1,uint8_t C2,uint8_t C3); // Default graphic colors for 4 color mode
void dai_dot(uint16_t x, uint8_t y, uint8_t c); // Plot a dot
void dai_draw(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint8_t c); // Plot a line
void dai_fill(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint8_t c); // Plot a rectangle
uint16_t dai_xmax(void); // Get max x of current graphic mode
uint8_t dai_ymax(void); // Get max y of current graphic mode
uint8_t dai_scrn(uint8_t x, uint8_t y);  // Get color of a dot in graphic mode

void dai_colort(uint8_t C0,uint8_t C1,uint8_t C2,uint8_t C3); // Default text colors
void dai_cursor(uint8_t x, uint8_t y); // Set cursor position in text mode
void dai_clearscreen(void); // Clear screen in text mode by sending charater 0x12 
uint8_t dai_curx(void); // Get cursor x position in text mode
uint8_t dai_cury(void); // Get cursor y position in text mode


// -----------------------------------------------------------------------------------
// Functions for debug
// -----------------------------------------------------------------------------------
void savereg_x2000(void); // Save registers at 0x2000
void savereg_x2010(void); // Save registers at 0x2010
uint16_t get_SP(void); // Return SP to check for stack overflow
void change_Stack(void); // Example to execute function requiring larger stack 


//====================================================================================
// Main
//====================================================================================
// Uncomment desired example function  
void main()
{

	// mandelbrot(); 
	test_graphics ();
	// test_texts();
}


// -----------------------------------------------------------------------------------
// Mandelbrot
// -----------------------------------------------------------------------------------
// Create Mandelbrot fractal (takes 4 hours) with 4 colors in the largest DAI definiion
// Static variables are used essentially to avoid stack overflow
// Requires to be compiled with mbf32 math library due to insufficient precision of dai32 library
// Does not exit
// On a DAI can exit with a long push on break
void mandelbrot(void)
{
	#define xmax 335
	#define ymax 255
	#define MAX_ITERATIONS 45 
	#define L_COLOR1 12
	#define L_COLOR2 7
	#define Colorg0 15
	#define Colorg1 5
	#define Colorg2 10
	#define Colorg3 3

	#define a0 (-1.85) 
	#define b0 (+0.55) 
	#define c0 (-1.2) 
	#define d0 (1.2) 

	#define e (4.0) 

    static double g, h, i, j;
    static uint16_t x, color ;
    static uint8_t y, k ;
    static double l, m, n, o, p;

	dai_colorg(Colorg0,Colorg1,Colorg2,Colorg3); 
	dai_mode (0x0A);

    g = (b0 - a0) / (double)xmax;
    h = (d0 - c0) / (double)ymax;

	y = ymax / 2 ;
    do{
        j = (double)y * h + c0;
		x = xmax;
        do {
            i = (double)x * g + a0;
            k = 0;
            l = 0.0;
            m = 0.0;
            n = 0.0;
            o = 0.0;
            while ((k < MAX_ITERATIONS) & ((n + o) < e) ) //Iterates
            {
                p = n - o + i;
                m = 2.0 * l * m + j;
                l = p;
                n = l * l;
                o = m * m;
				k++;
            }
			color = (k==MAX_ITERATIONS?Colorg3:(k>L_COLOR1?Colorg2:(k>L_COLOR2?Colorg1:Colorg0))) ;

			dai_dot((uint16_t)x,  y, (uint8_t) color);
			dai_dot((uint16_t)x, (ymax - y), (uint8_t) color) ;
			x-- ;
        } while (x!=0) ;
		y--;
    } while (y!=0) ;

	// Draw a border
	dai_draw(0,0, 0, ymax, Colorg3) ;
	dai_draw(0,ymax, xmax, ymax, Colorg3) ;
	dai_draw(xmax,0, xmax, ymax, Colorg3) ;
	dai_draw(xmax,0, 0, 0, Colorg3) ;
	
	while(1); 
}


// -----------------------------------------------------------------------------------
// test_graphics
// -----------------------------------------------------------------------------------
// Plot some simple graphic figures
// Does not exit
// On a DAI can exit with a long push on break
void test_graphics(void)
{
	static uint16_t c ;
	dai_colorg(15,5,10,3); // white background, green, orange, red
	dai_mode(0x0B); // Mode 6
	dai_fill (0,0, dai_xmax(),20,3) ; // red rectangle at bottom
	dai_draw(0,0, dai_xmax(), dai_ymax(), 10) ; // Orange cross 1st line
	dai_draw(0,dai_ymax(), dai_xmax(),0 , 10) ; // Orange cross 2nd line
	dai_dot(dai_xmax()/2, dai_ymax()/2, 5); // Green dot in the middle
	for (uint16_t i=0;i<60000;i++){}  // Wait
	c = dai_scrn(10,40);
	printf("Background 15 color %d \n",c) ;
	for (uint16_t i=0;i<60000;i++){}  // Wait
	c = dai_scrn(dai_xmax()/2, dai_ymax()/2) ; // get color of center
	printf("center 5 color %d \n",c) ;
	for (uint16_t i=0;i<60000;i++){}  // Wait
	c = dai_scrn(dai_xmax()/2, 0) ; // get color of rectangle
	printf("rectangle 3 color %d \n",c) ;
	while(1);
}


// -----------------------------------------------------------------------------------
// test_texts
// -----------------------------------------------------------------------------------
// In text mode, change color and move cursor
// Does not exit
// On a DAI can exit with a long push on break
void test_texts(void)
{
	static uint8_t px1,px2 ;
	static uint8_t py1,py2 ;
	dai_colort(0,10,5,15); // black background , orange, green, white
	px2 = 18 ;
	py2 = 7 ;
	dai_clearscreen(); // clear text screen by sending char 0x0C
	px1 = dai_curx() ;
	py1 = dai_cury() ;
	printf("Pos 1 %d,%d\n",px1,py1) ; // Print current position of cursor (orange on black background)
	for (uint16_t i=0;i<60000;i++){}  // Wait
	dai_cursor(px2,py2) ;
	printf("Pos 18,7 %d,%d\n",px2,py2) ; // Print new position of cursor

	while(1);
}


//====================================================================================
// LIBRARY EMULATING FUNCTIONS
//====================================================================================
// "__asm__" 8080 mnemonics are in Z80 style due to z88 compiler requirements
// No purely Z80 mnemonic is used


// -----------------------------------------------------------------------------------
// dai_mode : Change graphic modes
// -----------------------------------------------------------------------------------
// Graphic mode (*A = graphic area with 4 text lines of 60 characters)
// Input :  
// 0xFF = text, 24 lines, 60 characters
// 0  = mode 1, 1  = mode 1A, 72x65, 16 colors
// 2  = mode 2, 3  = mode 2A, 72x65, 4 colors
// 4  = mode 3, 5  = mode 3A, 160x130, 16 colors
// 6  = mode 4, 7  = mode 4A, 160x130, 4 colors
// 8  = mode 5, 9  = mode 5A, 336x256, 16 colors
// 10 = mode 6, 11 = mode 6A, 336x256, 4 colors
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_mode(uint8_t m) {
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" ld hl,$0006");	
	__asm__(" add hl,sp"); 
	__asm__(" ld a,(hl)");
	__asm__(" rst 5");
	__asm__(" defb $18"); // call $E3D9 in ROM
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_colorg 
// -----------------------------------------------------------------------------------
// Change graphic colors (equivalent of colorg)
// Input : color 0,1,2,3 (from 0 to 15)
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_colorg(uint8_t C0,uint8_t C1,uint8_t C2,uint8_t C3) {
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" push de");
	__asm__(" ld hl,$0008");	
	__asm__(" add hl,sp"); 
	__asm__(" ld de,$011C");
	__asm__(" ld a,(hl)"); // C3, Color 0-15
	__asm__(" ld (de),a"); 
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C2, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C1, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C0, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" ld hl,$0119");
	__asm__(" rst 5");
	__asm__(" defb $1B"); // call $E6A4 in ROM, input color vectors (coding 0-15) pointer in hl
	__asm__(" pop de");
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_dot 
// -----------------------------------------------------------------------------------
// Draw a dot using color reference set by Colorg
// Default avaialble colors on reset are 0, 5, 10, 15
// Input : x, y, color
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_dot(uint16_t x, uint8_t y, uint8_t c){
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" push bc");
	__asm__(" ld hl,$0008");	
	__asm__(" add hl,sp"); 
	__asm__(" ld a, (hl)"); // color in a
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld c,(hl)"); // y in c
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld b,(hl)"); // x in hl
	__asm__(" inc hl");
	__asm__(" ld h,(hl)");
	__asm__(" ld l, b");		
	// Input for Dai functions: x = HL, y = C, A : Color
	__asm__(" rst 5");
	__asm__(" defb $1E"); // call $E710 in ROM, input color vectors (coding 0-15) pointer in hl
	__asm__(" pop bc");
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_draw 
// -----------------------------------------------------------------------------------
// Draw a dot using color reference set by Colorg
// Input : x0, y0, x1, y1, color
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_draw(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint8_t c) {
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" push bc");	
	__asm__(" push de");	
	__asm__(" ld hl,$000A");	
	__asm__(" add hl,sp"); 
	__asm__(" ld a, (hl)"); // color in a
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld b,(hl)"); // y1 in b
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld e,(hl)"); // x1 in de
	__asm__(" inc hl");
	__asm__(" ld d,(hl)"); 
	__asm__(" inc hl");
	__asm__(" ld c,(hl)"); // y0 in c
	__asm__(" push bc");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld b,(hl)"); // x0 in hl via b
	__asm__(" inc hl");
	__asm__(" ld h,(hl)");
	__asm__(" ld l, b");		
	__asm__(" pop bc");
	// Input for Dai functions: x0 = HL, x1 = DE, y0 = C, y1 = B, A = Color
	__asm__(" rst 5");
	__asm__(" defb $21");
	__asm__(" pop de");	
	__asm__(" pop bc");
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_fill 
// -----------------------------------------------------------------------------------
// Draw a rectangle using color reference set by Colorg
// Input : x0, y0, x1, y1, color
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_fill(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint8_t c) {
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" push bc");	
	__asm__(" push de");	
	__asm__(" ld hl,$000A");	
	__asm__(" add hl,sp"); 
	__asm__(" ld a, (hl)"); // color in a
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld b,(hl)"); // y1 in b
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld e,(hl)"); // x1 in de
	__asm__(" inc hl");
	__asm__(" ld d,(hl)"); 
	__asm__(" inc hl");
	__asm__(" ld c,(hl)"); // y0 in c
	__asm__(" push bc");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld b,(hl)"); // x0 in hl via b
	__asm__(" inc hl");
	__asm__(" ld h,(hl)");
	__asm__(" ld l, b");		
	__asm__(" pop bc");
	// Input for Dai functions: x0 = HL, x1 = DE, y0 = C, y1 = B, A = Color
	__asm__(" rst 5");
	__asm__(" defb $24"); // call $E818 in ROM, input color vectors (coding 0-15) pointer in hl
	__asm__(" pop de");	
	__asm__(" pop bc");
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_xmax 
// -----------------------------------------------------------------------------------
// get xmax of a graphic screen. 0,0 bottom left
// Registers are saved except hl used for result
// Uses Dai ROM related function
// return in hl
uint16_t dai_xmax(void)
{
	__asm__(" push af");
	__asm__(" push bc");
	__asm__(" push de");
	__asm__(" ld hl,$0000"); // x in hl
	__asm__(" ld c,$00"); // y in c
	__asm__(" rst 5");
	__asm__(" defb $27"); // call $E884 in ROM, return a=color, b=ymax, de=xmax
	__asm__(" ld h,d"); // xmax in hl
	__asm__(" ld l,e"); 
	__asm__(" pop de");
	__asm__(" pop bc");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_ymax 
// -----------------------------------------------------------------------------------
// get ymax of a graphic screen. 0,0 bottom left
// Registers are saved except hl used for result
// Uses Dai ROM related function
// return in hl
uint8_t dai_ymax(void)
{
	__asm__(" push af");
	__asm__(" push bc");
	__asm__(" push de");
	__asm__(" ld hl,$0000"); // x in hl
	__asm__(" ld c,$00"); // y in c
	__asm__(" rst 5");
	__asm__(" defb $27"); // call $E884 in ROM, return a=color, b=ymax, de=xmax
	__asm__(" ld h,$00"); // ymax in hl
	__asm__(" ld l,b"); 
	__asm__(" pop de");
	__asm__(" pop bc");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_scrn 
// -----------------------------------------------------------------------------------
// Get color of dot. 0,0 bottom left
// Input : x, y
// Registers are saved except hl used for result
// Uses Dai ROM related function
uint8_t dai_scrn(uint8_t x, uint8_t y)
{
	__asm__(" push af");
	__asm__(" push bc");
	__asm__(" push de");
	__asm__(" ld hl,$0008");	
	__asm__(" add hl,sp"); 
	__asm__(" ld c, (hl)"); // y in c
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // x in hl
	__asm__(" inc hl");
	__asm__(" ld h,(hl)"); 
	__asm__(" ld l,a"); 
	// Input for Dai functions: x = hl, y = c
	__asm__(" rst 5");
	__asm__(" defb $27"); // call $E884 in ROM, return a=color, b=ymax, de=xmax
	__asm__(" ld h,$00"); // color in hl
	__asm__(" ld l,a"); 
	__asm__(" pop de");
	__asm__(" pop bc");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_colort 
// -----------------------------------------------------------------------------------
// Change text colors (equivalent of colort)
// Input : color 0,1,2,3 (from 0 to 15)
// Registers are saved
// Uses Dai ROM related function
// -----------------------------------------------------------------------------------
void dai_colort(uint8_t C0,uint8_t C1,uint8_t C2,uint8_t C3) {
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" push de");
	__asm__(" ld hl,$0008");	
	__asm__(" add hl,sp"); 
	__asm__(" ld de,$011C");
	__asm__(" ld a,(hl)"); // C3, Color 0-15
	__asm__(" ld (de),a"); 
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C2, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C1, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" dec de");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld a,(hl)"); // C0, Color 0-15
	__asm__(" ld (de),a");
	__asm__(" ld hl,$0119");
	__asm__(" rst 5");
	__asm__(" defb $06"); // call $E237 in ROM, input color vectors (coding 0-15) pointer in hl
	__asm__(" pop de");
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_cursor 
// -----------------------------------------------------------------------------------
// Set position of cursor. 0,0 = bottom left
// Input : x, y 
// Registers are saved
// Uses Dai ROM related function
void dai_cursor(uint8_t x, uint8_t y)
{
	__asm__(" push af");
	__asm__(" push hl");
	__asm__(" ld hl,$0006");	
	__asm__(" add hl,sp"); 
	__asm__(" ld a, (hl)"); // y in a
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld l,(hl)"); // x in l
	__asm__(" ld h,a"); // y in h
	// Input for Dai functions: h = y, l = x
	__asm__(" rst 5");
	__asm__(" defb $09"); // call $E279 in ROM
	__asm__(" pop hl");
	__asm__(" pop af");
}


// -----------------------------------------------------------------------------------
// dai_curx 
// -----------------------------------------------------------------------------------
// get x position of cursor. 0,0 bottom left
// Registers are saved except hl used for result
// Uses Dai ROM related function
// return in l
uint8_t dai_curx(void)
{
	__asm__(" push de");
	__asm__(" rst 5");
	__asm__(" defb $0C"); // call $E2CC in ROM, return h=y, l=x, d=ymax, e=xmax
	__asm__(" ld h,$00"); // x in l
	__asm__(" pop de");
}


// -----------------------------------------------------------------------------------
// dai_cury
// -----------------------------------------------------------------------------------
// get y position of cursor = 0,0 bottom left
// Registers are saved except hl used for result
// Uses Dai ROM related function
// return in l
uint8_t dai_cury(void)
{
	__asm__(" push de");
	__asm__(" rst 5");
	__asm__(" defb $0C"); // call $E2CC in ROM, return h=y, l=x, d=ymax, e=xmax
	__asm__(" ld l,h"); 
	__asm__(" ld h,$00"); // y in l
	__asm__(" pop de");
}


// -----------------------------------------------------------------------------------
// dai_clearscreen 
// -----------------------------------------------------------------------------------
// Clear scren by print character 0x0C
// Registers are saved
// Uses Dai ROM related function
void dai_clearscreen(void)
{
	__asm__(" push af"); 
	__asm__(" ld a, 0x0C"); // Load character 0x12 = clear screen
	__asm__(" rst 5"); // call $E102 in ROM
	__asm__(" defb $03");
	__asm__(" pop af"); 
}




//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
// Functions for debugging stack registers
//===================================================================================

//-----------------------------------------------------------------------------------
// savereg_x2000
// ----------------------------------------------------------------------------------
// Save registers at $2000 (A), $2002 (BC), $2004 (DE), $2006 (HL), $2008 (SP or SP-2)
// Registers are saved except F
void savereg_x2000 (void) {
	__asm__(" push hl"); 
	__asm__(" ld ($2000),a"); 
	__asm__(" ld ($2002),bc"); 
	__asm__(" ld ($2004),de"); 
	__asm__(" ld ($2006),hl"); 
	__asm__(" ld hl,$0000");	
	__asm__(" add hl,sp");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld ($2008),hl"); 
	__asm__(" pop hl"); 
}

// -----------------------------------------------------------------------------------
// savereg_x2010
// -----------------------------------------------------------------------------------
// Save registers at $2010 (A), $2012 (BC), $2014 (DE), $2016 (HL), $2018 (SP or SP-2)
// Registers are saved except F
void savereg_x2010 (void) {
	__asm__(" push hl"); 
	__asm__(" ld ($2010),a"); 
	__asm__(" ld ($2012),bc"); 
	__asm__(" ld ($2014),de"); 
	__asm__(" ld ($2016),hl"); 
	__asm__(" ld hl,$0000");	
	__asm__(" add hl,sp");
	__asm__(" inc hl");
	__asm__(" inc hl");
	__asm__(" ld ($2018),hl"); 
	__asm__(" pop hl"); 
}


// -----------------------------------------------------------------------------------
// get_SP
// -----------------------------------------------------------------------------------
// get stack pointer (with an offset due to call)
// returns SP in hl
// Other regsiters unchanged
uint16_t get_SP(void)
{
	__asm__(" ld hl,sp"); // sp in hl
}


// -----------------------------------------------------------------------------------
// change_Stack
// -----------------------------------------------------------------------------------
// Example to call another function
// restore all registers
void change_Stack(void)
{
	// Uncomment desired function  

	__asm__(" push de");
	__asm__(" push hl");
	__asm__(" ld hl,sp");	
	__asm__(" ld de,hl");	
	__asm__(" ld sp,$5800"); // New stack area upper limit
	__asm__(" push de"); // Store previous SP in new stack area
	
	// Put the function requiring a stack larger than the original on (128 bytes) below
	// call ...
	
	__asm__(" pop hl");
	__asm__(" ld sp,hl");		
	__asm__(" pop hl");
	__asm__(" pop de");
}