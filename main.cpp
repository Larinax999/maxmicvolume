#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>

#pragma comment(lib, "ole32.lib")

class AudioController {
private:
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pEndpointVolume = nullptr;

public:
    bool initialize() {
        HRESULT hr = CoInitialize(nullptr);
        if (FAILED(hr)) return false;

        // Create device enumerator
        hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            (void**)&pEnumerator
        );
        if (FAILED(hr)) return false;

        // Get default recording endpoint
        hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
        if (FAILED(hr)) return false;

        // Get endpoint volume interface
        hr = pDevice->Activate(
            __uuidof(IAudioEndpointVolume),
            CLSCTX_ALL,
            nullptr,
            (void**)&pEndpointVolume
        );
        if (FAILED(hr)) return false;

        return true;
    }

    bool setMaxVolume() {
        if (pEndpointVolume) {
            // 1.0f represents maximum volume (0.0f would be minimum)
            HRESULT hr = pEndpointVolume->SetMasterVolumeLevelScalar(1.0f, nullptr);
            return SUCCEEDED(hr);
        }
        return false;
    }

    void cleanup() {
        if (pEndpointVolume) {
            pEndpointVolume->Release();
            pEndpointVolume = nullptr;
        }
        if (pDevice) {
            pDevice->Release();
            pDevice = nullptr;
        }
        if (pEnumerator) {
            pEnumerator->Release();
            pEnumerator = nullptr;
        }
        CoUninitialize();
    }

    ~AudioController() {
        cleanup();
    }
};

int main() {
    AudioController controller;
    if (!controller.initialize()) {
        std::cerr << "Failed to initialize audio controller\n";
        Sleep(5000);
        return 1;
    }
    FreeConsole();
    while (true) {
        if (!controller.setMaxVolume()) controller.initialize();
        Sleep(500); // 500 milliseconds
    }
    return 0;
}