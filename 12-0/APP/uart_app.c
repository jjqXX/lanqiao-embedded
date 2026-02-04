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

typedef struct
{
    char vehicle_id[5];    // 车辆ID
    time_t entry_time;     // 进入停车场时间，时间戳格式
    uint8_t is_in_parking; // 标志车辆是否在停车场
    char parking_type[5];  // 停车场类型
} VehicleRecord;

typedef struct
{
    char parking_type[5]; // 停车场类型
    char vehicle_id[5];   // 车辆ID
    char entry_time[13];  // 进入停车场时间，格式为YYYYMMDDHHMM
} VehicleInfo;

#define MAX_VEHICLES 8                     //最大停车数量
VehicleRecord vehicle_records[MAX_VEHICLES]; // 停车记录数组
uint8_t vehicle_count = 0;                       // 停车记录数组中有效的车辆数量

VehicleRecord *find_vehicle_record(const char *vehicle_id)
{
    for (int i = 0; i < vehicle_count; i++)
    {
        if (strcmp(vehicle_records[i].vehicle_id, vehicle_id) == 0)
        {
            return &vehicle_records[i];
        }
    }
    return NULL;
}

time_t convert_to_timestamp(const char *time_str)
{
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    sscanf(time_str, "%4d%2d%2d%2d%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min);
    t.tm_year -= 1900; // struct tm中的年份是从1900年开始计算的
    t.tm_mon -= 1;     // struct tm中的月份是0-11
    return mktime(&t);
}

uint32_t calculate_parking_duration(time_t entry_time, time_t exit_time)
{
    double duration_seconds = difftime(exit_time, entry_time);
    uint32_t duration_hours = (uint32_t)((duration_seconds + 3599) / 3600); // 3600秒为1小时，四舍五入
    return duration_hours;
}

void format_time(time_t time, char *formatted_time)
{
    struct tm *tm_info = localtime(&time);
    strftime(formatted_time, 100, "%Y年%m月%d日%H小时%M分钟", tm_info);
}

uint8_t validate_command_format(const char *data)
{
    // 校验字符串长度和分隔符位置
    if (strlen(data) != 22 || data[4] != ':' || data[9] != ':')
    {
        printf("命令格式错误: 长度或分隔符位置不正确\n");
        return 0;
    }

    // 校验停车类型，必须是CNBR或VNBR
    if (strncmp(data, "CNBR", 4) != 0 && strncmp(data, "VNBR", 4) != 0)
    {
        printf("命令格式错误: 停车类型不正确\n");
        return 0;
    }

    // 校验车辆编号部分，必须是4个字母或数字组成
    for (int i = 5; i < 9; i++)
    {
        if (!isalnum(data[i]))
        {
            printf("命令格式错误: 车辆编号部分包含非法字符\n");
            return 0;
        }
    }

    // 解析并校验日期时间是否合法
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    int result = sscanf(data + 10, "%4d%2d%2d%2d%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min);
    if (result != 5)
    {
        printf("日期/时间解析失败: %s\n", data + 10);
        return 0;
    }

    t.tm_year -= 1900;
    t.tm_mon -= 1;

    printf("解析后的时间: 年=%d, 月=%d, 日=%d, 时=%d, 分=%d\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min);

    // 检查年份、月份、日期、小时和分钟的合理性
    if (t.tm_year < 0 || t.tm_mon < 0 || t.tm_mon > 11 || t.tm_mday < 1 || t.tm_mday > 31 || t.tm_hour < 0 || t.tm_hour > 23 || t.tm_min < 0 || t.tm_min > 59)
    {
        printf("日期/时间组件不合理\n");
        return 0;
    }

    // 检查时间转换是否有效
    time_t timestamp = mktime(&t);
    if (timestamp == (time_t)-1)
    {
        printf("mktime转换时间失败\n");
        return 0;
    }

    // 车辆记录数组已满
    if (vehicle_count >= MAX_VEHICLES)
    {
        printf("车辆记录数组已满\n");
        return 0;
    }

    // 查找车辆记录，判断是否存在
    char vehicle_id[5];
    strncpy(vehicle_id, data + 5, 4); // 提取车辆ID
    vehicle_id[4] = '\0';             // 添加字符串终止符

    VehicleRecord *record = find_vehicle_record(vehicle_id);

    if (record != NULL)
    {
        printf("车辆记录已存在: ID=%s, 停车场类型=%s\n", record->vehicle_id, record->parking_type);

        if (record->is_in_parking)
        {
            // 车辆在库中，检查停车场类型是否一致
            if (strncmp(record->parking_type, data, 4) != 0)
            {
                printf("停车场类型不一致\n");
                return 0;
            }

            // 车辆在库中，则当前时间必须大于入库时间
            if (difftime(timestamp, record->entry_time) < 0)
            {
                printf("当前时间小于入库时间\n");
                return 0;
            }
        }
    }
    else
    {
        printf("车辆记录不存在: ID=%s\n", vehicle_id);
    }
    printf("命令格式校验通过\n");
    return 1;
}

void handle_vehicle_entry_or_exit(char *data)
{
    if (!validate_command_format(data))
    {
        printf("Error\n");
        return;
    }

    VehicleInfo vehicle_info;
    sscanf(data, "%4s:%4s:%12s", vehicle_info.parking_type, vehicle_info.vehicle_id, vehicle_info.entry_time);

    // 将时间字符串转换为时间戳
    time_t parsed_time = convert_to_timestamp(vehicle_info.entry_time);

    // 查找车辆记录
    VehicleRecord *record = find_vehicle_record(vehicle_info.vehicle_id);

    if (record == NULL)
    {   // 车辆记录不存在，表示入场
        // 创建新的车辆记录
        strcpy(vehicle_records[vehicle_count].vehicle_id, vehicle_info.vehicle_id);
        strcpy(vehicle_records[vehicle_count].parking_type, vehicle_info.parking_type);
        vehicle_records[vehicle_count].entry_time = parsed_time;
        vehicle_records[vehicle_count].is_in_parking = 1;
        vehicle_count++;
        printf("车辆ID %s (类型: %s) 进入停车场\n", vehicle_info.vehicle_id, vehicle_info.parking_type);

        if (strcmp(vehicle_info.parking_type, "VNBR") == 0)
            vnbr_cnt++;
        else if (strcmp(vehicle_info.parking_type, "CNBR") == 0)
            cnbr_cnt++;
    }
    else
    {   // 车辆记录存在，表示出场
        printf("车辆ID %s (类型: %s) 离开停车场\n", vehicle_info.vehicle_id, record->parking_type);

        // 计算停车时长和费用
        uint32_t parking_duration = calculate_parking_duration(record->entry_time, parsed_time);
        float fee = 0;
        if (strcmp(vehicle_info.parking_type, "VNBR") == 0)
        {
            fee = (float)parking_duration * vnbr_fee;
            vnbr_cnt--;
        }
        else if (strcmp(vehicle_info.parking_type, "CNBR") == 0)
        {
            fee = (float)parking_duration * cnbr_fee;
            cnbr_cnt--;
        }
        printf("%.4s:%.4s:%u:%.2f\n", vehicle_info.parking_type, vehicle_info.vehicle_id, parking_duration, fee);
        printf("停车时长: %u小时\n", parking_duration);
        printf("停车费用: %.2f元\n", fee);

        // 移除车辆记录并调整数组，前移记录
        int index = record - vehicle_records;
        for (int i = index; i < vehicle_count - 1; i++)
        {
            vehicle_records[i] = vehicle_records[i + 1];
        }
        vehicle_count--;
    }
}

void print_all_parking_info(void)
{
    printf("---------------------------------------------------\r\n");
    printf("车库内所有停车信息:\n");
    for (int i = 0; i < vehicle_count; i++)
    {
        if (vehicle_records[i].is_in_parking)
        {
            char formatted_time[100];
            format_time(vehicle_records[i].entry_time, formatted_time);
            printf("车辆ID: %s 类型: %s 进入时间: %s\r\n", vehicle_records[i].vehicle_id, vehicle_records[i].parking_type, formatted_time);
        }
    }
    printf("---------------------------------------------------\r\n");
}

/**
 * @brief  处理UART接收缓冲区中的数据。
 *         如果在100ms内没有接收到新的数据，将清空缓冲区。
 * @param  None
 * @retval None
 */
void uart_proc(void)
{
    // 如果环形缓冲区为空，直接返回
    if (ringbuffer_is_empty(&usart_rb))
        return;

    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb, usart_read_buffer, usart_rb.itemCount);

    // 打印读取缓冲区中的数据
    printf("ringbuffer data: %s\n", usart_read_buffer);

    // 处理车辆入场或出场
    handle_vehicle_entry_or_exit((char *)usart_read_buffer);

    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);

    // 打印所有停车信息
    print_all_parking_info();
}


