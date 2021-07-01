#pragma once

struct PresenceData
{
    char state[128];
    char details[128];
    char largeText[128];
    float uptime;
    short partySize;
    short partyMax;
    bool hasParty;
};