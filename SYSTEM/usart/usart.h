#ifndef _USART_H
#define _USART_H

#include "tkc/ring_buffer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2017/6/8
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.0修改说明 
// Li XianJing <xianjimli@hotmai.com> 2023-11-09 封装成串口函数。
////////////////////////////////////////////////////////////////////////////////// 	

#define WITH_PRINTF 1

/**
 * @method uart_open
 * @param {const char*} name 串口名称，如"1"，"2"，"3"，"4"，"5"，"6"，"7"，"8"。
 * @return {int} 串口句柄。
*/
int uart_open(const char *name);

/**
 * @method uart_read
 * 读串口。
 * @param {int} fd 串口句柄。
 * @param {void*} buff 读取缓冲区。
 * @param {uint32_t} nbytes 读取字节数。
 * @return {int} 实际读取字节数。
*/
int uart_read(int fd, void *buff, uint32_t nbytes);

/**
 * @method uart_write
 * 写串口。
 * @param {int} fd 串口句柄。
 * @param {const void*} buff 写入缓冲区。
 * @param {uint32_t} nbytes 写入字节数。
 * @return {int} 实际写入字节数。
*/
int uart_write(int fd, const void *buff, uint32_t nbytes);

/**
 * @method uart_close
 * 关闭串口。
 * @param {int} fd 串口句柄。
 * @return {int} 0表示成功，-1表示失败。
*/
int uart_close(int fd);

/*for test*/
void uart_test(const char* name);

#endif
