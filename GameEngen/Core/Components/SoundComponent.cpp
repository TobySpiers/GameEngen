#include "Components/SoundComponent.h"

#include "Audio/SoundManager.h"
#include "GameObject.h"

SoundComponent::SoundComponent()
{
    SetTickEnabled(false);
    SetDrawEnabled(false);
}

void SoundComponent::PlaySound(SoundId id)
{
    SoundManager::Get().PlayAt(id, GetOwner()->transform.position);
}

void SoundComponent::PlaySound(const std::string& path)
{
    SoundManager::Get().Play(SoundManager::Get().LoadSound(path));
}

void SoundComponent::PlayMusic(MusicId id, bool bLoop)
{
    SoundManager::Get().PlayMusic(id, bLoop);
}

void SoundComponent::PlayMusic(const std::string& path, bool bLoop)
{
    SoundManager::Get().PlayMusic(SoundManager::Get().LoadMusic(path), bLoop);
}

void SoundComponent::StopMusic(MusicId id)
{
    SoundManager::Get().StopMusic(id);
}
