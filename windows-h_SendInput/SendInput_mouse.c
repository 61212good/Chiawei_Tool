// gcc -o SendInput_mouse_demo SendInput_mouse.c & SendInput_mouse_demo.exe -lUser32
#include <windows.h>
#include <stdio.h>

// Swap
void Swap(int *tmpa, int *tmpb) {
	int tmp;

	tmp = *tmpa;
	*tmpa = *tmpb;
	*tmpb = tmp;
}

// Function to simulate mouse input using SendInput
void simulateMouseInput(DWORD dx, DWORD dy) {
	// INPUT input[3];
	INPUT input[1];

	// Move the mouse to the specified position
	input[0].type = INPUT_MOUSE;
	input[0].mi.dx = dx;
	input[0].mi.dy = dy;
	input[0].mi.mouseData = 0;
	input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
	input[0].mi.time = 0;
	input[0].mi.dwExtraInfo = 0;

	// // Press the left mouse button
	// input[1].type = INPUT_MOUSE;
	// input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	// // Release the left mouse button
	// input[2].type = INPUT_MOUSE;
	// input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	// Send the mouse input events
	// SendInput(3, input, sizeof(INPUT));
	SendInput(1, input, sizeof(INPUT));

}

int main() {

	int x = 0;
	int timeout;
	float f_timeout;
	int max_mouse_thr = 4;//4hr
	int sleep_tms = 5000;//5000ms
	int mousedx1 = 50;
	int mousedy1 = 50;
	int mousedx2 = -50;
	int mousedy2 = -50;

	timeout = ((max_mouse_thr * 3600) * 1000) / sleep_tms;

	f_timeout = timeout;

	do{
		Sleep(sleep_tms);
		x++;
		Swap(&mousedx1,&mousedx2);
		Swap(&mousedy1,&mousedy2);

		// Simulate moving the mouse to the position (100, 100) and clicking the left mouse button
		// simulateMouseInput(100, 100);

		simulateMouseInput(mousedx1, mousedy1);

		printf("%d sec, %f%%\n", ((sleep_tms/1000)*x), (x/f_timeout)*100);

	} while ( x < timeout );

	return 0;
}
