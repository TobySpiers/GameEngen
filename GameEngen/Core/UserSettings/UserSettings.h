#pragma once

class UserSettings
{
public:
    virtual ~UserSettings() = default;

    virtual void Save()            {}
    virtual void Load()            {}
    virtual void ResetToDefaults() = 0;
    virtual void LogValues()       const {}
};
