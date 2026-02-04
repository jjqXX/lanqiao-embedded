#include "uart_app.h"
#include <stdlib.h>

#define BUUFER_SIZE 64

// 定义环形缓冲区和接收缓冲区
ringbuffer_t usart_rb;
uint8_t usart_read_buffer[BUUFER_SIZE];

/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区,并重启DMA接收
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 如果环形缓冲区未满
    if (!ringbuffer_is_full(&usart_rb))
    {
        // 将DMA缓冲区中的数据写入环形缓冲区
        ringbuffer_write(&usart_rb, uart_rx_dma_buffer, Size);
    }
    // 清空DMA缓冲区
    memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
}

/*链表实现方式*/
// typedef struct CoordinateNode
// {
//     uint16_t x;
//     uint16_t y;
//     struct CoordinateNode *next;
// } CoordinateNode;

// CoordinateNode *waypoint_head = NULL; // 链表头指针，初始为空(无节点)

// uint8_t add_waypoint(uint16_t x, uint16_t y) // 添加途径地函数
// {
//     CoordinateNode *current = waypoint_head;
//     while (current != NULL)
//     {
//         if (current->x == x && current->y == y) // 已存在相同的坐标点
//         {
//             return 0;
//         }
//         // if(current->next == NULL) break;
//         current = current->next; // 移动到下一个节点
//     } // malloc申请一个节点大小的内存
//     CoordinateNode *new_node = (CoordinateNode *)malloc(sizeof(CoordinateNode)); // 创建新节点
//     if (new_node == NULL)                                                        // malloc失败返回NULL（内存不足）
//     {
//         return 0;
//     }
//     new_node->x = x;
//     new_node->y = y;
//     // new_node->next = NULL;
//     // if(waypoint_head == NULL)
//     // {
//     //     waypoint_head = new_node;
//     // }
//     // else
//     // {
//     //     current->next = new_node;
//     // }
//     new_node->next = waypoint_head;
//     waypoint_head = new_node;

//     return 1; // 添加成功
// }

// uint8_t remove_waypoint(uint16_t x, uint16_t y) // 删除指定节点
// {
//     CoordinateNode *current = waypoint_head; // current:当前节点
//     CoordinateNode *prev = NULL;             // prev:前置节点
//     while (current != NULL)
//     {
//         if (current->x == x && current->y == y)
//         {
//             if (prev == NULL) // 情况1：删除的是头节点
//             {
//                 waypoint_head = current->next;
//             }
//             else // 情况2：删除的是中间或尾部节点
//             {
//                 prev->next = current->next;
//             }
//             free(current); // 释放被删除节点的内存
//             return 1;
//         }
//         prev = current;          // prev前进一步
//         current = current->next; // current前进一步
//     }
//     return 0;
// }

// void print_waypoints(void) // 打印全部节点
// {
//     CoordinateNode *current = waypoint_head;
//     while (current != NULL)
//     {
//         printf("(%d,%d)\r\n", current->x, current->y);
//         current = current->next;
//     }
// }

// void handle_uart(const char *cmd)
// {
//     if (strstr((const char *)cmd, "(") != NULL && strstr((const char *)cmd, ")") != NULL)
//     {
//         char data[64];
//         strncpy(data, strstr((const char *)cmd, "(") + 1, strstr((const char *)cmd, ")") - strstr((const char *)cmd, "(") - 1);
//         char *token = strtok(data, ",");
//         uint16_t coordinates[64];
//         uint8_t index = 0;
//         while (token != NULL && index < 64)
//         {
//             printf("%d\n", atoi(token));
//             coordinates[index++] = atoi(token);
//             token = strtok(NULL, ",");
//         }
//         if (index % 2 == 0)
//         {
//             for (int i = 0; i < index / 2; i++)
//             {
//                 if (!add_waypoint(coordinates[i * 2], coordinates[i * 2 + 1]))
//                 {
//                     printf("Error\n");
//                     return;
//                 }
//                 else
//                 {
//                     printf("Add waypoint (%d,%d)\n", coordinates[i * 2], coordinates[i * 2 + 1]);
//                 }
//             }
//         }
//     }
//     else if (strstr((const char *)cmd, "{") != NULL && strstr((const char *)cmd, "}") != NULL) // 删除一个节点
//     {
//         char data[16];
//         strncpy(data, strstr((const char *)cmd, "{") + 1, strstr((const char *)cmd, "}") - strstr((const char *)cmd, "{") - 1);
//         uint16_t x = atoi(strtok(data, ","));
//         uint16_t y = atoi(strtok(NULL, ","));
//         if (remove_waypoint(x, y)) // 删除成功
//         {
//             printf("Got it\n");
//         }
//         else
//         {
//             printf("Nonexistent\n");
//         }
//     }
//     else if (strstr((const char *)cmd, "#"))
//     {
//         printf("%s\n", st_value[st_index]);
//     }
// }

uint8_t freq_number = 1; // 频率参数

/*数组的实现方法*/

uint16_t coordinate_arry[MAX_COORDINATES][2];
uint16_t coordinate_count = 0;

uint8_t coordinate_add(uint16_t x, uint16_t y)
{
    for (uint8_t i = 0; i < MAX_COORDINATES; i++)
    {
        if (coordinate_arry[i][0] == x && coordinate_arry[i][1] == y)
        {
            return 0;
        }
    }
    if (coordinate_count >= MAX_COORDINATES)
    {
        return 0;
    }

    coordinate_arry[coordinate_count][0] = x;
    coordinate_arry[coordinate_count][1] = y;
    coordinate_count++;
    return 1;
}

uint8_t coordinate_remove(uint16_t x, uint16_t y)
{
    for (uint8_t i = 0; i < coordinate_count; i++)
    {
        if (coordinate_arry[i][0] == x && coordinate_arry[i][1] == y)
        {
            for (uint8_t j = i; j < coordinate_count - 1; j++)
            {
                coordinate_arry[j][0] = coordinate_arry[j + 1][0];
                coordinate_arry[j][1] = coordinate_arry[j + 1][1];
            }
            coordinate_count--;
            return 1;
        }
    }
    return 0;
}

void coordinate_print(void)
{
    for (uint8_t i = 0; i < coordinate_count; i++)
    {
        printf("(%d,%d)\r\n", coordinate_arry[i][0], coordinate_arry[i][1]);
    }
}

void coordinate_handle(const char *cmd)
{
    // char response[32];
    int scene_id = 0;
    int x = 0;
    int y = 0;

    if (strstr((const char *)cmd, "(") != NULL && strstr((const char *)cmd, ")") != NULL)
    {
        uint16_t length = strstr((const char *)cmd, ")") - strstr((const char *)cmd, "(") - 1;
        char *data = (char *)malloc(length + 1);
        data[length] = '\0';
        strncpy(data, strstr((const char *)cmd, "(") + 1, length);
        char *token = strtok(data, ",");
        uint16_t coordinates[MAX_COORDINATES * 2];
        uint16_t index = 0;
        while (token != NULL && index < MAX_COORDINATES * 2)
        {
            coordinates[index++] = atoi(token);
            token = strtok(NULL, ",");
        }

        if (index % 2 == 0)
        {
            for (uint8_t i = 0; i < index / 2; i++)
            {
                if (coordinates[i * 2] <= 999 && coordinates[i * 2 + 1] <= 999)
                {
                    if (!coordinate_add(coordinates[i * 2], coordinates[i * 2 + 1]))
                    {
                        printf("Error\r\n");
                    }
                }
                else
                {
                    printf("Error\r\n");
                }
            }
            printf("Got it");
        }
        else
        {
            printf("Error\r\n");
        }
        free(data);
    }
    // else if(strstr((const char *)cmd, "{") != NULL && strstr((const char *)cmd, "}") != NULL)//删除判断
    // {
    //     char data[16];
    //     uint16_t length = strstr((const char *)cmd, "}") - strstr((const char *)cmd, "{") - 1;
    //     strncpy(data, strstr((const char *)cmd, "{") + 1, length);
    //     data[length] = '\0';
    //     uint16_t x = atoi(strtok(data, ","));
    //     uint16_t y = atoi(strtok(NULL, ","));
    //     if (remove_waypoint(x, y)) // 删除成功
    //     {
    //         printf("Got it\n");
    //     }
    //     else
    //     {
    //         printf("Nonexistent\n");
    //     }
    // }
    else if (strstr((const char *)cmd, "?")) // 查询当前状态
    {
        printf("%s\n", st_value[st_index]);
    }
    else if (strstr((const char *)cmd, "#")) // 查询当前位置
    {
    }
    else if (sscanf((const char *)cmd, "{%d,%d}", &x, &y) == 2)
    {
        if (coordinate_remove(x, y)) // 删除成功
        {
            printf("Got it\r\n");
        }
        else
        {
            printf("Nonexistent\n");
        }
    }
    else if (sscanf((const char *)cmd, "[%d]", &scene_id) == 1) //
    {
        if (st_index == 0)
        {
            freq_number = scene_id;
            printf("Got it");
        }
        else
        {
            printf("Device offline");
        }
    }
    else
    {
        printf("Error\r\n");
    }
}

/**
 * @brief  处理UART接收缓冲区中的数据
 * @param  None
 * @retval None
 */
void uart_proc(void)
{
    // 如果环形缓冲区为空,直接返回
    if (ringbuffer_is_empty(&usart_rb))
        return;

    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb, usart_read_buffer, usart_rb.itemCount);

    printf("usart_read_buffer:%s\r\n", usart_read_buffer);

    // if (strncmp((const char *)usart_read_buffer, "add_wp", 6) == 0)
    // {
    //     add_waypoint(1, 2);
    //     add_waypoint(3, 4);
    //     add_waypoint(5, 6);
    // }
    // else if (strncmp((const char *)usart_read_buffer, "remove_wp", 9) == 0)
    // {
    //     remove_waypoint(3, 4);
    // }

    if (strncmp((const char *)usart_read_buffer, "print_wp", 8) == 0)
    {
        coordinate_print();
    }
    else
    {
        coordinate_handle((const char *)usart_read_buffer);
    }

    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

uint16_t current_target_index = 0;
uint32_t last_time = 0;
uint32_t time_ms = 0;
uint8_t first_run = 1;
float pos_x = 0.0f;
float pos_y = 0.0f;

void report_system_info(void)
{
    static uint32_t last_report_time = 0;
    uint32_t current_time = HAL_GetTick();

    // 每100ms上报一次信息
    if (current_time - last_report_time >= 100)
    {
        last_report_time = current_time;

        char buffer[64];

        // 上报状态信息
        switch (st_index)
        {
        case 0:
            printf("$STATUS,IDLE\n");
            break;
        case 1:
            printf("$STATUS,BUSY\n");
            break;
        case 2:
            printf("$STATUS,WAIT\n");
            break;
        }

        // 上报位置信息
        sprintf(buffer, "$POS,%d,%d\n", cp_value[0], cp_value[1]);
        printf("%s", buffer);

        // 上报目标点信息
        sprintf(buffer, "$TARGET,%d,%d\n", tp_value[0], tp_value[1]);
        printf("%s", buffer);

        // 上报速度信息
        sprintf(buffer, "$SPEED,%.2f\n", se_value);
        printf("%s", buffer);

        // 上报总距离和运行时间
        sprintf(buffer, "$DIST,%.2f\n", ts_value);
        printf("%s", buffer);
        sprintf(buffer, "$TIME,%lu\n", (uint32_t)tt_value);
        printf("%s", buffer);
    }
}

void motion_control_proc(void)
{
    if (st_index != 1)
        return;

    report_system_info();

    if (first_run)
    {
        pos_x = (float)cp_value[0];
        pos_y = (float)cp_value[1];
        last_time = HAL_GetTick();
        first_run = 0;
    }

    float dt = (float)(uwTick - last_time) / 1000.0f; // 实际运算的参数

    time_ms += uwTick - last_time;
    tt_value = time_ms / 1000; // 显示器显示的参数

    last_time = HAL_GetTick();

    float target_x = (float)coordinate_arry[current_target_index][0];
    float target_y = (float)coordinate_arry[current_target_index][1];

    tp_value[0] = (uint16_t)target_x;
    tp_value[1] = (uint16_t)target_y;

    rn_value = coordinate_count - current_target_index;

    float dx = target_x - pos_x;
    float dy = target_y - pos_y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance <= 5.0f)
    {
        pos_x = target_x;
        pos_y = target_y;
        cp_value[0] = (uint16_t)pos_x;
        cp_value[1] = (uint16_t)pos_y;

        current_target_index++;

        if (current_target_index >= coordinate_count) // 已经到达目的地
        {
            st_index = 0; // 回到初始状态
            tp_value[0] = cp_value[0];
            tp_value[1] = cp_value[1];
            first_run = 1;
            current_target_index = 0;  // 重置索引，为下次运动做准备
            return;
        }

        target_x = (float)coordinate_arry[current_target_index][0];
        target_y = (float)coordinate_arry[current_target_index][1];

        tp_value[0] = (uint16_t)target_x;
        tp_value[1] = (uint16_t)target_y;

        dx = target_x - pos_x;
        dy = target_y - pos_y;
        distance = sqrt(dx * dx + dy * dy);
    }

    if (distance > 0)
    {
        float unit_x = dx / distance;
        float unit_y = dy / distance;

        float step_distance = se_value * dt; // 步进距离

        if (step_distance >= distance) // 步进距离超过实际距离
        {
            ts_value += distance;
            pos_x = target_x;
            pos_y = target_y;
        }
        else
        {
            ts_value += step_distance;
            pos_x += unit_x * step_distance;
            pos_y += unit_y * step_distance;
        }

        cp_value[0] = (uint16_t)pos_x;
        cp_value[1] = (uint16_t)pos_y;
    }
}
