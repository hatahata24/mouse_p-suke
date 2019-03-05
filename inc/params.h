/*
 * params.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef PARAMS_H_
#define PARAMS_H_
#include "math.h"

/*============================================================
		各種定数（パラメータ）設定
============================================================*/
/*------------------------------------------------------------
		試験導入　物理量系
------------------------------------------------------------*/

#define WHEEL_DIAMETER		54
#define WHEEL_DISTANCE		88

#define ROUND_STEP			400
#define ONE_STEP			(WHEEL_DIAMETER*M_PI/ROUND_STEP)

#define SEC_HALF			90


/*------------------------------------------------------------
		走行系
------------------------------------------------------------*/
//----走行パルス関連----
#define PULSE_SEC_HALF		SEC_HALF/ONE_STEP //206				//半区画走行用パルス。2倍すると1区画分に 208
#define PULSE_ROT_R90 		WHEEL_DISTANCE*M_PI/4/ONE_STEP //158		//右90度回転用パルス数 145
#define	PULSE_ROT_L90 		WHEEL_DISTANCE*M_PI/4/ONE_STEP//158		//左90度回転用パルス数 145
#define PULSE_ROT_180 		WHEEL_DISTANCE*M_PI/2/ONE_STEP//304//316		//180度回転用パルス数 289
#define PULSE_SETPOS_BACK 	200//250,300	//後ろ壁に当てるために下がるパルス数
#define PULSE_SETPOS_SET 	108//100//92	//後ろ壁から中央までのパルス数

#define SLALOM_U1			60//80//1						//スラローム直線1
#define SLALOM_R1			50//195//200//80//2				//スラローム曲線1
#define SLALOM_R2			145//60//300//100				//スラローム曲線2
#define SLALOM_R3			50//50//80//4//2				//スラローム曲線3
#define SLALOM_U2			20//10							//スラローム直線2

//----テーブルカウンタ関連----
#define MAX_T_CNT	600				//テーブルカウンタの最大値
#define STR_T_CNT	PULSE_SEC_HALF	//テーブルカウンタの直線値
#define MIN_T_CNT	0				//テーブルカウンタの最小値

//----タイマ関連----
#define DEFAULT_INTERVAL	3000	//デフォルトのインターバル



/*------------------------------------------------------------
		センサ系
------------------------------------------------------------*/
//----壁判断閾値（しきい値）----
#define WALL_BASE_FR 	20//30//30		//前壁右センサ
#define WALL_BASE_FL 	80//100//30		//前壁左センサ
#define WALL_BASE_R		100		//右壁センサ
#define WALL_BASE_L 	600     //左壁センサ

//----制御閾値（しきい値）----
#define CTRL_BASE_L		50//100		//左制御閾値
#define CTRL_BASE_R		50//100		//右制御閾値
#define CTRL_MAX		3500		//制御量上限値
#define CTRL_CONT		0.5F//0.3F	//比例制御係数

//----赤外線（赤色）LED発光待機時間（単位はマイクロ秒）
#define IR_WAIT_US	15

//----AD変換スピード
#define ADC_SPEED	0b100	//19.5 Clock Cycles


/*------------------------------------------------------------
		探索系
------------------------------------------------------------*/
//----ゴール座標----
#define GOAL_X 9	//7
#define GOAL_Y 6	//7


#endif /* PARAMS_H_ */
