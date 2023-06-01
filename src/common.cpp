Vector3 operator+(Vector3 a, Vector3 b) {
    return Vector3Add(a, b);
}

Vector3 operator-(Vector3 a, Vector3 b) {
    return Vector3Subtract(a, b);
}

Vector3 operator*(Vector3 v, float s) {
    return {v.x * s, v.y * s, v.z * s};
}

Vector3 operator/(Vector3 v, float s) {
    return {v.x / s, v.y / s, v.z / s};
}

Vector2 operator+(Vector2 a, Vector2 b) {
    return Vector2Add(a, b);
}

Vector2 operator-(Vector2 a, Vector2 b) {
    return Vector2Subtract(a, b);
}

Vector2 operator*(Vector2 v, float s) {
    return {v.x * s, v.y * s};
}

Vector2 operator/(Vector2 v, float s) {
    return {v.x / s, v.y / s};
}

Matrix operator*(Matrix a, Matrix b) {
    return MatrixMultiply(a, b);
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