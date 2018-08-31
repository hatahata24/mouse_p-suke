/*
 * pinassign.h
 *
 *  Created on: 2017/11/17
 *      Author: blue
 */

#ifndef PINASSIGN_H_
#define PINASSIGN_H_


/*============================================================
		ピン配置設定
============================================================*/
/*------------------------------------------------------------
		走行系
------------------------------------------------------------*/
//----ドライバIC関連----
enum {
	PIN_CLK_L = PB4,		//クロック端子左
	PIN_CLK_R = PB5,		//クロック端子右
	PIN_CW_R = PB6,
	PIN_CW_L = PB3,
	//PIN_REF = PXn,		//リファレンス
	//PIN_M1 = PXn,			//励磁切り替えロジック1
	//PIN_M2 = PXn,			//励磁切り替えロジック2
	PIN_M3 = PB7			//励磁切り替えロジック3
};

/* 励磁切り替えロジック
・SLA7078MPRT
 M1 | M2 | M3 |励磁モード
  L |  L |  L | 2相励磁（Mode8固定）
  L |  H |  L | 2相励磁（ModeF固定）
  H |  L |  L | 1-2相励磁（2分割）
  H |  H |  L | 1-2相励磁（ModeF固定）
  L |  L |  H |  W1-2相（ 4分割）
  L |  H |  H | 4W1-2相（16分割）
  H |  L |  H | 2W1-2相（ 8分割）
  H |  H |  H | スリープモード2

・SLA7073MPRT
 M1 | M2 | 励磁モード
  L |  L | 4W1-2相（16分割）
  L |  H | 2W1-2相（ 8分割）
  H |  L |  W1-2相（ 4分割）
  H |  H |   1-2相（ 2分割）
*/

//----動作方向関連----
#define MT_FWD_L	HIGH		//CW/CCWで前に進む出力（左）
#define MT_BACK_L	LOW			//CW/CCWで後ろに進む出力（左）
#define MT_FWD_R	LOW			//CW/CCWで前に進む出力（右）
#define MT_BACK_R	HIGH		//CW/CCWで後ろに進む出力（右）


/*------------------------------------------------------------
		LED・スイッチ系
------------------------------------------------------------*/
/*
 * SW_INC, SW_DEC, SW_RET : Input, Pullup
 */
enum {
	PIN_SW_INC = PA8,
	PIN_SW_DEC = PA11,
	PIN_SW_RET = PA15
};

/*
 * LED0~2 : Output
 */
enum {
	PIN_LED1 = PA13,
	PIN_LED2 = PA12,
	PIN_LED3 = PA14
};


/*------------------------------------------------------------
		センサ系
------------------------------------------------------------*/
//----センサ（PhotoTr）ピン番号----
enum {
	PIN_SENSOR_R = PA2,
	PIN_SENSOR_L = PA5,
	PIN_SENSOR_FR = PA0,
	PIN_SENSOR_FL = PA7,
	PIN_VOLTAGE_CHECK = PB1
};


//----赤外線（赤色）LEDピン番号----
enum {
	PIN_IR_R = PA3,
	PIN_IR_L = PA4,
	PIN_IR_FR = PA1,
	PIN_IR_FL = PA6
};

//----センサ（PhotoTr）ADコンバータポート番号----
enum {
	ADC_SENSOR_R = ADC1_CH3,
	ADC_SENSOR_L = ADC2_CH2,
	ADC_SENSOR_FR = ADC1_CH1,
	ADC_SENSOR_FL = ADC2_CH4,
	ADC_VOLTAGE_CHECK = ADC1_CH12
};


#endif /* PINASSIGN_H_ */
