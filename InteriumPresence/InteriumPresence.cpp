#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>
#include <thread>
#include <csignal>
#include <string>
#include <wininet.h>

#include "Discord/discord.h"

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

HINSTANCE fileReader = 0;

typedef discord::Activity (*f_readactivity)();
typedef float (*f_readuptime)();

struct DiscordState {
    discord::User currentUser;

    std::unique_ptr<discord::Core> core;
};

discord::Core* core{};
DiscordState state{};

DWORD getActivityPointer = NULL;
DWORD getUptimePointer = NULL;

float oldUptime = 0;

bool interrupted = false;

std::chrono::time_point lastSwitch = std::chrono::system_clock::now();

void updateTask()
{
    std::string oldState;
    while (true)
    {
        if (getActivityPointer != NULL) {
            auto readUptime = (f_readuptime)getUptimePointer;
            auto uptime = readUptime();

            if (uptime != oldUptime) {
                auto readActivity = (f_readactivity)getActivityPointer;
                auto activity = readActivity();

                if (oldState != activity.GetState())
                    lastSwitch = std::chrono::system_clock::now();

                //Get Timestamp
                auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(lastSwitch);
                auto epoch = now_ms.time_since_epoch();
                activity.GetTimestamps().SetStart(epoch.count());

                core->ActivityManager().UpdateActivity(activity, NULL);

                oldUptime = uptime;
                oldState = (char*)activity.GetState();
            }
            else {
                discord::Activity activity{};
                activity.SetDetails("Loading...");
                activity.GetAssets().SetLargeImage("logo");
                activity.SetType(discord::ActivityType::Playing);

                if (oldState != activity.GetState())
                    lastSwitch = std::chrono::system_clock::now();

                core->ActivityManager().UpdateActivity(activity, NULL);

                oldState = (char*)activity.GetState();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

int main()
{
    SetEnvironmentVariable(L"DISCORD_INSTANCE_ID", L"0");
    auto result = discord::Core::Create(859925359738880030, DiscordCreateFlags_Default, &core);
    state.core.reset(core);
    if (!state.core) {
        std::cout << "Failed to instantiate discord core! (err " << static_cast<int>(result)
            << ")\n";
        system("pause");
        std::exit(-1);
    }

    //Download the file reader to the latest version
    #ifndef DEBUG
        std::cout << "Downloading the file reader..." << std::endl;
        DeleteUrlCacheEntry(L"https://kibbewater.com/interium/assets/FileReader.dll");
        URLDownloadToFile(NULL, L"https://kibbewater.com/interium/assets/FileReader.dll", L"FileReader.dll", NULL, NULL);
    #endif

    //Load the dynamic file reader to read the .dat file. Able to dynamically change the way the files are read
    std::cout << "Loading FileReader.dll" << std::endl;
    fileReader = LoadLibraryA("FileReader.dll");

    if (!fileReader) {
        std::cout << "Unable to load the DLL library" << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    f_readactivity readActivity = (f_readactivity)GetProcAddress(fileReader, "readActivity");
    getActivityPointer = (DWORD)readActivity;
    if (!readActivity) {
        std::cout << "Could not mount readActivity" << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    f_readuptime readUptime = (f_readuptime)GetProcAddress(fileReader, "readUptime");
    getUptimePointer = (DWORD)readUptime;
    if (!readUptime) {
        std::cout << "Could not mount readUptime" << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    std::cout << "Sucessfully started" << std::endl;

    std::thread update(updateTask);
    
    while (!interrupted) {
        state.core->RunCallbacks();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    FreeLibrary(fileReader);
    return 0;
}
