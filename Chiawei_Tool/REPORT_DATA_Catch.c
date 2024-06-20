#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "./include/myfunction.h"

#define Nlen 10000				//讀檔案每一行最大容量
#define Default_PEC_0xFF 1		//Default是否PEC後為0xFF
#define AutoR_PEC_0xFF 0		//AutoRecovery是否PEC後為0xFF

int maxlen_default 		= 241;//default_log每一行最大的資料數量
int maxlen_autorecovery	= 10;//autorecovery每一行最大的資料數量
int Log_head_len1 = 20;//log檔的地址和寫入數據長度ex:" S 58 W 00 Sr 58  R "
int autorecovery_line	= 24;//autorecovery資料行數
int catchlen_default;//擷取default_log的字元總數(包含log header)
int catchlen_autorecovery;//擷取autorecovery的字元總數(包含log header)
int addtime_sec;//要加上的時間(秒)
int addtime_microsec;//要加上的時間(微秒)
int totaltime_microsec;//總微秒 = 加上的微秒 + 讀到的微秒
int add_microsec_len;//需要補上微秒長度的數量
int readhour;//讀到的整數小時
int readmin;//讀到的整數分鐘
int readsec;//讀到的整數秒
int readmicrosec;//讀到的微秒
int readyear;//讀到的整數西元年
int readmonth;//讀到的整數月份
int readday;//讀到的整數日期
char addtime_microsec_str[20];//加上的微秒(字串型態)
int bin_byte_cnt;//bin檔的一行byte數
int PMBUS_byte_cnt;//PMBUS的一行byte數
int bin_rows;//PMBUS一行等於bin檔幾行數
int bin_rows_cnt;//寫到PMBUS行中第幾行bin檔的標記

char *input_case;//輸入參數
char *input_filename;//文件地址參數
char *input_val_1;//任意參數
FILE *fptr_read;//開啟文件(讀)
FILE *fptr_write_default_reportdata;//開啟輸出Default文件(寫)
FILE *fptr_write_default_7eh_reportdata;//開啟輸出Default_7E文件(寫)
FILE *fptr_write_query_readvalue;//開啟輸出Query文件(寫)
FILE *fptr_write_Time;//開啟輸出Time_count文件(寫)
FILE *fptr_write_autorecovery;//開啟輸出autorecovery文件(寫)
FILE *fptr_write_TravelBusLog_Convert;//開啟輸出TravelBusLog文件(寫)
FILE *fptr_write_PMBUS_Bootloader;//開啟輸出PMBUS_Bootloader文件(寫)

char *output_filename_Default_ReportData;//輸出Case Default的檔名
char *output_filename_Default_7E_ReportData;//輸出Case Default 7E的檔名
char *output_filename_Query_ReportData;//輸出Case Query的檔名
char *output_filename_Timecount_ReportData;//輸出Time_count的檔名
char *output_filename_AutoRecovery_ReportData;//輸出AutoRecovery的檔名
char *output_filename_TravelBusLog_Convert;//輸出TravelBusLog轉換的檔名
char *output_filename_PMBUS_Bootloader;//輸出PMBUS_Bootloader轉換的檔名

char strcat_default_data[1000];//初始化字串
char strcat_time_count[50];//初始化字串
char strcat_microsec[20];//初始化字串
char strcat_autorecovery_data[1000];//初始化字串
char strcat_autorecovery_readHex[1000];//初始化字串
char strcat_travellog_convert[500];//初始化字串

const char* split_colon_fullwidth 	= "：";//全形冒號字元切割
const char* split_colon_halfwidth 	= ":";//半形冒號字元切割
const char* split_hyphen 	= "-";//連字號字元切割
const char* split_period 	= ".";//點號字元切割
const char* split_comma  	= ",";//逗號字元切割
char readstr[Nlen + 1];//讀一行文件的容器
char last_readstr[Nlen + 1];//上一行文件的容器
int fgets_count;//讀到第幾行的標記
char *split_log_after;//切割一行log的字串
int split_log_count;//讀到切割一行log後第幾個分割的標記
char *after_replace_log;//取代後的log字串
char *split_Default_after;//切割一行Default資料的字串
int split_Default_count;//讀到切割一行Default資料後第幾個分割的標記
char *split_AutoRecovery_after;//切割一行AutoRecovery資料的字串
int split_AutoRecovery_count;//讀到切割一行AutoRecovery資料後第幾個分割的標記
char *split_last_data;//切割後上一筆資料的紀錄
int nowdata_address;//目前讀到的分割位置
int savedata_count;//要擷取的資料數量
int savedata_total;//要擷取的字元總數
char *split_time_after;//切割time分成Hr Mim Sec Secmin的字串
int split_time_count;//讀到切割time後第幾個分割的標記
char *split_date_after;//切割date分成Y M D的字串
int split_date_count;//讀到切割date後第幾個分割的標記
time_t totaltime;//宣告時間(秒)容器
char *totaltime_str;//宣告時間(字串)容器
char *split_travellog1_after;//切割一行travellog1的字串
int split_travellog1_count;//讀到切割一行travellog1後第幾個分割的標記
char *split_travellog2_after;//切割一行travellog2的字串
int split_travellog2_count;//讀到切割一行travellog2後第幾個分割的標記
char travel_date_str[10];//宣告travel日期(字串)容器
char month_array[12][10] = {{"Jan"},{"Feb"},{"Mar"},{"Apr"},{"May"},{"Jun"},{"Jul"},{"Aug"},{"Sep"},{"Oct"},{"Nov"},{"Dec"}};
char month_num_array[12][10] = {{"01"},{"02"},{"03"},{"04"},{"05"},{"06"},{"07"},{"08"},{"09"},{"10"},{"11"},{"12"}};

typedef struct {
	char Timestamp[40];	//一行travellog中的時間
	char Status[15];	//一行travellog中的狀態
	char Address[5];	//一行travellog中的地址
	char RW[5];			//一行travellog中的讀或寫
	char Data[100];		//一行travellog中的資料
} Travellog_data;
Travellog_data travellog1_data;//宣告travellog資料容器
Travellog_data travellog2_data;//宣告travellog資料容器

char *Default_rowdata;//一行Default資料，不含CMD
char *Default_rowdata_tmp;//Default_rowdata的副本
char *Default_7E_rowdata;//一行7E資料，不含CMD
char *Query_rowdata;//一行Query資料，不含CMD
char *AutoRecovery_rowdata;//一行AutoRecovery資料，不含CMD
char *AutoRecovery_rowdata_tmp;//AutoRecovery_rowdata的副本
char *colon_in_log;//擷取冒號位置
char *Date_in_log;//擷取日期的容器
char *Time_in_log;//擷取時間的容器
char *time_count_output_1;//時間log輸出的第一部分
char *time_count_output_3;//時間log輸出的第二部分
char *LowByte_read = "";//讀到的LowByte
char *HighByte_read = "";//讀到的HighByte
char *Linear_to_Dec_str = "";//Linear格式轉十進制的字串
char *Direct_last_read = "";//讀到的上一個Direct(不包含06)
char *Direct_now_read = "";//讀到目前的Direct(不包含06)
char *Direct_to_Dec_str = "";//Direct格式轉十進制的字串

int main(int argc, char *argv[]){
	//printf("argc_enter: %d\n",argc);//參數數量
	for(int i = 0 ; i < argc ; i++){							//讀取 cmd 參數--------------------------------
		//printf("argv_%d_enter: %s\n",i,argv[i]);//參數字串
		if(i == 1){												//1.模式參數-----------------------------------
			input_case = argv[1];//輸入參數
		}else if(i == 2){										//2.文件地址參數-------------------------------
			input_filename = argv[2];//文件地址參數
		}else if(i == 3){										//3.任意參數-----------------------------------
			input_val_1 = argv[3];//任意參數
		}
	}

	catchlen_default 	= Log_head_len1 + (maxlen_default * 3) - 1;//擷取default_log的字元總數(包含log header)
	catchlen_autorecovery 	= Log_head_len1 + (maxlen_autorecovery * 3) - 1;//擷取autorecovery的字元總數(包含log header)

	output_filename_Default_ReportData = str_replace(input_filename,".txt","_Default_ReportData.txt");//輸出Default檔名
	output_filename_Default_ReportData = str_replace(output_filename_Default_ReportData,"READ_ONLY","OUTPUT");//輸出Default檔名
	output_filename_Default_7E_ReportData = str_replace(input_filename,".txt","_Default_ReportData_7E.txt");//輸出Default_7Eh檔名
	output_filename_Default_7E_ReportData = str_replace(output_filename_Default_7E_ReportData,"READ_ONLY","OUTPUT");//輸出Default_7Eh檔名
	output_filename_Query_ReportData = str_replace(input_filename,".txt","_Query_ReportData.txt");//輸出Query檔名
	output_filename_Query_ReportData = str_replace(output_filename_Query_ReportData,"READ_ONLY","OUTPUT");//輸出Query檔名
	output_filename_Timecount_ReportData = str_replace(input_filename,".txt","_Timecount_ReportData.txt");//輸出Time_count檔名
	output_filename_Timecount_ReportData = str_replace(output_filename_Timecount_ReportData,"READ_ONLY","OUTPUT");//輸出Time_count檔名
	output_filename_AutoRecovery_ReportData = str_replace(input_filename,".txt","_AutoRecovery_ReportData.txt");//輸出AutoRecovery檔名
	output_filename_AutoRecovery_ReportData = str_replace(output_filename_AutoRecovery_ReportData,"READ_ONLY","OUTPUT");//輸出AutoRecovery檔名
	output_filename_TravelBusLog_Convert = str_replace(input_filename,".txt","_Convert_to_ChartLog.txt");//輸出TravelBusLog轉換檔名
	output_filename_TravelBusLog_Convert = str_replace(output_filename_TravelBusLog_Convert,".TXT","_Convert_to_ChartLog.txt");//輸出TravelBusLog轉換檔名
	output_filename_PMBUS_Bootloader = str_replace(input_filename,".bin","_PMBUS_Bootloader.txt");//輸出PMBUS_Bootloader檔名
	output_filename_PMBUS_Bootloader = str_replace(output_filename_PMBUS_Bootloader,"READ_ONLY","OUTPUT");//輸出PMBUS_Bootloader檔名

	fptr_read = fopen(input_filename,"r");//開啟文件(讀)

	if(strncmp("Default", input_case, 7) == 0){//判斷輸入參數Default=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{
			fgets_count = 0;//讀到第幾行的標記
			fptr_write_default_reportdata = fopen(output_filename_Default_ReportData,"w");//開啟輸出Default文件(寫)
			fptr_write_default_7eh_reportdata = fopen(output_filename_Default_7E_ReportData,"w");//開啟輸出Default_7E文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行
				if(fgets_count != 0){//第一行切換PAGE指令不讀
					if((fgets_count % 3) == 1){//第一行Default_log--------------------------------------------------------------

						split_log_after = strtok(readstr, split_colon_fullwidth);//用全形冒號切割一行log的字串
						split_log_count = 0;//讀到切割一行log後第幾個分割的標記
						while (split_log_after != NULL) {//讀取每一行切割後的分割
							if(split_log_count == 1){//讀取切割後第二個分割
								after_replace_log = str_replace(split_log_after," ","-");//取代後的Default資料

								Default_rowdata = substr(after_replace_log, Log_head_len1, catchlen_default);//一行Default資料，不含log head
								Default_rowdata_tmp = substr(after_replace_log, Log_head_len1, catchlen_default);//Default_rowdata的副本

								split_Default_after = strtok(Default_rowdata, split_hyphen);//用連字號切割一行Default資料
								split_Default_count = 0;//讀到切割一行Default資料後第幾個分割的標記
								split_last_data = "";//切割後上一筆資料的紀錄
								nowdata_address = 0;//目前讀到的分割位置
								savedata_count = 0;//要擷取的資料數量

								while (split_Default_after != NULL) {//讀取一行的DEFAULT資料切割後
#if(Default_PEC_0xFF)//--------------------PEC後為0xFF----------------------------------------
									if(strncmp("FF", split_Default_after, 2) == 0 && 
										nowdata_address != 0 && 
										strncmp("FF", split_last_data, 2) != 0){//[當前資料為FF] 且 [不是第一個資料] 且 [上一個資料不是FF]
										savedata_count = nowdata_address - 1;//更新擷取的資料數量
									}
#else//------------------------------------PEC後不為0xFF---------------------------------------
									if(strncmp(split_last_data, split_Default_after, 2) == 0 &&
										savedata_count == 0){//[當前資料等於上一個資料] 且 [擷取的資料數量尚未被寫入]
										savedata_count = nowdata_address - 1;//更新擷取的資料數量
									}else if(strncmp(split_last_data, split_Default_after, 2) != 0){//[當前資料不等於上一個資料]
										savedata_count = 0;//歸零擷取的資料數量
									}
#endif//--------------------------------------------------------------------------------------

									split_last_data = split_Default_after;//切割後上一筆資料的紀錄
									nowdata_address++;//目前讀到的分割位置

									split_Default_after = strtok(NULL, split_hyphen);
									split_Default_count++;//讀到切割一行Default資料後第幾個分割的標記
								}

#if(Default_PEC_0xFF)//--------------------PEC後為0xFF----------------------------------------
								if(savedata_count == 0 || strncmp("FF", split_last_data, 2) != 0){//若全為FF或最後一個資料不是FF
									savedata_count = maxlen_default;//擷取的資料數量
								}
#else//----------------------------------PEC後不為0xFF---------------------------------------
								if(savedata_count == 0){//若都不符合PEC條件 (最後資料不等於上一個資料)
									savedata_count = maxlen_autorecovery;//擷取的資料數量
								}
#endif//------------------------------------------------------------------------------------

								savedata_total = (savedata_count * 3) - 1;//要擷取的字元總數

								Default_rowdata = substr(Default_rowdata_tmp,0,savedata_total);//截去PEC後的一行DEFAULT資料

								memset(strcat_default_data, 0, sizeof(strcat_default_data));//初始化字串
								strcat(strcat_default_data, Default_rowdata);//字串合併
								strcat(strcat_default_data, "h\n");//字串合併

								fprintf(fptr_write_default_reportdata,"%s",strcat_default_data);//寫輸出Default文件
							}
							split_log_after = strtok(NULL, split_colon_fullwidth);
							split_log_count++;//讀到切割一行log後第幾個分割的標記
						}
					}else if((fgets_count%3)==2){//第二行7Eh_log--------------------------------------------------------------

						split_log_after = strtok(readstr, split_colon_fullwidth);//用全形冒號切割一行log的字串
						split_log_count = 0;//讀到切割一行log後第幾個分割的標記
						while (split_log_after != NULL) {//讀取每一行的切割後的分割
							if(split_log_count==1){//讀取切割後第二個分割
								Default_7E_rowdata = substr(split_log_after,32,35);//一行7E資料，不含log head和PEC

								after_replace_log = str_replace(Default_7E_rowdata," ","h\n");//取代後的7E字串

								fprintf(fptr_write_default_7eh_reportdata,"%s",after_replace_log);//寫輸出Default_7E文件
							}
							split_log_after = strtok(NULL, split_colon_fullwidth);
							split_log_count++;//讀到切割一行log後第幾個分割的標記
						}
					}else if((fgets_count%3)==3){//第三行clearfault
						
					}
				}
				fgets_count++;//讀到第幾行的標記
			}
			fclose(fptr_write_default_reportdata);//關閉輸出Default文件
			fclose(fptr_write_default_7eh_reportdata);//關閉輸出Default_7E文件

			printf("%s\n", output_filename_Default_ReportData);
			printf("%s\n", output_filename_Default_7E_ReportData);
		}

		fclose(fptr_read);
	}else if(strncmp("Query", input_case, 5) == 0){//判斷輸入參數Query=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{
			fptr_write_query_readvalue = fopen(output_filename_Query_ReportData,"w");//開啟輸出Query文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行
				split_log_after = strtok(readstr, split_colon_fullwidth);//用全形冒號切割一行log的字串
				split_log_count = 0;//讀到切割一行log後第幾個分割的標記
				while (split_log_after != NULL) {//讀取每一行的切割後的分割
					if(split_log_count==1){//讀取切割後第二個字串
						Query_rowdata = substr(split_log_after,26,32);//一行Query資料，不含log head和PEC

						after_replace_log = str_replace(Query_rowdata,"01 ","01-");//取代後的Query資料
						after_replace_log = str_replace(after_replace_log," ","h\n");//取代後的Query資料

						fprintf(fptr_write_query_readvalue,"%s",after_replace_log);//寫輸出Query文件
					}
					split_log_after = strtok(NULL, split_colon_fullwidth);
					split_log_count++;//讀到切割一行log後第幾個分割的標記
				}
			}
			fclose(fptr_write_query_readvalue);//關閉輸出Query文件

			printf("%s\n", output_filename_Query_ReportData);
		}

		fclose(fptr_read);
	}else if(strncmp("Time_count", input_case, 10) == 0){//判斷輸入參數Time_count=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{

			if(input_val_1 == NULL){//判斷是否輸入參數字串
				printf("Please enter 'input_val_1'\n");
				exit(0);
			}

			split_log_after = strtok(input_val_1, split_period);//用點號切割一個參數字串
			split_log_count = 0;//讀到參數字串第幾個分割的標記
			while (split_log_after != NULL) {//讀取參數字串的分割
				if(split_log_count == 0){//讀取切割後秒數字串
					addtime_sec = atoi(split_log_after);//字串轉成整數
				}else if(split_log_count == 1){//讀取切割後微秒字串

					memset(strcat_microsec, 0, sizeof(strcat_microsec));//初始化字串

					if(strlen(split_log_after) < 6){//若微秒字串長度小於6
						add_microsec_len = 6 - strlen(split_log_after);//需要補上長度的數量

						strcat(strcat_microsec, split_log_after);//字串合併

						for(int i = 1;i <= add_microsec_len;i++){//補0
							strcat(strcat_microsec, "0");//字串合併
						}
					}else if(strlen(split_log_after) > 6){//若微秒字串長度大於6
						strcpy(strcat_microsec, substr(split_log_after,0,6));//將分割字串擷取前6位複製到strcat_microsec
					}else{
						strcpy(strcat_microsec, split_log_after);//將分割字串複製到strcat_microsec
					}
					addtime_microsec = atoi(strcat_microsec);//字串轉成整數
				}
				split_log_after = strtok(NULL, split_period);
				split_log_count++;//讀到參數字串第幾個分割的標記
			}

			fgets_count = 0;//讀到第幾行的標記
			fptr_write_Time = fopen(output_filename_Timecount_ReportData,"w");//開啟輸出Time_count文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行
				colon_in_log = substr(readstr,30,32);//擷取冒號位置
				if(strncmp("¡G", colon_in_log, 2) == 0){//如果這一行是有日期的就進入修改，如果沒有就跳過這行

					Date_in_log = substr(readstr,1,11);//擷取日期位置
					Time_in_log = substr(readstr,13,28);//擷取時間位置

					split_date_after = strtok(Date_in_log, split_hyphen);//用連字號切割日期的字串
					split_date_count = 0;//讀到切割date後第幾個分割的標記
					readyear = 0;//讀到的整數西元年
					readmonth = 0;//讀到的整數月份
					readday = 0;//讀到的整數日期

					while (split_date_after != NULL) {//讀取日期的切割後
						if(split_date_count==0){//讀取切割後Year字串
							readyear = atoi(split_date_after);//字串轉成整數
						}
						if(split_date_count==1){//讀取切割後Month字串
							readmonth = atoi(split_date_after);//字串轉成整數
						}
						if(split_date_count==2){//讀取切割後Day字串
							readday = atoi(split_date_after);//字串轉成整數
						}
						split_date_after = strtok(NULL, split_hyphen);
						split_date_count++;//讀到切割date後第幾個分割的標記
					}

					split_time_after = strtok(Time_in_log, split_colon_halfwidth);//用半形冒號切割時間的字串
					split_time_count = 0;//讀到切割time後第幾個分割的標記
					readhour = 0;//讀到的整數小時
					readmin = 0;//讀到的整數分鐘
					readsec = 0;//讀到的整數秒
					readmicrosec = 0;//讀到的微秒

					while (split_time_after != NULL) {//讀取時間的切割後
						if(split_time_count==0){//讀取切割後Hr字串
							readhour = atoi(split_time_after);//字串轉成整數
						}
						if(split_time_count==1){//讀取切割後Min字串
							readmin = atoi(split_time_after);//字串轉成整數
						}
						if(split_time_count==2){//讀取切割後Sec字串
							readsec = atoi(split_time_after);//字串轉成整數
						}
						if(split_time_count==3){//讀取切割後microSec字串
							readmicrosec = atoi(split_time_after);//字串轉成整數
						}
						split_time_after = strtok(NULL, split_colon_halfwidth);
						split_time_count++;//讀到切割time後第幾個分割的標記
					}

					totaltime = Time_to_Sec(readyear,readmonth,readday,readhour,readmin,readsec);//將結構 tm 中所儲存的時間格式轉換成日曆時間(秒數)

					totaltime = totaltime + addtime_sec;//更新時間 = 原本時間 + 加上的時間
					totaltime_microsec = addtime_microsec + readmicrosec;//總微秒 = 加上的微秒 + 讀到的微秒

					if(totaltime_microsec >= 1000000){
						totaltime += 1;//更新時間，微秒進位加一秒
						totaltime_microsec = totaltime_microsec - 1000000;//更新時間，微秒進位減一秒
					}

					totaltime_str = Time_to_Str(totaltime);//將日曆時間(秒數)轉換成格式%Y-%m-%d %H:%M:%S的字串

					sprintf(addtime_microsec_str,"%d",totaltime_microsec);//加上的微秒(字串型態)
					if(strlen(addtime_microsec_str) < 6){//若微秒字串長度小於6
						memset(strcat_microsec, 0, sizeof(strcat_microsec));//初始化字串
						add_microsec_len = 6 - strlen(addtime_microsec_str);//需要補上長度的數量
						for(int i = 1;i <= add_microsec_len;i++){
							strcat(strcat_microsec, "0");//字串合併
						}
						strcat(strcat_microsec, addtime_microsec_str);//字串合併
						strcpy(addtime_microsec_str, strcat_microsec);//複製strcat_microsec到addtime_microsec_str
					}
					
					memset(strcat_time_count, 0, sizeof(strcat_time_count));//初始化字串
					strcat(strcat_time_count, totaltime_str);//字串合併
					strcat(strcat_time_count, ":");//字串合併
					strcat(strcat_time_count, addtime_microsec_str);//字串合併

					time_count_output_1 = substr(readstr,0,1);//時間log輸出的第一部分
					time_count_output_3 = substr(readstr,28,(strlen(readstr)-1));//時間log輸出的第三部分
					
					fprintf(fptr_write_Time,"%s",time_count_output_1);//寫輸出Time_count文件
					fprintf(fptr_write_Time,"%s",strcat_time_count);//寫輸出Time_count文件
					fprintf(fptr_write_Time,"%s\n",time_count_output_3);//寫輸出Time_count文件
				}else{
					fprintf(fptr_write_Time,"%s",readstr);//寫輸出Time_count文件
				}
				fgets_count++;//讀到第幾行的標記
			}
			fclose(fptr_write_Time);//關閉輸出Time_count文件

			printf("%s\n", output_filename_Timecount_ReportData);
		}

		fclose(fptr_read);
	}else if(strncmp("AutoRecovery", input_case, 12) == 0){//判斷輸入參數AutoRecovery=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{
			fgets_count = 1;//讀到第幾行的標記
			fptr_write_autorecovery = fopen(output_filename_AutoRecovery_ReportData,"w");//開啟輸出autorecovery文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行
				if((fgets_count % autorecovery_line)!=1){//第一行WritePage不讀,只讀後面的

					split_log_after = strtok(readstr, split_colon_fullwidth);//用全形冒號切割一行log的字串
					split_log_count = 0;//讀到切割一行log後第幾個分割的標記

					while (split_log_after != NULL) {//讀取每一行的切割後的分割
						if(split_log_count==1){//讀取切割後第二個字串
							after_replace_log = str_replace(split_log_after," ","-");//取代後的AutoRecovery資料

							AutoRecovery_rowdata = substr(after_replace_log, Log_head_len1, catchlen_autorecovery);//一行AutoRecovery資料，不含log head和PEC
							AutoRecovery_rowdata_tmp = substr(after_replace_log, Log_head_len1, catchlen_autorecovery);//AutoRecovery_rowdata的副本

							split_AutoRecovery_after = strtok(AutoRecovery_rowdata, split_hyphen);//用連字號切割一行AutoRecovery資料
							split_AutoRecovery_count = 0;//讀到切割一行AutoRecovery資料後第幾個分割的標記
							split_last_data = "";//切割後上一筆資料的紀錄
							nowdata_address = 0;//目前讀到的分割位置
							savedata_count = 0;//要擷取的資料數量

							while (split_AutoRecovery_after != NULL) {//讀取一行的AutoRecovery資料切割後
#if(AutoR_PEC_0xFF)//--------------------PEC後為0xFF----------------------------------------
								if(strncmp("FF", split_AutoRecovery_after, 2) == 0 && 
									nowdata_address != 0 && 
									strncmp("FF", split_last_data, 2) != 0){//[當前資料為FF] 且 [不是第一個資料] 且 [上一個資料不是FF]
									savedata_count = nowdata_address - 1;//更新擷取的資料數量
								}
#else//----------------------------------PEC後不為0xFF---------------------------------------
								if(strncmp(split_last_data, split_AutoRecovery_after, 2) == 0 &&
									savedata_count == 0){//[當前資料等於上一個資料] 且 [擷取的資料數量尚未被寫入]
									savedata_count = nowdata_address - 1;//更新擷取的資料數量
								}else if(strncmp(split_last_data, split_AutoRecovery_after, 2) != 0){//[當前資料不等於上一個資料]
									savedata_count = 0;//歸零擷取的資料數量
								}
#endif//------------------------------------------------------------------------------------
								split_last_data = split_AutoRecovery_after;//將目前資料更新到上一筆資料
								nowdata_address++;//目前讀到的分割位置

								split_AutoRecovery_after = strtok(NULL, split_hyphen);
								split_AutoRecovery_count++;//讀到切割一行AutoRecovery資料後第幾個分割的標記
							}
#if(AutoR_PEC_0xFF)//--------------------PEC後為0xFF----------------------------------------
							if(savedata_count == 0 || strncmp("FF", split_last_data, 2) != 0){//若全為FF或最後一個資料不是FF
								savedata_count = maxlen_autorecovery;//擷取的資料數量
							}
#else//----------------------------------PEC後不為0xFF---------------------------------------
							if(savedata_count == 0){//若都不符合PEC條件 (最後資料不等於上一個資料)
								savedata_count = maxlen_autorecovery;//擷取的資料數量
							}
#endif//------------------------------------------------------------------------------------

							savedata_total = (savedata_count * 3) - 1;//擷取的字元總數

							AutoRecovery_rowdata = substr(AutoRecovery_rowdata_tmp,0,savedata_total);//截去PEC後的一行AutoRecovery資料

							memset(strcat_autorecovery_data, 0, sizeof(strcat_autorecovery_data));//初始化字串
							strcat(strcat_autorecovery_data, AutoRecovery_rowdata);//字串合併
							strcat(strcat_autorecovery_data, "h");//字串合併

							if((fgets_count % autorecovery_line) == 11 || (fgets_count % autorecovery_line) == 13){//第11行或第13行存Derict的值
								memset(strcat_autorecovery_data, 0, sizeof(strcat_autorecovery_data));//初始化字串

								Direct_last_read = substr(AutoRecovery_rowdata,3,20);//讀到的上一個Direct(不包含06)

							}else if((fgets_count % autorecovery_line) == 12 || (fgets_count % autorecovery_line) == 14){//第12行或第14行計算Derict
								strcat(strcat_autorecovery_data, "	");//字串合併

								Direct_now_read = substr(AutoRecovery_rowdata,3,20);//讀到目前的Direct(不包含06)

								Direct_to_Dec_str = Direct_to_DEC(Direct_last_read,Direct_now_read);//Direct格式轉十進制的字串

								strcat(strcat_autorecovery_data, Direct_to_Dec_str);//字串合併
								strcat(strcat_autorecovery_data, "\n");//字串合併

							}else if((fgets_count % autorecovery_line) > 14 || (fgets_count % autorecovery_line) == 0){//第14行以後計算Linear11或Linear16
								strcat(strcat_autorecovery_data, "	");//字串合併

								split_AutoRecovery_after = strtok(AutoRecovery_rowdata, split_hyphen);//用連字號切割截去PEC後的一行AutoRecovery資料
								split_AutoRecovery_count = 0;//讀到切割截去PEC後的一行AutoRecovery資料後第幾個分割的標記

								LowByte_read = "";//讀到的LowByte
								HighByte_read = "";//讀到的HighByte
								Linear_to_Dec_str = "";//Linear格式轉十進制的字串
								while (split_AutoRecovery_after != NULL) {//讀取截去PEC後的一行AutoRecovery資料的切割後

									if(split_AutoRecovery_count == 0){//LowByte
										LowByte_read = split_AutoRecovery_after;//讀到的LowByte
									}else if(split_AutoRecovery_count == 1){//HighByte
										HighByte_read = split_AutoRecovery_after;//讀到的HighByte
									}

									split_AutoRecovery_after = strtok(NULL, split_hyphen);
									split_AutoRecovery_count++;//讀到切割截去PEC後的一行AutoRecovery資料後第幾個分割的標記
								}

								memset(strcat_autorecovery_readHex, 0, sizeof(strcat_autorecovery_readHex));//初始化字串
								strcat(strcat_autorecovery_readHex, HighByte_read);//字串合併HighByte
								strcat(strcat_autorecovery_readHex, LowByte_read);//字串合併LowByte

								if((fgets_count % autorecovery_line) == 17){//第17行計算Linear16
									Linear_to_Dec_str = Linear16_to_DEC(strcat_autorecovery_readHex);//Linear16_to_DEC
								}else{//計算Linear11
									Linear_to_Dec_str = Linear11_to_DEC(strcat_autorecovery_readHex);//Linear11_to_DEC
								}

								strcat(strcat_autorecovery_data, Linear_to_Dec_str);//字串合併
								strcat(strcat_autorecovery_data, "\n");//字串合併
							}else{
								strcat(strcat_autorecovery_data, "	");//字串合併
								strcat(strcat_autorecovery_data, AutoRecovery_rowdata);//字串合併
								strcat(strcat_autorecovery_data, "h\n");//字串合併
							}

							fprintf(fptr_write_autorecovery,"%s",strcat_autorecovery_data);//寫輸出autorecovery文件
						}
						split_log_after = strtok(NULL, split_colon_fullwidth);
						split_log_count++;//讀到切割一行log後第幾個分割的標記
					}
				}else{//第一行WritePage
					fprintf(fptr_write_autorecovery,"\n");//寫輸出autorecovery文件
				}
				fgets_count++;//讀到第幾行的標記
			}
			fclose(fptr_write_autorecovery);//關閉輸出autorecovery文件

			printf("%s\n", output_filename_AutoRecovery_ReportData);
		}

		fclose(fptr_read);
	}else if(strncmp("TravelBusLog", input_case, 12) == 0){//判斷輸入參數TravelBusLog=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{			
			fgets_count = 0;//讀到第幾行的標記
			fptr_write_TravelBusLog_Convert = fopen(output_filename_TravelBusLog_Convert,"w");//開啟輸出Query文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行

				if((strlen(last_readstr) == 0) && (fgets_count > 1)){//若前一行資料為空,且不讀前兩行
					strcpy(last_readstr, readstr);//將此行存入前一行資料變數,下行再處理
				}else if(fgets_count > 1){//前一行資料不為空,且不讀前兩行
					split_travellog1_after = strtok(last_readstr, split_comma);//用逗號切割上一行log的字串
					split_travellog1_count = 0;//讀到切割一行log後第幾個分割的標記
					while (split_travellog1_after != NULL) {//讀取每一行的切割後的分割
						if(split_travellog1_count == 0){//讀取切割後第1個字串
							memset(travellog1_data.Timestamp, 0, sizeof(travellog1_data.Timestamp));//初始化字串
							strcpy(travellog1_data.Timestamp, split_travellog1_after);//複製時間資料到容器
							if(strlen(travellog1_data.Timestamp) == 36){//只有第三行進來處理日期資料
								memset(travel_date_str, 0, sizeof(travel_date_str));//初始化字串
								strcat(travel_date_str, substr(travellog1_data.Timestamp,31,35));//合併日期資料
								strcat(travel_date_str, "-");//合併日期資料
								for(int i = 0; i < 12; i++){
									if(strncmp(month_array[i], substr(travellog1_data.Timestamp,24,27), 3) == 0){
										strcat(travel_date_str, month_num_array[i]);//合併日期資料
									}
								}
								strcat(travel_date_str, "-");//合併日期資料
								strcat(travel_date_str, substr(travellog1_data.Timestamp,28,30));//合併日期資料
							}
						}else if(split_travellog1_count == 1){//讀取切割後第2個字串
							memset(travellog1_data.Status, 0, sizeof(travellog1_data.Status));//初始化字串
							strcpy(travellog1_data.Status, split_travellog1_after);//複製狀態資料到容器
						}else if(split_travellog1_count == 2){//讀取切割後第3個字串
							memset(travellog1_data.Address, 0, sizeof(travellog1_data.Address));//初始化字串
							strcpy(travellog1_data.Address, str_replace(split_travellog1_after,"*",""));//取代*符號
						}else if(split_travellog1_count == 3){//讀取切割後第4個字串
							memset(travellog1_data.RW, 0, sizeof(travellog1_data.RW));//初始化字串
							strcpy(travellog1_data.RW, split_travellog1_after);//複製讀或寫資料到容器
						}else if(split_travellog1_count == 4){//讀取切割後第5個字串
							memset(travellog1_data.Data, 0, sizeof(travellog1_data.Data));//初始化字串
							strcpy(travellog1_data.Data, str_replace(split_travellog1_after,"*",""));//取代*符號
							strcpy(travellog1_data.Data, str_replace(travellog1_data.Data,"   "," "));//取代3個空格符號
						}
						split_travellog1_after = strtok(NULL, split_comma);
						split_travellog1_count++;//讀到切割一行log後第幾個分割的標記
					}

					memset(last_readstr, 0, (Nlen + 1));//初始化前一行資料
					strcpy(last_readstr, readstr);//將此行存入前一行資料變數,下行再處理

					split_travellog2_after = strtok(readstr, split_comma);//用逗號切割此行log的字串
					split_travellog2_count = 0;//讀到切割一行log後第幾個分割的標記
					while (split_travellog2_after != NULL) {//讀取每一行的切割後的分割
						if(split_travellog2_count == 0){//讀取切割後第1個字串
							memset(travellog2_data.Timestamp, 0, sizeof(travellog2_data.Timestamp));//初始化字串
							strcpy(travellog2_data.Timestamp, split_travellog2_after);//複製時間資料到容器
						}else if(split_travellog2_count == 1){//讀取切割後第2個字串
							memset(travellog2_data.Status, 0, sizeof(travellog2_data.Status));//初始化字串
							strcpy(travellog2_data.Status, split_travellog2_after);//複製狀態資料到容器
						}else if(split_travellog2_count == 2){//讀取切割後第3個字串
							memset(travellog2_data.Address, 0, sizeof(travellog2_data.Address));//初始化字串
							strcpy(travellog2_data.Address, str_replace(split_travellog2_after,"*",""));//取代*符號
						}else if(split_travellog2_count == 3){//讀取切割後第4個字串
							memset(travellog2_data.RW, 0, sizeof(travellog2_data.RW));//初始化字串
							strcpy(travellog2_data.RW, split_travellog2_after);//複製讀或寫資料到容器
						}else if(split_travellog2_count == 4){//讀取切割後第5個字串
							memset(travellog2_data.Data, 0, sizeof(travellog2_data.Data));//初始化字串
							strcpy(travellog2_data.Data, str_replace(split_travellog2_after,"*",""));//取代*符號
							strcpy(travellog2_data.Data, str_replace(travellog2_data.Data,"   "," "));//取代3個空格符號
						}
						split_travellog2_after = strtok(NULL, split_comma);
						split_travellog2_count++;//讀到切割一行log後第幾個分割的標記
					}

					if(strncmp("Wr", travellog2_data.RW, 2) == 0){//判斷此行為寫
						if(strncmp("Wr", travellog1_data.RW, 2) == 0){//判斷上一行為寫
							memset(strcat_travellog_convert, 0, sizeof(strcat_travellog_convert));//初始化字串
							strcat(strcat_travellog_convert, "[");//字串合併
							strcat(strcat_travellog_convert, travel_date_str);//字串合併日期
							strcat(strcat_travellog_convert, "  ");//字串合併
							strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,0,8));//字串合併時間
							strcat(strcat_travellog_convert, ":");//字串合併
							strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,9,12));//字串合併時間
							strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,13,16));//字串合併時間
							strcat(strcat_travellog_convert, "] ： S ");//字串合併
							strcat(strcat_travellog_convert, travellog1_data.Address);//字串合併地址
							strcat(strcat_travellog_convert, " W ");//字串合併
							strcat(strcat_travellog_convert, substr(travellog1_data.Data,0,(strlen(travellog1_data.Data) - 1)));//字串合併資料
							strcat(strcat_travellog_convert, "\n");//字串合併
							fprintf(fptr_write_TravelBusLog_Convert,"%s",strcat_travellog_convert);//寫輸出travellog文件

						}else{
							//判斷上一行為讀,do nothing
						}
					}else{//判斷此行為讀
						memset(strcat_travellog_convert, 0, sizeof(strcat_travellog_convert));//初始化字串
						strcat(strcat_travellog_convert, "[");//字串合併
						strcat(strcat_travellog_convert, travel_date_str);//字串合併日期
						strcat(strcat_travellog_convert, "  ");//字串合併
						strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,0,8));//字串合併時間
						strcat(strcat_travellog_convert, ":");//字串合併
						strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,9,12));//字串合併時間
						strcat(strcat_travellog_convert, substr(travellog1_data.Timestamp,13,16));//字串合併時間
						strcat(strcat_travellog_convert, "] ： S ");//字串合併
						strcat(strcat_travellog_convert, travellog1_data.Address);//字串合併地址
						strcat(strcat_travellog_convert, " W ");//字串合併
						strcat(strcat_travellog_convert, substr(travellog1_data.Data,0,(strlen(travellog1_data.Data) - 1)));//字串合併資料
						strcat(strcat_travellog_convert, " Sr ");//字串合併
						strcat(strcat_travellog_convert, travellog1_data.Address);//字串合併地址
						strcat(strcat_travellog_convert, "  R ");//字串合併
						strcat(strcat_travellog_convert, substr(travellog2_data.Data,0,(strlen(travellog2_data.Data) - 1)));//字串合併資料
						strcat(strcat_travellog_convert, "\n");//字串合併
						fprintf(fptr_write_TravelBusLog_Convert,"%s",strcat_travellog_convert);//寫輸出travellog文件
					}
				}
				fgets_count++;//讀到第幾行的標記
			}

			if(strncmp("Wr", travellog2_data.RW, 2) == 0){//判斷最後一行為寫
				memset(strcat_travellog_convert, 0, sizeof(strcat_travellog_convert));//初始化字串
				strcat(strcat_travellog_convert, "[");//字串合併
				strcat(strcat_travellog_convert, travel_date_str);//字串合併日期
				strcat(strcat_travellog_convert, "  ");//字串合併
				strcat(strcat_travellog_convert, substr(travellog2_data.Timestamp,0,8));//字串合併時間
				strcat(strcat_travellog_convert, ":");//字串合併
				strcat(strcat_travellog_convert, substr(travellog2_data.Timestamp,9,12));//字串合併時間
				strcat(strcat_travellog_convert, substr(travellog2_data.Timestamp,13,16));//字串合併時間
				strcat(strcat_travellog_convert, "] ： S ");//字串合併
				strcat(strcat_travellog_convert, travellog2_data.Address);//字串合併地址
				strcat(strcat_travellog_convert, " W ");//字串合併
				strcat(strcat_travellog_convert, substr(travellog2_data.Data,0,(strlen(travellog2_data.Data) - 1)));//字串合併資料
				strcat(strcat_travellog_convert, "\n");//字串合併
				fprintf(fptr_write_TravelBusLog_Convert,"%s",strcat_travellog_convert);//寫輸出travellog文件
			}

			fclose(fptr_write_TravelBusLog_Convert);//關閉輸出travellog文件

			printf("%s\n", output_filename_TravelBusLog_Convert);
		}

		fclose(fptr_read);
	}else if(strncmp("PMBUS_Bootloader", input_case, 16) == 0){//判斷輸入參數PMBUS_Bootloader=========================================================
		if(fptr_read == NULL){//判斷文件是否打開失敗
			puts("Fail to open file!");
			exit(0);
		}else{

			if(input_val_1 == NULL){//判斷是否輸入參數字串
				printf("Please enter 'input_val_1'\n");
				exit(0);
			}

			PMBUS_byte_cnt = atoi(input_val_1);//字串轉成整數;//PMBUS的一行byte數

			fgets_count = 0;//讀到第幾行的標記
			bin_rows_cnt = 0;//寫到PMBUS行中第幾行bin檔的標記
			fptr_write_PMBUS_Bootloader = fopen(output_filename_PMBUS_Bootloader,"w");//開啟輸出PMBUS_Bootloader文件(寫)
			while(fgets(readstr, Nlen, fptr_read) != NULL){//讀取每一行

				after_replace_log = str_replace(readstr,"\n","\0");//若一行最後為換行，則取代成空白

				if(fgets_count == 0){//bin檔第一行判斷byte數
					bin_byte_cnt = strlen(after_replace_log) / 2;//bin檔的一行byte數
					bin_rows = PMBUS_byte_cnt / bin_byte_cnt;//PMBUS一行等於bin檔幾行數
				}

				if(bin_rows_cnt == 0){//PMBUS行中第一行bin
					fprintf(fptr_write_PMBUS_Bootloader,"1,Write			,S,B0,W,D7,%X,",PMBUS_byte_cnt);//寫輸出PMBUS_Bootloader文件
				}

				for (split_log_count = 0; split_log_count < strlen(after_replace_log); split_log_count++) {//讀取bin檔一行每個字元
					if(split_log_count % 2 == 0){//讀取奇數字元
						fprintf(fptr_write_PMBUS_Bootloader,"%c",after_replace_log[split_log_count]);//寫輸出PMBUS_Bootloader文件
					}else if(split_log_count % 2 == 1){//讀取偶數字元
						fprintf(fptr_write_PMBUS_Bootloader,"%c,",after_replace_log[split_log_count]);//寫輸出PMBUS_Bootloader文件
					}
				}

				bin_rows_cnt++;//寫到PMBUS行中第幾行bin檔的標記

				if(bin_rows_cnt == bin_rows){//PMBUS行中最後一行bin
					bin_rows_cnt = 0;//寫到PMBUS行中第幾行bin檔的標記
					fprintf(fptr_write_PMBUS_Bootloader,"P\n");//寫輸出PMBUS_Bootloader文件
				}

				// fprintf(fptr_write_PMBUS_Bootloader,"%s",after_replace_log);//寫輸出PMBUS_Bootloader文件
				// bin_rows_cnt++;//寫到PMBUS行中第幾行bin檔的標記

				// if(bin_rows_cnt == bin_rows){//PMBUS行中最後一行bin
				// 	bin_rows_cnt = 0;//寫到PMBUS行中第幾行bin檔的標記
				// 	fprintf(fptr_write_PMBUS_Bootloader,"\n");//寫輸出PMBUS_Bootloader文件
				// }

				fgets_count++;//讀到第幾行的標記
			}
			fclose(fptr_write_PMBUS_Bootloader);//關閉輸出Query文件

			printf("%s\n", output_filename_PMBUS_Bootloader);
		}

		fclose(fptr_read);
	}else if(strncmp("test", input_case, 4) == 0){//判斷輸入參數test=========================================================
		char* test_Linear16 = "1B00";
		printf("test_Linear16:%s\nLinear16_to_DEC:%s\n\n",test_Linear16,Linear16_to_DEC(test_Linear16));
		char* test_Linear11 = "F076";
		printf("test_Linear11:%s\nLinear11_to_DEC:%s\n\n",test_Linear11,Linear11_to_DEC(test_Linear11));

		//char *test_Direct_to_DEC_1 = "88-70-01-45-01-00";
		//char *test_Direct_to_DEC_2 = "1A-7E-01-57-01-00";
		char *test_Direct_to_DEC_1 = "1F-43-06-7E-09-00";
		char *test_Direct_to_DEC_2 = "81-44-06-80-09-00";
		char *Direct_to_DEC_return = Direct_to_DEC(test_Direct_to_DEC_1,test_Direct_to_DEC_2);
		printf("test_Direct_to_DEC_1:%s\ntest_Direct_to_DEC_2:%s\nAverage_Power:%s\n\n",test_Direct_to_DEC_1,test_Direct_to_DEC_2,Direct_to_DEC_return);

		fclose(fptr_read);

		// //addr_test_cmd--------------------------------------//
		// FILE *print_txt;
		// char *filename_print_txt;
		// char *W_Addr;
		// char *R_Addr;
		// int R_Addr_int;


		// filename_print_txt = "Address_CMD.txt";

		// print_txt = fopen(filename_print_txt,"w");

		// fprintf(print_txt,"%s","	PEC_on\n\n");

		// for(int i = 0; i < 255; i=i+2){
		// 	W_Addr = malloc (sizeof (char) * 30);
		// 	R_Addr = malloc (sizeof (char) * 30);
		// 	R_Addr_int = i + 1;
		// 	if(i < 16){
		// 		sprintf(W_Addr,"0x0%X",i);
		// 		sprintf(R_Addr,"0x0%X",R_Addr_int);
		// 	}else{
		// 		sprintf(W_Addr,"0x%X",i);
		// 		sprintf(R_Addr,"0x%X",R_Addr_int);
		// 	}

		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	%s\n",W_Addr);
		// 	fprintf(print_txt,"	i2c_write	0x00\n");
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	%s\n",R_Addr);
		// 	fprintf(print_txt,"	i2c_read_N	1\n");
		// 	fprintf(print_txt,"	i2c_readNA\n");
		// 	fprintf(print_txt,"	store_i2c\n");
		// 	fprintf(print_txt,"	i2c_stop\n\n");
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	%s\n",W_Addr);
		// 	fprintf(print_txt,"	i2c_write	0x00\n");
		// 	fprintf(print_txt,"	i2c_write	0x01\n");
		// 	fprintf(print_txt,"	i2c_stop\n\n");
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	%s\n",W_Addr);
		// 	fprintf(print_txt,"	i2c_write	0x00\n");
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	%s\n",R_Addr);
		// 	fprintf(print_txt,"	i2c_read_N	1\n");
		// 	fprintf(print_txt,"	i2c_readNA\n");
		// 	fprintf(print_txt,"	store_i2c\n");
		// 	fprintf(print_txt,"	i2c_stop\n\n");
		// }

		// fprintf(print_txt,"%s","	USBSendBuf\n	end");

		// fclose(print_txt);

		// //PEC_READ_cmd--------------------------------------//
		// FILE *print_txt;
		// char *filename_print_txt;

		// int Cmd_int_array[] = {0,1,2,6,25,26,27,32,48,58,59,64,68,70,74,79,81,85,89,93,96,106,107,120,121,122,123,124,125,126,128,129,134,135,136,137,138,139,140,141,142,143,144,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,174,176,192,193,194,208,209,210,213,216,224,225,226,227,228,229,230,231,240,241,243,249,126};
		// char Cmd_array[88][10] = {{"00"},{"01"},{"02"},{"06"},{"19"},{"1A"},{"1B"},{"20"},{"30"},{"3A"},{"3B"},{"40"},{"44"},{"46"},{"4A"},{"4F"},{"51"},{"55"},{"59"},{"5D"},{"60"},{"6A"},{"6B"},{"78"},{"79"},{"7A"},{"7B"},{"7C"},{"7D"},{"7E"},{"80"},{"81"},{"86"},{"87"},{"88"},{"89"},{"8A"},{"8B"},{"8C"},{"8D"},{"8E"},{"8F"},{"90"},{"96"},{"97"},{"98"},{"99"},{"9A"},{"9B"},{"9C"},{"9D"},{"9E"},{"9F"},{"A0"},{"A1"},{"A2"},{"A3"},{"A4"},{"A5"},{"A6"},{"A7"},{"A8"},{"A9"},{"AA"},{"AB"},{"AE"},{"B0"},{"C0"},{"C1"},{"C2"},{"D0"},{"D1"},{"D2"},{"D5"},{"D8"},{"E0"},{"E1"},{"E2"},{"E3"},{"E4"},{"E5"},{"E6"},{"E7"},{"F0"},{"F1"},{"F3"},{"F9"},{"7E"}};
		// char ReadByteCnt_array[88][10] = {{"1"},{"1"},{"1"},{"2"},{"1"},{"1"},{"2"},{"1"},{"6"},{"1"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"1"},{"2"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"7"},{"7"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"1"},{"13"},{"17"},{"3"},{"17"},{"5"},{"16"},{"1"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"15"},{"15"},{"17"},{"17"},{"2"},{"2"},{"2"},{"2"},{"2"},{"2"},{"8"},{"1"},{"1"},{"7"},{"1"},{"1"},{"1"},{"4"},{"4"},{"2"},{"3"},{"1"},{"16"},{"1"},{"1"}};
		// //char ReadByteCnt_array[88][10] = {{"0"},{"0"},{"0"},{"1"},{"0"},{"0"},{"1"},{"0"},{"5"},{"0"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"0"},{"1"},{"0"},{"0"},{"0"},{"0"},{"0"},{"0"},{"0"},{"6"},{"6"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"0"},{"12"},{"16"},{"2"},{"16"},{"4"},{"15"},{"0"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"14"},{"14"},{"16"},{"16"},{"1"},{"1"},{"1"},{"1"},{"1"},{"1"},{"7"},{"0"},{"0"},{"6"},{"0"},{"0"},{"0"},{"3"},{"3"},{"1"},{"2"},{"0"},{"15"},{"0"},{"0"}};
		// filename_print_txt = "PECtest_CMD.txt";

		// print_txt = fopen(filename_print_txt,"w");

		// for(int i = 0; i < 88; i++){
		// 	fprintf(print_txt,";%sh\n",Cmd_array[i]);
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB0\n");
		// 	fprintf(print_txt,"	i2c_write	0x%s\n",Cmd_array[i]);
		// 	if(Cmd_int_array[i] == 0x06){
		// 		fprintf(print_txt,"	i2c_write	0x03\n");
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 		fprintf(print_txt,"	i2c_write	0x1B\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}else if(Cmd_int_array[i] == 0x1A){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 		fprintf(print_txt,"	i2c_write	0x00\n");
		// 	}else if(Cmd_int_array[i] == 0x1B){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}else if(Cmd_int_array[i] == 0x30){
		// 		fprintf(print_txt,"	i2c_write	0x02\n");
		// 		fprintf(print_txt,"	i2c_write	0x86\n");
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 	}
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB1\n");
		// 	fprintf(print_txt,"	i2c_read_N	%s\n",ReadByteCnt_array[i]);
		// 	fprintf(print_txt,"	i2c_readNA\n");
		// 	fprintf(print_txt,"	store_i2c\n");
		// 	fprintf(print_txt,"	i2c_stop\n");
		// }

		// fprintf(print_txt,"%s","\n	USBSendBuf\n	end");

		// fclose(print_txt);

		// //PEC_Write_cmd--------------------------------------//
		// FILE *print_txt;
		// char *filename_print_txt;

		// int Cmd_int_array[] = {0,1,2,6,27,59,96,224,225,226,228,231,240,241,243};
		// char Cmd_array[15][10] = {{"00"},{"01"},{"02"},{"06"},{"1B"},{"3B"},{"60"},{"E0"},{"E1"},{"E2"},{"E4"},{"E7"},{"F0"},{"F1"},{"F3"}};
		// char ReadByteCnt_array[15][10] = {{"1"},{"1"},{"1"},{"2"},{"2"},{"2"},{"2"},{"1"},{"7"},{"1"},{"1"},{"2"},{"3"},{"1"},{"16"}};
		// //char ReadByteCnt_array[15][10] = {{"0"},{"0"},{"0"},{"1"},{"1"},{"1"},{"1"},{"0"},{"6"},{"0"},{"0"},{"1"},{"2"},{"0"},{"15"}};
		// filename_print_txt = "PECtest_CMD.txt";

		// print_txt = fopen(filename_print_txt,"w");

		// for(int i = 0; i < 15; i++){
		// 	fprintf(print_txt,";%sh\n",Cmd_array[i]);
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB0\n");
		// 	fprintf(print_txt,"	i2c_write	0x%s\n",Cmd_array[i]);
		// 	if(Cmd_int_array[i] == 0x06){
		// 		fprintf(print_txt,"	i2c_write	0x03\n");
		// 		fprintf(print_txt,"	i2c_write	0x00\n");
		// 		fprintf(print_txt,"	i2c_write	0x1B\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}else if(Cmd_int_array[i] == 0x1B){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB1\n");
		// 	fprintf(print_txt,"	i2c_read_N	%s\n",ReadByteCnt_array[i]);
		// 	fprintf(print_txt,"	i2c_readNA\n");
		// 	fprintf(print_txt,"	store_i2c\n");
		// 	fprintf(print_txt,"	i2c_stop\n\n");

		// 	fprintf(print_txt,"	PEC_on\n");
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB0\n");
		// 	if(Cmd_int_array[i] != 0x06){
		// 		fprintf(print_txt,"	i2c_write	0x%s\n",Cmd_array[i]);
		// 	}
		// 	if(Cmd_int_array[i] == 0x00){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 	}else if(Cmd_int_array[i] == 0x01){
		// 		fprintf(print_txt,"	i2c_write	0x40\n");
		// 	}else if(Cmd_int_array[i] == 0x02){
		// 		fprintf(print_txt,"	i2c_write	0x48\n");
		// 	}else if(Cmd_int_array[i] == 0x06){
		// 		fprintf(print_txt,"	i2c_write	0x05\n");
		// 		fprintf(print_txt,"	i2c_write	0x04\n");
		// 		fprintf(print_txt,"	i2c_write	0x00\n");
		// 		fprintf(print_txt,"	i2c_write	0x1B\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 		fprintf(print_txt,"	i2c_write	0x20\n");
		// 	}else if(Cmd_int_array[i] == 0x1B){
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 	}else if(Cmd_int_array[i] == 0x3B){
		// 		fprintf(print_txt,"	i2c_write	0x32\n");
		// 		fprintf(print_txt,"	i2c_write	0x00\n");
		// 	}else if(Cmd_int_array[i] == 0x60){
		// 		fprintf(print_txt,"	i2c_write	0x10\n");
		// 		fprintf(print_txt,"	i2c_write	0x27\n");
		// 	}else if(Cmd_int_array[i] == 0xE0){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 	}else if(Cmd_int_array[i] == 0xE1){
		// 		fprintf(print_txt,"	i2c_write	0x06\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 	}else if(Cmd_int_array[i] == 0xE2){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 	}else if(Cmd_int_array[i] == 0xE4){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 	}else if(Cmd_int_array[i] == 0xE7){
		// 		fprintf(print_txt,"	i2c_write	0xCE\n");
		// 		fprintf(print_txt,"	i2c_write	0xBB\n");
		// 	}else if(Cmd_int_array[i] == 0xF0){
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 	}else if(Cmd_int_array[i] == 0xF1){
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 	}else if(Cmd_int_array[i] == 0xF3){
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 		fprintf(print_txt,"	i2c_write	0xFF\n");
		// 	}
		// 	fprintf(print_txt,"	i2c_write_PEC\n");
		// 	fprintf(print_txt,"	i2c_stop\n\n");

		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB0\n");
		// 	fprintf(print_txt,"	i2c_write	0x%s\n",Cmd_array[i]);
		// 	if(Cmd_int_array[i] == 0x06){
		// 		fprintf(print_txt,"	i2c_write	0x03\n");
		// 		fprintf(print_txt,"	i2c_write	0x00\n");
		// 		fprintf(print_txt,"	i2c_write	0x1B\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}else if(Cmd_int_array[i] == 0x1B){
		// 		fprintf(print_txt,"	i2c_write	0x01\n");
		// 		fprintf(print_txt,"	i2c_write	0x7B\n");
		// 	}
		// 	fprintf(print_txt,"	i2c_start\n");
		// 	fprintf(print_txt,"	i2c_write	0xB1\n");
		// 	fprintf(print_txt,"	i2c_read_N	%s\n",ReadByteCnt_array[i]);
		// 	fprintf(print_txt,"	i2c_readNA\n");
		// 	fprintf(print_txt,"	store_i2c\n");
		// 	fprintf(print_txt,"	i2c_stop\n");
		// }

		// fprintf(print_txt,";7Eh\n");
		// fprintf(print_txt,"	i2c_start\n");
		// fprintf(print_txt,"	i2c_write	0xB0\n");
		// fprintf(print_txt,"	i2c_write	0x7E\n");
		// fprintf(print_txt,"	i2c_start\n");
		// fprintf(print_txt,"	i2c_write	0xB1\n");
		// fprintf(print_txt,"	i2c_read_N	1\n");
		// fprintf(print_txt,"	i2c_readNA\n");
		// fprintf(print_txt,"	store_i2c\n");
		// fprintf(print_txt,"	i2c_stop\n");

		// fprintf(print_txt,"%s","\n\n	PEC_off\n");
		// fprintf(print_txt,"%s","\n\n	USBSendBuf\n	end");

		// fclose(print_txt);

		// //BLACKBOX_REPORT--------------------------------------//
		// FILE *print_txt;
		// FILE *read_txt;
		// char *filename_print_txt;
		// char *input_filename;
		// char *catch_data;
		// char tmp_E5h[50];
		// char tmp_E6h[50];
		// char *tmp_byte1;
		// char *tmp_byte2;
		// char *tmp_byte3;
		// char *tmp_byte4;
		// char tmp_word[100];
		// char tmp_hex[100];
		// int tmp_word_int;

		// input_filename = "BLACKBOX_Data.txt";
		// filename_print_txt = "BLACKBOX_REPORT.txt";

		// read_txt = fopen(input_filename,"r");//開啟文件(讀)
		// if(read_txt == NULL){//判斷文件是否打開失敗
		// 	puts("Fail to open file!");
		// 	exit(0);
		// }

		// fgets_count = 0;
		// print_txt = fopen(filename_print_txt,"w");
		// while(fgets(readstr, Nlen, read_txt) != NULL){//讀取每一行
		// 	if(fgets_count==0 || fgets_count==1){
		// 		catch_data = substr(readstr,53,64);
		// 		catch_data = str_replace(catch_data," ","-");

		// 		memset(tmp_hex, 0, sizeof(tmp_hex));//初始化字串
		// 		strcat(tmp_hex, catch_data);//字串合併
		// 		strcat(tmp_hex, "h");//字串合併

		// 		split_log_after = strtok(catch_data, split_hyphen);
		// 		split_log_count = 0;//讀到切割一行log後第幾個分割的標記
		// 		while (split_log_after != NULL) {//讀取每一行的切割後的分割
		// 			if(split_log_count==0){
		// 				tmp_byte1 = split_log_after;
		// 			}else if(split_log_count==1){
		// 				tmp_byte2 = split_log_after;
		// 			}else if(split_log_count==2){
		// 				tmp_byte3 = split_log_after;
		// 			}else if(split_log_count==3){
		// 				tmp_byte4 = split_log_after;
		// 			}
		// 			split_log_after = strtok(NULL, split_hyphen);
		// 			split_log_count++;//讀到切割一行log後第幾個分割的標記
		// 		}

		// 		memset(tmp_word, 0, sizeof(tmp_word));//初始化字串
		// 		strcat(tmp_word, "0x");//字串合併
		// 		strcat(tmp_word, tmp_byte4);//字串合併
		// 		strcat(tmp_word, tmp_byte3);//字串合併
		// 		strcat(tmp_word, tmp_byte2);//字串合併
		// 		strcat(tmp_word, tmp_byte1);//字串合併
		// 		tmp_word_int = strtol(tmp_word, NULL, 16);

		// 		if(fgets_count==0){	
		// 			memset(tmp_E5h, 0, sizeof(tmp_E5h));//初始化字串
		// 			sprintf(tmp_E5h,"%s	%d",tmp_hex,tmp_word_int);
		// 		}else if(fgets_count==1){
		// 			memset(tmp_E6h, 0, sizeof(tmp_E6h));//初始化字串
		// 			sprintf(tmp_E6h,"%s	%d",tmp_hex,tmp_word_int);
		// 		}
		// 	}else if((fgets_count >= 2) && (fgets_count <= 9)){
		// 		if(fgets_count == 2){
		// 			catch_data = substr(readstr,53,58);
		// 		}else{
		// 			catch_data = substr(readstr,53,55);
		// 		}
		// 		catch_data = str_replace(catch_data," ","-");

		// 		memset(tmp_hex, 0, sizeof(tmp_hex));//初始化字串
		// 		strcat(tmp_hex, catch_data);//字串合併
		// 		strcat(tmp_hex, "h");//字串合併

		// 		fprintf(print_txt,"%s	%s\n",tmp_hex,tmp_hex);
		// 	}else if(fgets_count != 14){
		// 		catch_data = substr(readstr,53,58);
		// 		catch_data = str_replace(catch_data," ","-");

		// 		memset(tmp_hex, 0, sizeof(tmp_hex));//初始化字串
		// 		strcat(tmp_hex, catch_data);//字串合併
		// 		strcat(tmp_hex, "h");//字串合併

		// 		split_log_after = strtok(catch_data, split_hyphen);
		// 		split_log_count = 0;//讀到切割一行log後第幾個分割的標記
		// 		while (split_log_after != NULL) {//讀取每一行的切割後的分割
		// 			if(split_log_count==0){
		// 				tmp_byte1 = split_log_after;
		// 			}else if(split_log_count==1){
		// 				tmp_byte2 = split_log_after;
		// 			}
		// 			split_log_after = strtok(NULL, split_hyphen);
		// 			split_log_count++;//讀到切割一行log後第幾個分割的標記
		// 		}

		// 		memset(tmp_word, 0, sizeof(tmp_word));//初始化字串
		// 		strcat(tmp_word, tmp_byte2);//字串合併
		// 		strcat(tmp_word, tmp_byte1);//字串合併

		// 		if(fgets_count==13){
		// 			fprintf(print_txt,"%s	%s\n",tmp_hex,Linear16_to_DEC(tmp_word));
		// 		}else{
		// 			fprintf(print_txt,"%s	%s\n",tmp_hex,Linear11_to_DEC(tmp_word));
		// 		}

		// 	}
		// 	fgets_count++;
		// }

		// fprintf(print_txt,"%s\n",tmp_E5h);
		// fprintf(print_txt,"%s\n",tmp_E6h);

		// fclose(print_txt);
		// fclose(read_txt);

		// //QUERY_cmd--------------------------------------//
		// FILE *print_txt;
		// char *filename_print_txt;

		// char Cmd_array[94][10] = {{"00"},{"01"},{"02"},{"03"},{"05"},{"06"},{"19"},{"1A"},{"1B"},{"20"},{"30"},{"3A"},{"3B"},{"40"},{"44"},{"46"},{"4A"},{"4F"},{"51"},{"55"},{"59"},{"5D"},{"60"},{"6A"},{"6B"},{"78"},{"79"},{"7A"},{"7B"},{"7C"},{"7D"},{"7E"},{"80"},{"81"},{"86"},{"87"},{"88"},{"89"},{"8A"},{"8B"},{"8C"},{"8D"},{"8E"},{"8F"},{"90"},{"91"},{"96"},{"97"},{"98"},{"99"},{"9A"},{"9B"},{"9C"},{"9D"},{"9E"},{"9F"},{"A0"},{"A1"},{"A2"},{"A3"},{"A4"},{"A5"},{"A6"},{"A7"},{"A8"},{"A9"},{"AA"},{"AB"},{"AE"},{"B0"},{"C0"},{"C1"},{"C2"},{"D0"},{"D1"},{"D2"},{"D5"},{"D8"},{"E0"},{"E1"},{"E2"},{"E3"},{"E4"},{"E5"},{"E6"},{"E7"},{"F0"},{"F1"},{"F2"},{"F3"},{"F5"},{"F6"},{"F9"},{"FD"}};

		// filename_print_txt = "QUERYtest_CMD.txt";

		// print_txt = fopen(filename_print_txt,"w");

		// for(int i = 0; i < 94; i++){
		// 	fprintf(print_txt,"1,R_1Ah		,S,B0,W,1A,01,%s,S,B0,R,FF,FF,FF,P\n",Cmd_array[i]);
		// }

		// fclose(print_txt);

		// //OS_item--------------------------------------//
		// FILE *print_txt;
		// char *filename_print_txt;

		// int Chapter_int = 19;
		// int item_start = 7;
		// int item_end = 25;
		// int Programming_start = 0;
		// int Programming_end = 0;

		// filename_print_txt = "OS_item.txt";

		// print_txt = fopen(filename_print_txt,"w");

		// fprintf(print_txt,"Chapter %d Exercises\n\n", Chapter_int);


		// for(int i = item_start; i <= item_end; i++){
		// 	fprintf(print_txt,"//------------------------------------------------------------------------//\n");
		// 	fprintf(print_txt,"//---------------------------------[%d.%d]---------------------------------//\n", Chapter_int, i);
		// 	fprintf(print_txt,"%d.%d \n\n%d.%d \n\nA: \n\n", Chapter_int, i, Chapter_int, i);
		// }

		// if(Programming_start != 0 && Programming_end != 0){
		// 	fprintf(print_txt,"\n\n\n\n\n\n\n\n\n\n\nProgramming Problems\n\n");

		// 	for(int i = Programming_start; i <= Programming_end; i++){
		// 		fprintf(print_txt,"//------------------------------------------------------------------------//\n");
		// 		fprintf(print_txt,"//---------------------------------[%d.%d]---------------------------------//\n", Chapter_int, i);
		// 		fprintf(print_txt,"%d.%d \n\n%d.%d \n\nA: \n\n", Chapter_int, i, Chapter_int, i);
		// 	}
		// }
	
		// fclose(print_txt);

		// //Linear11toDec--------------------------------------//
		// FILE *print_txt;
		// FILE *read_txt;
		// char *filename_print_txt;
		// char *input_filename;

		// input_filename = "./READ_ONLY/test_Linear11toDec.txt";
		// filename_print_txt = "Linear11toDec.txt";

		// read_txt = fopen(input_filename,"r");//開啟文件(讀)
		// if(read_txt == NULL){//判斷文件是否打開失敗
		// 	puts("Fail to open file!");
		// 	exit(0);
		// }

		// fgets_count = 0;
		// print_txt = fopen(filename_print_txt,"w");
		// while(fgets(readstr, Nlen, read_txt) != NULL){//讀取每一行
		// 	fprintf(print_txt,"%s\n",Linear11_to_DEC(readstr));//計算Linear11轉十進制並輸出
		// 	fgets_count++;
		// }

		// fclose(print_txt);
		// fclose(read_txt);

		// //Linear16toDec--------------------------------------//
		// FILE *print_txt;
		// FILE *read_txt;
		// char *filename_print_txt;
		// char *input_filename;

		// input_filename = "./READ_ONLY/test_Linear16toDec.txt";
		// filename_print_txt = "Linear16toDec.txt";

		// read_txt = fopen(input_filename,"r");//開啟文件(讀)
		// if(read_txt == NULL){//判斷文件是否打開失敗
		// 	puts("Fail to open file!");
		// 	exit(0);
		// }

		// fgets_count = 0;
		// print_txt = fopen(filename_print_txt,"w");
		// while(fgets(readstr, Nlen, read_txt) != NULL){//讀取每一行
		// 	fprintf(print_txt,"%s\n",Linear16_to_DEC(readstr));//計算Linear16轉十進制並輸出
		// 	fgets_count++;
		// }

		// fclose(print_txt);
		// fclose(read_txt);

	}else{
		puts("Fail to switch_case!");
		exit(0);
	}

	return 0;
}