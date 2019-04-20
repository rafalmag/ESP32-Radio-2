// ILI9341.h
//
// Contributed by Uksa007@gmail.com
// Separated from the main sketch to allow several display types.
// Includes for various ILI9341 displays.  Tested on 320 x 240.
// Requires Adafruit ILI9341 library, available from library manager.
// Below set your dsp_getwidth() and dsp_getwidth() to suite your display.

#include <Adafruit_ILI9341.h>

// Color definitions for the TFT screen (if used)
// TFT has bits 6 bits (0..5) for RED, 6 bits (6..11) for GREEN and 4 bits (12..15) for BLUE.
#define BLACK   ILI9341_BLACK
#define BLUE    ILI9341_BLUE
#define RED     ILI9341_RED
#define GREEN   ILI9341_GREEN
#define CYAN    GREEN | BLUE
#define MAGENTA RED | BLUE
#define YELLOW  RED | GREEN
#define WHITE   RED | BLUE | GREEN

// Data to display.  There are TFTSECS sections
#define TFTSECS 4
scrseg_struct     tftdata[TFTSECS] =                        // Screen divided in 3 segments + 1 overlay
{                                                           // One text line is 8 pixels
  { false, WHITE, 309,  8, "" },                            // 1 line - name and old clock
  { false, CYAN,   24, 32, "" },                            // 4 lines - song/station detailed info
  { false, YELLOW,  0, 24, "" },                            // 3 lines - station
  { false, GREEN,  24, 32, "" }                             // 4 lines - mute/unmute/station select
} ;


Adafruit_ILI9341*     tft = NULL ;                                  // For instance of display driver

// Various macro's to mimic the ILI9341 version of display functions
#define dsp_setRotation()       tft->setRotation ( 0 )             // Use landscape format (3 for upside down, 0 for vertical)
#define dsp_print(a)            tft->print ( a )                   // Print a string 
#define dsp_println(b)          tft->println ( b )                 // Print a string followed by newline 
#define dsp_fillRect(a,b,c,d,e) tft->fillRect ( a, b, c, d, e ) ;  // Fill a rectange
#define dsp_setTextSize(a)      tft->setTextSize(a)                // Set the text size
#define dsp_setTextColor(a)     tft->setTextColor(a)               // Set the text color
#define dsp_setCursor(a,b)      tft->setCursor ( a, b )            // Position the cursor
#define dsp_erase()             tft->fillScreen ( BLACK ) ;        // Clear the screen
#define dsp_getwidth()          240                                // Adjust to your display
#define dsp_getheight()         320                                // Get height of screen
#define dsp_update()                                               // Updates to the physical screen
#define dsp_usesSPI()           true                               // Does use SPI


bool dsp_begin()
{
  tft = new Adafruit_ILI9341 ( ini_block.tft_cs_pin,
                               ini_block.tft_dc_pin ) ;            // Create an instant for TFT

  tft->begin();                                                    // Init TFT interface
  return ( tft != NULL ) ;
}

//**************************************************************************************************
//                                      D I S P L A Y B A T T E R Y                                *
//**************************************************************************************************
// Show the current battery charge level on the screen.                                            *
// It will overwrite the top divider.                                                              *
// No action if bat0/bat100 not defined in the preferences.                                        *
//**************************************************************************************************
void displaybattery()
{
  if ( tft )
  {
    if ( ini_block.bat0 < ini_block.bat100 )              // Levels set in preferences?
    {
      static uint16_t oldpos = 0 ;                        // Previous charge level
      uint16_t        ypos ;                              // Position on screen
      uint16_t        v ;                                 // Constrainted ADC value
      uint16_t        newpos ;                            // Current setting

      v = constrain ( adcval, ini_block.bat0,             // Prevent out of scale
                      ini_block.bat100 ) ;
      newpos = map ( v, ini_block.bat0,                   // Compute length of green bar
                     ini_block.bat100,
                     0, dsp_getwidth() ) ;
      if ( newpos != oldpos )                             // Value changed?
      {
        oldpos = newpos ;                                 // Remember for next compare
        ypos = tftdata[1].y - 5 ;                         // Just before 1st divider
        dsp_fillRect ( 0, ypos, newpos, 2, GREEN ) ;      // Paint green part
        dsp_fillRect ( newpos, ypos,
                       dsp_getwidth() - newpos,
                       2, RED ) ;                          // Paint red part
      }
    }
  }
}

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;

#define TFT_BLACK   ILI9341_BLACK
#define TFT_WHITE   ILI9341_WHITE
#define TFT_RED   ILI9341_RED
#define CLOCK_BEZEL_COLOR   ILI9341_MAROON
#define TFT_BACKGROUND ILI9341_BLACK
#define CLOCK_Y_OFFSET 56

void initClock(){
  // Draw clock face
  tft->fillCircle(120, 120 + CLOCK_Y_OFFSET, 118, CLOCK_BEZEL_COLOR);
  tft->fillCircle(120, 120 + CLOCK_Y_OFFSET, 110, TFT_BLACK);

  // Draw 12 lines
  for (int i = 0; i < 360; i += 30)
  {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 114 + 120;
    yy0 = sy * 114 + 120;
    x1 = sx * 100 + 120;
    yy1 = sy * 100 + 120;

    tft->drawLine(x0, yy0 + CLOCK_Y_OFFSET, x1, yy1 + CLOCK_Y_OFFSET, CLOCK_BEZEL_COLOR);
  }

  // Draw 60 dots
  for (int i = 0; i < 360; i += 6)
  {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 102 + 120;
    yy0 = sy * 102 + 120;
    // Draw minute markers
    tft->drawPixel(x0, yy0+CLOCK_Y_OFFSET, TFT_WHITE);

    // Draw main quadrant dots
    if (i == 0 || i == 180)
      tft->fillCircle(x0, yy0+CLOCK_Y_OFFSET, 2, TFT_WHITE);
    if (i == 90 || i == 270)
      tft->fillCircle(x0, yy0+CLOCK_Y_OFFSET, 2, TFT_WHITE);
  }

  tft->fillCircle(120, 121+CLOCK_Y_OFFSET, 3, TFT_WHITE);
}

void        displaytime2 ( tm* timeinfo, boolean redraw) {
  if(!tft) return;
  if(redraw){
    initClock();
  }

   // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = timeinfo->tm_sec * 6;                     // 0-59 -> 0-354
    mdeg = timeinfo->tm_min * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
    hdeg = timeinfo->tm_hour * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (timeinfo->tm_sec == 0 || redraw)
    {
      // Erase hour and minute hand positions every minute
      tft->drawLine(ohx, ohy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_BLACK);
      ohx = hx * 62 + 121;
      ohy = hy * 62 + 121;
      tft->drawLine(omx, omy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_BLACK);
      omx = mx * 84 + 120;
      omy = my * 84 + 121;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft->drawLine(osx, osy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_BLACK);
    osx = sx * 90 + 121;
    osy = sy * 90 + 121;
    tft->drawLine(osx, osy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_RED);
    tft->drawLine(ohx, ohy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_WHITE);
    tft->drawLine(omx, omy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_WHITE);
    tft->drawLine(osx, osy+CLOCK_Y_OFFSET, 120, 121+CLOCK_Y_OFFSET, TFT_RED);

    tft->fillCircle(120, 121+CLOCK_Y_OFFSET, 3, TFT_RED);
}

//**************************************************************************************************
//                                      D I S P L A Y V O L U M E                                  *
//**************************************************************************************************
// Show the current volume as an indicator on the screen.                                          *
// The indicator is 2 pixels heigh.                                                                *
//**************************************************************************************************
void displayvolume()
{
  if ( tft )
  {
    static uint8_t oldvol = 0 ;                         // Previous volume
    uint8_t        newvol ;                             // Current setting
    uint16_t       pos ;                                // Positon of volume indicator

    newvol = vs1053player->getVolume() ;                // Get current volume setting
    if ( newvol != oldvol )                             // Volume changed?
    {
      oldvol = newvol ;                                 // Remember for next compare
      pos = map ( newvol, 0, 100, 0, dsp_getwidth() ) ; // Compute position on TFT
      dsp_fillRect ( 0, dsp_getheight() - 2,
                     pos, 2, RED ) ;                    // Paint red part
      dsp_fillRect ( pos, dsp_getheight() - 2,
                     dsp_getwidth() - pos, 2, GREEN ) ; // Paint green part
    }
  }
}


//**************************************************************************************************
//                                      D I S P L A Y T I M E                                      *
//**************************************************************************************************
// Show the time on the LCD at a fixed position in a specified color                               *
// To prevent flickering, only the changed part of the timestring is displayed.                    *
// An empty string will force a refresh on next call.                                              *
// A character on the screen is 8 pixels high and 6 pixels wide.                                   *
//**************************************************************************************************
// void displaytime ( const char* str, uint16_t color )
// {
//   static char oldstr[9] = "........" ;             // For compare
//   uint8_t     i ;                                  // Index in strings
//   uint16_t    pos = dsp_getwidth() + TIMEPOS ;     // X-position of character, TIMEPOS is negative

//   if ( str[0] == '\0' )                            // Empty string?
//   {
//     for ( i = 0 ; i < 8 ; i++ )                    // Set oldstr to dots
//     {
//       oldstr[i] = '.' ;
//     }
//     return ;                                       // No actual display yet
//   }
//   if ( tft )                                       // TFT active?
//   {
//     dsp_setTextColor ( color ) ;                   // Set the requested color
//     for ( i = 0 ; i < 8 ; i++ )                    // Compare old and new
//     {
//       if ( str[i] != oldstr[i] )                   // Difference?
//       {
//         dsp_fillRect ( pos, 0, 6, 8, BLACK ) ;     // Clear the space for new character
//         dsp_setCursor ( pos, 0 ) ;                 // Prepare to show the info
//         dsp_print ( str[i] ) ;                     // Show the character
//         oldstr[i] = str[i] ;                       // Remember for next compare
//       }
//       pos += 6 ;                                   // Next position
//     }
//   }
// }


