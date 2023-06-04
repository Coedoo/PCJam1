
enum AnimationDir {
    Horizontal,
    Vertical,
};

struct Sprite {
    Texture texture;
    Rectangle textureRect;
    Rectangle currentTexRect;

    // animation
    int animationFrames;
    float frameTime;
    AnimationDir animDir;
};


Sprite CreateSprite(Texture texture) {
    return Sprite{
        texture,
        {0, 0, (float) texture.width, (float) texture.height},
        {0, 0, (float) texture.width, (float) texture.height},
    };
}

Sprite CreateSprite(Texture texture, Rectangle rect) {
    return Sprite{
        texture, rect, rect
    };
}

void UpdateAnimFrame(Sprite* sprite, float time) {
    Vector2 dir = sprite->animDir == Horizontal ? Vector2{1, 0} : Vector2{0, 1};

    int n = (int)(time / sprite->frameTime) % sprite->animationFrames;
    Rectangle r = sprite->textureRect;
    r.x += dir.x * sprite->textureRect.width * n;
    r.y += dir.y * sprite->textureRect.height * n;

    sprite->currentTexRect = r;
}