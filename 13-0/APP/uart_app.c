#include "uart_app.h"

#define BUUFER_SIZE 64

// 定义环形缓冲区和接收缓冲区
ringbuffer_t usart_rb;
uint8_t usart_read_buffer[BUUFER_SIZE];

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   if (huart->Instance == USART1)
//   {
//     uart_rx_ticks = uwTick;
//     uart_rx_index++;
//     HAL_UART_Receive_IT(&huart1, &uart_rx_buffer[uart_rx_index], 1);
//   }
// }

/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区，并清空DMA缓冲区
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    //    printf("dma data:%s\r\n", uart_rx_dma_buffer);
    // 如果环形缓冲区未满
    if (!ringbuffer_is_full(&usart_rb))
    {
        // 将DMA缓冲区中的数据写入环形缓冲区
        ringbuffer_write(&usart_rb, uart_rx_dma_buffer, Size);
    }
    // 清空DMA缓冲区
    memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
}

/**
 * @brief  处理UART接收缓冲区中的数据。
 *         如果在100ms内没有接收到新的数据，将清空缓冲区。
 * @param  None
 * @retval None
 */
// 处理UART接收缓冲区中的数据
void uart_proc(void)
{
    // // 如果接收索引为0，说明没有数据需要处理，直接返回
    // if(uart_rx_index == 0) return;

    // // 如果从最后一次接收到数据到现在已经超过100ms
    // if(uwTick - uart_rx_ticks > 100) // 100ms内没有收到新的数据
    // {
    //     printf("uart data: %s\n", uart_rx_buffer);

    //     // 清空接收缓冲区，将接收索引置零
    //     memset(uart_rx_buffer, 0, uart_rx_index);
    //     uart_rx_index = 0;

    //     // 将UART接收缓冲区指针重置为接收缓冲区的起始位置
    //     huart1.pRxBuffPtr = uart_rx_buffer;
    // }

    // 如果环形缓冲区为空，直接返回
    if (ringbuffer_is_empty(&usart_rb))
        return;

    uint8_t itemCount = usart_rb.itemCount;
    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb, usart_read_buffer, usart_rb.itemCount);
    

    if (usart_read_buffer[3] != '-' || itemCount != 7)
    {
        return;
    }
    
    for (uint8_t i = 4; i < 7; i++)
    {
        if (usart_read_buffer[i] < '0' || usart_read_buffer[i] > '9')
            return;
    }

    if (memcmp(password_data, usart_read_buffer, sizeof(password_data)) == 0)
    {
        printf("password correct! %s\n", usart_read_buffer);
        memcpy(password_data, &usart_read_buffer[4], 3);
    }
    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}
