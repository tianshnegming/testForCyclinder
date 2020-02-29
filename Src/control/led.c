#include "led.h"
#include "ctrl.h"
#include "address.h"

void LED_Red(FlagStatus status) {
	if (status == SET)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}

void LED_Green(FlagStatus status) {
	if (status == SET)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

void LED_RedToggle(void) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
}

void LED_GreenToggle(void) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
}

void LED_Work(void) {
	if (CTRL_TASK_INSTANCE.errorRet == 0 && ADDRESS_INDEX != 0xFF) {
		LED_Red(RESET);
		LED_GreenToggle();
	} else {
		LED_Green(RESET);
		LED_RedToggle();
	}
}
