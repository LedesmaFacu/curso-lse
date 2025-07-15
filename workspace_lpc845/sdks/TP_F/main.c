#include "board.h"
#include "task.h"
#include "FreeRTOS.h"
#include "app_tasks.h"

    // Estructura para crear las tareas

    // xTaskCreate(TaskCode,     
    //     "Nombre",
    //     STACK_SIZE,
    //     &ucParametertoPass,
    //     tskIDLE_Priority,
    //     &xHandle
    // );

int main(void){

    BOARD_BootClockFRO30M

    xTaskCreate(task_init,         
        "Iniciar periféricos",
        tskINIT_STACK,
        NULL,
        tskINIT_PRIORITY,
        NULL
    );
    
    xTaskCreate(task_adc,         
        "ADC",
        tskADC_STACK,
        NULL,
        tskADC_PRIORITY,
        NULL
    );

    xTaskCreate(task_BH1750,         
        "BH1750",
        tskBH1750_STACK,
        NULL,
        tskBH1750_PRIORITY,
        NULL
    );

    xTaskCreate(task_setpoint,         
        "Setpoint",
        tskSETPOINT_STACK,
        NULL,
        tskSETPOINT_PRIORITY,
        NULL
    );

    xTaskCreate(task_display_change,         
        "Display Change",
        tskDISPLAY_CHANGE_STACK,
        NULL,
        tskDISPLAY_CHANGE_PRIORITY,
        NULL
    )

    xTaskCreate(task_control,         
        "Control",
        tskCONTROL_STACK,
        NULL,
        tskCONTROL_PRIORITY,
        NULL
    );

    xTaskCreate(task_display,         
        "Display",
        tskDISPLAY_STACK,
        NULL,
        tskDISPLAY_PRIORITY,
        NULL
    );

    xTaskCreate(task_led_azul_rv22,         
        "LED Azul y RV22",
        tskLED_AZUL_RV22_STACK,
        NULL,
        tskLED_AZUL_RV22_PRIORITY,
        NULL
    );

    xTaskCreate(task_tricolor,         
        "LED Tricolor",
        tskTRICOLOR_STACK,
        NULL,
        tskTRICOLOR_PRIORITY,
        NULL
    );

    xTaskCreate(task_buzzer,         
        "Buzzer",
        tskBUZZER_STACK,
        NULL,
        tskBUZZER_PRIORITY,
        NULL
    );

    xTaskCreate(task_logger,         
        "Logger",
        tskLOGGER_STACK,
        NULL,
        tskLOGGER_PRIORITY,
        NULL
    );

    

}