enum Sounds {
    Scream0,
    Scream1,
    Scream2,
    Shot1,
    Dead1,
    SoundsCount,
};

Sound audioLib[SoundsCount];

void LoadAudioLib() {
    audioLib[Scream0] = LoadSound("assets/scream0.wav");
    audioLib[Scream1] = LoadSound("assets/scream1.wav");
    audioLib[Scream2] = LoadSound("assets/scream2.wav");
    audioLib[Shot1] = LoadSound("assets/Shot1.mp3");
    audioLib[Dead1] = LoadSound("assets/dead1.mp3");
}