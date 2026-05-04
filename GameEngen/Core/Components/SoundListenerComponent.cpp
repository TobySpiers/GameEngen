#include "Components/SoundListenerComponent.h"

#include "GameObject.h"
#include "Log.h"

#include <AL/al.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SoundListenerComponent* SoundListenerComponent::s_activeListener = nullptr;

SoundListenerComponent::SoundListenerComponent()
{
    SetDrawEnabled(false);
}

void SoundListenerComponent::OnSpawn()
{
    Ensure(!s_activeListener || s_activeListener == this, "SoundListenerComponent: only one listener may be active at a time");
    s_activeListener = this;
    ApplyToAL();
}

void SoundListenerComponent::OnTick(float deltaTime)
{
    ApplyToAL();
}

void SoundListenerComponent::OnDestroy()
{
    if (s_activeListener == this)
    {
        s_activeListener = nullptr;

        // Reset AL listener to identity defaults
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        float defaultOrientation[6] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        alListenerfv(AL_ORIENTATION, defaultOrientation);
    }
}

void SoundListenerComponent::ApplyToAL()
{
    const glm::vec3& pos = GetOwner()->transform.position;
    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    // Build a rotation matrix from Euler angles (degrees), applied Y → X → Z
    const glm::vec3& rot = GetOwner()->transform.rotation;
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f))
                     * glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f))
                     * glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // OpenAL default forward is (0, 0, -1) and up is (0, 1, 0)
    glm::vec3 forward = glm::vec3(rotMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    glm::vec3 up      = glm::vec3(rotMat * glm::vec4(0.0f, 1.0f,  0.0f, 0.0f));

    float orientation[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, orientation);
}
