#include "bms_comm.h"


#define uart_len 255
#define uart_time 500

uint8_t rx_count = 0;
uint8_t tx_count = 0;
uint8_t rx_buffer[uart_len];
uint8_t tx_buffer[uart_len];
uint8_t uart_tmp = 0;
uint8_t rx_buffer_ble[uart_len];
extern uint8_t rx_data[uart_len];
extern uint32_t tx_bms_time;
extern uint8_t bms_cmd;
extern uint32_t uart_tmp_time;

void PrintfReadData(uint8_t rx_cmd)
{
    uint16_t voltage = 0;
    int16_t current = 0;

    switch (rx_cmd)
    {
        case 0x10:
            switch (rx_buffer_ble[7])
            {
                case 0x01:
                    printf("\n 放电模式");
                    break;

                case 0x10:
                    printf("\n 充电模式");
                    break;

                case 0x21:
                    printf("\n 保护模式");
                    break;

                case 0x30:
                    printf("\n 待机无输出模式");
                    break;

                case 0x31:
                    printf("\n 待机预放模式");
                    break;

                case 0xFF:
                    printf("\n 故障需返厂");
                    break;
            }

            voltage = (rx_buffer_ble[9] << 8) + rx_buffer_ble[8];
//						printf("\n %d \n",rx_buffer_ble[8]);
//						printf("\n %d \n",rx_buffer_ble[9]);
            printf("\n 实时电压：%dmV", voltage);
            current = (rx_buffer_ble[11] << 8) + rx_buffer_ble[10];
            printf("\n 实时电流：%dmA", current);
            printf("\n SOC：%d", rx_buffer_ble[12]);

            typedef union Protection_State
            {
                uint16_t P_state;
                struct
                {
                    uint16_t DCLT: 1;	//放电低温
                    uint16_t DCHT: 1;	//放电高温
                    uint16_t CHT:  1;	//充电高温
                    uint16_t CLT:  1;	//充电低温
                    uint16_t DOC:  1;	//放电过流
                    uint16_t DUV:  1;	//放电欠压
                    uint16_t COC:  1;	//充电过流
                    uint16_t COV:  1;	//充电过压
                    uint16_t SC:   1;	//短路
                    uint16_t ETD:  1;	//温差过大
                    uint16_t EDP:  1;	//压差过大
                    uint16_t ICCT: 1;	//智能充电通信超时
                    uint16_t ICP:  1;	//IC保护
                    uint16_t PDF:  1;	//预放电失败
                    uint16_t RET1: 1;	//保留
                    uint16_t RET2: 1;	//保留
                }	bits;

            } Protection_State_Struct;

            volatile Protection_State_Struct BMS_FAULT;
            BMS_FAULT.P_state = (rx_buffer_ble[14] << 8) + rx_buffer_ble[13];

            if(BMS_FAULT.P_state != 0)
            {
                if(BMS_FAULT.bits.DCLT == 1)
                {
                    printf ("\n 放电低温");
                }
                else if(BMS_FAULT.bits.DCHT == 1)
                {
                    printf ("\n 放电高温");
                }
                else if(BMS_FAULT.bits.CHT == 1)
                {
                    printf ("\n 充电高温");
                }
                else if(BMS_FAULT.bits.CLT == 1)
                {
                    printf ("\n 充电低温");
                }
                else if(BMS_FAULT.bits.DOC == 1)
                {
                    printf ("\n 放电过流");
                }
                else if(BMS_FAULT.bits.DUV == 1)
                {
                    printf ("\n 放电欠压");
                }
                else if(BMS_FAULT.bits.COC == 1)
                {
                    printf ("\n 充电过流");
                }
                else if(BMS_FAULT.bits.COV == 1)
                {
                    printf ("\n 充电过压");
                }
                else if(BMS_FAULT.bits.SC == 1)
                {
                    printf ("\n 短路");
                }
                else if(BMS_FAULT.bits.ETD == 1)
                {
                    printf ("\n 温差过大");
                }
                else if(BMS_FAULT.bits.EDP == 1)
                {
                    printf ("\n 压差过大");
                }
                else if(BMS_FAULT.bits.ICCT == 1)
                {
                    printf ("\n 智能充电通信超时");
                }
                else if(BMS_FAULT.bits.ICP == 1)
                {
                    printf ("\n IC保护");
                }
                else if(BMS_FAULT.bits.PDF == 1)
                {
                    printf ("\n 预放电失败");
                }
                else if(BMS_FAULT.bits.RET1 == 1)
                {
                    printf ("\n 保留");
                }
                else if(BMS_FAULT.bits.RET2 == 1)
                {
                    printf ("\n 保留");
                }
            }
            else
            {
                printf ("\n 保护状态正常");
            }

            typedef union Fault_State
            {
                uint8_t F_state;
                struct
                {
                    uint8_t RET:  1;	//保留
                    uint8_t BRE:  1;	//采样线断线、虚焊
                    uint8_t TSD:  1;	//温度传感器损坏
                    uint8_t ICD:  1;	//IC损坏
                    uint8_t CMD:  1;	//充电MOS损坏
                    uint8_t DMD:  1;	//放电MOS损坏
                    uint8_t UNB:  1;	//失衡
                    uint8_t INV:  1;	//失效

                }	bits;

            } Fault_State_Struct;

            volatile Fault_State_Struct BMS_FAULT_STATE;
            BMS_FAULT_STATE.F_state = rx_buffer_ble[15];

            if(BMS_FAULT.P_state != 0)
            {
                if(BMS_FAULT_STATE.bits.RET == 1)
                {
                    printf ("\n 保留");
                }
                else if(BMS_FAULT_STATE.bits.BRE == 1)
                {
                    printf ("\n 采样线断线、虚焊");
                }
                else if(BMS_FAULT_STATE.bits.TSD == 1)
                {
                    printf ("\n 温度传感器损坏");
                }
                else if(BMS_FAULT_STATE.bits.ICD == 1)
                {
                    printf ("\n IC损坏");
                }
                else if(BMS_FAULT_STATE.bits.CMD == 1)
                {
                    printf ("\n 充电MOS损坏");
                }
                else if(BMS_FAULT_STATE.bits.DMD == 1)
                {
                    printf ("\n 放电MOS损坏");
                }
                else if(BMS_FAULT_STATE.bits.UNB == 1)
                {
                    printf ("\n 失衡");
                }
                else if(BMS_FAULT_STATE.bits.INV == 1)
                {
                    printf ("\n 失效");
                }
            }
            else
            {
                printf ("\n 无故障");
            }


            printf("\n 当前电芯最高温度：%d度", (int8_t)rx_buffer_ble[16]);
            printf("\n 当前电芯最低温度：%d度", (int8_t)rx_buffer_ble[17]);
            printf("\n 当前MOS最高温度：%d度", (int8_t)rx_buffer_ble[18]);
            printf("\n 当前PCB最高温度：%d度\n", (int8_t)rx_buffer_ble[19]);
            break;
    }
}

/* Based on the polynomial x^16+x^12+x^5+1 */
const uint16_t CrcCcittTable[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};



unsigned int GetCRCCode(const uint8_t *pBuf, uint16_t len)
{
    uint16_t i;
    uint16_t j, crc_text = 0;

    for( i = 0; i < len; i++ )
    {
        j = (crc_text >> 8) ^ pBuf[i];
        crc_text = (crc_text << 8) ^ CrcCcittTable[j];
    }

    return crc_text;
}



void Tx_Meituan_Data(uint8_t cmd, uint8_t cmd_data)
{

    uint16_t crc;

    crc = 0;

    tx_count ++;

    switch (cmd)
    {
        case 0x00:		//电池输出开关
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 1;
            tx_buffer[6] = 0x00;
            tx_buffer[7] = cmd_data;

            if(cmd_data == 0x01 || cmd_data == 0x02)
            {
                crc = GetCRCCode(tx_buffer, 8);
                tx_buffer[8] = (uint8_t)crc;
                tx_buffer[9] = (uint8_t)(crc >> 8);
                HAL_UART_Transmit_IT(&huart1, tx_buffer, 10);

                crc = 1;
            }

            break;

        case 0x02:		//电池休眠开关
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 1;
            tx_buffer[6] = 0x00;
            tx_buffer[7] = cmd_data;

            if(cmd_data == 0xF0)
            {
                crc = GetCRCCode(tx_buffer, 8);
                tx_buffer[8] = (uint8_t)crc;
                tx_buffer[9] = (uint8_t)(crc >> 8);
                HAL_UART_Transmit_IT(&huart1, tx_buffer, 10);
                crc = 1;
            }

            break;

        case 0x10:		//电池工作状态
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x11:		//电池使用状态
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x13:		//电池基本信息
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x14:		//电芯信息
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x15:		//MOS状态查询
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0x00;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x16:		//物料变更记录查询
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0x00;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x17:		//静态数据
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0x00;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x18:		//动态数据
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0x00;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x19:		//电池日志信息查询
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 0;
            tx_buffer[6] = 0x00;

            crc = GetCRCCode(tx_buffer, 7);
            tx_buffer[7] = (uint8_t)crc;
            tx_buffer[8] = (uint8_t)(crc >> 8);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, 9);
            crc = 1;

            break;

        case 0x21:		//电池工作模式
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 1;
            tx_buffer[6] = 0x00;
            tx_buffer[7] = cmd_data;

            if(cmd_data == 0x01 || cmd_data == 0x02 || cmd_data == 0x03 || cmd_data == 0xA5)
            {
                crc = GetCRCCode(tx_buffer, 8);
                tx_buffer[8] = (uint8_t)crc;
                tx_buffer[9] = (uint8_t)(crc >> 8);
                HAL_UART_Transmit_IT(&huart1, tx_buffer, 10);
                crc = 1;
            }

            break;

        case 0x22:		//电池充电模式
            tx_buffer[0] = 0xAA;
            tx_buffer[1] = 0x55;
            tx_buffer[2] = 0xBB;
            tx_buffer[3] = 0x03;
            tx_buffer[4] = cmd;
            tx_buffer[5] = 1;
            tx_buffer[6] = 0x00;
            tx_buffer[7] = cmd_data;

            if(cmd_data == 0x01 || cmd_data == 0x02)
            {
                crc = GetCRCCode(tx_buffer, 8);
                tx_buffer[8] = (uint8_t)crc;
                tx_buffer[9] = (uint8_t)(crc >> 8);
                HAL_UART_Transmit_IT(&huart1, tx_buffer, 10);
                crc = 1;
            }

            break;
    }

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef  * huart)
{
    if(huart->Instance == USART1)
    {
        if(rx_count < uart_len)
        {
            rx_count ++;
            HAL_UART_Receive_IT(&huart1, rx_data + rx_count, 1);

            uart_tmp_time = HAL_GetTick();

            rx_buffer[rx_count - 1] = rx_data[rx_count - 1];
        }
        else
        {
            HAL_UART_Receive_IT(&huart1, &uart_tmp, 1);
        }
    }
}


void ReadData(uint8_t ble_cmd, uint8_t ble_cmd_data)
{
    int i, j, k;

    if(rx_count == 0 && (HAL_GetTick() - tx_bms_time > 500) && tx_count > 0 && tx_count < 3)
    {
        Tx_Meituan_Data(ble_cmd, ble_cmd_data);
        tx_bms_time = HAL_GetTick();
    }
    else
    {
        if (rx_count != 0 && (HAL_GetTick() - uart_tmp_time > uart_time))
        {
            HAL_UART_Abort_IT(&huart1);
//            HAL_UART_Transmit(&huart1, rx_buffer, rx_count, 100);

            for(i = 0; i <= rx_count; i ++)
            {
                k = 0;

                if(rx_buffer[i] == 0xAA && rx_buffer[i + 1] == 0x55 && rx_buffer[i + 2] == 0xBB && rx_buffer[i + 3] == 0x03)
                {
                    for(j = i; j <= rx_count; j ++)
                    {
                        rx_buffer_ble[k++] = rx_buffer[j];

                        if(k > 4)
                        {
                            uint16_t rx_crc;
                            uint8_t  ret_H;
                            uint8_t  ret_L;
                            rx_crc = GetCRCCode(rx_buffer_ble, k - 3);
                            ret_L = (uint8_t)rx_crc;
                            ret_H = (uint8_t)(rx_crc >> 8);;

                            if(rx_buffer_ble[k - 3] == ret_L && rx_buffer_ble[k - 2] == ret_H)
                            {
//                                HAL_UART_Transmit_IT(&huart1, rx_buffer_ble, k - 1);
                                PrintfReadData(rx_buffer_ble[4]);

                                rx_count = 0;
                                tx_count = 0;
                            }
                        }
                    }
                }
                else if(i > rx_count - 2)
                {
                    rx_count = 0;
                }
            }

            HAL_UART_Receive_IT(&huart1, rx_data, 1);
        }
    }
}


