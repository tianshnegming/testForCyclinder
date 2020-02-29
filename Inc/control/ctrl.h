#ifndef __control_ctrl_H
#define __control_ctrl_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

typedef enum {
	CTRL_MODE_INIT = 0, CTRL_MODE_QUERY = 1, CTRL_MODE_WORK_SHOOT = 2, CTRL_MODE_WORK_BACK = 3, CTRL_MDOE_WORK_ROUND = 4
} CTRL_MODE;

typedef struct {
	CTRL_MODE mode;
	unsigned char action;
	FlagStatus isBusy;
	unsigned char errorRet;
} CTRL_TASK;

extern CTRL_TASK CTRL_TASK_INSTANCE;
extern uint32_t ADC_VALUES[8];

void CTRL_Init(void);
void CTRL_AddTask(uint8_t *data);
void CTRL_Work(void);

#ifdef __cplusplus
}
#endif
#endif
