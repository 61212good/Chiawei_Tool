// gcc -o SendInput_keyboard_demo SendInput_keyboard.c & SendInput_keyboard_demo.exe -lUser32
#include <windows.h>
#include <stdio.h>

// Function to simulate pressing and releasing a key using SendInput
void simulateKeyPress(WORD key) {
	INPUT input[2];

	// Set up the key press event
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = key;
	input[0].ki.dwFlags = 0;

	// Set up the key release event
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = key;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;

	// Send the input events
	SendInput(2, input, sizeof(INPUT));
}

int main() {

	int x = 0;
	int timeout = 0;
	float f_timeout;
	int max_mouse_thr = 4;//4hr
	int sleep_tms = 5000;//5000ms

	timeout = ((max_mouse_thr * 3600) * 1000) / sleep_tms;

	f_timeout = timeout;

	do{
		Sleep(sleep_tms);
		x++;

		// Simulate pressing and releasing the 'A' key
		simulateKeyPress('A');

		printf("%d sec, %f%%\n", ((sleep_tms/1000)*x), (x/f_timeout)*100);

	} while ( x < timeout );

	return 0;
}
