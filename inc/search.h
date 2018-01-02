/*
 * search.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef SEARCH_H_
#define SEARCH_H_


/*============================================================
		各種定数・変数宣言
============================================================*/
/**********
迷路の絶対座標について，スタート地点が左下になるような位置から見たとき，
上方向を北，右方向を東，下方向を南，左方向を西として定義している。
m_dirの場合，北は0x00，東は0x01，南は0x02，西は0x03で表される。（turn_dir関数参照）
また，マップ格納配列map[][]について，要素は上位4nitと下位4bitに分かれていて，
北壁は3bit目，東壁は2bit目，南壁は1bit目，西壁は0bit目に，
壁がある場合は1，ない場合は0が格納されている。
下位4bitは1次走行用(未探索壁は無しと判定)で上位4bitは2次走行用（未探索壁は有りと判定）
を表している。（write_map関数およびmap_Init関数参照）
最後に，最短経路格納配列route[]について，進行順に移動が記録されている。
各要素に機体が前進する際は0x88が，右折する際は0x44が，Uターンする際は0x22が，左折する場合は0x11が，
それ以外の場合には0x00が格納される（make_route関数参照）。なお，進行の経過はr_cntで管理されている。
**********/


//----現在地・方角格納構造体----
struct coordinate_and_direction{
	uint8_t x;
	uint8_t y;
	uint8_t dir;
};

#ifdef MAIN_C_							//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
	volatile struct coordinate_and_direction mouse;
#else									//main.c以外からこのファイルが呼ばれている場合
	/*グローバル変数の宣言*/
	extern volatile struct coordinate_and_direction mouse;
#endif


//----方向転換用定数----
#define DIR_TURN_R90	0x01	//右90度回転
#define DIR_TURN_L90	0xff	//左90度回転
#define DIR_TURN_180	0x02	//180度回転


//====変数====
#ifdef MAIN_C_											//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
	uint8_t map[16][16];								//マップ格納配列
	uint8_t smap[16][16];								//歩数マップ格納配列
	uint8_t wall_info;									//壁情報格納変数
	uint8_t goal_x, goal_y;								//ゴール座標
	uint8_t route[256];									//最短経路格納配列
	uint8_t r_cnt;										//経路カウンタ
#else													//main.c以外からこのファイルが呼ばれている場合
	/*グローバル変数の宣言*/
	extern uint8_t map[16][16];							//マップ格納配列
	extern uint8_t smap[16][16];						//歩数マップ格納配列
	extern uint8_t wall_info;							//壁情報格納変数
	extern uint8_t goal_x, goal_y;						//ゴール座標
	extern uint8_t route[256];							//最短経路格納配列
	extern uint8_t r_cnt;								//経路カウンタ
#endif


/*============================================================
		関数プロトタイプ宣言
============================================================*/
//====探索系====

void search_init(void);

void searchA();											//1区画停止型探索走行
void searchB();											//連続探索走行

void adv_pos();											//マウスの位置情報を前進
void conf_route();										//次ルートの確認
void map_Init();										//マップデータ初期化
void write_map();										//マップ書き込み
void turn_dir(uint8_t);									//自機方向情報変更
void make_smap();										//歩数マップ作成
void make_route();										//最短経路検索

void store_map_in_eeprom(void);
void load_map_from_eeprom(void);

#endif /* SEARCH_H_ */
