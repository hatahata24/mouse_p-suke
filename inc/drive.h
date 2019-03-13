/*
 * drive.h
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#ifndef DRIVE_H_
#define DRIVE_H_

/*============================================================
		各種定数・変数宣言
============================================================*/

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


#define FORWARD		0x00		//前進向き
#define BACK		0x11		//後退
#define ROTATE_L	0x01		//回転向き（左）
#define ROTATE_R	0x10		//回転向き（右）


//====変数====
#ifdef MAIN_C_										//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
	const uint16_t table[] = {
		#include "table.h"
	};												//table.hに貼り付けた値を保持する配列
	volatile int16_t t_cnt_l, t_cnt_r;				//テーブルカウンタ
	volatile int16_t min_t_cnt, str_t_cnt, max_t_cnt;			//テーブルカウンタの最低値・最大値
	volatile uint16_t pulse_l, pulse_r;				//左右パルスカウンタ
	volatile int16_t dl, dr;						//比例制御量
	//volatile uint16_t Kp;
	volatile uint8_t turn;							//回転方向確認用　1=右回転　2=左回転
	volatile uint8_t style;							//スラロームの走行モード確認用　1=直線1　2=曲線1　3=曲線2　4=曲線3　5=直線2

	volatile float speed, widthR, widthL;
	volatile float speed_min, speed_max, accel;

	volatile float speedL, old_speedL;
	volatile float speedR, old_speedR;
	volatile float target, target_flag;

	volatile int16_t accel_hs, speed_max_hs;

#else												//main.c以外からこのファイルが呼ばれている場合
	/*グローバル変数の宣言*/
	extern const uint16_t table[];
	extern volatile int16_t t_cnt_l, t_cnt_r;		//符号付き整数型に変更
	extern volatile int16_t min_t_cnt, str_t_cnt, max_t_cnt;	//符号付き整数型に変更
	extern volatile uint16_t pulse_l, pulse_r;
	extern volatile int16_t	dl, dr;
	//extern volatile uint16_t Kp;
	extern volatile uint8_t turn;					//回転方向確認用　1=右回転　2=左回転
	extern volatile uint8_t style;					//スラロームの走行モード確認用　1=直線1　2=曲線1　3=曲線2　4=曲線3　5=直線2

	extern volatile float speed, widthR, widthL;
	extern volatile float speed_min, speed_max, accel;

	extern volatile float speedL, old_speedL;
	extern volatile float speedR, old_speedR;
	extern volatile float target, target_flag;

	extern volatile int16_t accel_hs, speed_max_hs;
#endif


#define drive_wait()	ms_wait(50)


/*============================================================
		関数プロトタイプ宣言
============================================================*/
void drive_init(void);
void drive_enable_motor(void);
void drive_disable_motor(void);
void drive_start(void);
void drive_start2(void);
void drive_stop(void);
void drive_stop2(void);
void drive_set_dir(uint8_t);	//進む方向の設定
float dist_pulse(uint16_t);

//====走行系====
//----基幹関数----
void driveA(uint16_t);			//加速走行
void driveA2(uint16_t, uint16_t, uint16_t, uint16_t);		//加速走行
void driveD(uint16_t);			//減速走行
void driveD2(int16_t, uint16_t, uint16_t, uint16_t);		//減速走行
void driveU(uint16_t);			//等速走行（前の速度を維持）
void driveU2(uint16_t);			//等速走行（前の速度を維持）
void driveC(uint16_t);			//デフォルトインターバルで走行
void driveC2(uint16_t);			//デフォルトインターバルで走行
void slalomU1(uint16_t);
void slalomU12(uint16_t);
void slalomR1(uint16_t);
void slalomR12(uint16_t, uint16_t, uint16_t, uint16_t);
void slalomR2(uint16_t);
void slalomR22(uint16_t);
void slalomR3(uint16_t);
void slalomR32(int32_t, uint16_t, uint16_t, uint16_t, uint16_t);
void slalomU2(uint16_t);
void slalomU22(uint16_t);


//----上位関数----
void half_sectionA(void);		//加速半区画
void half_sectionA2(void);		//加速半区画
void half_sectionD(void);		//減速半区画
void half_sectionD2(void);		//減速半区画
void one_section(void);			//加減速一区画
void one_section2(void);		//加減速一区画
void one_sectionA2(void);		//加速一区画
void one_sectionD2(void);		//減速一区画
void one_sectionU(void);		//等速一区画
void one_sectionU2(void);		//等速一区画
void rotate_R90(void);			//右90回転
void rotate_R902(void);			//右90回転
void rotate_L90(void);			//左90回転
void rotate_L902(void);			//左90回転
void rotate_180(void);			//180度回転
void rotate_1802(void);			//180度回転
void slalom_R90(void);			//スラローム右90回転
void slalom_R902(void);			//スラローム右90回転
void slalom_L90(void);			//スラローム左90回転
void slalom_L902(void);			//スラローム左90回転
void set_position(uint8_t);		//上下位置合わせ
void set_position2(uint8_t);	//上下位置合わせ
void set_positionX(uint8_t);	//上下左右位置合わせ
void set_positionX2(uint8_t);	//上下左右位置合わせ


//----走行関数----
void simple_run(void);			//超新地走行
void slalom_run(void);			//スラローム走行
void test_run(void);			//テスト走行
void test_run2(void);			//テスト走行2物理量導入
void test_run3(void);			//テスト走行3既知区間加速
void sample_course_run(void);		//試験コース走行
void perfect_run(void);			//本番用走行
void perfect_slalom(void);

#endif /* DRIVE_H_ */
