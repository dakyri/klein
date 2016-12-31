#include "Klein.h"
#ifdef MOCK
class AEffect {

};
typedef int(*audioMasterCallback)(int i1, int i2, int i3, int i4, int i5, int i6);
class AudioEffect {
public:
	AEffect* getAeffect() { return nullptr; }
};
class Klein : public AudioEffect {
public:
	Klein(audioMasterCallback) {}
};

int audioMasterVersion;
int audioMaster() { return 0;  }
#endif
bool oome = false;

#if MAC
#pragma export on
#endif

// prototype of the export function main
#if BEOS
#define main main_plugin
extern "C" __declspec(dllexport) AEffect *main_plugin (audioMasterCallback audioMaster);

#else
AEffect *VSTPluginMain (audioMasterCallback audioMaster);
#endif

AEffect *VSTPluginMain(audioMasterCallback audioMaster)
{
	// get vst version
	if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
		return 0;  // old version

	AudioEffect* effect = new Klein (audioMaster);
	if (!effect)
		return 0;
	if (oome)
	{
		delete effect;
		return 0;
	}
	return effect->getAeffect ();
}

#if MAC
#pragma export off
#endif

#if WIN32

#include <windows.h>

void* hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
#endif

