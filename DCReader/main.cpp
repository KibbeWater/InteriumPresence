#include "main.h"

using namespace std;

#pragma warning(disable : 4996)

extern "C" {
    DLLExport discord::Activity readActivity() {
        const string appdataPath = getenv("appdata") + string("\\INTERIUM\\CSGO\\FilesForLUA\\kibbewater\\RichPresence\\presence.dat");

        PresenceData pData{};

        fstream activityFile;
        activityFile.open(appdataPath);
        activityFile.read(pData.state, 128);
        activityFile.read(pData.details, 128);
        activityFile.read(pData.largeText, 128);
        activityFile.read(reinterpret_cast<char*>(&pData.uptime), sizeof(float));
        activityFile >> pData.partySize >> pData.partyMax >> pData.hasParty;

        discord::Activity activity{};
        activity.SetDetails(pData.details);
        activity.SetState(pData.state);
        activity.GetAssets().SetLargeImage("logo");
        activity.GetAssets().SetLargeText(pData.largeText);
        activity.SetType(discord::ActivityType::Playing);

        return activity;
    }

    DLLExport float readUptime() {
        string appdataPath = getenv("appdata");
        appdataPath += "\\INTERIUM\\CSGO\\FilesForLUA\\kibbewater\\RichPresence\\presence.dat";

        PresenceData pData{};

        fstream activityFile;
        activityFile.open(appdataPath);
        activityFile.read(pData.state, 128);
        activityFile.read(pData.details, 128);
        activityFile.read(pData.largeText, 128);
        activityFile.read(reinterpret_cast<char*>(&pData.uptime), sizeof(float));
        activityFile >> pData.partySize >> pData.partyMax >> pData.hasParty;

        return pData.uptime;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "Sucessfully loaded the file reader" << std::endl;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

