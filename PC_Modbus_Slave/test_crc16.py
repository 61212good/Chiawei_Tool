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

# 示例用法：
data = [0xD0, 0x02, 0x00, 0x00, 0x00, 0x08]
result = CRC16(data)
print("CRC-16 Result:", hex(result))