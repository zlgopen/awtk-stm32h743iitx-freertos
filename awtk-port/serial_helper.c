#include "usart.h"
#include "tkc/mem.h"
#include "tkc/serial_helper.h"

serial_handle_t serial_open(const char *port)
{
  int fd = uart_open(port, 115200);
  serial_handle_t handle = NULL;
  return_value_if_fail(fd >= 0, NULL);

  handle = TKMEM_ZALLOC(serial_info_t);
  goto_error_if_fail(handle != NULL);

  handle->dev = fd;

  return handle;
error:
  uart_close(fd);

  return NULL;
}

int32_t serial_read(serial_handle_t handle, uint8_t *buff, uint32_t max_size)
{
  return_value_if_fail(handle != NULL && buff != NULL, -1);

  return uart_read(handle->dev, buff, max_size);
}

int32_t serial_write(serial_handle_t handle, const uint8_t *buff, uint32_t max_size)
{
  return_value_if_fail(handle != NULL && buff != NULL, -1);

  return uart_write(handle->dev, buff, max_size);
}

ret_t serial_close(serial_handle_t handle)
{
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  uart_close(handle->dev);
  TKMEM_FREE(handle);

  return RET_OK;
}

ret_t serial_config(serial_handle_t handle, uint32_t baudrate, bytesize_t bytesize,
                    stopbits_t stopbits, flowcontrol_t flowcontrol, parity_t parity)
{
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);
  /*目前不支持配置*/
  return RET_OK;
}

int serial_handle_get_fd(serial_handle_t handle)
{
  return_value_if_fail(handle != NULL, -1);

  return handle->dev;
}

serial_dev_t serial_handle_get_dev(serial_handle_t handle)
{
  return_value_if_fail(handle != NULL, NULL);

  return handle->dev;
}

ret_t serial_iflush(serial_handle_t handle)
{
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  return RET_OK;
}

ret_t serial_oflush(serial_handle_t handle)
{
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  return RET_OK;
}

ret_t serial_wait_for_data(serial_handle_t handle, uint32_t timeout_ms)
{
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  return uart_has_data(handle->dev) ? RET_OK : RET_TIMEOUT;
}
