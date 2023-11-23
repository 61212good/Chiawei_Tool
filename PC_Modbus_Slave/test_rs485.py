import serial, time

ser = serial.Serial()
ser.port = "COM4"

#115200,N,8,1
ser.baudrate = 9600#115200
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check
ser.stopbits = serial.STOPBITS_ONE #number of stop bits

ser.timeout = 0.5          #non-block read 0.5s
ser.writeTimeout = 0.5     #timeout for write 0.5s
ser.xonxoff = False    #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False     #disable hardware (DSR/DTR) flow control
		
#[CRC16]******************************************************************************/
def CRC16(buf):
	crc = 0xFFFF

	for byte in buf:
		crc ^= byte

		for _ in range(8):
			if crc & 0x0001:
				crc >>= 1
				crc ^= 0xA001
			else:
				crc >>= 1

	return crc
#******************************************************************************/

try: 
	ser.open()
except Exception as ex:
	print ("open serial port error " + str(ex))
	exit()

if ser.isOpen():

	try:
		# ser.flushInput() #flush input buffer
		# ser.flushOutput() #flush output buffer

		# #/write byte data******************************************************************************/

		# Tx_Data_Array = [0x02, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09]
		# ser.write(Tx_Data_Array)
		# print("write 8 byte data: ", end = '')

		# for i in range(len(Tx_Data_Array)):
		# 	if i != (len(Tx_Data_Array) - 1):
		# 		print(hex(Tx_Data_Array[i]), end = ", ")
		# 	else:
		# 		print(hex(Tx_Data_Array[i]))

		# time.sleep(1)  #wait 1s

		#/read byte data******************************************************************************/
		print("Waiting Rx data...")

		while 1:
			ser.flushInput() #flush input buffer
			ser.flushOutput() #flush output buffer
			time.sleep(0.5)  #wait 1s

			response = ser.readline()	# read byte data
			response_hex_str = hex(int.from_bytes(response,byteorder='little'))	# response to hex_str

			if (int(response_hex_str, 16) != 0x0):
				print("[Rx data]: " + response_hex_str)

				Rx_Addr = int(response_hex_str, 16) & 0xFF;# (hex_str to int) and Extract the two bytes with the minimum weight.
				Rx_FunCode = ((int(response_hex_str, 16) & 0xFF00) >> 8) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

				print("read Rx_Addr: " + hex(Rx_Addr))
				print("read Rx_FunCode: " + hex(Rx_FunCode))

				if (Rx_FunCode == 0x2): #[Case: Read PWM Status]***************************************
					Rx_Starting_channel = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_Starting_channel = (((int(response_hex_str, 16) & 0xFF0000) >> 8) | Rx_Starting_channel) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_N_of_channel = ((int(response_hex_str, 16) & 0xFF0000000000) >> 40) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_N_of_channel = (((int(response_hex_str, 16) & 0xFF00000000) >> 24) | Rx_N_of_channel) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					
					print("read Rx_Starting_channel: " + hex(Rx_Starting_channel))
					print("read Rx_N_of_channel: " + hex(Rx_N_of_channel), end = "\n\n")
					print("[Case: Read PWM Status]", end = "\n\n")

					if (Rx_Starting_channel > 0x07):
						print("[Error: The starting channel is out of range]", end = "\n\n")

						Rx_FunCode = Rx_FunCode | 0x80;
						Rx_ExceptionCode = 0x02;
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
					elif (((Rx_Starting_channel + Rx_N_of_channel) > 0x08) or 
						(Rx_N_of_channel == 0x00)):
						print("[Error: (The starting channel + number of input channels) is out of range, or an incorrect number of bytes were received]", end = "\n\n")

						Rx_FunCode = Rx_FunCode | 0x80;
						Rx_ExceptionCode = 0x03;
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
					else:
						Tx_Byte_cnt = 1;
						Tx_Data = 0xF1;

						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Tx_Byte_cnt, Tx_Data]

				elif (Rx_FunCode == 0x4): #[Case: Read DI Count]***************************************
					Rx_Starting_channel = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_Starting_channel = (((int(response_hex_str, 16) & 0xFF0000) >> 8) | Rx_Starting_channel) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_N_of_channel = ((int(response_hex_str, 16) & 0xFF0000000000) >> 40) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					Rx_N_of_channel = (((int(response_hex_str, 16) & 0xFF00000000) >> 24) | Rx_N_of_channel) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
					
					print("read Rx_Starting_channel: " + hex(Rx_Starting_channel))
					print("read Rx_N_of_channel: " + hex(Rx_N_of_channel), end = "\n\n")
					print("[Case: Read DI Count]", end = "\n\n")

					if (Rx_Starting_channel > 0x0F):
						print("[Error: The starting channel is out of range]", end = "\n\n")

						Rx_FunCode = Rx_FunCode | 0x80;
						Rx_ExceptionCode = 0x02;
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
					elif (((Rx_Starting_channel + (Rx_N_of_channel * 2)) > 0x10) or 
						(Rx_N_of_channel == 0x00)):
						print("[Error: (The starting channel + number of input channels) is out of range, or an incorrect number of bytes were received]", end = "\n\n")

						Rx_FunCode = Rx_FunCode | 0x80;
						Rx_ExceptionCode = 0x03;
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
					else:
						Tx_Byte_cnt = Rx_N_of_channel * 2;
						
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Tx_Byte_cnt]

						for i in range(Tx_Byte_cnt):
							Tx_Data_Array.insert((i + 3), (0xF0 | (i + 1)))

				elif (Rx_FunCode == 0x46): #[Case: Read/Write Modbus Settings]***************************************
					Rx_SubFunCode = ((int(response_hex_str, 16) & 0xFF0000) >> 16) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

					print("read Rx_SubFunCode: " + hex(Rx_SubFunCode), end = "\n\n")
					print("[Case: Read/Write Modbus Settings]", end = "\n\n")

					Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_SubFunCode]

					if (Rx_SubFunCode == 0x00): #[Sub-Case: Read module name ]---------------------------------------
						print("[Sub-Case: Read module name]", end = "\n\n")

						if (int(response_hex_str, 16) >= 0x010000000000):
							print("[Error: An incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x00)
							Tx_Data_Array.insert(4, 0x70)
							Tx_Data_Array.insert(5, 0x88)
							Tx_Data_Array.insert(6, 0x00)
					elif (Rx_SubFunCode == 0x04): #[Sub-Case: Set module address ]---------------------------------------
						Rx_NewAddr = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved = ((int(response_hex_str, 16) & 0xFF000000000000) >> 48) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved = (((int(response_hex_str, 16) & 0xFF0000000000) >> 32) | Rx_Reserved) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved = (((int(response_hex_str, 16) & 0xFF00000000) >> 16) | Rx_Reserved) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

						print("[Sub-Case: Set module address]")
						print("read Rx_NewAddr: " + hex(Rx_NewAddr))
						print("read Rx_Reserved: " + hex(Rx_Reserved), end = "\n\n")

						if ((int(response_hex_str, 16) >= 0x01000000000000000000) or 
							(Rx_Reserved != 0x0) or
							((Rx_NewAddr < 1) or (Rx_NewAddr > 247))):
							print("[Error: The new address is out of range, reserved bytes should be filled with zero, or an incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x00)
							Tx_Data_Array.insert(4, 0x00)
							Tx_Data_Array.insert(5, 0x00)
							Tx_Data_Array.insert(6, 0x00)
					elif (Rx_SubFunCode == 0x05): #[Sub-Case: Read communication settings ]---------------------------------------
						Rx_Reserved = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

						print("[Sub-Case: Read communication settings]")
						print("read Rx_Reserved: " + hex(Rx_Reserved), end = "\n\n")

						if ((int(response_hex_str, 16) >= 0x01000000000000) or 
							(Rx_Reserved != 0x0)):
							print("[Error: Reserved bytes should be filled with zeros, or an incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x00)
							Tx_Data_Array.insert(4, 0x06) #Baud Rate: 9600
							Tx_Data_Array.insert(5, 0x00)
							Tx_Data_Array.insert(6, 0x00) #0x00: No parity, 1 stop bit
							Tx_Data_Array.insert(7, 0x00)
							Tx_Data_Array.insert(8, 0x01) #1: Modbus RTU protocol 
							Tx_Data_Array.insert(9, 0x00)
							Tx_Data_Array.insert(10, 0x00)
					elif (Rx_SubFunCode == 0x06): #[Sub-Case: Set communication settings ]---------------------------------------
						Rx_Reserved_1 = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_BaudRate = ((int(response_hex_str, 16) & 0xFF00000000) >> 32) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved_2 = ((int(response_hex_str, 16) & 0xFF0000000000) >> 40) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Parity = ((int(response_hex_str, 16) & 0xFF000000000000) >> 48) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved_3 = ((int(response_hex_str, 16) & 0xFF00000000000000) >> 56) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Mode = ((int(response_hex_str, 16) & 0xFF0000000000000000) >> 64) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved_4 = ((int(response_hex_str, 16) & 0xFF00000000000000000000) >> 80) ;# (hex_str to int) and Extract the two bytes with the minimum weight.
						Rx_Reserved_4 = (((int(response_hex_str, 16) & 0xFF000000000000000000) >> 64) | Rx_Reserved_4) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

						print("[Sub-Case: Set communication settings]")
						print("read Rx_Reserved_1: " + hex(Rx_Reserved_1))
						print("read Rx_BaudRate: " + hex(Rx_BaudRate))
						print("read Rx_Reserved_2: " + hex(Rx_Reserved_2))
						print("read Rx_Parity: " + hex(Rx_Parity))
						print("read Rx_Reserved_3: " + hex(Rx_Reserved_3))
						print("read Rx_Mode: " + hex(Rx_Mode))
						print("read Rx_Reserved_4: " + hex(Rx_Reserved_4), end = "\n\n")
						
						if ((int(response_hex_str, 16) >= 0x0100000000000000000000000000) or 
							((Rx_Reserved_1 != 0x0) or (Rx_Reserved_2 != 0x0) or (Rx_Reserved_3 != 0x0) or (Rx_Reserved_4 != 0x0)) or
							((Rx_BaudRate < 0x03) or (Rx_BaudRate > 0x0A)) or
							(Rx_Parity > 0x03) or
							(Rx_Mode > 0x01)):
							print("[Error: The Baud Rate or mode is out of range, reserved bytes should be filled with zeros, or an incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x00)
							Tx_Data_Array.insert(4, 0x00) #OK
							Tx_Data_Array.insert(5, 0x00)
							Tx_Data_Array.insert(6, 0x00) #OK
							Tx_Data_Array.insert(7, 0x00)
							Tx_Data_Array.insert(8, 0x00) #OK
							Tx_Data_Array.insert(9, 0x00)
							Tx_Data_Array.insert(10, 0x00)
					elif (Rx_SubFunCode == 0x20): #[Sub-Case: Read firmware version ]---------------------------------------
						print("[Sub-Case: Read firmware version]", end = "\n\n")
						
						if (int(response_hex_str, 16) >= 0x010000000000):
							print("[Error: An incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0xFF)
							Tx_Data_Array.insert(4, 0xF1)
							Tx_Data_Array.insert(5, 0xF1)
					elif (Rx_SubFunCode == 0x29): #[Sub-Case: Read miscellaneous ]---------------------------------------
						print("[Sub-Case: Read miscellaneous]", end = "\n\n")
						
						if (int(response_hex_str, 16) >= 0x010000000000):
							print("[Error: An incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x01) #checksum enable
					elif (Rx_SubFunCode == 0x2A): #[Sub-Case: Write miscellaneous settings ]---------------------------------------
						Rx_Miscellaneous = ((int(response_hex_str, 16) & 0xFF000000) >> 24) ;# (hex_str to int) and Extract the two bytes with the minimum weight.

						print("[Sub-Case: Write miscellaneous settings]")
						print("read Rx_Miscellaneous: " + hex(Rx_Miscellaneous), end = "\n\n")

						if ((int(response_hex_str, 16) >= 0x01000000000000) or
							((Rx_Miscellaneous != 0x40) and (Rx_Miscellaneous != 0x00))):
							print("[Error: Reserved bits should be filled with zeros, or an incorrect number of bytes were received]", end = "\n\n")

							Rx_FunCode = Rx_FunCode | 0x80;
							Rx_ExceptionCode = 0x03;
							Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						else:
							Tx_Data_Array.insert(3, 0x00) #OK
					else:
						Rx_FunCode = Rx_FunCode | 0x80;
						Rx_ExceptionCode = 0x02;
						Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
						print("[Error: Invalid sub-function code]", end = "\n\n")

				else:
					Rx_FunCode = Rx_FunCode | 0x80;
					Rx_ExceptionCode = 0x02;
					Tx_Data_Array = [Rx_Addr, Rx_FunCode, Rx_ExceptionCode]
					print("\n"+"[Case: Error Response: Undefind FunCode]", end = "\n\n")

				CRC16_result = CRC16(Tx_Data_Array)
				CRC16_result_H = CRC16_result >> 8;
				CRC16_result_L = CRC16_result & 0xFF;

				Tx_Data_Array.insert(len(Tx_Data_Array), CRC16_result_L)
				Tx_Data_Array.insert((len(Tx_Data_Array) + 1), CRC16_result_H)

				ser.write(Tx_Data_Array)
				print("[Tx "+str(len(Tx_Data_Array))+"byte data]: ", end = '')

				for i in range(len(Tx_Data_Array)):
					if i != (len(Tx_Data_Array) - 1):
						print(hex(Tx_Data_Array[i]), end = ", ")
					else:
						print(hex(Tx_Data_Array[i]), end = "\n\n")

			print("Waiting Rx data...")
		
		ser.close()
	except Exception as e1:
		print ("communicating error " + str(e1))

else:
	print ("open serial port error")
