#include "windows.h"
#include "resource.h"
#include "TetrisGame.h"

LRESULT CALLBACK WndProc (HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK OptionProc (HWND hOption,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutProc (HWND hAbout,UINT msg,WPARAM wParam,LPARAM lParam);
void BlankArray(int board[10][22]);
HINSTANCE hInstance;
int Diff = 1;
int BoardArray[10][22];
enum MauVe{BLACK,PURPLE,YELLOW};
void UpdateGraphic(HWND hwnd);//Cập nhật lại bảng vẽ bao gồm 
// ma trận gạch, ma trận động và ma trận next,điểm,độ khó.
void DrawCell (HWND hWnd,MauVe color,int x,int y);//Vẽ ô x,y với
//màu vẽ xác định
void DrawNext (HWND hWnd,int x,int y);//Vẽ ô Next
void DrawScore (HWND hWnd,int Score);//Vẽ điểm
void DrawDifficulty (HWND hWnd,int Diff);//Vẽ độ khó
CTetrisGame MainGame;
int const IDT_GAMETIME =49999;
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd ){
	HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = hBrush;
	WndClass.hCursor = LoadCursor (NULL,IDC_ARROW);
	WndClass.hIcon = LoadIcon (hInstance,MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpszClassName = TEXT("ClassTetris");
	WndClass.style = CS_VREDRAW|CS_HREDRAW;
	WndClass.lpszMenuName = MAKEINTRESOURCE (IDR_MENU1);
	WndClass.lpfnWndProc = WndProc;
	if (!RegisterClass (&WndClass)){
		MessageBox (NULL,TEXT("Lỗi"),MB_ICONSTOP|MB_OK);
		return 0;
	}
	HWND hWnd = CreateWindow (WndClass.lpszClassName,TEXT("Tetris"),WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_BORDER|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,290,355,NULL,NULL,NULL,NULL);
	ShowWindow(hWnd,nShowCmd);
	UpdateWindow(hWnd);
	SetTimer(hWnd,IDT_GAMETIME,1000,NULL);
	MSG msg;
	while (GetMessage (&msg,NULL,0,0)){
		TranslateAccelerator (hWnd,LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1)),&msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH hBrush;
	RECT rect;
	switch(msg){
	case WM_CREATE:// Tạo lập hInstance
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		for (int x= 0;x<10;x++){
			for(int y = 0;y<22;y++)
			{
				BoardArray[x][y] = false;
			}
		}
		return 0;
	case WM_TIMER:
		if (MainGame.UpdateGame()){
			KillTimer(hWnd,IDT_GAMETIME);
			if (MessageBox (hWnd,TEXT("Tiếp tục ?"),TEXT("Thua rồi"),MB_YESNO|MB_ICONQUESTION) == IDYES){
				MainGame.InitGame();
				MainGame.SetDifficulty(Diff);
				MainGame.ResumeGame();
				BlankArray(BoardArray);
				BlankArray(BoardArray);
				SetTimer(hWnd,IDT_GAMETIME,1000/Diff,NULL);
				return 0;
			}
			else{
				PostQuitMessage(0);
				return 0;
			}
		}
		UpdateGraphic(hWnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam){
		case VK_UP:
			MainGame.Rotate();
			UpdateGraphic(hWnd);
			return 0;
		case VK_LEFT:
			MainGame.MoveLeft();
			UpdateGraphic(hWnd);
			return 0;
		case VK_RIGHT:
			MainGame.MoveRight();
			UpdateGraphic(hWnd);
			return 0;
		case VK_DOWN:
			if (MainGame.MoveDown()){
				UpdateGraphic(hWnd);
				KillTimer(hWnd,IDT_GAMETIME);
				if (MessageBox (hWnd,TEXT("Tiếp tục ?"),TEXT("Thua rồi"),MB_YESNO|MB_ICONQUESTION) == IDYES){
					MainGame.InitGame();
					MainGame.SetDifficulty(Diff);
					MainGame.ResumeGame();
					BlankArray(BoardArray);
					SetTimer(hWnd,IDT_GAMETIME,1000/Diff,NULL);
					return 0;
				}
				else
				{
					PostQuitMessage(0);
					return 0;
				}
			}
			UpdateGraphic(hWnd);
			return 0;
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case ID_GAME_NEWGAME:
		case ID_NEWGAME:
			MainGame.InitGame();
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			UpdateGraphic(hWnd);
			return 0;
		case ID_PAUSEGAME:
		case ID_GAME_PAUSEGAME:
			MainGame.PauseGame();
			return 0;
		case ID_RESUMEGAME:
		case ID_GAME_RESUMEGAME:
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			return 0;
		case ID_GAME_EXITGAME:
			MainGame.PauseGame();
			if (MessageBox (hWnd,TEXT("Are you sure ?"),TEXT("Quit?"),MB_YESNO)==IDYES)
			{
				PostQuitMessage(0);
				return 0;
			}
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			return 0;
		case ID_TOOL_OPTION:
		case ID_OPTION:
			MainGame.PauseGame();
			DialogBox (hInstance,MAKEINTRESOURCE (IDD_OPTION),hWnd,OptionProc);
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			return 0;
		case ID_TOOL_RULE:
			MainGame.PauseGame();
			MessageBox (hWnd,TEXT("Di chuyển hoặc xoay các khối gạch để xếp thành hàng.\nNếu hàng trên cùng có gạch là thua."),TEXT("Rule"),MB_OK);
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			return 0;
		case ID_ABOUT_ABOUT:
			MainGame.PauseGame();
			DialogBox (hInstance,MAKEINTRESOURCE(IDD_ABOUT),hWnd,AboutProc);
			MainGame.ResumeGame();
			BlankArray(BoardArray);
			return 0;
		}
	case WM_PAINT:
		BeginPaint(hWnd,&ps);
		hdc = GetDC(hWnd);// Lấy DC của Window
		//1 ô sẽ được vẽ từ điểm chẵn 20 đến điểm 20+15*n-1
		//ô tiếp theo sẽ được vẽ từ 20+15*n đến 20+15*(n+1)-1
		MoveToEx(hdc,19,19,NULL);//Trái trên
		LineTo(hdc,170,19);//Phải trên
		LineTo(hdc,170,290);//Phải dưới
		LineTo(hdc,19,290);//Trái dưới
		LineTo(hdc,19,19);//Trái trên
		MoveToEx(hdc,200,160,NULL);//Trái trên của khung next
		//Ô thứ n được vẽ từ tọa độ x là 201+n*15 đến 200+(n+1)*15
		//Ô thứ m được vẽ từ tọa độ y là 161+n*15 đến 160+(n+1)*15
		LineTo(hdc,261,160);//Phải trên, cộng 1 pixel cuối
		LineTo(hdc,261,220);//Phải dưới, cộng 1 pixel cuối
		LineTo(hdc,200,220);//Trái dưới
		LineTo(hdc,200,160);//Trái trên
		SetBkMode(hdc,TRANSPARENT);
		TextOut (hdc,200,19,TEXT("Score : "),strlen("Score : "));
		TextOut (hdc,200,70,TEXT("Difficulty : "),strlen("Difficulty : "));
		TextOut (hdc,200,130,TEXT("Next : "),strlen("Next : "));
		rect.top = 20;rect.left = 20;rect.right = 170;rect.bottom = 290;
		hBrush = CreateSolidBrush(RGB(0,0,0));
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);
		ReleaseDC(hWnd,hdc);
		BlankArray(BoardArray);
		EndPaint(hWnd,&ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}
	return DefWindowProc (hWnd,msg,wParam,lParam);
}
void UpdateGraphic(HWND hwnd)
{
	for (int x = 0;x<10;x++)
	{
		for (int y = 4;y<22;y++)
		{
			if (!(MainGame.GetBoardCell(x,y) == BoardArray[x][y]))
			{
				BoardArray[x][y] = MainGame.GetBoardCell(x,y);
				switch (BoardArray[x][y])
				{
				case -1:
					DrawCell(hwnd,YELLOW,x,y-4);
					break;
				case 0:
					DrawCell(hwnd,BLACK,x,y-4);
					break;
				case 1:
					DrawCell(hwnd,PURPLE,x,y-4);
					break;
				}
			}
		}
	}
	DrawScore(hwnd,MainGame.GetScore());
	DrawDifficulty(hwnd,MainGame.GetDifficulty());
	HDC hdc = GetDC(hwnd);
	RECT rect;
	rect.left = 201;rect.right = 260;rect.top = 161;rect.bottom = 220;
	HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	FillRect(hdc,&rect,hBrush);
	DeleteObject(hBrush);
	for (int x = 0;x<4;x++)
	{
		for (int y = 0;y<4;y++)
		{
			if (MainGame.GetNext(x,y))
			{
				DrawNext(hwnd,x,y);
			}
		}
	}
}
void DrawCell(HWND hWnd,MauVe color,int x,int y)
{
	HDC hdc = GetDC(hWnd);
	if (color == BLACK)
	{
		for (int cX = x*15+20;cX<x*15+35;cX++)
		{
			for (int cY = y*15+20;cY < y*15+35;cY++)
			{
				SetPixel(hdc,cX,cY,RGB(0,0,0));
			}
		}
		ReleaseDC(hWnd,hdc);
		return;
	}
	if (color == PURPLE)
	{
		for (int cX = x*15+22;cX<x*15+33;cX++)
		{
			for (int cY = y*15+22;cY<y*15+33;cY++)
			{
				SetPixel(hdc,cX,cY,RGB(255,0,255));
			}
		}
		for (int cX = x*15+20;cX<x*15+35;cX++)
		{
			SetPixel(hdc,cX,y*15+20,RGB(255,255,0));
			SetPixel(hdc,cX,y*15+21,RGB(255,255,0));
			SetPixel(hdc,cX,y*15+33,RGB(255,255,0));
			SetPixel(hdc,cX,y*15+34,RGB(255,255,0));
		}
		for (int cY = y*15+22;cY<y*15+33;cY++){
			SetPixel(hdc,x*15+20,cY,RGB(255,255,0));
			SetPixel(hdc,x*15+21,cY,RGB(255,255,0));
			SetPixel(hdc,x*15+33,cY,RGB(255,255,0));
			SetPixel(hdc,x*15+34,cY,RGB(255,255,0));
		}
		SetPixel(hdc,x*15+20,y*15+20,RGB(255,0,255));
		SetPixel(hdc,x*15+21,y*15+21,RGB(255,0,255));
		SetPixel(hdc,x*15+33,y*15+33,RGB(255,0,255));
		SetPixel(hdc,x*15+34,y*15+34,RGB(255,0,255));
		ReleaseDC(hWnd,hdc);
		return;
	}
	if (color == YELLOW){
		for (int cX = x*15+22;cX<x*15+33;cX++){
			for (int cY = y*15+22;cY<y*15+33;cY++)
			{
				SetPixel(hdc,cX,cY,RGB(255,255,0));
			}
		}
		for (int cX = x*15+20;cX<x*15+35;cX++){
			SetPixel(hdc,cX,y*15+20,RGB(255,0,255));
			SetPixel(hdc,cX,y*15+21,RGB(255,0,255));
			SetPixel(hdc,cX,y*15+33,RGB(255,0,255));
			SetPixel(hdc,cX,y*15+34,RGB(255,0,255));
		}
		for (int cY = y*15+22;cY<y*15+33;cY++){
			SetPixel(hdc,x*15+20,cY,RGB(255,0,255));
			SetPixel(hdc,x*15+21,cY,RGB(255,0,255));
			SetPixel(hdc,x*15+33,cY,RGB(255,0,255));
			SetPixel(hdc,x*15+34,cY,RGB(255,0,255));
		}
		SetPixel(hdc,x*15+20,y*15+20,RGB(255,255,0));
		SetPixel(hdc,x*15+21,y*15+21,RGB(255,255,0));
		SetPixel(hdc,x*15+33,y*15+33,RGB(255,255,0));
		SetPixel(hdc,x*15+34,y*15+34,RGB(255,255,0));
	}
	ReleaseDC(hWnd,hdc);
}
void DrawNext(HWND hWnd,int x,int y){
	HDC hdc = GetDC(hWnd);
	for (int cX = 201+x*15;cX<=215+15*x;cX++)
	{
		for (int cY = 161+y*15;cY<=175+15*y;cY++)
		{
			SetPixel(hdc,cX,cY,RGB(0,0,0));
		}
	}
	ReleaseDC(hWnd,hdc);
}
void DrawScore(HWND hWnd,int Score){
	HDC hdc = GetDC(hWnd);
	RECT rect;
	TCHAR temp[1024];
	rect.left = 200;rect.right = 280;rect.top = 40;rect.bottom = 60;
	HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	FillRect(hdc,&rect,hBrush);
	DeleteObject(hBrush);
	SetBkMode(hdc,TRANSPARENT);
	wsprintf (temp,TEXT("%d"),MainGame.GetScore());
	TextOut (hdc,200,40,temp,wcslen(temp));
	ReleaseDC(hWnd,hdc);
}
void DrawDifficulty(HWND hWnd,int Diff){
	HDC hdc = GetDC(hWnd);
	RECT rect;
	TCHAR temp[1024];
	rect.left = 200;rect.top = 90;rect.right = 280;rect.bottom = 120;
	HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	FillRect(hdc,&rect,hBrush);
	DeleteObject(hBrush);
	wsprintf (temp,TEXT("%d"),MainGame.GetDifficulty());
	SetBkMode(hdc,TRANSPARENT);
	TextOut (hdc,200,90,temp,wcslen(temp));
	ReleaseDC(hWnd,hdc);
}
BOOL CALLBACK OptionProc(HWND hOption,UINT msg,WPARAM wParam,LPARAM lParam){
	TCHAR temp[5];
	switch (msg){
	case WM_INITDIALOG:
		wsprintf (temp,TEXT("%d"),Diff);
		SetWindowText (GetDlgItem(hOption,IDC_DIFFVALUE),temp);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_ADDDIF:
			Diff = min(Diff+1,15);
			wsprintf (temp,TEXT("%d"),Diff);
			SetWindowText (GetDlgItem(hOption,IDC_DIFFVALUE),temp);
			return 0;
		case IDC_DECDIF:
			Diff = max (Diff-1,1);
			wsprintf (temp,TEXT("%d"),Diff);
			SetWindowText (GetDlgItem(hOption,IDC_DIFFVALUE),temp);
			return 0;
		case IDOK:
			KillTimer(GetParent(hOption),IDT_GAMETIME);
			SetTimer(GetParent(hOption),IDT_GAMETIME,1000/Diff,NULL);
			MainGame.SetDifficulty(Diff);
			EndDialog(hOption,true);
			return 0;
		case IDCANCEL:
			Diff = MainGame.GetDifficulty();
			EndDialog(hOption,false);
			return 0;
		}
		return 0;
	}
	return 0;
}
BOOL CALLBACK AboutProc(HWND hAbout,UINT msg,WPARAM wParam,LPARAM lParam){
	if (msg == WM_COMMAND && LOWORD(wParam) == IDOK)
	{
		EndDialog(hAbout,true);
		return 0;
	}
	return 0;
}
void BlankArray(int board[10][22]){
	for (int x = 0;x<10;x++)
	{
		for (int y = 0;y<22;y++)
		{
			board[x][y] = 2;
		}
	}
}