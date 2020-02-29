#include "address.h"
#include "usart.h"

unsigned char ADDRESS_INDEX = 0xFF;

void ADDRESS_SendAddress(void) {
	unsigned char data[8] = { 0xFF, 0x60 | ADDRESS_INDEX, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE };

	RS485_SendData(data, 8);
}

void ADDRESS_Check(unsigned char index) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	if (ADDRESS_INDEX == 0xFF) {
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) {
			HAL_Delay(10);
			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) {
				GPIO_InitStruct.Pin = GPIO_PIN_0;
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

				ADDRESS_INDEX = index;
				ADDRESS_SendAddress();
			}
		} else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
			HAL_Delay(10);
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
				GPIO_InitStruct.Pin = GPIO_PIN_1;
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

				ADDRESS_INDEX = index;
				ADDRESS_SendAddress();
			}
		}
	} else if (ADDRESS_INDEX == index) {
		ADDRESS_SendAddress();
	}
}
