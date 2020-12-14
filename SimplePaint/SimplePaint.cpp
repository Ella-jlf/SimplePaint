#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>


#define btn_line 1
#define btn_broken_line 2
#define btn_rect 3 
#define btn_polygon 4
#define btn_ellipse 5
#define btn_text 6
#define edt_pen_width 7
#define edt_pen_color 8
#define edt_brush_color 9
#define edt_text 10


typedef struct MYCOLOR {
	int a;
	int b;
	int c;
} TMYCOLOR;


HWND hWnd;
HDC hdc;
RECT winSize;
int button_width = 80, button_height = 40;
int edit_width = 80, edit_height = 20;
int figure_type =0;
int mouseStartX= 0 , mouseStartY =0,mouseEndX = 0, mouseEndY =0;
int count = 0;
LPPOINT vertexes, tempVertexes;
bool drawing = false;
bool proceeding = false;
HWND btnLine, btnBrokenLine, btnRect, btnPolygon, btnEllipse, btnText,edtPenWidth, edtPenColor, edtBrushColor, edtText;
HPEN pen;
HBRUSH brush;
PTCHAR buff ;
int buffLenght = 0;
TMYCOLOR gPenColor = { 120,0,120 }, gBrushColor = { 0,200,50 };
int gPenWidth= 18;


struct Figure
{
	int startX;
	int startY;
	int endX;
	int endY;
	int type;
};
Figure figures[40];

void fillFigure() {
	Figure f;
	if (mouseStartX > mouseEndX) {
		int m = mouseEndX;
		mouseEndX = mouseStartX;
		mouseStartX = m;
	}
	if (mouseStartY > mouseEndY) {
		int m = mouseEndY;
		mouseEndY = mouseStartY;
		mouseStartY = m;
	}
	f.startX = mouseStartX;
	f.startY = mouseStartY;
	f.endX = mouseEndX;
	f.endY = mouseEndY;
	
	f.type = figure_type;
	figures[count] = f;
	count++;

}

void SetParams(HDC thisHdc,int penWidth, COLORREF penColor, COLORREF brushColor, bool brushPouring) {
	pen = CreatePen(PS_SOLID, penWidth, penColor);
	
	if (brushPouring)
		brush = CreateSolidBrush(brushColor);
	else {
		LOGBRUSH br;
		br.lbColor = brushColor;
		br.lbStyle = BS_HOLLOW;
		br.lbHatch = NULL;
		brush = CreateBrushIndirect(&br);
	}
	SelectObject(thisHdc, brush);
	SelectObject(thisHdc, pen);
}
void ClearParams() {
	DeleteObject(pen);
	DeleteObject(brush);
}
void WinDraw(HDC tempHdc) {
	HDC temp = CreateCompatibleDC(tempHdc);
	HBITMAP bitmap = CreateCompatibleBitmap(temp, 640, 480);
	SelectObject(temp, bitmap);
	BitBlt(temp, 0, 0, 640, 480, tempHdc, 0, 0, SRCCOPY  );
	SetParams(tempHdc, gPenWidth, RGB(gPenColor.a, gPenColor.b, gPenColor.c), RGB(gBrushColor.a, gBrushColor.b, gBrushColor.c), true);
	POINT mouseCursor;
	POINT t;
	if (drawing) {
		GetCursorPos(&mouseCursor);
		ScreenToClient(hWnd, &mouseCursor);

		switch (figure_type)
		{
		case btn_line:
			MoveToEx(tempHdc, mouseStartX, mouseStartY, NULL);
			LineTo(tempHdc, mouseCursor.x, mouseCursor.y);


			break;
		case  btn_broken_line:
			MoveToEx(tempHdc, vertexes[0].x, vertexes[0].y, NULL);
			for (int i = 1; i < count;i++) {
				LineTo(tempHdc, vertexes[i].x, vertexes[i].y);
			}
			LineTo(tempHdc, mouseCursor.x, mouseCursor.y);


			break;
		case  btn_rect:
			Rectangle(tempHdc, mouseStartX, mouseStartY, mouseCursor.x, mouseCursor.y);


			break;
		case  btn_polygon:
			if (tempVertexes == NULL) {
				tempVertexes = (LPPOINT)malloc((count + 1) * sizeof(POINT));
			}
			else {
				tempVertexes = (LPPOINT)realloc(tempVertexes,(count + 1) * sizeof(POINT));
			}
			if (count + 1 >= 2) {
				for (int i = 0;i < count;i++) {
					tempVertexes[i] = vertexes[i];
				}
				t = { mouseCursor.x,mouseCursor.y };
				tempVertexes[count] = t;
				Polygon(tempHdc, tempVertexes, count+1);
			}


			
			break;
		case  btn_ellipse:
			Ellipse(tempHdc, mouseStartX, mouseStartY, mouseCursor.x, mouseCursor.y);



			break;
		case  btn_text:
			RECT rt;
			rt.left = mouseStartX;
			rt.top = mouseStartY;
			rt.bottom = mouseCursor.x;
			rt.right = mouseCursor.y;

			DrawText(hdc, buff, buffLenght, &rt, NULL);
			break;
		default:
			figure_type = 0;
			break;
		}
		Sleep(10);

		BitBlt(tempHdc, 0, 0, 640, 480, temp, 0, 0, SRCCOPY | SRCINVERT );


	}
	else {

	}
	ClearParams();
	DeleteObject(bitmap);
	DeleteDC(temp);


}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDBLCLK:
		SetParams(hdc, gPenWidth, RGB(gPenColor.a, gPenColor.b, gPenColor.c), RGB(gBrushColor.a, gBrushColor.b, gBrushColor.c), true);
		if (proceeding) {
			switch (figure_type)
			{
			case btn_polygon:
				if (count >= 2) {
					Polygon(hdc, vertexes, count);
					}
				break;
			case btn_broken_line:
				if (count >= 2) {
					MoveToEx(hdc, vertexes[0].x, vertexes[0].y, NULL);
					for (int i = 1; i < count;i++) {
						LineTo(hdc, vertexes[i].x, vertexes[i].y);
					}
				}
				break;
			default:
				break;
			}
			figure_type = 0;
			count = 0;
			
			drawing = false;
			proceeding = false;
		}
		ClearParams();
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
		case VK_SPACE:
		
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		if (drawing) {
			POINT t;
			SetParams(hdc, gPenWidth, RGB(gPenColor.a, gPenColor.b, gPenColor.c), RGB(gBrushColor.a, gBrushColor.b, gBrushColor.c), true);
			switch (figure_type)
			{
			case btn_line:
				MoveToEx(hdc, mouseStartX, mouseStartY, NULL);
				LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
				drawing = false;
				figure_type = 0;
				break;
			case  btn_broken_line:

				t = { LOWORD(lParam), HIWORD(lParam) };
				count++;
				if (vertexes != NULL) {
					vertexes = (LPPOINT)realloc(vertexes, count * sizeof(POINT));
				}
				else {
					vertexes = (LPPOINT)malloc(count * sizeof(POINT));
				}
				vertexes[count-1] = t;


				if (proceeding) {
					MoveToEx(hdc, vertexes[0].x,vertexes[0].y, NULL);

					for (int i = 1; i < count;i++) {
						LineTo(hdc, vertexes[i].x, vertexes[i].y);
					}
				}

				break;
			case  btn_rect:

				Rectangle(hdc, mouseStartX, mouseStartY, LOWORD(lParam), HIWORD(lParam));
				drawing = false;
				figure_type = 0;
				break;
			case  btn_polygon:
				t = { LOWORD(lParam), HIWORD(lParam) };
				count++;
				if (vertexes != NULL) {
					vertexes = (LPPOINT)realloc(vertexes, count * sizeof(POINT));
				}
				else {
					vertexes = (LPPOINT)malloc(count * sizeof(POINT));
				}
				vertexes[count - 1] = t;



				if (proceeding) {
					Polygon(hdc, vertexes, count);
				}
				break;
			case  btn_ellipse:

				Ellipse(hdc, mouseStartX, mouseStartY, LOWORD(lParam), HIWORD(lParam));
				drawing = false;
				figure_type = 0;
				break;
			case  btn_text:
				RECT rt;
				rt.left = mouseStartX;
				rt.top = mouseStartY;
				rt.bottom = HIWORD(lParam);
				rt.right = LOWORD(lParam);

				DrawText(hdc, buff, buffLenght, &rt, DT_CENTER);
				free(buff);
				drawing = false;
				figure_type = 0;
				break;
			default:
				figure_type = 0;
				break;
			}
			ClearParams();
		}
		else {

			POINT t;
		switch (figure_type)
		{
		case btn_line:
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);
			
			drawing = true;
			break;
		case  btn_broken_line:
			
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);
			t = { mouseStartX,mouseStartY };
			count++;
			if (vertexes != NULL) {
				vertexes = (LPPOINT)realloc(vertexes, count * sizeof(POINT));
			}
			else {
				vertexes = (LPPOINT)malloc(count * sizeof(POINT));
			}
			vertexes[0] = t;
			drawing = true;
			proceeding = true;
			break;
		case  btn_rect:
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);

			drawing = true;
			break;
		case  btn_polygon:
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);
			t = { mouseStartX,mouseStartY };
			count++;
			if (vertexes != NULL) {
				vertexes = (LPPOINT)realloc(vertexes, count * sizeof(POINT));
			}
			else {
				vertexes = (LPPOINT)malloc(count * sizeof(POINT));
			}
			vertexes[0] = t;
			drawing = true;
			proceeding = true;
			break;
		case  btn_ellipse:
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);

			drawing = true;
			break;
		case  btn_text:
			mouseStartX = LOWORD(lParam);
			mouseStartY = HIWORD(lParam);
			buffLenght = GetWindowTextLength(edtText)+1;
			buff = (PTCHAR)malloc(sizeof(TCHAR)* buffLenght);
			memset(buff, 0, buffLenght);
			if (!GetWindowText(edtText, buff, buffLenght)) {
				MessageBox(hWnd, L"poebota", L"xyeta", MB_OKCANCEL);
			}
		



			drawing = true;
			break;
		default:
			SetFocus(hWnd);
			break;
		} 
		}
		break;
	case WM_LBUTTONUP:
		
		break;
	case WM_RBUTTONDOWN:
		figure_type = 0;
		drawing = false;
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &winSize);
		
		break;
	case WM_COMMAND:
		figure_type = LOWORD(wParam);
		break;
	case WM_DESTROY:
		free(vertexes);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"SimplePaintClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow(L"SimplePaintClass", L"SimplePaint",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,CW_USEDEFAULT,
		640,480,NULL, NULL,hInstance, NULL);
	SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	GetClientRect(hWnd, &winSize);
	hdc = GetWindowDC(hWnd);

	
	btnLine = CreateWindow(L"button",L"line",WS_VISIBLE | WS_CHILD,winSize.left,winSize.bottom-button_height,button_width,button_height,hWnd,(HMENU)btn_line,NULL,NULL);
	btnBrokenLine = CreateWindow(L"button", L"Brline", WS_VISIBLE | WS_CHILD, winSize.left, winSize.bottom - 2*button_height, button_width, button_height, hWnd, (HMENU)btn_broken_line, NULL, NULL);
		btnRect = CreateWindow(L"button", L"Rect", WS_VISIBLE | WS_CHILD, winSize.left, winSize.bottom - 3*button_height, button_width, button_height, hWnd, (HMENU)btn_rect, NULL, NULL);
		btnPolygon = CreateWindow(L"button", L"Poly", WS_VISIBLE | WS_CHILD, winSize.left, winSize.bottom - 4*button_height, button_width, button_height, hWnd, (HMENU)btn_polygon, NULL, NULL);
		btnEllipse = CreateWindow(L"button", L"Ellipse", WS_VISIBLE | WS_CHILD, winSize.left, winSize.bottom - 5*button_height, button_width, button_height, hWnd, (HMENU)btn_ellipse, NULL, NULL);
		btnText = CreateWindow(L"button", L"Text", WS_VISIBLE | WS_CHILD, winSize.left, winSize.bottom - 6*button_height, button_width, button_height, hWnd, (HMENU)btn_text, NULL, NULL);
		//edtPenWidth = CreateWindow(L"edit", L"18", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_RIGHT , winSize.right - edit_width, winSize.bottom, edit_width, edit_height, hWnd, (HMENU)edt_pen_width, NULL, NULL);
		//edtPenColor = CreateWindow(L"edit", L"120000120", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_RIGHT, winSize.right - 2*edit_width, winSize.bottom, edit_width, edit_height, hWnd, (HMENU)edt_pen_color, NULL, NULL);
		//edtBrushColor = CreateWindow(L"edit", L"000200050", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_RIGHT, winSize.right -3*edit_width, winSize.bottom, edit_width, edit_height, hWnd, (HMENU)edt_brush_color, NULL, NULL);
		edtText = CreateWindow(L"edit", L"Here some text", WS_VISIBLE | WS_CHILD | WS_BORDER , winSize.left, winSize.bottom - 7 * button_height, edit_width, edit_height, hWnd, (HMENU)edt_text, NULL, NULL);
	

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {


			WinDraw(hdc);
			Sleep(5);
		}
		
	}

	return (int)msg.wParam;
}
