/*
 * drive.c
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#include "global.h"


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_init
// 走行系の変数の初期化，モータードライバ関係のGPIO設定とPWM出力に使うタイマの設定をする
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_init(void){

	//====走行系の変数の初期化====
	max_t_cnt = MAX_T_CNT;			//テーブルカウンタ最高値初期化     MAX_T_CNTはparams.hにマクロ定義あり
	str_t_cnt = STR_T_CNT;			//テーブルカウンタ直線走行値初期化     STR_T_CNTはparams.hにマクロ定義あり
	min_t_cnt = MIN_T_CNT;			//テーブルカウンタ最低値初期化     MIN_T_CNTはparams.hにマクロ定義あり
	MF.FLAGS = 0;					//フラグクリア


	//====モータードライバ関係のGPIO設定====
	pin_dir(PIN_M3, OUT);
	pin_dir(PIN_CW_R, OUT);
	pin_dir(PIN_CW_L, OUT);

	pin_write(PIN_M3, HIGH);		//ステッピングモータ励磁OFF

	drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定


	//速度計算用
	/*--------------------------------------------------------------------
		TIM15 : 16ビットタイマ。時間計測に使う。出力はなし
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM15_CLK_ENABLE();

	//TIM15->CR1 = 0;						//タイマ無効
	TIM15->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM15->CR2 = 0;
	TIM15->DIER = TIM_DIER_UIE;			//タイマ更新割り込みを有効に

	TIM15->CNT = 0;						//タイマカウンタ値を0にリセット
	TIM15->PSC = 64-1;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM15->ARR = 1000-1;		//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定

	TIM15->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 2);	//タイマ更新割り込みの割り込み優先度を設定


	//====PWM出力に使うタイマの設定====
	/*--------------------------------------------------------------------
		TIM16 : 16ビットタイマ。左モータの制御に使う。出力はTIM16_CH1(PB4)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM16_CLK_ENABLE();

	TIM16->CR1 = 0;						//タイマ無効
	TIM16->CR2 = 0;
	TIM16->DIER = TIM_DIER_UIE;			//タイマ更新割り込みを有効に
	TIM16->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWMモード1
	TIM16->CCER = TIM_CCER_CC1E;		//TIM16_CH1出力をアクティブHighに
	TIM16->BDTR = TIM_BDTR_MOE;			//PWM出力を有効に

	TIM16->CNT = 0;						//タイマカウンタ値を0にリセット
	TIM16->PSC = 63;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM16->ARR = DEFAULT_INTERVAL;		//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM16->CCR1 = 25;					//タイマカウンタの比較一致値

	TIM16->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 2);	//タイマ更新割り込みの割り込み優先度を設定

	pin_set_alternate_function(PB4, 1);			//PB4 : TIM16_CH1はAF1に該当


	/*--------------------------------------------------------------------
		TIM17 : 16ビットタイマ。右モータの制御に使う。出力はTIM17_CH1(PB5)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM17_CLK_ENABLE();

	TIM17->CR1 = 0;						//タイマ無効
	TIM17->CR2 = 0;
	TIM17->DIER = TIM_DIER_UIE;			//タイマ更新割り込みを有効に
	TIM17->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWMモード1
	TIM17->CCER = TIM_CCER_CC1E;		//TIM16_CH1出力をアクティブHighに
	TIM17->BDTR = TIM_BDTR_MOE;			//PWM出力を有効に

	TIM17->CNT = 0;						//タイマカウンタ値を0にリセット
	TIM17->PSC = 63;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM17->ARR = DEFAULT_INTERVAL;		//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM17->CCR1 = 25;					//タイマカウンタの比較一致値

	TIM17->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 2);		//タイマ更新割り込みの割り込み優先度を設定

	pin_set_alternate_function(PB5, 10);	//PB5 : TIM17_CH1はAF10に該当
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_enable_motor
// ステッピングモータを励磁する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_enable_motor(void){
	pin_write(PIN_M3, LOW);			//ステッピングモータ励磁ON
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_disable_motor
// ステッピングモータの励磁を切る
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_disable_motor(void){
	pin_write(PIN_M3, HIGH);		//ステッピングモータ励磁OFF
}


/*----------------------------------------------------------
		16bitタイマ割り込み
----------------------------------------------------------*/
/**********
---走行の仕組み---
ステッピングモータの動作制御は16bitタイマで行われている。各16bitタイマの設定は，
・カウント開始からCCR1までの間は出力ピンがLowになる
・CCR1に達すると出力ピンがHighになる
・ARRに達すると割り込みを生成+タイマカウンタをリセット
となっている（drive_init関数参照）
モータドライバの（RefをHighにした状態で）ClockをHighにすることで一定角度回転し，
Lowに戻した後またHighにすることでまた一定角度回転する。
Clockにはタイマの出力ピンを繋いであるので，タイマの周期を速くするほど回転も速くなる。
このプログラムではカウント開始からCCR1の間を一定にして（モータドライバがHighを認識する分長さが必要），
カウント開始からARRの間を調整することで速度を変化させている。
加減速等の状態はMF.FLAG構造体（global.hで定義）で管理されている。
加減速について，事前にExcelで計算した時間（割り込み）ごとのARRの値をtable[]配列に記録しておく。
（配列の値はtable.hにExcelからコピー&ペーストをして設定する。table[]の定義はdrive.h参照）
今加減速のどの段階なのか（table[]の要素番号・インデックス）はt_cnt_l,t_cnt_rで記録している。
**********/


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_BRK_TIM15_IRQHandler
// 16ビットタイマーTIM16の割り込み関数，速度計算用
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_BRK_TIM15_IRQHandler(){

	if( !(TIM15->SR & TIM_SR_UIF) ){
		return;
	}

		if(style == 1){											//スラローム右折の場合
			speedL += accel * 0.001;							//左輪速度をUP
			speedR -= accel * 0.001;							//右輪速度をDOWN
		}
		if(style == 2){											//スラローム左折の場合
			speedL -= accel * 0.001;							//左輪速度をDOWN
			speedR += accel * 0.001;							//右輪速度をUP
		}else{													//一般走行の場合
			speedL += accel * 0.001;
			speedR += accel * 0.001;
		}

		if(target_flag == 1){									//ターゲットフラグONの場合(slalomR32を参照)
			if(style == 1){										//右折の場合
				speedL = max (speedL , target);
				speedR = min (speedR , target);					//左右輪が目標速度になるように
			}
			if(style == 2){										//左折の場合
				speedL = min (speedL , target);
				speedR = max (speedR , target);					//左右輪が目標速度になるように
			}
		}else{													//ターゲットフラグOFFの場合(slalomR32を参照)
			speedL = max (min (speedL , speed_max) , speed_min);
			speedR = max (min (speedR , speed_max) , speed_min);//左右輪が最大、最小スピードを上回る、下回らないように
		}

		widthL = ONE_STEP / speedL * 1000000;
		widthR = ONE_STEP / speedR * 1000000;					//出力速度を出力パルス間隔に変換

	TIM15->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_UP_TIM16_IRQHandler
// 16ビットタイマーTIM16の割り込み関数，左モータの管理を行う
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_UP_TIM16_IRQHandler(){

	if( !(TIM16->SR & TIM_SR_UIF) ){
		return;
	}

	pulse_l++;															//左パルスのカウンタをインクリメント

	if(MF.FLAG.OLD){													//テーブル走行の場合
		//====加減速処理====
		//----減速処理----
		if(MF.FLAG.DECL){												//減速フラグが立っている場合
			t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
		}
		//----加速処理----
		else if(MF.FLAG.ACCL){											//加速フラグが立っている場合
			t_cnt_l = min(t_cnt_l + 1, str_t_cnt);
		}
		//----スラローム処理----
		else if(MF.FLAG.SLLM){											//スラロームフラグが立っている場合
			if(turn == 1){												//右折の場合
				if(style == 2){
					t_cnt_l = min(t_cnt_l + 10, max_t_cnt);
				}
				if(style == 4){
					t_cnt_l = max(t_cnt_l - 10, str_t_cnt);
				}
			}
			if(turn == 2){												//左折の場合
				if(style == 2){
					t_cnt_l = max(t_cnt_l - 10, min_t_cnt);
				}
				if(style == 4){
					t_cnt_l = min(t_cnt_l + 10, str_t_cnt);
				}
			}
		}
		//----デフォルトインターバル----
		if(MF.FLAG.DEF){												//デフォルトインターバルフラグが立っている場合
			TIM16->ARR = DEFAULT_INTERVAL - dl;							//デフォルトインターバルに制御を加えた値に設定
		}
		//----それ以外の時はテーブルカウンタの指し示すインターバル----
		else {
			TIM16->ARR = table[t_cnt_l] - dl;							//左モータインターバル設定
		}

	}else{																//物理量走行の場合
		TIM16->ARR = widthL - dl;										//左モータインターバル設定
	}

	TIM16->SR &= ~TIM_SR_UIF;

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_TRG_CON_TIM17_IRQHandler
// 16ビットタイマーTIM17の割り込み関数，右モータの管理を行う
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_TRG_COM_TIM17_IRQHandler(){

	if( !(TIM17->SR & TIM_SR_UIF) ){
		return;
	}

	pulse_r++;															//右パルスのカウンタをインクリメント

	if(MF.FLAG.OLD){													//テーブル走行の場合
		//====加減速処理====
		//----減速処理----
		if(MF.FLAG.DECL){												//減速フラグが立っている場合
			t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
		}
		//----加速処理----
		else if(MF.FLAG.ACCL){											//加速フラグが立っている場合
			t_cnt_r = min(t_cnt_r + 1, str_t_cnt);
		}
		//----スラローム処理----
		else if(MF.FLAG.SLLM){											//スラロームフラグが立っている場合
			if(turn == 1){												//右折の場合
				if(style == 2){
					t_cnt_r = max(t_cnt_r - 10, min_t_cnt);
				}
				if(style == 4){
					t_cnt_r = min(t_cnt_r + 10, str_t_cnt);
				}
			}
			if(turn == 2){												//左折の場合
				if(style == 2){
					t_cnt_r = min(t_cnt_r + 10, max_t_cnt);
				}
				if(style == 4){
					t_cnt_r = max(t_cnt_r - 10, str_t_cnt);
				}
			}
		}
		//----デフォルトインターバル----
		if(MF.FLAG.DEF){												//デフォルトインターバルフラグが立っている場合
			TIM17->ARR = DEFAULT_INTERVAL - dr;							//デフォルトインターバルに制御を加えた値に設定
		}
		//----それ以外の時はテーブルカウンタの指し示すインターバル----
		else {
			TIM17->ARR = table[t_cnt_r] - dr;							//右モータインターバル設定
		}

	}else{																//物理量走行の場合
		TIM17->ARR = widthR - dr;										//右モータインターバル設定
	}

	TIM17->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_reset_t_cnt
// テーブルカウンタをリセット（min_t_cntの値にセット）する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_reset_t_cnt(void){
	t_cnt_l = t_cnt_r = min_t_cnt;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start
// 走行を開始する
// （pulse_l,pulse_rを0にリセットしてタイマを有効にする）
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start(void){
	pulse_l = pulse_r = 0;		//走行したパルス数の初期化
	TIM16->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM17->CR1 |= TIM_CR1_CEN;	// Enable timer
	MF.FLAG.OLD = 1;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start2
// 走行を開始する
// （pulse_l,pulse_rを0にリセットしてタイマを有効にする）
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start2(void){
	pulse_l = pulse_r = 0;		//走行したパルス数の初期化
	TIM15->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM16->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM17->CR1 |= TIM_CR1_CEN;	// Enable timer
	MF.FLAG.OLD = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_stop
// 走行を終了する
// （タイマを止めてタイマカウント値を0にリセットする）
// 引数1：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_stop(void){
	TIM16->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM17->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM16->CNT = 0;				// Reset Counter
	TIM17->CNT = 0;				// Reset Counter
	MF.FLAG.OLD = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_stop2
// 走行を終了する
// （タイマを止めてタイマカウント値を0にリセットする）
// 引数1：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_stop2(void){
	TIM15->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM16->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM17->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM15->CNT = 0;				// Reset Counter
	TIM16->CNT = 0;				// Reset Counter
	TIM17->CNT = 0;				// Reset Counter
	MF.FLAG.OLD = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_set_dir
// 進行方向を設定する
// 引数1：d_dir …… どの方向に進行するか  0桁目で左，1桁目で右の方向設定
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_set_dir(uint8_t d_dir){

	//====左モータ====
	switch(d_dir & 0x0f){									//0~3ビット目を取り出す
		//----正回転----
		case 0x00:											//0x00の場合
			pin_write(PIN_CW_L, MT_FWD_L);					//左を前進方向に設定
			break;
		//----逆回転----
		case 0x01:								 			//0x01の場合
			pin_write(PIN_CW_L, MT_BACK_L);					//左を後進方向に設定
			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){									//4~7ビット目を取り出す
		case 0x00:											//0x00の場合
			pin_write(PIN_CW_R, MT_FWD_R);					//右を前進方向に設定
			break;
		case 0x10:											//0x10の場合
			pin_write(PIN_CW_R, MT_BACK_R);					//右を後進方向に設定
			break;
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//dist_pulse
// 距離をパルス数に変換する
// 引数1：dist …… 変換してほしい距離
// 戻り値：返還後のパルス数
//+++++++++++++++++++++++++++++++++++++++++++++++
float dist_pulse(uint16_t dist){
	float pulse = 0;
	pulse = dist / ONE_STEP;
	return pulse;
}


/*==========================================================
		走行系関数
==========================================================*/
/*
		基本仕様として，
		基幹関数		第一引数:走行パルス数

		マウスフラグ(MF)
			6Bit:デフォルトインターバルフラグ
			5Bit:減速フラグ
			4Bit:加速フラグ
			3Bit:制御フラグ
			1Bit:二次走行フラグ
*/
/*----------------------------------------------------------
		基幹関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
// 指定パルス分加速走行する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveA(uint16_t dist){

	//====走行====
	//----走行開始----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 1;										//減速・デフォルトインターバルフラグをクリア，加速フラグをセット
	drive_reset_t_cnt();									//テーブルカウンタをリセット
	drive_start();											//走行開始

	//----走行----
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが指定パルス以上進むまで待機

	drive_stop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA2
// 指定パルス分指定加速度で加速走行する
// 引数1：accel_p 加速度, 引数2：speed_min_p 最低速度, 引数3：speed_max_p 最高速度, 引数4：dist 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveA2(uint16_t accel_p, uint16_t speed_min_p, uint16_t speed_max_p, uint16_t dist){

	speed_min = speed_min_p;
	speed_max = speed_max_p;
	accel = accel_p;										//引数の各パラメータをグローバル変数化

	if(MF.FLAG.STRT == 0) speedL = speedR = 100;						//最初の加速の際だけspeedを定義
	drive_start2();											//走行開始

	//----走行----
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが指定パルス以上進むまで待機

	drive_stop2();											//走行停止
	MF.FLAG.STRT = 1;										//2回目以降の加速の際はspeedは既存のスピードを用いる
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
// 指定パルス分減速走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveD(uint16_t dist){

	//====走行====
	//----走行開始----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	drive_start();											//走行開始

	int16_t c_pulse = dist - (t_cnt_l - min_t_cnt);			//等速走行距離 = 総距離 - 減速に必要な距離
	if(c_pulse > 0){
		//----等速走行----
		while((pulse_l < c_pulse) || (pulse_r < c_pulse));	//左右のモータが等速分のパルス以上進むまで待機
	}

	//----減速走行----
	MF.FLAG.DECL = 1;										//減速フラグをセット
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD2
// 指定パルス分指定減速度で減速走行する
// 引数1：accel_p 加速度, 引数2：speed_min_p 最低速度, 引数3：speed_max_p 最高速度, 引数4：dist 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveD2(int16_t accel_p, uint16_t speed_min_p, uint16_t speed_max_p, uint16_t dist){

	float speed_0 = speedL;								//直線パルス数を計算するためにTIM15より参照
	speed_min = speed_min_p;
	speed_max = speed_max_p;
	accel = accel_p;										//引数の各パラメータをグローバル変数化

	drive_start2();											//走行開始

	int16_t c_pulse = dist - (speed_min*speed_min  - speed_0*speed_0)/(2*accel)/ONE_STEP;			//等速走行距離 = 総距離 - 減速に必要な距離

	accel = 0;
	if(c_pulse > 0){
		//----等速走行----
		while((pulse_l < c_pulse) || (pulse_r < c_pulse));	//左右のモータが等速分のパルス以上進むまで待機
	}

	accel = accel_p;
	//----減速走行----
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	MF.FLAG.STRT = 0;
	drive_stop2();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveU
// 指定パルス分等速走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveU(uint16_t dist){

	//====等速走行開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	drive_start();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveU2
// 指定パルス分等速走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveU2(uint16_t dist){

	accel = 0;												//等速走行のため加速度は0
	drive_start2();											//走行開始

	//----走行----
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが指定パルス以上進むまで待機

	drive_stop2();											//走行停止
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
// 指定パルス分デフォルトインターバルで走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t dist){

	//====回転開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 1;
	MF.FLAG.ACCL = 0;										//加速・減速フラグをクリア，デフォルトインターバルフラグをセット
	drive_start();											//走行開始

	//====回転====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが定速分のパルス以上進むまで待機

	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC2
// 指定パルス分デフォルト速度で走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC2(uint16_t dist){

	speed_min = 150;
	speed_max = 300;
	accel = 0;												//150mm/sの定速走行

	drive_start2();											//走行開始

	//====回転====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが定速分のパルス以上進むまで待機

	drive_stop2();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomU1
// スラローム直線区間1
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomU1(uint16_t dist){
	//====等速走行開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	style = 1;												//直線1に入ったことを保存
	drive_start();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomU12
// スラローム直線区間1
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomU12(uint16_t dist){
	target_flag = 0;										//ターゲットフラグの初期化

	accel = 0;												//等速走行のため
	drive_start2();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop2();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR1
// スラローム回転区間1
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR1(uint16_t dist){
	style = 2;												//曲線1に入ったことを保存
	drive_start();											//走行開始

	//====走行====
	while((pulse_l + pulse_r)*0.5 < dist);					//左右のモータが指定分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR12
// スラローム回転区間1
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR12(uint16_t accel_p, uint16_t speed_min_p, uint16_t speed_max_p, uint16_t dist){

	speed_min = speed_min_p;
	speed_max = speed_max_p;
	accel = accel_p;										//引数をグローバル変数化

	drive_start2();											//走行開始

	//====走行====
	while((pulse_l + pulse_r) * 0.5 < dist);				//左右のモータが指定パルス以上進むまで待機

	//====走行終了====
	drive_stop2();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR2
// スラローム回転区間2
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR2(uint16_t dist){
	style = 3;												//曲線2に入ったことを保存
	drive_start();											//走行開始

	//====走行====
	while((pulse_l + pulse_r)*0.5 < dist);					//左右のモータが指定パルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR22
// スラローム回転区間2
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR22(uint16_t dist){
	accel = 0;												//左右輪がそれぞれ異なる速度だが、等速走行のため
	drive_start2();											//走行開始

	//====走行====
	while((pulse_l + pulse_r)*0.5 < dist);					//左右のモータが指定パルス以上進むまで待機

	//====走行終了====
	drive_stop2();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR3
// スラローム回転区間3
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR3(uint16_t dist){
	style = 4;												//曲線3に入ったことを保存
	drive_start();											//走行開始

	//====走行====
	while((pulse_l + pulse_r)*0.5 < dist);					//左右のモータが指定パルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomR32
// スラローム回転区間3
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomR32(int32_t accel_p, uint16_t target_p, uint16_t speed_min_p, uint16_t speed_max_p, uint16_t dist){

	target = target_p;										//直線走行に向けた目標値
	target_flag = 1;										//目標値に近づける走行モードの選択用
	speed_min = speed_min_p;
	speed_max = speed_max_p;
	accel = accel_p;										//引数をグローバル化

	drive_start2();											//走行開始

	//====走行====
	while((pulse_l + pulse_r) * 0.5 < dist);				//左右のモータが指定パルス以上進むまで待機

	//====走行終了====
	drive_stop2();											//走行停止
	target_flag = 0;										//ターゲットフラグをOFFに
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomU2
// スラローム直線区間2
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomU2(uint16_t dist){
	//====等速走行開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	style = 5;												//直線2に入ったことを保存
	drive_start();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();											//走行停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalomU22
// スラローム直線区間2
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalomU22(uint16_t dist){
	accel = 0;												//等速走行のため
	drive_start2();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop2();											//走行停止
}


/*----------------------------------------------------------
		上位関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA
// 半区画分加速しながら走行する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveA(PULSE_SEC_HALF);									//半区画のパルス分加速しながら走行。走行後は停止しない
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA2
// 半区画分加速しながら走行する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA2(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveA2(1000, 100, 400, PULSE_SEC_HALF);					//半区画のパルス分加速しながら走行。走行後は停止しない
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
// 半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveD(PULSE_SEC_HALF);									//半区画のパルス分減速しながら走行。走行後は停止する
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD2
// 半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD2(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveD2(-1000, 100, 400, PULSE_SEC_HALF);				//指定パルス分指定減速度で減速走行。走行後は停止する
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section
// 1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section(void){

	half_sectionA();										//半区画分加速走行
	half_sectionD();										//半区画分減速走行のち停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section2
// 1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section2(void){

	half_sectionA2();										//半区画分加速走行
	half_sectionD2();										//半区画分減速走行のち停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionA2
// 1区画分加速する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionA2(void){

	MF.FLAG.CTRL = 1;												//制御を有効にする

	//accel_hs = 3000;
	//speed_max_hs = 600;
	driveA2(accel_hs, 400, speed_max_hs, PULSE_SEC_HALF*2);			//1区画のパルス分加速走行。走行後は停止しない
	get_wall_info();												//壁情報を取得，片壁制御の有効・無効の判断
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionD2
// 1区画分減速する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionD2(void){

	MF.FLAG.CTRL = 1;												//制御を有効にする

	driveD2(-1*accel_hs, 400, speed_max_hs, PULSE_SEC_HALF*2);		//1区画のパルス分減速走行。走行後は停止しない
	get_wall_info();												//壁情報を取得，片壁制御の有効・無効の判断
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionU
// 等速で1区画分進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionU(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveU(PULSE_SEC_HALF);									//半区画のパルス分等速走行。走行後は停止しない
	driveU(PULSE_SEC_HALF);									//半区画のパルス分等速走行。走行後は停止しない
	get_wall_info();										//壁情報を取得
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionU2
// 等速で1区画分進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionU2(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveU2(PULSE_SEC_HALF);								//半区画のパルス分等速走行。走行後は停止しない
	driveU2(PULSE_SEC_HALF);								//半区画のパルス分等速走行。走行後は停止しない
	get_wall_info();										//壁情報を取得
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_R90
// 右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_R90(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_R);								//右に旋回するようモータの回転方向を設定
	driveA(PULSE_ROT_R90*0.5);
	driveD(PULSE_ROT_R90*0.5);
	turn_dir(DIR_TURN_R90);									//マイクロマウス内部位置情報でも左回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_R902
// 右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_R902(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_R);								//右に旋回するようモータの回転方向を設定
	driveA2(1000, 100, 400, PULSE_ROT_R90*0.5);
	driveD2(-1000, 100, 400, PULSE_ROT_R90*0.5);
	turn_dir(DIR_TURN_R90);									//マイクロマウス内部位置情報でも左回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_L90
// 左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_L90(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_L);								//左に旋回するようモータの回転方向を設定
	driveA(PULSE_ROT_L90*0.5);
	driveD(PULSE_ROT_L90*0.5);
	turn_dir(DIR_TURN_L90);									//マイクロマウス内部位置情報でも左回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_L902
// 左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_L902(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_L);								//左に旋回するようモータの回転方向を設定
	driveA2(1000, 100, 400, PULSE_ROT_R90*0.5);
	driveD2(-1000, 100, 400, PULSE_ROT_R90*0.5);
	turn_dir(DIR_TURN_L90);									//マイクロマウス内部位置情報でも左回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_180
// 右180度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_180(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_R);								//右に旋回するようモータの回転方向を設定
	driveA(PULSE_ROT_R90);
	driveD(PULSE_ROT_R90);
	turn_dir(DIR_TURN_180);									//マイクロマウス内部位置情報でも180度回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_1802
// 右180度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_1802(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_R);								//右に旋回するようモータの回転方向を設定
	driveA2(1000, 100, 400, PULSE_ROT_R90);
	driveD2(-1000, 100, 400, PULSE_ROT_R90);
	turn_dir(DIR_TURN_180);									//マイクロマウス内部位置情報でも180度回転処理
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalom_R90
// スラローム右旋回
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalom_R90(void){

	turn = 0;
	style = 0;
	MF.FLAG.SLLM = 1;

	t_cnt_l = t_cnt_r = str_t_cnt+50;

	turn = 1;												//右回転を保存する
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU1(SLALOM_U1-10);
	MF.FLAG.CTRL = 0;										//制御を無効にする
	slalomR1(SLALOM_R1);
	slalomR2(SLALOM_R2+18);
	slalomR3(SLALOM_R3);
	turn_dir(DIR_TURN_R90);									//マイクロマウス内部位置情報でも右回転処理
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU2(SLALOM_U2);

	turn = 0;
	style = 0;
	MF.FLAG.SLLM = 0;
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalom_R902
// スラローム右旋回
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalom_R902(void){

/*	style = 1;

	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU12(SLALOM_U12);
	MF.FLAG.CTRL = 0;										//制御を無効にする
	slalomR12(8000, 100, 700, SLALOM_R12);
	slalomR22(SLALOM_R22);
	slalomR32(-8000, 400, 100, 700, SLALOM_R32);
	turn_dir(DIR_TURN_R90);									//マイクロマウス内部位置情報でも右回転処理
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU22(SLALOM_U22);

	style = 0;
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
*/
	style = 1;
	//MF.FLAG.SLLM = 1;										//制御を有効にする


	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU12(SLALOM_U12);
	MF.FLAG.CTRL = 0;										//制御を無効にする
	slalomR12(8000, 100, 700, SLALOM_R12);
	slalomR22(SLALOM_R22);
	slalomR32(-8000, 400, 100, 700, SLALOM_R32);
	turn_dir(DIR_TURN_R90);									//マイクロマウス内部位置情報でも右回転処理
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU22(SLALOM_U22);

	style = 0;
	//MF.FLAG.SLLM = 0;										//制御を有効にする
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalom_L90
// スラローム左旋回
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalom_L90(void){

	turn = 0;
	style = 0;
	MF.FLAG.SLLM = 1;

	t_cnt_l = t_cnt_r = str_t_cnt;

	turn = 2;												//左回転を保存する
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU1(SLALOM_U1-5);
	MF.FLAG.CTRL = 0;										//制御を無効にする
	slalomR1(SLALOM_R1);
	slalomR2(SLALOM_R2+14);
	slalomR3(SLALOM_R3);
	turn_dir(DIR_TURN_L90);									//マイクロマウス内部位置情報でも左回転処理
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU2(SLALOM_U2);

	turn = 0;
	style = 0;
	MF.FLAG.SLLM = 0;
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalom_L902
// スラローム左旋回
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalom_L902(void){

	style = 2;

	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU12(SLALOM_U12);
	MF.FLAG.CTRL = 0;										//制御を無効にする
	slalomR12(8000, 100, 700, SLALOM_R12);
	slalomR22(SLALOM_R22);
	slalomR32(-8000, 400, 100, 700, SLALOM_R32);
	turn_dir(DIR_TURN_L90);									//マイクロマウス内部位置情報でも右回転処理
	MF.FLAG.CTRL = 1;										//制御を有効にする
	slalomU22(SLALOM_U22);

	style = 0;
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
// 機体の尻を壁に当てて場所を区画中央に合わせる
// 引数：sw …… 0以外ならget_base()する
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position(uint8_t sw){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(BACK);									//後退するようモータの回転方向を設定
	drive_wait();
	driveC(PULSE_SETPOS_BACK);								//尻を当てる程度に後退。回転後に停止する
	drive_wait();
	if(sw){
		get_base();
	}
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
	drive_wait();
	driveC(PULSE_SETPOS_SET);								//デフォルトインターバルで指定パルス分回転。回転後に停止する
	drive_wait();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position2
// 機体の尻を壁に当てて場所を区画中央に合わせる
// 引数：sw …… 0以外ならget_base()する
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position2(uint8_t sw){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(BACK);									//後退するようモータの回転方向を設定
	drive_wait();
	driveC2(PULSE_SETPOS_BACK);								//尻を当てる程度に後退。回転後に停止する
	drive_wait();
	if(sw){
		get_base();
	}
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
	drive_wait();
	driveC2(PULSE_SETPOS_SET);								//デフォルトインターバルで指定パルス分回転。回転後に停止する
	drive_wait();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_positionX
// 機体の尻を左と背後の壁に当てて場所を区画中央に合わせる
// 引数：sw …… 0以外ならget_base()する
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_positionX(uint8_t sw){

	rotate_R90();
	drive_wait();
	set_position(sw);
	drive_wait();
	rotate_L90();
	drive_wait();
	set_position(sw);
	drive_wait();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_positionX2
// 機体の尻を左と背後の壁に当てて場所を区画中央に合わせる
// 引数：sw …… 0以外ならget_base()する
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_positionX2(uint8_t sw){

	rotate_R902();
	drive_wait();
	set_position2(sw);
	drive_wait();
	rotate_L902();
	drive_wait();
	set_position2(sw);
	drive_wait();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//simple_run
// 超新地走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void simple_run(void){

	int mode = 0;
	printf("Simple Run, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){

		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Simple Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Simple Run, Mode : %d\n", mode);
		}

		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			switch(mode){

				case 0:
					//----尻当て----
					printf("Set Position.\n");
					set_positionX(0);
					break;
				case 1:
					//----一次探索走行----
					printf("First Run.\n");

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX(0);
					get_base();

					searchA();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchA();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 2:
					//----一次探索連続走行----
					printf("First Run. (Continuous)\n");

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX(0);
					get_base();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 3:
					//----二次探索走行----
					printf("Second Run. (Continuous)\n");

					MF.FLAG.SCND = 1;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX(0);
					get_base();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 4:
					//----一次探索走行2----
					printf("First Run.\n");

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchA2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchA2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 5:
					//----一次探索連続走行2----
					printf("First Run. (Continuous)\n");

					MF.FLAG.SCND = 0;
					MF.FLAG.ACCL2 = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchB2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 6:
					//----二次探索走行----
					printf("Second Run. (Continuous)\n");

					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchB2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 7:
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//slalom_run
// スラローム走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void slalom_run(void){

	int mode = 0;
	printf("Slalom Run, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){
		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Slalom Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Slalom Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			int i = 0;
			switch(mode){
				case 0:
					//セットポジション用
					printf("Set Position.\n");
					set_positionX(0);
					break;

				case 1:
					//----一次探索スラローム走行----
					printf("First Run. (Slalom)\n");

					//MF.FLAG.SLLM = 1;
					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 2:
					//---二次探索スラローム走行----
					printf("Second Run. (Slalom)\n");

					//MF.FLAG.SLLM = 1;
					MF.FLAG.SCND = 1;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 3:
					//----二次探索スラローム走行+既知区間加速----
					printf("Second Run. (Slalom+accel)\n");

					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 600;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					break;

				case 4:
					//----二次探索スラローム走行+既知区間加速----
					printf("Second Run. (Slalom+accel)\n");

					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 800;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					//----右90度スラローム回転*16----
/*					printf("Slalom R90.\n");
					get_base();
					ms_wait(500);
					for(i = 0; i < 16; i++){
						half_sectionA();
						slalom_R90();				//16回右回転、つまり4周回転
						half_sectionD();
					}
*/					break;

				case 5:
					//----二次探索スラローム走行+既知区間加速----
					printf("Second Run. (Slalom+accel)\n");

					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 1000;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					//----左90度スラローム回転*16----
/*					printf("Slalom L90.\n");
					get_base();
					ms_wait(500);
					for(i = 0; i < 16; i++){
						half_sectionA();
						slalom_L90();				//16回左回転、つまり4周回転
						half_sectionD();
					}
*/					break;

				case 6:
					//----右90度スラローム回転----
					printf("Slalom R90.\n");
					get_base();
					ms_wait(500);
					for(i = 0; i < 1; i++){
						half_sectionA();
						slalom_R90();				//1回右回転、つまり1/4周回転
						half_sectionD();
					}
					break;

				case 7:
					//----左90度スラローム回転----
					printf("Slalom L90.\n");
					get_base();
					ms_wait(500);
					for(i = 0; i < 1; i++){
						half_sectionA();
						slalom_L90();				//1回左回転、つまり1/4周回転
						half_sectionD();
					}
					break;

			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//test_run
// テスト走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void test_run(void){

	int mode = 0;
	printf("Test Run, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){
		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Test Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Test Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			int i = 0;
			switch(mode){
				case 0:
					//----尻当て----
					printf("Set Position.\n");
					set_position(0);
					break;
				case 1:
					//----4区画等速走行----
					printf("4 Section, Forward, Constant Speed.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					for(i = 0; i < 4; i++){
						driveC(PULSE_SEC_HALF*2);	//一区画のパルス分デフォルトインターバルで走行
						drive_wait();
					}
					break;
				case 2:
					//----右90度回転----
					printf("Rotate R90.\n");
					for(i = 0; i < 16; i++){
						rotate_R90();				//16回右90度回転、つまり4周回転
					}
					break;
				case 3:
					//----左90度回転----
					printf("Rotate L90.\n");
					for(i = 0; i < 16; i++){
						rotate_L90();				//16回左90度回転、つまり4周回転
					}
					break;
				case 4:
					//----180度回転----
					printf("Rotate 180.\n");
					for(i = 0; i < 8; i++){
						rotate_180();				//8回右180度回転、つまり4周回転
					}
					break;
				case 5:
					//----4区画連続走行----
					printf("4 Section, Forward, Continuous.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					driveA(PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					for(i = 0; i < 4-1; i++){
						driveU(PULSE_SEC_HALF*2);	//一区画のパルス分等速走行
					}
					driveD(PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					break;
				case 6:
					break;
				case 7:
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//test_run2
// テスト走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void test_run2(void){

	int mode = 0;
	printf("Test Run2, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){
		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Test Run2, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Test Run2, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			int i = 0;
			switch(mode){
				case 0:
					//----尻当て----
					printf("Set Position.\n");
					set_positionX2(0);
					break;

				case 1:
					//----4区画等速走行----
					printf("4 Section, Forward, Constant Speed.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					for(i = 0; i < 4; i++){
						driveC2(PULSE_SEC_HALF*2);	//一区画のパルス分デフォルトインターバルで走行
						drive_wait();
					}
					break;

				case 2:
					//----右90度回転----
					printf("Rotate R90.\n");
					for(i = 0; i < 16; i++){
						rotate_R902();				//16回右90度回転、つまり4周回転
					}
					break;

				case 3:
					//----左90度回転----
					printf("Rotate L90.\n");
					for(i = 0; i < 16; i++){
						rotate_L902();				//16回左90度回転、つまり4周回転
					}
					break;

				case 4:
					//----180度回転----
					printf("Rotate 180.\n");
					for(i = 0; i < 8; i++){
						rotate_1802();				//8回右180度回転、つまり4周回転
					}
					break;

				case 5:
					//----4区画連続走行----
					printf("4 Section, Forward, Continuous.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					get_base();
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					for(i = 0; i < 3-1; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する

					break;

				case 6:
					break;

				case 7:
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//test_run3
// テスト走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void test_run3(void){

	int mode = 0;
	printf("Test Run3, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){
		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Test Run3, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Test Run3, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			int i = 0;
			switch(mode){

				case 0:
					//----尻当て----
					printf("Set Position.\n");
					set_positionX2(0);
					break;

				case 1:
					//----2区画加減速走行----
					printf("4 Section, Forward, Accel & Deccel Speed.\n");
					half_sectionA2();
					half_sectionA2();
					half_sectionD2();
					half_sectionD2();
					break;

				case 2:
					//----1区画加減速走行----
					printf("1 Section, Forward, Accel & Deccel Speed.\n");
					half_sectionA2();
					half_sectionD2();
					break;

				case 3:
					//----2区画加減速走行----
					printf("2 Section, Forward, Accel & Deccel Speed.\n");
					driveA2(800, 100, 400, PULSE_SEC_HALF*2);					//半区画のパルス分加速しながら走行。走行後は停止しない
					driveD2(-800, 100, 600, PULSE_SEC_HALF*2);				//指定パルス分指定減速度で減速走行。走行後は停止する
					break;

				case 4:
					//----右90度スラローム回転*16----
					printf("Slalom R90.\n");
					MF.FLAG.SCND = 0;
					get_base();
					ms_wait(500);
					for(i = 0; i < 16; i++){
						half_sectionA2();
						slalom_R90();				//16回右回転、つまり4周回転
						half_sectionD2();
					}
					break;

				case 5:
					//----左90度スラローム回転*16----
					printf("Slalom L90.\n");
					MF.FLAG.SCND = 0;
					get_base();
					ms_wait(500);
					for(i = 0; i < 16; i++){
						half_sectionA2();
						slalom_L90();				//16回左回転、つまり4周回転
						half_sectionD2();
					}
					break;

				case 6:
					//----右90度スラローム回転----
					printf("Slalom R90.\n");
					MF.FLAG.SCND = 0;
					get_base();
					ms_wait(500);
					for(i = 0; i < 1; i++){
						half_sectionA2();
						slalom_R90();				//1回右回転、つまり1/4周回転
						half_sectionD2();
					}
					break;

				case 7:
					//----左90度スラローム回転----
					printf("Slalom L90.\n");
					MF.FLAG.SCND = 0;
					get_base();
					ms_wait(500);
					for(i = 0; i < 1; i++){
						half_sectionA2();
						slalom_L90();				//1回左回転、つまり1/4周回転
						half_sectionD2();
					}
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//sample_course_run
// サンプルコース走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void sample_course_run(void){

	int mode = 0;
	printf("Sample Course Run, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){

		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Sample Course Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Sample Course Run, Mode : %d\n", mode);
		}

		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			int i = 0;
			switch(mode){

				case 0:
					//----サンプルコース1走行　4*4連続超新地走行----
					printf("Sample course1 Run.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					get_base();
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					for(i = 0; i < 3-1; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_R902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					one_sectionU2();			//一区画のパルス分等速走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_R902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_R902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_L902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_L902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_R902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_L902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					rotate_L902();
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					for(i = 0; i < 3-1; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する

					break;
				case 1:
					//----サンプルコース1走行　4*4連続スラローム走行----
					printf("Sample course1 Run.\n");
					MF.FLAG.CTRL = 0;				//制御を無効にする
					get_base();
					drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行
					for(i = 0; i < 3-1; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					slalom_R90();
					one_sectionU2();			//一区画のパルス分等速走行
					slalom_R90();
					slalom_R90();
					slalom_L90();
					slalom_L90();
					slalom_R90();
					slalom_L90();
					slalom_L90();
					for(i = 0; i < 3-1; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する
					break;
				case 2:
					led_write(mode & 0b001, mode & 0b010, mode & 0b100);
					get_base();

					break;
				case 3:
					accel_hs = 3000;
					speed_max_hs = 1100;

					set_positionX2(0);

					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する

					break;
				case 4:
					accel_hs = 3500;
					speed_max_hs = 1200;

					set_positionX2(0);

					driveA2(400, 50, 400, PULSE_SEC_HALF);			//半区画のパルス分加速しながら走行

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 12; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					one_sectionA2();
					for(i = 0; i < 4; i++){
						one_sectionU2();			//一区画のパルス分等速走行
					}
					one_sectionD2();
					slalom_R90();

					driveD2(-400, 50, 500, PULSE_SEC_HALF);			//半区画のパルス分減速しながら走行。走行後は停止する

					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//perfect_run
// 本番用走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void perfect_run(void){

	int mode = 0;
	printf("Perfect Run, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){

		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Perfect Run, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Perfect Run, Mode : %d\n", mode);
		}

		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			switch(mode){

				case 0:
					//セットポジション用
					printf("Set Position.\n");
					set_positionX(0);
					break;

				case 1:
					//----一次探索連続走行----
					printf("First Run. (Continuous)\n");

					MF.FLAG.SCND = 0;
					goal_x = 7;
					goal_y = 7;

					set_positionX(0);
					get_base();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = 7;
					goal_y = 7;

					break;

				case 2:
					//----二次探索走行----
					printf("Second Run. (Continuous)\n");
					drive_enable_motor();

					MF.FLAG.SCND = 1;
					goal_x = 7;
					goal_y = 7;

					set_positionX(0);
					get_base();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = 7;
					goal_y = 7;

					break;

				case 3:
					//----一次探索スラローム走行----
					printf("First Run. (Slalom)\n");

					MF.FLAG.SLLM = 1;
					MF.FLAG.SCND = 0;
					goal_x = 7;
					goal_y = 7;

					set_positionX(0);
					get_base();

					searchC();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC();

					goal_x = 7;
					goal_y = 7;

					break;

				case 4:
					//---二次探索スラローム走行----
					printf("Second Run. (Slalom)\n");

					MF.FLAG.SLLM = 1;
					MF.FLAG.SCND = 1;
					goal_x = 7;
					goal_y = 7;

					set_positionX(0);
					get_base();

					searchC();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC();

					goal_x = 7;
					goal_y = 7;

					break;

				case 5:
					break;

				case 6:
					break;
				case 7:
					perfect_slalom();
					break;
			}
		}
	}
	drive_disable_motor();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//perfect_slalom
// 本番用走行モード
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void perfect_slalom(void){

	int mode = 0;
	printf("Perfect Slalom, Mode : %d\n", mode);
	drive_enable_motor();

	while(1){
		led_write(mode & 0b001, mode & 0b010, mode & 0b100);
		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;
			if(mode > 7){
				mode = 0;
			}
			printf("Perfect Slalom, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
			printf("Perfect Slalom, Mode : %d\n", mode);
		}
		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );
			switch(mode){
				case 0:
					//セットポジション用
					printf("Set Position.\n");
					set_positionX(0);
					break;

				case 1:
					//----一次探索スラローム走行----
					printf("First Run.\n");
					MF.FLAG.SCND = 0;
					MF.FLAG.ACCL2 = 0;
					goal_x = 7;
					goal_y = 7;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = 7;
					goal_y = 7;
					break;

				case 2:
					//----二次探索スラローム+既知区間加速走行 speed1----
					printf("First Run. (Continuous)\n");
					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 600;
					goal_x = 7;
					goal_y = 7;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = 7;
					goal_y = 7;
					break;

				case 3:
					//----二次探索スラローム+既知区間加速走行 speed2----
					printf("Second Run. (Continuous)\n");
					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 800;
					goal_x = 7;
					goal_y = 7;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = 7;
					goal_y = 7;
					break;

				case 4:
					//----二次探索スラローム+既知区間加速走行 speed3----
					printf("First Run. (Slalom)\n");
					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 1000;
					goal_x = 7;
					goal_y = 7;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = 7;
					goal_y = 7;
					break;

				case 5:
					//----二次探索スラローム+既知区間加速走行 speed4----
					printf("Second Run. (Slalom)\n");
					MF.FLAG.SCND = 1;
					MF.FLAG.ACCL2 = 1;
					accel_hs = 3000;
					speed_max_hs = 1100;
					goal_x = 7;
					goal_y = 7;

					set_positionX2(0);
					get_base();

					searchC2();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchC2();

					goal_x = 7;
					goal_y = 7;
					break;

				case 6:
					break;

				case 7:
					break;
			}
		}
	}
	drive_disable_motor();
}
