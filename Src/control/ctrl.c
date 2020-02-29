#include "ctrl.h"
#include "adc.h"
#include "solenoid.h"
#include "address.h"
#include "usart.h"

#define snsor_base_value_off 0x045D
#define snsor_base_value_on 0x05D1

uint32_t ADC_VALUES[8];

CTRL_TASK CTRL_TASK_INSTANCE;

uint32_t ADC_GetShootValue(unsigned char index) {
	switch (index) {
	case 0:
		return ADC_VALUES[5];
	case 1:
		return ADC_VALUES[4];
	case 2:
		return ADC_VALUES[3];
	case 3:
		return ADC_VALUES[2];
	}
	return 0;
}

uint32_t ADC_GetBackValue(unsigned char index) {
	switch (index) {
	case 0:
		return ADC_VALUES[6];
	case 1:
		return ADC_VALUES[7];
	case 2:
		return ADC_VALUES[0];
	case 3:
		return ADC_VALUES[1];
	}
	return 0;
}

void CTRL_SendStatus(void) {
	unsigned char data[8] = { 0xFF, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE };

	data[1] |= ADDRESS_INDEX;
	data[2] = CTRL_TASK_INSTANCE.isBusy << 4 | CTRL_TASK_INSTANCE.errorRet;
	RS485_SendData(data, 8);
}

void CTRL_Init(void) {
	CTRL_TASK_INSTANCE.mode = CTRL_MODE_INIT;
	CTRL_TASK_INSTANCE.action = 0;
	CTRL_TASK_INSTANCE.isBusy = RESET;
	CTRL_TASK_INSTANCE.errorRet = 0;
}

void CTRL_AddTask(uint8_t *data) {
	switch (data[1]) {
	case 0x23: //CTRL_MDOE_WORK_ROUND
		if (ADDRESS_INDEX != 0xff && CTRL_TASK_INSTANCE.isBusy == RESET
				&& data[3 + ADDRESS_INDEX] != 0) {
			CTRL_TASK_INSTANCE.isBusy = SET;
			CTRL_TASK_INSTANCE.errorRet = 0;
			CTRL_TASK_INSTANCE.mode = CTRL_MDOE_WORK_ROUND;
			CTRL_TASK_INSTANCE.action = data[3 + ADDRESS_INDEX];
		}
		break;
	case 0x25: //CTRL_MODE_WORK_SHOOT
		if (ADDRESS_INDEX != 0xff && CTRL_TASK_INSTANCE.isBusy == RESET
				&& data[3 + ADDRESS_INDEX] != 0) {
			CTRL_TASK_INSTANCE.isBusy = SET;
			CTRL_TASK_INSTANCE.errorRet = 0;
			CTRL_TASK_INSTANCE.mode = CTRL_MODE_WORK_SHOOT;
			CTRL_TASK_INSTANCE.action = data[3 + ADDRESS_INDEX];
		}
		break;
	case 0x26: //CTRL_MODE_WORK_BACK
		if (ADDRESS_INDEX != 0xff && CTRL_TASK_INSTANCE.isBusy == RESET
				&& data[3 + ADDRESS_INDEX] != 0) {
			CTRL_TASK_INSTANCE.isBusy = SET;
			CTRL_TASK_INSTANCE.errorRet = 0;
			CTRL_TASK_INSTANCE.mode = CTRL_MODE_WORK_BACK;
			CTRL_TASK_INSTANCE.action = data[3 + ADDRESS_INDEX];
		}
		break;
	case 0x40: //CTRL_MODE_QUERY
		if (ADDRESS_INDEX == data[2]) {
			CTRL_SendStatus();
		}
		break;
	default:
		if (data[2] == 0xAD) //CTRL_MODE_INIT
			ADDRESS_Check(data[1]);
		break;
	}
}

void CTRL_WorkShoot(unsigned char action) {
	unsigned char i;

	for (i = 0; i < 4; i++) {
		if ((action >> i) & 0x01)
			SOLENOID_WorkShoot(i);
	}
}

void CTRL_WorkBack(unsigned char action) {
	unsigned char i;

	for (i = 0; i < 4; i++) {
		if ((action >> i) & 0x01)
			SOLENOID_WorkBack(i);
	}
}

unsigned char CTRL_CheckWorkShoot(unsigned char action) {
	unsigned char i = 0;
	unsigned short timeout = 1000;

	while (1) {
		for (i = 0; i < 4; i++) {
			if ((action >> i) & 0x01) {
				if (ADC_GetShootValue(i) > snsor_base_value_on) {
					action &= ~(1 << i);
				}
			}
		}

		if (action == 0)
			break;

		HAL_Delay(1);
		timeout--;
		if (timeout == 0)
			break;
	}

	if (action != 0)
		CTRL_WorkBack(action);

	return action;
}

unsigned char CTRL_CheckWorkBack(unsigned char action) {
	unsigned char i = 0;
	unsigned short timeout = 1000;

	while (1) {
		for (i = 0; i < 4; i++) {
			if ((action >> i) & 0x01) {
				if (ADC_GetBackValue(i) < snsor_base_value_off) {
					action &= ~(1 << i);
				}
			}
		}

		if (action == 0)
			break;

		HAL_Delay(1);
		timeout--;
		if (timeout == 0)
			break;
	}
	return action;
}

void CTRL_Work(void) {
	if (CTRL_TASK_INSTANCE.isBusy == SET) {
		switch (CTRL_TASK_INSTANCE.mode) {
		case CTRL_MDOE_WORK_ROUND:
			CTRL_WorkShoot(CTRL_TASK_INSTANCE.action);
			HAL_Delay(50);
			CTRL_TASK_INSTANCE.errorRet = CTRL_CheckWorkShoot(CTRL_TASK_INSTANCE.action);
			if (CTRL_TASK_INSTANCE.errorRet == 0) {
				CTRL_WorkBack(CTRL_TASK_INSTANCE.action);
				HAL_Delay(50);
				CTRL_TASK_INSTANCE.errorRet = CTRL_CheckWorkBack(CTRL_TASK_INSTANCE.action);
			}
			break;
		case CTRL_MODE_WORK_SHOOT:
			CTRL_WorkShoot(CTRL_TASK_INSTANCE.action);
			HAL_Delay(50);
			CTRL_TASK_INSTANCE.errorRet = CTRL_CheckWorkShoot(CTRL_TASK_INSTANCE.action);
			break;
		case CTRL_MODE_WORK_BACK:
			CTRL_WorkBack(CTRL_TASK_INSTANCE.action);
			HAL_Delay(50);
			CTRL_TASK_INSTANCE.errorRet = CTRL_CheckWorkBack(CTRL_TASK_INSTANCE.action);
			break;
		case CTRL_MODE_QUERY:
			CTRL_SendStatus();
			break;
		default: //CTRL_MODE_INIT CTRL_MODE_QUERY
			break;
		}

		CTRL_TASK_INSTANCE.isBusy = RESET;
	}
}
