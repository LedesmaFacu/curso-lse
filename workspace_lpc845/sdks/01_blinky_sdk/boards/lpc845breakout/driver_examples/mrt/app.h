<<<<<<< HEAD
/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
#define APP_LED_INIT   LED_RED_INIT(1)
#define APP_LED_ON     (LED_RED_ON())
#define APP_LED_TOGGLE (LED_RED_TOGGLE())
#define MRT_CLK_FREQ   CLOCK_GetFreq(kCLOCK_CoreSysClk)
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
/*${prototype:end}*/

#endif /* _APP_H_ */
=======
/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
#define APP_LED_INIT   LED_RED_INIT(1)
#define APP_LED_ON     (LED_RED_ON())
#define APP_LED_TOGGLE (LED_RED_TOGGLE())
#define MRT_CLK_FREQ   CLOCK_GetFreq(kCLOCK_CoreSysClk)
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
/*${prototype:end}*/

#endif /* _APP_H_ */
>>>>>>> 499c6fc4aa5e186623e87db8d36a35287ffd5cbb
