#include "system.h"
#include "converter.h"

static char tankBuffer[256];
static uint16_t bufferIndex = 0;
static uint8_t inTankData = 0;

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

    // ʹ�ò���Ż�pow(10,n)
    const double pow10[] = {1e0, 1e1, 1e2, 1e3, 1e4, 1e5};
    double divisor = (decimal_places < 6) ? pow10[decimal_places] : pow(10.0, decimal_places);

    return sign * (integer + fraction / divisor);
}

void ParseTankData(uint16_t data)
{
    static char currentChar;
    static uint8_t dataCount = 0;
    static char valueBuffer[16];
    static uint8_t valueIndex = 0;
    static double tankValues[5];

    currentChar = (char)data;

    // ������ݿ�ʼ���
    if (!inTankData)
    {
        if (currentChar == 'C')
        {
            bufferIndex = 0;
            tankBuffer[bufferIndex++] = currentChar;
        }
        else if (bufferIndex > 0 && bufferIndex < 10)
        {
            tankBuffer[bufferIndex++] = currentChar;
            tankBuffer[bufferIndex] = '\0';

            // ����Ƿ�ƥ��"CSharpST{"
            if (bufferIndex == 9 && strcmp(tankBuffer, "CSharpST{") == 0)
            {
                inTankData = 1;
                dataCount = 0;
                valueIndex = 0;
                memset(tankValues, 0, sizeof(tankValues));
            }
        }
        else
        {
            bufferIndex = 0;
        }
        return;
    }

    // ����Tank��������
    if (inTankData)
    {
        // ������ݽ������
        if (currentChar == 'C' && valueIndex >= 7)
        {
            char checkBuffer[10];
            strncpy(checkBuffer, &tankBuffer[bufferIndex - 7], 8);
            checkBuffer[8] = '\0';

            if (strcmp(checkBuffer, "CSharpED") == 0)
            {
                // ���ݰ�������ɣ��������һ��ֵ
                if (valueIndex > 0 && dataCount < 5)
                {
                    valueBuffer[valueIndex] = '\0';
                    tankValues[dataCount++] = StrToDouble(valueBuffer);
                }

                ProcessTankValues(tankValues, dataCount);

                inTankData = 0;
                bufferIndex = 0;
                return;
            }
        }

        // ������������
        if (currentChar == ',' || currentChar == '}')
        {
            // ���һ��ֵ�Ľ���
            if (valueIndex > 0 && dataCount < 5)
            {
                valueBuffer[valueIndex] = '\0';
                tankValues[dataCount++] = StrToDouble(valueBuffer);
                valueIndex = 0;
            }
        }
        else if ((currentChar >= '0' && currentChar <= '9') ||
                 currentChar == '.' || currentChar == '-')
        {
            // �ռ���ֵ�ַ�
            if (valueIndex < 15 && dataCount < 5)
            {
                valueBuffer[valueIndex++] = currentChar;
            }
        }

        // ���浽�����������ڼ�������ǣ�
        if (bufferIndex < 255)
        {
            tankBuffer[bufferIndex++] = currentChar;
            tankBuffer[bufferIndex] = '\0';
        }
    }
}
