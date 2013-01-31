#include <windows.h>
#include "resource.h"


#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "comdlg32.lib")


HINSTANCE g_hInst;
HWND g_hwndDlg;
static OPENFILENAME ofn;

static TCHAR szFilter[] = TEXT ("Driver Files (*.SYS)\0*.sys\0")
                          TEXT ("All Files (*.*)\0*.*\0\0");

static TCHAR szFileName[MAX_PATH]; 
static TCHAR szTitleName [MAX_PATH];


void SetWindowIcon(HWND hWnd, HICON hIcon)
{
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (WPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG,(WPARAM)hIcon);
}


void CentreDialog(HWND hwndDlg)
{
	RECT rc,rcDlg,rcOwner;
	HWND hwndOwner;

	hwndOwner = GetDesktopWindow();

	GetWindowRect(hwndDlg, &rcDlg);
	GetWindowRect(hwndOwner, &rcOwner);
	CopyRect(&rc, &rcOwner);

	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	SetWindowPos(hwndDlg,
				 HWND_TOP, 
				 rcOwner.left + rc.right / 2, 
				 rcOwner.top + rc.bottom / 2, 
				 rcDlg.right, 
				 rcDlg.bottom, 
				 SWP_NOSIZE);

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwndDlg ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.lpstrFile         = szFileName ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = szTitleName ;
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;   
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("sys") ;
	ofn.lCustData         = 0 ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;

}
int line = 0;

INT_PTR CALLBACK DlgProc(HWND hwndDlg,
	 					 UINT uMsg, 
						 WPARAM wParam, 
						 LPARAM lParam)
{
	HDC hdc;
	HICON hIcon;
	char msg[MAX_PATH];
	switch (uMsg) {
		case WM_INITDIALOG:
			CentreDialog(hwndDlg);

			hIcon = LoadIcon(g_hInst, (LPCTSTR)IDI_ICON);
			SetWindowIcon(hwndDlg, hIcon);

			break;

		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_INSTALL:
					break;
				case IDC_START:
					break;
				case IDC_STOP:
					break;
				case IDC_REMOVE:
					break;
				case IDC_SRVNAME:
					//hdc = GetDC(NULL);
					//line += 20;
					//sprintf(msg, "%10d %10d ", wParam, lParam);
					//TextOutA(hdc, 0, line, msg, strlen(msg));
					//ReleaseDC(hdc, NULL);
					break;
				case IDC_CLOSE:
					EndDialog(hwndDlg, 0);
					break;
			}
			break;
		case WM_LBUTTONDBLCLK:
               ofn.Flags = 0 ;
               if (!GetOpenFileName(&ofn))
                    return FALSE;
			   SetDlgItemText(hwndDlg, IDC_SRVNAME, szFileName);
				
			break;

		default:
			return FALSE;
	}

		
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, 
				   int nCmdShow)
{
	g_hInst = GetModuleHandle(NULL);
	
	DialogBoxParam(g_hInst,
				   MAKEINTRESOURCE(IDD_INSTDRV),
				   NULL,
		   		   (DLGPROC)DlgProc,
				   0);

	return 0;
}
