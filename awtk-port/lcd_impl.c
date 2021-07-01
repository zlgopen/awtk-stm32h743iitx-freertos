#include "FreeRTOS.h"
#include "lcd/lcd_mem_bgr565.h"
#include "lcd/lcd_mem_bgra8888.h"
#include "ltdc.h"
#include "semphr.h"
#include "tkc/mem.h"

#define FB_ADDR (uint8_t *)LCD_FRAME_BUF_ADDR

static uint8_t *s_framebuffers[2];

static SemaphoreHandle_t s_vbiSemaphore = NULL;

static ret_t ltdc_wait_vbi(void *ctx) {
  if (xSemaphoreTake(s_vbiSemaphore, 10000000) == pdTRUE) {
    return RET_OK;
  } else {
    return RET_FAIL;
  }
}

void LTDC_IRQHandler(void) { HAL_LTDC_IRQHandler(&LTDC_Handler); }

void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *LTDC_Handler) {
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (s_vbiSemaphore != NULL) {
    xSemaphoreGiveFromISR(s_vbiSemaphore, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdFALSE) {
    }
  }
  HAL_LTDC_ProgramLineEvent(LTDC_Handler, lcdltdc.height - 1);
}

lcd_t *lcd_impl_create(wh_t w, wh_t h) {
  lcd_t *lcd = NULL;
  uint32_t size = w * h * lcdltdc.pixsize;

  s_framebuffers[0] = FB_ADDR;
  s_framebuffers[1] = FB_ADDR + size;

#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888
  lcd = lcd_mem_bgra8888_create_double_fb(w, h, s_framebuffers[0],
                                          s_framebuffers[1]);
#else
  lcd = lcd_mem_bgr565_create_double_fb(w, h, s_framebuffers[0],
                                        s_framebuffers[1]);
#endif /*LCD_PIXFORMAT*/

  s_vbiSemaphore = xSemaphoreCreateBinary();
  lcd_mem_set_wait_vbi(lcd, ltdc_wait_vbi, NULL);

  HAL_NVIC_SetPriority(LTDC_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
  HAL_LTDC_ProgramLineEvent(&LTDC_Handler, lcdltdc.height - 1);
  HAL_LTDC_SetAddress(&LTDC_Handler, (uint32_t)s_framebuffers[0], 0);
  __HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);

  return lcd;
}
