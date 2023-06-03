
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


///////////////

Vector2 GetMovementInput() {
    Vector2 input = {};

    if(IsKeyDown(KEY_LEFT))       input.x = -1;
    else if(IsKeyDown(KEY_RIGHT)) input.x =  1;

    if(IsKeyDown(KEY_UP))        input.y =  1;
    else if(IsKeyDown(KEY_DOWN)) input.y = -1;

    return input;
}
