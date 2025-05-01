/* ASSERT Handler */

#include "FreeRTOS.h"
#include "task.h"
#include "stm324xg_eval_lcd.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>   // for bool, true, false

#define ASSERT_MSG_FONT       Font12
#define ASSERT_BG_COLOR       LCD_COLOR_RED
#define ASSERT_TEXT_COLOR     LCD_COLOR_WHITE

/**
 * @brief  Breaks a long path into wrapped lines, splitting at path separators
 *         ('/' or '\\') or spaces, so each new line starts at a directory or filename.
 */
static int WrapText(const char *text, char lines[][32], int maxLines, int maxChars)
{
    int lineCount = 0;
    const char *ptr = text;
    int remaining = strlen(text);

    while (remaining > 0 && lineCount < maxLines)
    {
        int take = remaining;
        if (take > maxChars) take = maxChars;

        // try to break at slash/backslash first
        int breakPos = -1;
        for (int i = take - 1; i >= 0; --i)
        {
            if (ptr[i] == '/' || ptr[i] == '\\')
            {
                breakPos = i + 1; // include separator on this line
                break;
            }
        }
        // if no slash found, look for space
        if (breakPos < 0)
        {
            for (int i = take - 1; i >= 0; --i)
            {
                if (ptr[i] == ' ')
                {
                    breakPos = i + 1;
                    break;
                }
            }
        }
        // fallback to hard break
        if (breakPos < 0)
        {
            breakPos = take;
        }

        // copy the line
        int len = breakPos;
        if (len > maxChars) len = maxChars;
        strncpy(lines[lineCount], ptr, len);
        lines[lineCount][len] = '\0';

        // advance pointers
        ptr += breakPos;
        remaining -= breakPos;
        lineCount++;
    }
    return lineCount;
}

/**
 * @brief  Called on failed configASSERT or HAL assert.
 *         Displays the wrapped file path and line number centered.
 */
void vAssertCalled(const char *file, int line)
{
    // halt scheduler and interrupts
    vTaskSuspendAll();
    __disable_irq();

    // prepare LCD
    BSP_LCD_Clear(ASSERT_BG_COLOR);
    BSP_LCD_SetTextColor(ASSERT_TEXT_COLOR);
    BSP_LCD_SetBackColor(ASSERT_BG_COLOR);
    BSP_LCD_SetFont(&ASSERT_MSG_FONT);

    const char *title = "*** ASSERT FAILED ***";
    int fontW = ASSERT_MSG_FONT.Width;
    int fontH = ASSERT_MSG_FONT.Height;
    int screenW = BSP_LCD_GetXSize();
    int screenH = BSP_LCD_GetYSize();

    // wrap file path
    char wrapped[4][32];
    int maxChars = screenW / fontW;
    int fileLines = WrapText(file, wrapped, 4, maxChars);

    // calculate vertical centering
    int totalLines = 1 + fileLines + 1; // title + file + line#
    int blockH = totalLines * fontH;
    int yStart = (screenH - blockH) / 2;

    // print title
    int titleLen = strlen(title);
    int titlePx = titleLen * fontW;
    BSP_LCD_DisplayStringAt((screenW - titlePx) / 2, yStart, (uint8_t*)title, LEFT_MODE);

    // print wrapped lines
    for (int i = 0; i < fileLines; i++)
    {
        int len = strlen(wrapped[i]);
        int px = len * fontW;
        BSP_LCD_DisplayStringAt((screenW - px) / 2, yStart + fontH * (i + 1), (uint8_t*)wrapped[i], LEFT_MODE);
    }

    // print line number
    char numBuf[32];
    snprintf(numBuf, sizeof(numBuf), "Line: %d", line);
    int numLen = strlen(numBuf);
    int numPx = numLen * fontW;
    BSP_LCD_DisplayStringAt((screenW - numPx) / 2, yStart + fontH * (fileLines + 1), (uint8_t*)numBuf, LEFT_MODE);

    // blink LED3 in infinite loop
    while (1)
    {
        BSP_LED_Off(LED3);
        for (volatile uint32_t i = 0; i < 20000000; ++i) { __NOP(); }
        BSP_LED_On(LED3);
        for (volatile uint32_t i = 0; i < 500000; ++i) { __NOP(); }
    }
}