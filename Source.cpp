#include <Windows.h>
#include <stdint.h>
#include <Xinput.h>

#define intern static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint32_t uint32;

struct win32_offscreen_buffer
{
	
	BITMAPINFO bitmapInfo;
	void* bitmapMemory;
	int bitmapW;
	int bitmapH;
	int pitch;
	int bytesPerPixel;
};

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibratio)
typedef X_INPUT_GET_STATE(x_InputGetState);
typedef X_INPUT_SET_STATE(x_InputSetState);

X_INPUT_GET_STATE(XInputGetStateStub)
{
	return 0;
}
global_variable x_InputGetState* XInputGetState_ = XInputGetStateStub;

X_INPUT_SET_STATE(XInputSetStateStub)
{
	return 0;
}
global_variable x_InputSetState* XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_


global_variable bool running;
global_variable win32_offscreen_buffer backBuffer;

struct win32_window_dimension
{
	int width;
	int height;
};

intern void win32_load_xinput()
{
	HMODULE xInputLibrary = LoadLibrary("xinput1_3.dll");
	if (xInputLibrary)
	{
		XInputGetState = (x_InputGetState*)GetProcAddress(xInputLibrary, "XInputGetState");
		XInputSetState = (x_InputSetState*)GetProcAddress(xInputLibrary, "XInputSetState");
	}
}

intern win32_window_dimension Win32GetWindowDimension(HWND window)
{
	win32_window_dimension result;
	RECT clientRect;
	GetClientRect(window, &clientRect);
	result.width = clientRect.right - clientRect.left;
	result.height = clientRect.bottom - clientRect.top;
	return result;
}

intern void RenderWeirdGradient(win32_offscreen_buffer buffer, int x_offset, int y_offset)
{
	int width = buffer.bitmapW;
	int height = buffer.bitmapH;

	//int pitch = width * buffer.bytesPerPixel;
	uint8* row = (uint8*)buffer.bitmapMemory;
	for (int y = 0; y < height; ++y)

	{
		uint32* pixel = (uint32*)row;
		for (int x = 0; x < width; ++x)
		{
			//               0  1  2  3 
			//Pixel in mem: xx BB GG RR
			uint8 b = x + x_offset;
			uint8 g = y + y_offset;

			*pixel++ = ((g << 8) | b);
			/*
				Mem: 0xBBGGRRXX
				REG: xx RR GG BB

			*/
		}
		row += buffer.pitch;
	}
}

intern void Win32ResizeDIBSection(win32_offscreen_buffer* buffer, int w, int h)
{
	if (buffer->bitmapMemory)
	{
		VirtualFree(buffer->bitmapMemory, 0, MEM_RELEASE);
	}

	buffer->bitmapW = w;
	buffer->bitmapH = h;
	buffer->bytesPerPixel = 4;

	buffer->bitmapInfo.bmiHeader.biSize = sizeof(buffer->bitmapInfo.bmiHeader);
	buffer->bitmapInfo.bmiHeader.biWidth = buffer->bitmapW;
	buffer->bitmapInfo.bmiHeader.biHeight = -(buffer->bitmapH);
	buffer->bitmapInfo.bmiHeader.biPlanes = 1;
	buffer->bitmapInfo.bmiHeader.biBitCount = 32;
	buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;
	
	int bitmapMemorySize = (buffer->bitmapW * buffer->bitmapH)* buffer->bytesPerPixel;
	buffer->bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	buffer->pitch = buffer->bitmapW * buffer->bytesPerPixel;
}

intern void Win32DisplayBufferInWindow(HDC context, win32_offscreen_buffer* buffer, int x, int y, int width, int height)
{
	int windowW = width;
	int windowH = height;

	StretchDIBits(context, 0, 0, windowW, windowH, 0, 0, buffer->bitmapW, buffer->bitmapH, buffer->bitmapMemory, &buffer->bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

intern LRESULT CALLBACK Win32MainWindowCallBack(
	HWND   Window,
	UINT   Message,
	WPARAM WParam,
	LPARAM LParam
)
{
	LRESULT Result = 0;

	win32_load_xinput();

	switch (Message)
	{
		case WM_SIZE:
		{
			
			OutputDebugStringA("WM_SIZE\n");
		}
		break;

		case WM_DESTROY:
			running = false;
			OutputDebugStringA("WM_DESTROY\n");
		break;

		case WM_CLOSE:
			running = false;
			OutputDebugStringA("WM_CLOSE\n");
		break;

		case WM_ACTIVATEAPP:
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;

		case WM_SYSKEYDOWN:
		{

		}
		break;
		case WM_SYSKEYUP:
		{

		}
		break;
		case WM_KEYDOWN:
		{

		}
		break;
		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			bool wasDown = ((LParam & (1 << 30)) != 0);
			bool isDown = ((LParam && (1 << 30)) == 0);
			if (wasDown != isDown)
			{
				if (VKCode == 'W')
				{

				}
				else if (VKCode == 'A')
				{

				}
				else if (VKCode == 'S')
				{

				}
				else if (VKCode == 'D')
				{

				}
				else if (VKCode == 'Q')
				{

				}
				else if (VKCode == 'E')
				{

				}
				else if (VKCode == VK_UP)
				{

				}
				else if (VKCode == VK_DOWN)
				{

				}
				else if (VKCode == VK_LEFT)
				{

				}
				else if (VKCode == VK_RIGHT)
				{

				}
			}
		}
		break;

		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC DeviceContext = BeginPaint(Window, &paint);
			int x = paint.rcPaint.left;
			int y = paint.rcPaint.top;
			int height = paint.rcPaint.bottom - paint.rcPaint.top;
			int width = paint.rcPaint.right - paint.rcPaint.left;
			Win32DisplayBufferInWindow(DeviceContext, &backBuffer, x, y, width, height);
			EndPaint(Window, &paint);
			OutputDebugStringA("WM_PAINT\n");
		}
		break;

		default:
			Result = DefWindowProc(Window, Message, WParam, LParam);
		break;
	}

	return Result;
}


int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeWindowClass";

	Win32ResizeDIBSection(&backBuffer, 1280, 720);

	if(RegisterClass(&WindowClass))
	{ 
		HWND window = CreateWindowEx(
			0, 
			WindowClass.lpszClassName, 
			"Handmade Code", 
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			0, 
			0, 
			Instance, 
			0);

		if (window)
		{
			running = true;

			int x_offset = 0;
			int y_offset = 0;

			while (running)
			{
				MSG message;
				
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
						running = false;

					TranslateMessage(&message);
					DispatchMessageA(&message);
				}

				DWORD dwResult;
				for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
				{
					XINPUT_STATE controllerState;

					if (XInputGetState(i, &controllerState) == ERROR_SUCCESS)
					{
						// Controller is plugged in.
						XINPUT_GAMEPAD* pad = &controllerState.Gamepad;

						bool up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool start = (pad->wButtons & XINPUT_GAMEPAD_START);
						bool back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool buttonA = (pad->wButtons & XINPUT_GAMEPAD_A);
						bool buttonB = (pad->wButtons & XINPUT_GAMEPAD_B);
						bool buttonX = (pad->wButtons & XINPUT_GAMEPAD_X);
						bool buttonY = (pad->wButtons & XINPUT_GAMEPAD_Y);
						bool leftShoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool rightShoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

						short int leftStickX = pad->sThumbLX;
						short int leftStickY = pad->sThumbLY;

						x_offset += leftStickX/10000;
						y_offset += leftStickY/10000;
					}
					else
					{
						// Controller is not connected.
					}
				}



				RenderWeirdGradient(backBuffer,x_offset, y_offset);

				HDC DeviceContext = GetDC(window);
				win32_window_dimension dim = Win32GetWindowDimension(window);
				Win32DisplayBufferInWindow(DeviceContext, &backBuffer, 0, 0, dim.width, dim.height);
				ReleaseDC(window, DeviceContext);
				//++x_offset;
				//++y_offset;
			}
		}
		else
		{
			//Log Errors
		}

	}
	else
	{
		//Log Errors
	}

	return 0;
}
	