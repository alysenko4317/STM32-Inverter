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
 * @brief  Breaks a long string into wrapped lines, preferring to break
 *         at path separators ('/' or '\\') or spaces.
 */
static int WrapText(const char *text, char lines[][32], int maxLines, int maxChars)
{
    int lineCount = 0;
    const char *ptr = text;
    size_t remaining = strlen(text);

    while (remaining > 0 && lineCount < maxLines)
    {
        if (remaining <= (size_t)maxChars)
        {
            // remainder fits on one line
            strncpy(lines[lineCount], ptr, remaining);
            lines[lineCount][remaining] = '\0';
            lineCount++;
            break;
        }

        // find last separator before maxChars
        int breakPos = maxChars;
        bool found = false;
        for (int i = maxChars - 1; i > 0; --i)
        {
            char c = ptr[i];
            if (c == '/' || c == '\\' || c == ' ')
            {
                breakPos = i + 1;  // include the separator
                found = true;
                break;
            }
        }
        if (!found)
        {
            // no separator found, hard break
            breakPos = maxChars;
        }

        // copy that chunk
        strncpy(lines[lineCount], ptr, breakPos);
        lines[lineCount][breakPos] = '\0';
        ptr += breakPos;
        remaining -= breakPos;
        lineCount++;
    }

    return lineCount;
}

/**
 * @brief  Called on failed configASSERT or HAL assert.
 *         Displays the file path (wrapped) and line number,
 *         centered both horizontally and vertically.
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

    // title and dimensions
    const char *title = "*** ASSERT FAILED ***";
    int fontW = ASSERT_MSG_FONT.Width;
    int fontH = ASSERT_MSG_FONT.Height;
    int screenW = BSP_LCD_GetXSize();
    int screenH = BSP_LCD_GetYSize();

    // wrap file path
    char wrapped[4][32];
    int maxCharsPerLine = screenW / fontW;
    int fileLines = WrapText(file, wrapped, 4, maxCharsPerLine);

    // calculate vertical centering
    int totalLines = 1 + fileLines + 1;  // title + file + line#
    int blockHeight = totalLines * fontH;
    int yStart = (screenH - blockHeight) / 2;

    // print title centered
    int titleLen = strlen(title);
    int titlePx = titleLen * fontW;
    int x = (screenW - titlePx) / 2;
    BSP_LCD_DisplayStringAt(x, yStart, (uint8_t*)title, LEFT_MODE);

    // print wrapped file lines centered
    for (int i = 0; i < fileLines; i++)
    {
        int len = strlen(wrapped[i]);
        int px = len * fontW;
        int xi = (screenW - px) / 2;
        BSP_LCD_DisplayStringAt(xi, yStart + fontH * (i + 1), (uint8_t*)wrapped[i], LEFT_MODE);
    }

    // print line number centered
    char numBuf[32];
    snprintf(numBuf, sizeof(numBuf), "Line: %d", line);
    int numLen = strlen(numBuf);
    int numPx = numLen * fontW;
    int xn = (screenW - numPx) / 2;
    BSP_LCD_DisplayStringAt(xn, yStart + fontH * (fileLines + 1), (uint8_t*)numBuf, LEFT_MODE);

    // blink LED3 to indicate assert state
    // Busy-loop blink LED3, since interrupts (for TIM6) are off
    while (1)
    {
        // LED3 off period (5 units)
        BSP_LED_Off(LED3);
        for (volatile uint32_t i = 0; i < 20000000; ++i)
        {
            __NOP();
        }

        // LED3 on period (1 unit)
        BSP_LED_On(LED3);
        for (volatile uint32_t i = 0; i < 500000; ++i)
        {
            __NOP();
        }
    }
}
