
enum AnimationDir {
    Horizontal,
    Vertical,
};

struct Sprite {
    Texture texture;
    Rectangle textureRect;

    // animation
    int animationFrames;
    float frameTime;
    AnimationDir animDir;
};


Sprite CreateSprite(Texture texture) {
    return Sprite{
        texture,
        {0, 0, (float) texture.width, (float) texture.height},
    };
}

Sprite CreateSprite(Texture texture, Rectangle rect) {
    return Sprite{
        texture, rect
    };
}