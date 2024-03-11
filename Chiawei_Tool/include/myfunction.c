#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "myfunction.h"

/* CalPec [計算PEC] 
* @param {UINT8} ubData 要計算PEC的Hex
* */ 
//----- CRC8 -----
C_UINT8 ubaHiCrc[16] = {
	0x00, 0x70, 0xE0, 0x90, 0xC7, 0xB7, 0x27, 0x57,
	0x89, 0xF9, 0x69, 0x19, 0x4E, 0x3E, 0xAE, 0xDE};		// Q0, HiCrc
C_UINT8 ubaLoCrc[16] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D};		// Q0, LoCrc

UINT8 ubPmbusPec = 0x00;									// Q0, PMBus PEC

void CalPec(UINT8 ubData) {
	/********************************************************/
	/*				________								*/
	/*				|		|								*/
	/*	ubData	--->|		|--->	ubPmbusPec				*/
	/*				|_______|								*/
	/*														*/
	/*	Description:										*/
	/*	Calculation PEC (CRC-8) for communication			*/
	/*														*/
	/********************************************************/
//-- C
	UINT8 iubHiByte;										// Q0, High byte
	UINT8 iubLoByte;										// Q0, Low byte

	ubPmbusPec ^= ubData;									// Q0, ubPmbusPec
	iubHiByte = (ubPmbusPec >> 4) & 0x0F;					// Q0, iubHiByte
	iubLoByte = ubPmbusPec & 0x0F;							// Q0, iubLoByte
	ubPmbusPec = ubaHiCrc[iubHiByte];						// Q0, ubPmbusPec
	ubPmbusPec ^= ubaLoCrc[iubLoByte];						// Q0, ubPmbusPec
}
/* Petit_CRC16 [計算CRC16] 
* @param {const unsigned char} Data 要計算CRC的資料
* @param {unsigned int*} CRC 存放CRC的變數
* */ 
unsigned int Petit_TxRx_CRC16 = 0xFFFF;
void Petit_CRC16(const unsigned char Data, unsigned int* CRC)
{
	unsigned int i;

	*CRC = *CRC ^(unsigned int) Data;
	for (i = 8; i > 0; i--)
	{
		if (*CRC & 0x0001)
			*CRC = (*CRC >> 1) ^ 0xA001;
		else
			*CRC >>= 1;
	}
}
/* substr [提取 `src` 中存在的字符]
* @param {char*} src 原始的文字
* @param {int} m 開始的位置
* @param {int} n n-1=結束位置
* */ 
char* substr(const char *src, int m, int n)
{
	// 獲取目標字符串的長度
	int len = n - m;
 
	// 為目標分配 (len + 1) 個字符(+1 用於額外的空字符)
	char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
	// 從源字符串中提取第 m 個和第 n 個索引之間的字符
	// 並將它們複製到目標字符串中
	for (int i = m; i < n && (*(src + i) != '\0'); i++)
	{
		*dest = *(src + i);
		dest++;
	}
 
	// 以空值結束目標字符串
	*dest = '\0';
 
	// 返回目標字符串
	return dest - len;
}
/* str_replace [字串取代] 
* @param {char*} source 原始的文字 
* @param {char*} find 搜尋的文字   
* @param {char*} rep 替換的文字 
* */ 
char *str_replace(char *source, char *find,  char *rep){ 
	// 搜尋文字的長度 
	int find_L=strlen(find); 
	// 替換文字的長度 
	int rep_L=strlen(rep); 
	// 結果文字的長度 
	int length=strlen(source)+1; 
	// 定位偏移量 
	int gap=0; 
 
	// 建立結果文字，並複製文字 
	char *result = (char*)malloc(sizeof(char) * length); 
	strcpy(result, source);	 
 
	// 尚未被取代的字串 
	char *former=source; 
	// 搜尋文字出現的起始位址指標 
	char *location= strstr(former, find); 
 
	// 漸進搜尋欲替換的文字 
	while(location!=NULL){ 
		// 增加定位偏移量 
		gap+=(location - former); 
		// 將結束符號定在搜尋到的位址上 
		result[gap]='\0'; 
 
		// 計算新的長度 
		length+=(rep_L-find_L); 
		// 變更記憶體空間 
		result = (char*)realloc(result, length * sizeof(char)); 
		// 替換的文字串接在結果後面 
		strcat(result, rep); 
		// 更新定位偏移量 
		gap+=rep_L; 
 
		// 更新尚未被取代的字串的位址 
		former=location+find_L; 
		// 將尚未被取代的文字串接在結果後面 
		strcat(result, former); 
 
		// 搜尋文字出現的起始位址指標 
		location= strstr(former, find); 
	}
	return result; 
}
/* Time_to_Sec [將結構 tm 中所儲存的時間格式轉換成日曆時間(秒數)] 
* @param {int} readYear 西元年的整數
* @param {int} readMon 月份的整數
* @param {int} readDay 日期的整數
* @param {int} readHour 24小時制的整數
* @param {int} readMin 分鐘的整數
* @param {int} readSec 秒的整數
* */ 
int Time_to_Sec(int readYear, int readMon, int readDay, int readHour, int readMin, int readSec){
	time_t time_sec;
	struct tm tm_data;
	tm_data.tm_sec = readSec;
	tm_data.tm_min = readMin;
	tm_data.tm_hour = readHour;
	tm_data.tm_mday = readDay;
	tm_data.tm_mon = readMon - 1;
	tm_data.tm_year = readYear - 1900;
	tm_data.tm_isdst = -1;

	time_sec = mktime(&tm_data);//將結構 tm 中所儲存的時間格式轉換成日曆時間(秒數)

	return time_sec;
}
/* Time_to_Str [將日曆時間(秒數)轉換成格式%Y-%m-%d %H:%M:%S的字串] 
* @param {time_t} time_totle_sec 日曆時間(自1970年1月1日到現在所過的總秒數)
* */ 
char *Time_to_Str(time_t time_totle_sec){
	char *result = malloc (sizeof (char) * 30);
	char time_str[30];
	
	time_t totaltime = time_totle_sec;
	struct tm *totaltimePtr = localtime(&totaltime);

	strftime(time_str, 30, "%Y-%m-%d  %H:%M:%S", totaltimePtr);

	sprintf(result,"%s",time_str);

	return result;
}
/* Hexstr_to_int [HexStr to int] 
* @param {char*} hex_val 要轉換的HexData
* */ 
int HexStr_to_int(char *hex_val){
	int result = 0;
	char HexStr[100];//字串合併
	memset(HexStr, 0, sizeof(HexStr));//初始化字串
	strcat(HexStr, "0X");//字串合併
	strcat(HexStr, hex_val);//字串合併
	result = strtoul(HexStr, NULL, 0);

	return result;
}
/* Linear16_to_DEC [Linear16 to DEC] 
* @param {char*} hex_val 要轉換的HexData
* */ 
char *Linear16_to_DEC(char *hex_val){ 
	char *result = malloc (sizeof (char) * 20);

	int hex_val_int = 0;
	int Linear16_N = -9;
	int Linear16_Y = 0;
	double Linear16_X = 0;

	hex_val_int = HexStr_to_int(hex_val);

	Linear16_Y = hex_val_int;
	Linear16_X = Linear16_Y * pow(2,Linear16_N);
	sprintf(result,"%lf",Linear16_X);

	return result;
}
/* Linear11_to_DEC [Linear11 to DEC] 
* @param {char*} hex_val 要轉換的HexData
* */ 
char *Linear11_to_DEC(char *hex_val){ 
	char *result = malloc (sizeof (char) * 20);

	int hex_val_int = 0;
	int Linear11_N = 0;
	int Linear11_Y = 0;
	double Linear11_X = 0;

	hex_val_int = HexStr_to_int(hex_val);

	Linear11_N = hex_val_int >> 11;
	Linear11_Y = hex_val_int & 2047;//0000011111111111
	if(Linear11_N>=16){//10000
		Linear11_N = 0 - ((~Linear11_N & 31) + 1);//00000000000000000000000000011111
	}
	if(Linear11_Y>=1024){//0000010000000000
		Linear11_Y = 0 - ((~Linear11_Y & 2047) + 1);//00000000000000000000011111111111
	}
	Linear11_X = Linear11_Y * pow(2,Linear11_N);
	sprintf(result,"%lf",Linear11_X);

	return result;
}
/* Direct_to_DEC [Direct to DEC] 
* @param {char*} read_data_last I2C讀到的Data(上一筆資料)(不含開頭0x06)
* @param {char*} read_data_current I2C讀到的Data(新的一筆資料)(不含開頭0x06)
* Example->
* 88-70-01-45-01-00(上一筆資料)
* 1A-7E-01-57-01-00(新的一筆資料)
* ->193
* 53-65-0B-4E-0C-00(上一筆資料)
* 1F-39-0C-FB-0C-00(新的一筆資料)
* ->123
* */
char *Direct_to_DEC(char *read_data_last, char *read_data_current){
	char *read_data_last_clear = malloc (sizeof (char) * 20);//初始化收到的變數
	char *read_data_current_clear = malloc (sizeof (char) * 20);//初始化收到的變數
	char *result = malloc (sizeof (char) * 20);

	read_data_last_clear = str_replace(read_data_last," ","-");//取代後的data字串
	read_data_current_clear = str_replace(read_data_current," ","-");//取代後的data字串

	int Maximum_Direct_Format_Value = 32767;
	double Average_Power = 0;

	int last_ACCUMULATOR_LOW = 0;
	int last_ACCUMULATOR_HIGH = 0;
	int last_ACCUMULATOR = 0;
	int last_ROLLOVER_COUNT = 0;
	int last_SAMPLE_COUNT_HIGH = 0;
	int last_SAMPLE_COUNT_MID = 0;
	int last_SAMPLE_COUNT_LOW = 0;
	int last_SAMPLE_COUNT = 0;
	int last_ENERGY_COUNT = 0;

	int current_ACCUMULATOR_LOW = 0;
	int current_ACCUMULATOR_HIGH = 0;
	int current_ACCUMULATOR = 0;
	int current_ROLLOVER_COUNT = 0;
	int current_SAMPLE_COUNT_HIGH = 0;
	int current_SAMPLE_COUNT_MID = 0;
	int current_SAMPLE_COUNT_LOW = 0;
	int current_SAMPLE_COUNT = 0;
	int current_ENERGY_COUNT = 0;

	const char* split_Direct_last = "-";
	char *after_split_Direct_last = strtok(read_data_last_clear, split_Direct_last);
	int strtok_count_Direct_last = 0;
	while (after_split_Direct_last != NULL) {//讀取每一行的切割後
		if(strtok_count_Direct_last==0){//ACCUMULATOR_LOW
			last_ACCUMULATOR_LOW = HexStr_to_int(after_split_Direct_last);
		}else if(strtok_count_Direct_last==1){//ACCUMULATOR_HIGH
			last_ACCUMULATOR_HIGH = HexStr_to_int(after_split_Direct_last);
			last_ACCUMULATOR_HIGH = last_ACCUMULATOR_HIGH << 8;
			current_ACCUMULATOR = last_ACCUMULATOR_HIGH | last_ACCUMULATOR_LOW;
		}else if(strtok_count_Direct_last==2){//ROLLOVER_COUNT
			last_ROLLOVER_COUNT = HexStr_to_int(after_split_Direct_last);
			last_ENERGY_COUNT = (last_ROLLOVER_COUNT * Maximum_Direct_Format_Value) + current_ACCUMULATOR;
		}else if(strtok_count_Direct_last==3){//SAMPLE_COUNT_LOW
			last_SAMPLE_COUNT_LOW = HexStr_to_int(after_split_Direct_last);
		}else if(strtok_count_Direct_last==4){//SAMPLE_COUNT_MID
			last_SAMPLE_COUNT_MID = HexStr_to_int(after_split_Direct_last);
			last_SAMPLE_COUNT_MID = last_SAMPLE_COUNT_MID << 8;
		}else if(strtok_count_Direct_last==5){//SAMPLE_COUNT_HIGH
			last_SAMPLE_COUNT_HIGH = HexStr_to_int(after_split_Direct_last);
			last_SAMPLE_COUNT_HIGH = last_SAMPLE_COUNT_HIGH << 16;
			last_SAMPLE_COUNT = last_SAMPLE_COUNT_HIGH | last_SAMPLE_COUNT_MID | last_SAMPLE_COUNT_LOW;
		}
		after_split_Direct_last = strtok(NULL, split_Direct_last);
		strtok_count_Direct_last++;
	}

	const char* split_Direct_current = "-";
	char *after_split_Direct_current = strtok(read_data_current_clear, split_Direct_current);
	int strtok_count_Direct_current = 0;
	while (after_split_Direct_current != NULL) {//讀取每一行的切割後
		if(strtok_count_Direct_current==0){//ACCUMULATOR_LOW
			current_ACCUMULATOR_LOW = HexStr_to_int(after_split_Direct_current);
		}else if(strtok_count_Direct_current==1){//ACCUMULATOR_HIGH
			current_ACCUMULATOR_HIGH = HexStr_to_int(after_split_Direct_current);
			current_ACCUMULATOR_HIGH = current_ACCUMULATOR_HIGH << 8;
			current_ACCUMULATOR = current_ACCUMULATOR_HIGH | current_ACCUMULATOR_LOW;
		}else if(strtok_count_Direct_current==2){//ROLLOVER_COUNT
			current_ROLLOVER_COUNT = HexStr_to_int(after_split_Direct_current);
			current_ENERGY_COUNT = (current_ROLLOVER_COUNT * Maximum_Direct_Format_Value) + current_ACCUMULATOR;
		}else if(strtok_count_Direct_current==3){//SAMPLE_COUNT_LOW
			current_SAMPLE_COUNT_LOW = HexStr_to_int(after_split_Direct_current);
		}else if(strtok_count_Direct_current==4){//SAMPLE_COUNT_MID
			current_SAMPLE_COUNT_MID = HexStr_to_int(after_split_Direct_current);
			current_SAMPLE_COUNT_MID = current_SAMPLE_COUNT_MID << 8;
		}else if(strtok_count_Direct_current==5){//SAMPLE_COUNT_HIGH
			current_SAMPLE_COUNT_HIGH = HexStr_to_int(after_split_Direct_current);
			current_SAMPLE_COUNT_HIGH = current_SAMPLE_COUNT_HIGH << 16;
			current_SAMPLE_COUNT = current_SAMPLE_COUNT_HIGH | current_SAMPLE_COUNT_MID | current_SAMPLE_COUNT_LOW;
		}

		after_split_Direct_current = strtok(NULL, split_Direct_current);
		strtok_count_Direct_current++;
	}

	if(current_SAMPLE_COUNT == last_SAMPLE_COUNT){//相減等於0會發生錯誤
		sprintf(result,"%s","Error: division");
		return result;
	}else{
		Average_Power = (current_ENERGY_COUNT - last_ENERGY_COUNT) / (current_SAMPLE_COUNT - last_SAMPLE_COUNT);
		sprintf(result,"%lf",Average_Power);

		return result;
	}

}
/* ASCII_Hex_to_Text [ASCII_Hex to Text] 
* @param {char*} hex_val 要轉換的ASCII_Hex
* */ 
char *ASCII_Hex_to_Text(char *hex_val){
	char *result = malloc (sizeof (char) * 100);
	char *after_replace;
	char *ASCII_Hex_str;
	char text_str[1];
	char result_str[100];//字串合併
	memset(result_str, 0, sizeof(result_str));//初始化字串
	int text_int = 0;
	int val_len = 0;
	int m = 0;//開始的位置
	int n = 2;//n-1=結束位置

	after_replace = str_replace(hex_val," ","");

	val_len = strlen(after_replace); 

	if(val_len%2 == 1 || val_len == 0){
		result = "Error length!";
		return result;
	}

	for(int i=0; i<(val_len/2); i++){
		ASCII_Hex_str = substr(after_replace, m, n);
		m = m + 2;
		n = n + 2;

		text_int = HexStr_to_int(ASCII_Hex_str);
		sprintf(text_str,"%c",text_int);
		strcat(result_str, text_str);//字串合併
	}

	sprintf(result,"%s",result_str);
	return result;
}