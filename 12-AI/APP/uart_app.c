#include "uart_app.h"
#include "parking_app.h"

#define BUFFER_SIZE 64

// 环形缓冲区和接收缓冲
ringbuffer_t usart_rb;
uint8_t usart_read_buffer[BUFFER_SIZE];

/**
 * @brief UART DMA空闲中断回调函数
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        // 环形缓冲区未满时写入数据
        if (!ringbuffer_is_full(&usart_rb))
        {
            ringbuffer_write(&usart_rb, uart_rx_dma_buffer, Size);
        }
        // 清空DMA缓冲区
        memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));

        // 重新启动DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_rx_dma_buffer, sizeof(uart_rx_dma_buffer));
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

/**
 * @brief 串口数据处理函数
 *
 * 解析接收到的车辆进出信息，计算并输出计费信息
 *
 * 串口协议:
 * - 入停车场: 停车类型:车辆编号:进入时间(YYMMDDHHmmSS)
 *   示例: CNBR:A392:200202120000
 * - 出停车场: 停车类型:车辆编号:退出时间
 *   示例: VNBR:D583:200202132500
 * - 计费输出: 停车类型:车辆编号:停车时长:费用
 *   示例: VNBR:D583:10:20.00
 */
void uart_proc(void)
{
    // 环形缓冲区为空时直接返回
    if (ringbuffer_is_empty(&usart_rb))
        return;

    // 获取缓冲区中的数据量
    uint32_t data_len = usart_rb.itemCount;

    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(usart_read_buffer));

    // 从环形缓冲区读取数据
    ringbuffer_read(&usart_rb, usart_read_buffer, data_len);

    // 解析消息
    int result = parking_parse_message((const char*)usart_read_buffer, data_len);

    // 格式错误时输出Error
    if (result == -1)
    {
        printf("Error\r\n");
    }
    // result == 0: 入场成功,不需要回复
    // result == 1: 出场成功,已在parking_parse_message中输出计费信息
}
