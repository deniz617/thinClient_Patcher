#include "main.h"
#include "process_handle.h"
#include "pattern_search.h"
#include "gui.h"
#include <string>
#include <fstream>


static int debugLv = 0;
DWORD patchpoint = 0;
//#define DEBUG_LV1

std::string hex_to_string(DWORD hex)
{
	std::string str = "0x";
	char hexbuffer[100]; 

	sprintf_s(hexbuffer, "%X", hex);
	str += hexbuffer;

	return str;
}

void print_text(std::string str)
{
	if(debugLv > 0)
		std::cout << str << std::endl;
}

bool findPatchpoint(int procid, std::string procname, HANDLE handle)
{
	DWORD basemod = getProcessModuleAddr(procid, procname);
	if (basemod != 0)
	{
		//  003BA6E1               | 74 1D                  | je thinclient.3BA700                                  | not taken on bring front | taken on start game
		//51 8B 4D E0 56 FF D0 84 C0 74 1D 8B 4D E0 8D 55 B4 52
		BYTE pattern[] = { 0x51, 0x8B, 0x4D, 0x90, 0x56, 0xFF, 0xD0, 0x84, 0xC0, 0x74, 0x1D, 0x8B, 0x4D, 0x90, 0x8D, 0x55, 0x90, 0x52 };
		const char* sign = "xxx?xxxxxxxxx?xx?x";
		patchpoint = findPatternEx(handle, pattern, sign, basemod);

		if (patchpoint != 0)
			return true;
	}
	return false;
}

bool MulticlientPatch(int procid, std::string procname, HANDLE handle)
{
	bool res = false;
	if (handle != 0)
	{
		if (patchpoint != 0)
		{
			patchpoint += 0x9;

#ifdef DEBUG_LV1
			std::string resline = "Patch Point: " + hex_to_string(patchpoint);
			print_text(resline);
#endif

			//If correct (0x74) patch to => 0xEB
			if (getMemByte(handle, patchpoint) == 0x74)
			{
				if (setMemByte(handle, patchpoint, 0xEB))
				{
#ifdef DEBUG_LV1
					print_text("Patched successfully.");
#endif						
					res = true;
				}
				else
				{
#ifdef DEBUG_LV1
					print_text("Patch error on re-write.");
#endif
				}
			}
		}
#ifdef DEBUG_LV1
		else
			print_text("Failed to get base module.");
#endif
	}
	return res;
}

void GUI()
{
	HANDLE handle;
	int procid = 0;
	std::string procname = "thinClient.exe";

	nana::form fm;
	fm.caption("NT_thinClient Patcher");
	fm.size(nana::size(400, 120));

	// Disable resize
	nana::API::track_window_size(fm, fm.size(), false);
	nana::API::track_window_size(fm, fm.size(), true);

	nana::label lbl(fm, nana::rectangle(5, 0, 400, 30));
	lbl.caption("Looking for thinClient..");

	nana::button btn(fm, nana::rectangle(150, 40, 100, 20));
	btn.caption("Patch");

	btn.enabled(false);
	btn.events().click([&fm, &lbl, &procid, procname, &handle] {
		if (procid && handle)
		{
			if (MulticlientPatch(procid, procname, handle))
				lbl.caption("Patch applied. You can close this now.");
			else
				lbl.caption("An error occured try re-starting this application.");
		}
		});

	nana::label lbl2(fm, nana::rectangle(120, 80, 200, 15));
	lbl2.caption("Want to support or thank me?");

	nana::button btn2(fm, nana::rectangle(150, 95, 100, 20));
	btn2.caption("Buy me a Coffee");
	btn2.events().click([]() {
		ShellExecute(0, 0, "https://ko-fi.com/blackout", 0, 0, SW_SHOW);
		});

	nana::timer tmr;
	tmr.elapse([&tmr, &handle, &procid, &lbl, &btn, procname]() {
		// Find thinClient
		procid = getProcID(procname);
		if (procid)
		{
			handle = getProcessHandle(procid);
			if (handle)
			{
				tmr.stop();
				lbl.caption("Please wait for a while this window can be unresponsive during this time..");
				if (findPatchpoint(procid, procname, handle))
				{
					lbl.caption("Ready to patch!");
					btn.enabled(true);
				}
				else
				{
					lbl.caption("Something went wrong, maybe was already patched?\nIf not please contact Blackout#9914 (Discord)");
				}
			}
			else
			{
				lbl.caption("Failed, You have to run me as admin!");
			}

		}
	});
	tmr.interval(std::chrono::milliseconds(3000));
	tmr.start();

	fm.show();
	nana::exec();

	// Close handle on exit
	if(handle)
		CloseHandle(handle);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	GUI();
	return 0;
}