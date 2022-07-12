#ifndef __RTOS_CONFIG_H__
#define __RTOS_CONFIG_H__

#define STATUS_TASK_STACK_SIZE 2048
#define STATUS_TASK_PRIORITY 5
#define STATUS_TASK_DELAY 100

#define FSM_TASK_STACK_SIZE 1024
#define FSM_TASK_PRIORITY 2
#define FSM_TASK_DELAY 50

#define CONTROL_TASK_STACK_SIZE 1024
#define CONTROL_TASK_PRIORITY 1
#define CONTROL_TASK_DELAY 500

#define TRP_TASK_STACK_SIZE 1280
#define TRP_TASK_PRIORITY 1

#define PID_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define PID_TASK_PRIORITY 3

#endif