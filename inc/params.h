/*
 * params.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef PARAMS_H_
#define PARAMS_H_

/*============================================================
		各種定数（パラメータ）設定
============================================================*/
/*------------------------------------------------------------
		走行系
------------------------------------------------------------*/
//----走行パルス関連----
#define PULSE_SEC_HALF		208		//半区画走行用パルス。2倍すると1区画分に 208
#define PULSE_ROT_R90 		158		//右90度回転用パルス数 145
#define	PULSE_ROT_L90 		158		//左90度回転用パルス数 145
#define PULSE_ROT_180 		316		//180度回転用パルス数 289
#define PULSE_SETPOS_BACK 	300		//後ろ壁に当てるために下がるパルス数
#define PULSE_SETPOS_SET 	92		//後ろ壁から中央までのパルス数
//----テーブルカウンタ関連----
#define MAX_T_CNT	PULSE_SEC_HALF	//テーブルカウンタの最大値
#define MIN_T_CNT	0				//テーブルカウンタの最小値
//----タイマ関連----
#define DEFAULT_INTERVAL	3000	//デフォルトのインターバル

/*------------------------------------------------------------
		センサ系
------------------------------------------------------------*/
//----壁判断閾値（しきい値）----
#define WALL_BASE_FR 	30		//前壁右センサ
#define WALL_BASE_FL 	30		//前壁左センサ
#define WALL_BASE_R		100		//右壁センサ
#define WALL_BASE_L 	100     //左壁センサ

//----制御閾値（しきい値）----
#define CTRL_BASE_L		100		//左制御閾値
#define CTRL_BASE_R		100		//右制御閾値
#define CTRL_MAX		350		//制御量上限値
#define CTRL_CONT		0.3F	//比例制御係数

//----赤外線（赤色）LED発光待機時間（単位はマイクロ秒）
#define IR_WAIT_US	15

//----AD変換スピード
#define ADC_SPEED	0b100	//19.5 Clock Cycles


/*------------------------------------------------------------
		探索系
------------------------------------------------------------*/
//----ゴール座標----
#define GOAL_X 7	//7
#define GOAL_Y 7	//7


#endif /* PARAMS_H_ */
