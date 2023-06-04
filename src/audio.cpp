enum Sounds {
    Scream0,
    Scream1,
    Scream2,
    Shot1,
    Dead1,
    SoundsCount,
};

enum Musics { 
    MenuMusic,
    GameMusic,
    MusicCount,
};

Sound audioLib[SoundsCount];
Music musicLib[MusicCount];


void LoadAudioLib() {
    audioLib[Scream0] = LoadSound("assets/scream0.wav");
    audioLib[Scream1] = LoadSound("assets/scream1.wav");
    audioLib[Scream2] = LoadSound("assets/scream2.wav");
    audioLib[Shot1] = LoadSound("assets/shot1.mp3");
    audioLib[Dead1] = LoadSound("assets/dead1.mp3");

    musicLib[MenuMusic] = LoadMusicStream("assets/menumusic.mp3");
    musicLib[GameMusic] = LoadMusicStream("assets/gamemusic.mp3");
    musicLib[GameMusic].looping = true;
}