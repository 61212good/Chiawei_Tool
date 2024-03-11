//宣告函數原型
typedef volatile unsigned char		UINT8;					// Volatele UINT8
typedef const unsigned char			C_UINT8;				// Constant UINT8
extern UINT8 ubPmbusPec;									// Q0, PMBus PEC
extern unsigned int Petit_TxRx_CRC16;							// CRC16

void CalPec(UINT8 ubData);
void Petit_CRC16(const unsigned char Data, unsigned int* CRC);
char* substr(const char *src, int m, int n);
char *str_replace(char *source, char *find,  char *rep);
int Time_to_Sec(int readYear, int readMon, int readDay, int readHour, int readMin, int readSec);
char *Time_to_Str(time_t time_totle_sec);
int HexStr_to_int(char *hex_val);
char *Linear16_to_DEC(char *hex_val);
char *Linear11_to_DEC(char *hex_val);
char *Direct_to_DEC(char *read_data_last, char *read_data_current);
char *ASCII_Hex_to_Text(char *hex_val);