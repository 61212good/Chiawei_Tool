#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include "./include/myfunction.h"

#define READ_ONLY_Path "./READ_ONLY/"						//READ_ONLY PATH
#define OUTPUT_LIST_Path "./READ_ONLY/output_list/"			//READ_ONLY OUTPUT LIST PATH

#define REPORT_case_num 4						//REPORT_DATA_Catch的case數量

UINT8 integer;//存PEC的容器

DIR *DIR_result = NULL;//DIR_result = DIR某個路徑後的結果
struct dirent *DIRresult_item;//DIRresult_item = DIR某個路徑後的結果的細項

char REPORT_case_array[REPORT_case_num][100] = {{"Default"},{"Query"},{"Time_count"},{"AutoRecovery"}};//case list
char *Direct_to_DEC_return;//Direct轉十進制
char *ASCII_Hex_to_Text_return;//ASCII_Hex轉Text
char *after_replace_pec_cmd;//取代後的cmd字串
char *after_replace_modbus_cmd;//取代後的cmd字串

char select_function[100];//function的代表號碼
char PEC_cmd_str[1000];//CMD字串
char REPORT_case_str[100];//REPORT_DATA_Catch case的代表號碼
char REPORT_addtime[100];//REPORT_DATA_Catch的addtime
char REPORT_cmd[1000];//REPORT_DATA_Catch的執行命令
char Linear11_str[1000];//輸入Linear11_str
char Linear16_str[1000];//輸入Linear16_str
char Direct_str_1[1000];//輸入Direct_str_1
char Direct_str_2[1000];//輸入Direct_str_2
char ASCII_Hex_str[1000];//輸入ASCII_Hex
char Modebus_cmd_str[1000];//CMD字串

int REPORT_case_int;//REPORT_DATA_Catch case的代表號碼
int OUTPUT_LIST_count;//目錄"./READ_ONLY/output_list/"裡的文件數量

const char* split_hyphen = "-";//連字號字元切割
char *split_cmd_after;//切割一行cmd資料的字串
int split_cmd_count;//讀到切割一行cmd資料後第幾個分割的標記

int main(){
	while(1){
		printf("//------------------------------------------------//\n");
		printf("//function List:\n");
		printf("//( 1 ) PEC\n");
		printf("//( 2 ) DIR_READONLY\n");
		printf("//( 3 ) REPORT_DATA_Catch output_list\n");
		printf("//( 4 ) Linear11 to Dec\n");
		printf("//( 5 ) Linear16 to Dec\n");
		printf("//( 6 ) Direct to Dec\n");
		printf("//( 7 ) ASCII_Hex to Text\n");
		printf("//( 8 ) CRC16\n");
		printf("//------------------------------------------------//\n\n");
		printf("[FUNCTION.exe]: select function num:");//提示請輸入function代碼
		scanf("%s",&select_function);//接收輸入的值存入select_function

		if(strncmp("1", select_function, 1) == 0){//function PEC========================================================================================
			while(1){
				printf("[FUNCTION.exe_PEC]: input I2C cmd:");//提示請輸入I2C cmd
				scanf(" %[^\n]",&PEC_cmd_str);//接收輸入的值存入PEC_cmd_str
				if(strncmp("exit", PEC_cmd_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}printf("\n");

				after_replace_pec_cmd = str_replace(PEC_cmd_str," ","-");//取代後的cmd字串
				split_cmd_after = strtok(after_replace_pec_cmd, split_hyphen);//用連字號切割cmd的字串

				while (split_cmd_after != NULL) {//讀取cmd的切割後
					integer = HexStr_to_int(split_cmd_after);//Hex轉十進制整數
					CalPec(integer);//計算PEC值
					
					printf("string = 0X%s,int = %d\n",split_cmd_after,integer);//輸出計算值的紀錄

					split_cmd_after = strtok(NULL, split_hyphen);
				}
				printf("PEC = 0X%X\n\n",ubPmbusPec);//輸出PEC
				ubPmbusPec = 0x00;//初始化PEC
			}
		}else if(strncmp("2", select_function, 1) == 0){//function DIR_READONLY========================================================================================
			if((DIR_result = opendir(READ_ONLY_Path)) == NULL){//開啟目錄"./READ_ONLY/"
				printf("[FUNCTION.exe_DIR_READONLY]: opendir failed!\n");//若無法開啟則顯示錯誤訊息
				system("pause");//等待使用者
				printf("\n");
			}else{//開啟目錄"./READ_ONLY/"
				printf("[FUNCTION.exe_DIR_READONLY]: DIR\n");
				while(DIRresult_item = readdir(DIR_result)) {//一項一項讀取目錄裡的檔案或資料夾
					// printf("filename= %s\n",DIRresult_item->d_name);  //輸出檔案或者目錄的名稱
					// printf("filetype = %d\n",DIRresult_item->d_type);  //輸出檔案型別
					printf("%s\n",DIRresult_item -> d_name);  //輸出檔案或者目錄的名稱
				}
				closedir(DIR_result);//關閉目錄
				system("pause");//等待使用者
				printf("\n");
			}

		}else if(strncmp("3", select_function, 1) == 0){//function REPORT_DATA_Catch output_list========================================================================================
			while(1){
				printf("[FUNCTION.exe_output_list]: //------------------------------------------------//\n");
				printf("[FUNCTION.exe_output_list]: //case List:\n");
				for (int i = 0; i < REPORT_case_num; i++){//列印case list
					printf("[FUNCTION.exe_output_list]: //( %d ) %s\n", (i+1), REPORT_case_array[i]);
				}
				printf("[FUNCTION.exe_output_list]: //------------------------------------------------//\n");
				printf("[FUNCTION.exe_output_list]: input report case:");//提示請輸入case代碼
				scanf("%s",&REPORT_case_str);//接收輸入的值存入REPORT_case_str
				REPORT_case_int = atoi(REPORT_case_str) - 1;//字串轉成整數

				if(strncmp("exit", REPORT_case_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}else if(REPORT_case_int > (REPORT_case_num - 1)){//若輸入case超出範圍則重新選擇case
					printf("select case not found!\n\n");
					system("pause");//等待使用者
					printf("\n");
				}else{//進入case
					if((DIR_result = opendir(OUTPUT_LIST_Path)) == NULL){//開啟目錄"./READ_ONLY/output_list/"
						printf("[FUNCTION.exe_output_list]: opendir failed!\n\n");//若無法開啟則顯示錯誤訊息
						system("pause");//等待使用者
						printf("\n");
					}else{//開啟目錄"./READ_ONLY/output_list/"

						if(strncmp("Time_count", REPORT_case_array[REPORT_case_int], 10) == 0){//若輸入Time_count則詢問addtime
							printf("[FUNCTION.exe_output_list]: input addtime:");//提示請輸入addtime
							scanf("%s",&REPORT_addtime);//接收輸入的值存入REPORT_addtime
						}

						OUTPUT_LIST_count = 0;//初始化目錄"./READ_ONLY/output_list/"裡的文件數量
						while(DIRresult_item = readdir(DIR_result)) {//一項一項讀取目錄裡的檔案或資料夾
							if(DIRresult_item -> d_type == 0){//只開啟文件型別
								OUTPUT_LIST_count++;
								memset(REPORT_cmd, 0, sizeof(REPORT_cmd));//初始化字串
								strcat(REPORT_cmd, "REPORT_DATA_Catch.exe ");//字串合併
								strcat(REPORT_cmd, REPORT_case_array[REPORT_case_int]);//字串合併
								strcat(REPORT_cmd, " .\\READ_ONLY\\output_list\\");//字串合併
								strcat(REPORT_cmd, DIRresult_item->d_name);//字串合併
								if(strncmp("Time_count", REPORT_case_array[REPORT_case_int], 10) == 0){//若輸入Time_count則需合併addtime參數
									strcat(REPORT_cmd, " ");//字串合併
									strcat(REPORT_cmd, REPORT_addtime);//字串合併
								}
								system(REPORT_cmd);//執行REPORT_DATA_Catch
								printf("\n");
							}
						}
						closedir(DIR_result);//關閉目錄

						if(OUTPUT_LIST_count == 0){//若目錄裡沒有文件
							printf("[FUNCTION.exe_output_list]: Folder is empty!\n\n");//若目錄裡沒有文件則顯示錯誤訊息
							system("pause");//等待使用者
							printf("\n");
						}
					}
				}
			}
		}else if(strncmp("4", select_function, 1) == 0){//function Linear11 to Dec========================================================================================
			while(1){
				printf("[FUNCTION.exe_Linear11]: input Linear11:");//提示請輸入Linear11資料
				scanf("%s",&Linear11_str);//接收輸入的值存入Linear11_str
				if(strncmp("exit", Linear11_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}else{
					printf("\nDec = %s\n\n",Linear11_to_DEC(Linear11_str));//計算Linear11轉十進制並輸出
				}
			}
		}else if(strncmp("5", select_function, 1) == 0){//function Linear16 to Dec========================================================================================
			while(1){
				printf("[FUNCTION.exe_Linear16]: input Linear16:");//提示請輸入Linear16資料
				scanf("%s",&Linear16_str);//接收輸入的值存入Linear16_str
				if(strncmp("exit", Linear16_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}else{
					printf("\nDec = %s\n\n",Linear16_to_DEC(Linear16_str));//計算Linear16轉十進制並輸出
				}
			}
		}else if(strncmp("6", select_function, 1) == 0){//function Direct to Dec========================================================================================
			while(1){
				printf("[FUNCTION.exe_Direct]: input first Direct:");//提示請輸入Direct資料
				scanf(" %[^\n]",&Direct_str_1);//接收輸入的值存入Direct_str_1
				if(strncmp("exit", Direct_str_1, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}
				printf("[FUNCTION.exe_Direct]: input second Direct:");//提示請輸入Direct資料
				scanf(" %[^\n]",&Direct_str_2);//接收輸入的值存入Direct_str_2
				if(strncmp("exit", Direct_str_2, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}

				Direct_to_DEC_return = Direct_to_DEC(Direct_str_1,Direct_str_2);//計算Direct轉十進制
				printf("\nfirst Direct = %s,\nsecond Direct = %s,\nDec = %s\n\n",Direct_str_1,Direct_str_2,Direct_to_DEC_return);
			}
		}else if(strncmp("7", select_function, 1) == 0){//function ASCII_Hex to Text========================================================================================
			while(1){
				printf("[FUNCTION.exe_ASCII]: input ASCII_Hex:");//提示請輸入ASCII_Hex資料
				scanf(" %[^\n]",&ASCII_Hex_str);//接收輸入的值存入ASCII_Hex_str
				if(strncmp("exit", ASCII_Hex_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}

				ASCII_Hex_to_Text_return = ASCII_Hex_to_Text(ASCII_Hex_str);//ASCII_Hex轉Text
				printf("\nThe ASCII_Text = %s\n\n", ASCII_Hex_to_Text_return);
			}
		}else if(strncmp("8", select_function, 1) == 0){//function CRC16========================================================================================
			while(1){
				printf("[FUNCTION.exe_CRC16]: input Modbus cmd:");//提示請輸入Modbus cmd
				scanf(" %[^\n]",&Modebus_cmd_str);//接收輸入的值存入Modebus_cmd_str
				if(strncmp("exit", Modebus_cmd_str, 4) == 0){//若輸入exit則跳出迴圈等待下一個function代碼
					printf("\n");
					break;
				}printf("\n");

				after_replace_modbus_cmd = str_replace(Modebus_cmd_str," ","-");//取代後的cmd字串
				split_cmd_after = strtok(after_replace_modbus_cmd, split_hyphen);//用連字號切割cmd的字串

				while (split_cmd_after != NULL) {//讀取cmd的切割後
					integer = HexStr_to_int(split_cmd_after);//Hex轉十進制整數
					Petit_CRC16(integer, &Petit_TxRx_CRC16);//計算CRC16值
					
					printf("string = 0X%s,int = %d\n",split_cmd_after,integer);//輸出計算值的紀錄

					split_cmd_after = strtok(NULL, split_hyphen);
				}
				printf("CRC16 = 0X%X\n\n",Petit_TxRx_CRC16);//輸出CRC16
				Petit_TxRx_CRC16 = 0xFFFF;//初始化CRC16
			}
		}else if(strncmp("test", select_function, 4) == 0){//test========================================================================================
			char test_str[1000];
			while(1){
				printf("[FUNCTION.exe_test]: input:");
				scanf("%s",&test_str);

				if(0b1100 & 0b1000){
					printf("0b1100 & 0b1000 True\n");
				}else{
					printf("0b1100 & 0b1000 Fail\n");
				}
				if(0b1100 && 0b1000){
					printf("0b1100 && 0b1000 True\n");
				}else{
					printf("0b1100 && 0b1000 Fail\n");
				}
				if(!(0b1100 ^ 0b1000)){
					printf("!(0b1100 ^ 0b1000) True\n");
				}else{
					printf("!(0b1100 ^ 0b1000) Fail\n");
				}

				if(strncmp("exit", test_str, 4) == 0){
					break;
				}
			}
		}else if(strncmp("exit", select_function, 4) == 0){//exit========================================================================================
			break;
		}else{
			printf("select function not found!\n\n");
			system("pause");//等待使用者
			printf("\n");
		}
	}

	return 0;
}