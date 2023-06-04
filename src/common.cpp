
RMAPI Vector2 operator+(Vector2 left, Vector2 right) {
    return Vector2Add(left, right);
}

RMAPI Vector3 operator+(Vector3 left, Vector3 right) {
    return Vector3Add(left, right);
}

RMAPI Vector4 operator+(Vector4 left, Vector4 right) {
    return {left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w};
}

RMAPI Vector2& operator+=(Vector2& left, Vector2 right) {
    return left = left + right;
}

RMAPI Vector3& operator+=(Vector3& left, Vector3 right) {
    return left = left + right;
}

RMAPI Vector4& operator+=(Vector4& left, Vector4 right) {
    return left = left + right;
}

RMAPI Vector2 operator-(Vector2 left, Vector2 right) {
    return Vector2Subtract(left, right);
}

RMAPI Vector3 operator-(Vector3 left, Vector3 right) {
    return Vector3Subtract(left, right);
}

RMAPI Vector4 operator-(Vector4 left, Vector4 right) {
    return {left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w};
}

RMAPI Vector2& operator-=(Vector2& left, Vector2 right) {
    return left = left - right;
}

RMAPI Vector3& operator-=(Vector3& left, Vector3 right) {
    return left = left - right;
}

RMAPI Vector4& operator-=(Vector4& left, Vector4 right) {
    return left = left - right;
}

RMAPI Vector2 operator*(Vector2 left, float right) {
    return Vector2Scale(left, right);
}

RMAPI Vector3 operator*(Vector3 left, float right) {
    return Vector3Scale(left, right);
}

RMAPI Vector4 operator*(Vector4 left, float right) {
    return {left.x * right, left.y * right, left.z * right, left.w * right};
}

RMAPI Vector2 operator*(float left, Vector2 right) {
    return right * left;
}

RMAPI Vector3 operator*(float left, Vector3 right) {
    return right * left;
}

RMAPI Vector4 operator*(float left, Vector4 right) {
    return {right.x * left, right.y * left, right.z * left, right.w * left};
}

RMAPI Vector2& operator*=(Vector2& left, float right) {
    return left = left * right;
}

RMAPI Vector3& operator*=(Vector3& left, float right) {
    return left = left * right;
}

RMAPI Vector4& operator*=(Vector4& left, float right) {
    return left = left * right;
}

RMAPI Vector2 operator/(Vector2 left, float right) {
    return {left.x / right, left.y / right};
}

RMAPI Vector3 operator/(Vector3 left, float right) {
    return {left.x / right, left.y / right, left.z / right};
}

RMAPI Vector4 operator/(Vector4 left, float right) {
    return {left.x / right, left.y / right, left.z / right, left.w / right};
}

RMAPI Vector2& operator/=(Vector2& left, float right) {
    return left = left / right;
}

RMAPI Vector3& operator/=(Vector3& left, float right) {
    return left = left / right;
}

RMAPI Vector4& operator/=(Vector4& left, float right) {
    return left = left / right;
}


////////////

float RandomRange(float a, float b) {
    float p = (float) rand() / (float) RAND_MAX;
    return Lerp(a, b, p);
}

int RandomRange(int a, int b) {
    float p = (float) rand() / (float) RAND_MAX;
    return (int) (Lerp((float)a, (float)b, p) + 0.5f);
}

///////

template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
T min(T a, T b) {
    return a < b ? a : b;
}


///////////////

#define ARR_LEN(x) (sizeof(x) / sizeof((x)[0]))

/////////////
float fract(float a) {
    return a - floorf(a);
}

float PingPong(float x, float length) {
    return length - fabsf(fract((x - length) /  (2*length)) - length);
}


Color ColorLerp(Color a, Color b, float t) {
    return {
        (unsigned char)(a.r + (unsigned char)((b.r - a.r) * t)),
        (unsigned char)(a.g + (unsigned char)((b.g - a.g) * t)),
        (unsigned char)(a.b + (unsigned char)((b.b - a.b) * t)),
        (unsigned char)(a.a + (unsigned char)((b.a - a.a) * t)),
    };
}