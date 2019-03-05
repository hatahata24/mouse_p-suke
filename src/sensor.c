/*
 * sensor.c
 *
 *  Created on: 2017/10/24
 *      Author: Blue
 */


#include "global.h"


//+++++++++++++++++++++++++++++++++++++++++++++++
//sensor_init
// センサ系の変数の初期化，ADコンバータの設定とセンサ値取得に使用するタイマの設定をする
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void sensor_init(void){

	//====センサ系の変数の初期化====
	tp = 0;
	ad_l = ad_r = ad_fr = ad_fl = 0;
	base_l = base_r = 0;


	//====ADコンバータの設定====
	__HAL_RCC_ADC12_CLK_ENABLE();	//ADコンバータへのクロック供給を有効に

	ADC12_COMMON->CCR = ADC_CCR_VREFEN | ADC_CCR_CKMODE_1;	//VREFINTチャンネルを有効に, Clock源：AHB，2分周

	/*----------------
		ADC1の設定
	----------------*/
	ADC1->CR = 0;					//ADVREGEN = 0b00, ADEN = 0;
	ADC1->CR = ADC_CR_ADVREGEN_0;	//ADVREGE1N = 0b01, ADEN = 0;

	ms_wait(1);

	ADC1->CR = ADC_CR_ADCAL | ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0, ADCAL = 1;
	while(ADC1->CR & ADC_CR_ADCAL);					//wait until ADCAL == 0

	ADC1->CR = ADC_CR_ADVREGEN_0 | ADC_CR_ADEN;		//ADVREGEN = 0b01, ADEN = 0;
	while( !(ADC1->ISR & ADC_ISR_ADRDY) );			//wait until ADRDY == 1
	ADC1->ISR |= ADC_ISR_ADRDY;

	ADC1->CFGR = 0;

	ADC1->SMPR1 = ADC_SPEED << ADC_SMPR1_SMP1_Pos | ADC_SPEED << ADC_SMPR1_SMP2_Pos | ADC_SPEED << ADC_SMPR1_SMP3_Pos | ADC_SPEED << ADC_SMPR1_SMP4_Pos;
	ADC1->SMPR2 = 0b111  << ADC_SMPR2_SMP11_Pos | 0b111 << ADC_SMPR2_SMP12_Pos | 0b111 << ADC_SMPR2_SMP18_Pos;	//601.5 clk cycle

	//ADC1->SQR1 = ch << ADC_SQR1_SQ1_Pos;			//SQ1 = ch, L = 0 (1 conversion)
	ADC1->SQR2 = 0;
	ADC1->SQR3 = 0;
	ADC1->SQR4 = 0;

	ADC1->DIFSEL = 0;	//single end mode


	/*----------------
		ADC2の設定
	----------------*/
	ADC2->CR = 0;					//ADVREGEN = 0b00, ADEN = 0;
	ADC2->CR = ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0;

	ms_wait(1);

	ADC2->CR = ADC_CR_ADCAL | ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0, ADCAL = 1;
	while(ADC2->CR & ADC_CR_ADCAL);					//wait until ADCAL == 0

	ADC2->CR = ADC_CR_ADVREGEN_0 | ADC_CR_ADEN;		//ADVREGEN = 0b01, ADEN = 0;
	while( !(ADC2->ISR & ADC_ISR_ADRDY) );			//wait until ADRDY == 1
	ADC2->ISR |= ADC_ISR_ADRDY;

	ADC2->CFGR = 0;

	ADC2->SMPR1 = ADC_SPEED << ADC_SMPR1_SMP1_Pos | ADC_SPEED << ADC_SMPR1_SMP2_Pos | ADC_SPEED << ADC_SMPR1_SMP3_Pos | ADC_SPEED << ADC_SMPR1_SMP4_Pos;
	ADC2->SMPR2 = 0;	//1.5 clk cycle

	ADC2->SQR2 = 0;
	ADC2->SQR3 = 0;
	ADC2->SQR4 = 0;

	ADC2->DIFSEL = 0;	//single end mode

	/*----------------------------
		ADC GPIO Initialization
	----------------------------*/
	pin_set_analog_mode(PIN_VOLTAGE_CHECK);		//PB1 : ADC1_CH12

	pin_set_analog_mode(PIN_SENSOR_FR);			//PA0 : ADC1_CH2
	pin_set_analog_mode(PIN_SENSOR_R);			//PA2 : ADC1_CH3
	pin_set_analog_mode(PIN_SENSOR_L);			//PA5 : ADC2_CH2
	pin_set_analog_mode(PIN_SENSOR_FL);			//PA7 : ADC2_CH4


	/*--------------------------------
		IR LED GPIO Initialization
	--------------------------------*/
	pin_dir(PIN_IR_FR, OUT);
	pin_dir(PIN_IR_R, OUT);
	pin_dir(PIN_IR_L, OUT);
	pin_dir(PIN_IR_FL, OUT);


	/*------------------------------------
		TIM6 Interrupt Initialization
	------------------------------------*/
	__HAL_RCC_TIM6_CLK_ENABLE();

	TIM6->CR1 = TIM_CR1_CEN;		//タイマ有効
	TIM6->CR2 = 0;
	TIM6->DIER = TIM_DIER_UIE;		//タイマ更新割り込みを有効に
	TIM6->CNT = 0;					//タイマカウンタ値を0にリセット
	TIM6->PSC = 63;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM6->ARR = 4000;				//タイマカウンタの上限値。4000に設定。ゆえに4msごとに割り込み発生

	TIM6->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM6_DAC1_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM6_DAC1_IRQn, 1);	//タイマ更新割り込みの割り込み優先度を設定

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//sensor_wait_us
// 1us毎にカウントアップするTIM6を使ってusマイクロ秒処理を止める関数。
// （whileループ中にオーバーフローが起こると機能しないのでTIM6タイマ更新割り込みハンドラ内のみで使用することを推奨する）
// 引数：us …… 処理を止めたいマイクロ秒
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void sensor_wait_us(uint32_t us){
	uint32_t dest = TIM6->CNT + us;
	while(TIM6->CNT < dest);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM6_DAC1_IRQHandler
// 16ビットタイマーTIM6の割り込み関数，センサ値の取得，制御比例値の計算をおこなう
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM6_DAC1_IRQHandler(void){

	if( !(TIM6->SR & TIM_SR_UIF) ){
		return;
	}

	switch(tp){

		//----センサ処理----
		case 0:
			//右センサ値の取得
			pin_write(PIN_IR_R, HIGH);													//発光部LEDをON
			sensor_wait_us(IR_WAIT_US);													//光が強まるまで少し待機
			ad_r = get_adc_value(ADC_SENSOR_R);											//受光部のアナログ値を記録
			pin_write(PIN_IR_R, LOW);													//発光部LEDをOFF
			//左センサ値の取得
			pin_write(PIN_IR_L, HIGH);													//発光部LEDをON
			sensor_wait_us(IR_WAIT_US);													//光が強まるまで少し待機
			ad_l = get_adc_value(ADC_SENSOR_L);											//受光部のアナログ値を記録
			pin_write(PIN_IR_L, LOW);													//発光部LEDをOFF
			break;

		case 1:
			//正面センサ値の取得
			pin_write(PIN_IR_FR, HIGH);													//発光部LEDをON
			sensor_wait_us(IR_WAIT_US);													//光が強まるまで少し待機
			ad_fr = get_adc_value(ADC_SENSOR_FR);										//受光部のアナログ値を記録
			pin_write(PIN_IR_FR, LOW);													//発光部LEDをOFF
			pin_write(PIN_IR_FL, HIGH);													//発光部LEDをON
			sensor_wait_us(IR_WAIT_US);													//光が強まるまで少し待機
			ad_fl = get_adc_value(ADC_SENSOR_FL);										//受光部のアナログ値を記録
			pin_write(PIN_IR_FL, LOW);													//発光部LEDをOFF
			break;

		//----制御処理----
		case 2:
			//制御フラグがあれば制御
			if(MF.FLAG.CTRL){
				//比例制御値を一次保存する変数を宣言し0で初期化
				int16_t dl_tmp = 0, dr_tmp = 0;
				//基準値からの差を見る
				dif_l = (int32_t) ad_l - base_l;
				dif_r = (int32_t) ad_r - base_r;

				if(CTRL_BASE_L < dif_l){				//制御の判断
					dl_tmp += CTRL_CONT * dif_l;		//比例制御値を決定
					dr_tmp += -1 * CTRL_CONT * dif_l;	//比例制御値を決定
				}
				if(CTRL_BASE_R < dif_r){
					dl_tmp += -1 * CTRL_CONT * dif_r;	//比例制御値を決定
					dr_tmp += CTRL_CONT * dif_r;		//比例制御値を決定
				}

				//一次保存した制御比例値をdlとdrに反映させる
				dl = max(min(CTRL_MAX, dl_tmp), -1 * CTRL_MAX);
				dr = max(min(CTRL_MAX, dr_tmp), -1 * CTRL_MAX);
			}else{
				//制御フラグがなければ制御値0
				dl = dr = 0;
			}
			break;
	}

	//====タスクポインタを進める====
	tp = (tp+1) % 3;

	TIM6->SR &= ~TIM_SR_UIF;
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//get_adc_value
// 指定されたチャンネルのアナログ電圧値を取り出す
// 引数1：ch …… 電圧値を取り出すチャンネル
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_adc_value(int ch){
	ADC_TypeDef *adc;
	if(ch / 32){
		adc = ADC2;
	}else{
		adc = ADC1;
	}

	adc->SQR1 = (ch % 32) << ADC_SQR1_SQ1_Pos;		//SQ1 = ch % 32, L = 0 (1 conversion)
	adc->CR |= ADC_CR_ADSTART;
	while( !(adc->ISR & ADC_ISR_EOC) );
	adc->ISR |= ADC_ISR_EOS;
	return adc->DR;
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//get_base
// 制御用の基準値を取得する
// 引数：なし
// 戻り値：理想的な値を取得できたか　1:できた　0:できなかった
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t get_base(){
	uint8_t res = 1;									//理想的な値を取得できたか

	//----制御用の基準を取得----
	base_l = ad_l;										//現在の左側のセンサ値で決定
	base_r = ad_r;										//現在の右側のセンサ値で決定

	return res;											//理想的な値を取得できたかを返す

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_wall_info
// 壁情報を取得する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void get_wall_info(){

	//----壁情報の初期化----
	wall_info = 0x00;									//壁情報を初期化
	//----前壁を見る----
	if(ad_fr > WALL_BASE_FR || ad_fl > WALL_BASE_FL){
		//AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
		wall_info |= 0x88;								//壁情報を更新
	}
	//----右壁を見る----
	if(ad_r > WALL_BASE_R){
		//AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
		wall_info |= 0x44;								//壁情報を更新
	}
	//----左壁を見る----
	if(ad_l > WALL_BASE_L){
		//AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
		wall_info |= 0x11;								//壁情報を更新
	}

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_battery_voltage
// バッテリーの電圧を取得する
// 引数：なし
// 戻り値：バッテリー電圧値 * 100
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_battery_voltage(void){

	int vrefint_data = get_adc_value(ADC1_VREFINT);
	int value = get_adc_value(ADC_VOLTAGE_CHECK);

	float voltage = 3.3 * VREFINT_CAL * value / vrefint_data / 4095 * 11;
	return (int)(voltage * 100);
}
