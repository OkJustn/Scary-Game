#ifndef AUDIO_H
#define AUDIO_H

#include "types.h"

// Holds all game audio handles
typedef struct AudioAssets {
    Music ambiance;
    Sound jumpscareSound;
    Sound levelUpSound;
    Sound monsterNear;
} AudioAssets;

// Loads all audio files from the audio/ folder
AudioAssets LoadAudioAssets(void);

// Unloads all audio handles and closes the audio device
void UnloadAudioAssets(AudioAssets *a);

// Triggers game-over audio: stops ambiance, plays jumpscare sting
void PlayGameOverAudio(AudioAssets *a);

// Plays the level-up chime
void PlayLevelUpAudio(AudioAssets *a);

#endif // AUDIO_H
