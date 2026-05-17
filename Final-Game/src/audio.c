#include "audio.h"

// Return struct instance with all audio assets loaded
AudioAssets LoadAudioAssets(void)
{
    AudioAssets a;

    a.ambiance       = LoadMusicStream("audio/ambiance.mp3");
    a.jumpscareSound = LoadSound("audio/jumpscare.mp3");
    a.levelUpSound   = LoadSound("audio/levelup.mp3");

    SetMusicVolume(a.ambiance,       1.0f);
    SetSoundVolume(a.jumpscareSound, 1.0f);
    SetSoundVolume(a.levelUpSound,   0.8f);

    PlayMusicStream(a.ambiance);

    return a;
}

// Unload assets
void UnloadAudioAssets(AudioAssets *a)
{
    UnloadMusicStream(a->ambiance);
    UnloadSound(a->jumpscareSound);
    UnloadSound(a->levelUpSound);
}

// Game over Audio
void PlayGameOverAudio(AudioAssets *a)
{
    StopMusicStream(a->ambiance);
    PlaySound(a->jumpscareSound);
}

// Next level Audio
void PlayLevelUpAudio(AudioAssets *a)
{
    PlaySound(a->levelUpSound);
}
