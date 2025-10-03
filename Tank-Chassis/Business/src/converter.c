#include "system.h"
#include "converter.h"

double StrToDouble(const char *str)
{
    double integer = 0.0, fraction = 0.0;
    int sign = 1, decimal_places = 0;
    const char *p = str;

    if (*p == '-')
    {
        sign = -1;
        p++;
    }

    while (*p >= '0' && *p <= '9')
    {
        integer = integer * 10.0 + (*p - '0');
        p++;
    }

    if (*p == '.')
    {
        p++;
        while (*p >= '0' && *p <= '9')
        {
            fraction = fraction * 10.0 + (*p - '0');
            decimal_places++;
            p++;
        }
    }

    // 使用查表法优化pow(10,n)
    const double pow10[] = {1e0, 1e1, 1e2, 1e3, 1e4, 1e5};
    double divisor = (decimal_places < 6) ? pow10[decimal_places] : pow(10.0, decimal_places);

    return sign * (integer + fraction / divisor);
}

void ParseTankData(uint16_t data)
{
    char ch = (char)(data & 0x00FF);

    static enum {
        STATE_SEARCH_IPD,    // 搜索IPD前缀
        STATE_IN_IPD,        // 处理IPD头部
        STATE_SEARCH_HEADER, // 搜索数据包头部
        STATE_IN_HEADER,     // 处理数据包头部
        STATE_IN_DATA,       // 处理数据
        STATE_IN_TAIL        // 处理尾部
    } state = STATE_SEARCH_IPD;

    static char buffer[512]; // 更大的缓冲区
    static uint16_t buf_index = 0;
    static double values[5];
    static uint8_t value_count = 0;
    static char num_buf[16];
    static uint8_t num_index = 0;
    static uint8_t ipd_length = 0;
    static uint8_t ipd_received = 0;

    switch (state)
    {
    case STATE_SEARCH_IPD:
        if (ch == '+')
        {
            buffer[0] = ch;
            buf_index = 1;
            state = STATE_IN_IPD;
        }
        break;

    case STATE_IN_IPD:
        if (buf_index < sizeof(buffer) - 1)
        {
            buffer[buf_index++] = ch;
        }

        // 检查是否完成IPD头部 "+IPD,0,129:"
        if (ch == ':')
        {
            buffer[buf_index] = '\0';

            // 解析数据长度
            char *comma1 = strchr(buffer, ',');
            char *comma2 = strchr(comma1 + 1, ',');
            if (comma1 && comma2)
            {
                ipd_length = StrToDouble(comma2 + 1);
                ipd_received = 0;
                buf_index = 0;
                state = STATE_SEARCH_HEADER;
            }
            else
            {
                state = STATE_SEARCH_IPD;
                buf_index = 0;
            }
        }
        break;

    case STATE_SEARCH_HEADER:
        if (ch == 'C')
        {
            buffer[0] = ch;
            buf_index = 1;
            state = STATE_IN_HEADER;
        }
        ipd_received++;

        // 如果已经接收完IPD指定的长度，回到搜索IPD状态
        if (ipd_received >= ipd_length)
        {
            state = STATE_SEARCH_IPD;
            buf_index = 0;
        }
        break;

    case STATE_IN_HEADER:
        if (buf_index < sizeof(buffer) - 1)
        {
            buffer[buf_index++] = ch;
        }
        ipd_received++;

        if (buf_index == 9)
        {
            if (strncmp(buffer, "CSharpST{", 9) == 0)
            {
                state = STATE_IN_DATA;
                value_count = 0;
                num_index = 0;
            }
            else
            {
                state = STATE_SEARCH_HEADER;
                buf_index = 0;
            }
        }

        if (ipd_received >= ipd_length)
        {
            state = STATE_SEARCH_IPD;
            buf_index = 0;
        }
        break;

    case STATE_IN_DATA:
        if (buf_index < sizeof(buffer) - 1)
        {
            buffer[buf_index++] = ch;
        }
        ipd_received++;

        if (ch == ',' || ch == '}')
        {
            if (num_index > 0 && value_count < 5)
            {
                num_buf[num_index] = '\0';
                values[value_count++] = StrToDouble(num_buf);
                num_index = 0;
            }

            if (ch == '}')
            {
                state = STATE_IN_TAIL;
            }
        }
        else if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-')
        {
            if (num_index < sizeof(num_buf) - 1)
            {
                num_buf[num_index++] = ch;
            }
        }

        if (ipd_received >= ipd_length)
        {
            state = STATE_SEARCH_IPD;
            buf_index = 0;
        }
        break;

    case STATE_IN_TAIL:
        if (buf_index < sizeof(buffer) - 1)
        {
            buffer[buf_index++] = ch;
        }
        ipd_received++;

        // 检查是否完成尾部 "CSharpED"
        if (buf_index >= 8)
        {
            if (strncmp(&buffer[buf_index - 8], "CSharpED", 8) == 0)
            {
                if (value_count == 5)
                {
                    ProcessTankValues(values, value_count);
                }
                // 继续处理可能的下一个数据包
                state = STATE_SEARCH_HEADER;
                buf_index = 0;
            }
        }

        if (ipd_received >= ipd_length)
        {
            state = STATE_SEARCH_IPD;
            buf_index = 0;
        }
        break;
    }
}
