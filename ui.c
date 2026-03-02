#include <windows.h>
#include <unistd.h>
#include "Chloe.h"
#define ID_EDITCHILD 100
HWND hwndInput;
HWND hwndOutput;
WNDPROC OldEditProc;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow){
    const wchar_t CLASS_NAME[] = L"CHLOE WINDOW CLASS";

    WNDCLASSW wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0,CLASS_NAME,L"Chloe UI",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
        return 0;
    
    ShowWindow(hwnd,nCmdShow);

    MSG msg = { };
    while(GetMessage(&msg,NULL,0,0)>0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void append_txt(HWND hwndEdit, const wchar_t *text){
    SendMessageW(hwndEdit, EM_SETSEL, -1, -1);
    SendMessageW(hwndEdit, EM_REPLACESEL, 0, (LPARAM) text);
}

LRESULT CALLBACK InpProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_CHAR && wparam == '\r'){
        wchar_t buff[1024];
        GetWindowTextW(hwnd, buff, 1024);
        SetWindowTextW(hwnd, L"");
        
        // Chloe conn 
        //wchar_t *op[1024] = call_chloe(buff,op);
        
        
        int len = GetWindowTextLengthW(hwndOutput);
        SendMessageW(hwndOutput, EM_SETSEL, len, len);
        append_txt(hwndOutput,L"(づ￣ ³￣)づ ");
        SendMessageW(hwndOutput, EM_REPLACESEL, 0, (LPARAM)buff); //change buff to op after chloe conn
        SendMessageW(hwndOutput, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
        SendMessageW(hwndOutput, EM_SCROLLCARET, 0, 0);
        return 0;
    }
    return CallWindowProc(OldEditProc, hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
    switch (uMsg)
    {
        case WM_CREATE:
            hwndOutput = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_READONLY, 0,0,0,0, hwnd, (HMENU) 1, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            append_txt(hwndOutput,L"chloe strt .. \r\n");
            hwndInput = CreateWindowEx(0, L"EDIT",L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, 0,0,0,0, hwnd, (HMENU) 2, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
            OldEditProc = (WNDPROC)SetWindowLongPtr(hwndInput, GWLP_WNDPROC, (LONG_PTR)InpProc);
            SetFocus(hwndInput);
            return 0;

        case WM_SIZE: 
            int wid = LOWORD(lParam);
            int hei = HIWORD(lParam);

            int inpHei = 30;

            MoveWindow(hwndOutput, 0, 0, wid, hei - inpHei, TRUE);
            MoveWindow(hwndInput, 0, hei - inpHei, wid, inpHei, TRUE);
            return 0;
        
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
                EndPaint(hwnd,&ps);
            }
            return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        
    }
    return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

