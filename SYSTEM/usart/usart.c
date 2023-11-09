#include "sys.h"
#include "stdio.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32H7开发板
// 串口1初始化
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 修改日期:2017/6/8
// 版本：V1.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2009-2019
// All rights reserved
//********************************************************************************
// V1.0修改说明
// Li XianJing <xianjimli@hotmai.com> 2023-11-09 封装成串口函数。
//////////////////////////////////////////////////////////////////////////////////

#define RXBUFFERSIZE 1
#define MAX_UART_COUNT 3

#if defined(WITH_PRINTF)
// #pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
  int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
  x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
  return ch;
}
#endif /*WITH_PRINTF*/

typedef struct _uart_device_t
{
  UART_HandleTypeDef handler;
  uint8_t rx_buff[RXBUFFERSIZE];
  ring_buffer_t *rx_ring_buff;
} uart_device_t;

static uart_device_t s_uart_devices[MAX_UART_COUNT];

int uart_open(const char *name, int baudrate)
{
  uart_device_t *dev = NULL;
  uint32_t index = name != NULL ? atoi(name) : 1;
  return_value_if_fail(index < MAX_UART_COUNT && index > 0, -1);
  dev = &s_uart_devices[index - 1];

  if (dev->rx_ring_buff != NULL)
  {
    return index - 1;
  }

  // UART 初始化设置
  if (index == 1)
  {
    dev->handler.Instance = USART1;
  }
  else if (index == 2)
  {
    dev->handler.Instance = USART2;
  }
  else if (index == 3)
  {
    dev->handler.Instance = USART3;
  }
  else
  {
    return -1;
  }
  dev->handler.Init.BaudRate = baudrate;               // 波特率
  dev->handler.Init.WordLength = UART_WORDLENGTH_8B; // 字长为8位数据格式
  dev->handler.Init.StopBits = UART_STOPBITS_1;      // 一个停止位
  dev->handler.Init.Parity = UART_PARITY_NONE;       // 无奇偶校验位
  dev->handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; // 无硬件流控
  dev->handler.Init.Mode = UART_MODE_TX_RX;          // 收发模式
  HAL_UART_Init(&dev->handler);                      // HAL_UART_Init()会使能UART1

  HAL_UART_Receive_IT(&(dev->handler), (u8 *)(dev->rx_buff), sizeof(dev->rx_buff)); // 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量

  dev->rx_ring_buff = ring_buffer_create(2048, 2048);

  return index;
}

int uart_close(int fd)
{
	int index = fd - 1;	
  uart_device_t *dev = NULL;
  return_value_if_fail(fd < MAX_UART_COUNT && index >= 0, -1);
  dev = &s_uart_devices[index];

  HAL_UART_DeInit(&dev->handler);
  ring_buffer_destroy(dev->rx_ring_buff);
  dev->rx_ring_buff = NULL;

  return 0;
}

int uart_read(int fd, void *buffer, uint32_t size)
{
	int index = fd - 1;	
  uart_device_t *dev = NULL;
  return_value_if_fail(fd < MAX_UART_COUNT && index >= 0, -1);
  dev = &s_uart_devices[index];
  return_value_if_fail(dev->rx_ring_buff != NULL, -1);

  return ring_buffer_read(dev->rx_ring_buff, buffer, size);
}

static int uart_send_char(uart_device_t *dev, uint8_t ch)
{
  while ((dev->handler.Instance->ISR & 0X40) == 0)
    ;
  dev->handler.Instance->TDR = (u8)ch;

  return 1;
}

int uart_write(int fd, const void *buffer, uint32_t size)
{
  uint32_t i = 0;
	int index = fd - 1;		
  uart_device_t *dev = NULL;
  return_value_if_fail(fd < MAX_UART_COUNT && index >= 0, -1);
  dev = &s_uart_devices[index];
  return_value_if_fail(dev->rx_ring_buff != NULL, -1);

  for (i = 0; i < size; i++)
  {
    uart_send_char(dev, ((uint8_t *)buffer)[i]);
  }

  return size;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  // GPIO端口设置
  GPIO_InitTypeDef GPIO_Initure;

  if (huart->Instance == USART1) // 如果是串口1，进行串口1 MSP初始化
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();  // 使能GPIOA时钟
    __HAL_RCC_USART1_CLK_ENABLE(); // 使能USART1时钟

    GPIO_Initure.Pin = GPIO_PIN_9;             // PA9
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;       // 复用推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    GPIO_Initure.Alternate = GPIO_AF7_USART1;  // 复用为USART1
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);       // 初始化PA9

    GPIO_Initure.Pin = GPIO_PIN_10;      // PA10
    HAL_GPIO_Init(GPIOA, &GPIO_Initure); // 初始化PA10

    HAL_NVIC_EnableIRQ(USART1_IRQn);         // 使能USART1中断通道
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 3); // 抢占优先级3，子优先级3
  }

  /*TODO: init other uarts*/
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uart_device_t *dev = NULL;
  if (huart->Instance == USART1)
  {
    dev = &s_uart_devices[0];
  }
  else if (huart->Instance == USART2)
  {
    dev = &s_uart_devices[1];
  }
  else if (huart->Instance == USART3)
  {
    dev = &s_uart_devices[2];
  }
  else
  {
    return;
  }

  ring_buffer_write(dev->rx_ring_buff, dev->rx_buff, sizeof(dev->rx_buff));
}

void USART_IRQHandler(uart_device_t *dev)
{
  u32 timeout = 0;
  u32 maxDelay = 0x1FFFF;
  UART_HandleTypeDef *handler = &dev->handler;

  HAL_UART_IRQHandler(handler); // 调用HAL库中断处理公用函数

  timeout = 0;
  while (HAL_UART_GetState(handler) != HAL_UART_STATE_READY) // 等待就绪
  {
    timeout++; ////超时处理
    if (timeout > maxDelay)
      break;
  }

  timeout = 0;
  while (HAL_UART_Receive_IT(handler, (u8 *)(dev->rx_buff), sizeof(dev->rx_buff)) != HAL_OK) // 一次处理完成之后，重新开启中断并设置RxXferCount为1
  {
    timeout++; // 超时处理
    if (timeout > maxDelay)
      break;
  }
}

void USART1_IRQHandler(void)
{
  USART_IRQHandler(&s_uart_devices[0]);
}

void uart_test(const char* name)
{
  int len = 0;
  int fd = uart_open(name, 115200);
  uint8_t buff[128] = {0};

  if (fd >= 0)
  {
		int count = 10000;
    while (count-- > 0)
    {
      len = uart_read(fd, buff, sizeof(buff));
      if (len > 0)
      {
        uart_write(fd, buff, len);
      }
    }

    uart_close(fd);
  }
}
