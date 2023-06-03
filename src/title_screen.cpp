
struct TitleScreen {
    Texture2D logo;
    Music music;

    // state
    bool isPlaying;
    float time;
};

TitleScreen CreateTitleScreen() {
    TitleScreen ret = {};

    ret.logo = LoadTexture("assets/logo.png");
    ret.music = LoadMusicStream("assets/menumusic.mp3");

    return ret;
}

void StartTitleScreenAnim(TitleScreen* title) {
    title->time = 0;
    title->isPlaying = true;

    if(IsMusicStreamPlaying(title->music)) {
        StopMusicStream(title->music);
    }

    PlayMusicStream(title->music);
}

void UpdateAndDrawTitleScreen(TitleScreen* title) {
    title->time += GetFrameTime();

    float p = title->time / titleAnimationTime;

    p = min(1.0f, p);
    Vector2 startPos = {windowWidth / 2.0f - title->logo.width / 2.0f, -title->logo.height};
    Vector2 endPos   = {windowWidth / 2.0f - title->logo.width / 2.0f, windowHeight / 4};
    
    Vector2 pos = Vector2Lerp(startPos, endPos, p);

    DrawTextureV(title->logo, pos, WHITE);

    UpdateMusicStream(title->music);

}