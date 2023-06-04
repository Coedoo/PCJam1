
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

void UpdateTitleScreen(TitleScreen* title) {
    title->time += GetFrameTime();
    UpdateMusicStream(title->music);

    if(IsKeyPressed(KEY_SPACE)) {
        if(title->time < titleAnimationTime) {
            title->time = titleAnimationTime;
        }
        else {
            GoToGame();
        }
    }

}

void DrawTextCentered(char* text, Vector2 position, float fontSize, float spacing);
void DrawTitleScreen(TitleScreen* title) {
    float p = title->time / titleAnimationTime;

    p = min(1.0f, p);
    Vector2 startPos = {windowWidth / 2.0f - title->logo.width / 2.0f, (float) -title->logo.height};
    Vector2 endPos   = {windowWidth / 2.0f - title->logo.width / 2.0f, windowHeight / 4.0f};

    Vector2 pos = Vector2Lerp(startPos, endPos, p);
    DrawTextureV(title->logo, pos, WHITE);

    if(title->time > titleAnimationTime) {
        char* text = "Controls:\nARROWS - move\nSPACE - shoot\nPress SPACEBAR to play";

        pos = {windowWidth / 2.0f, windowHeight / 2.0f + 230};
        DrawTextCentered(text, pos, 35, 2);
    }
}