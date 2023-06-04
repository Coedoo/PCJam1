
float EaseOutQuart(float x) {
    float omx = 1 - x;
    return 1 - omx * omx * omx * omx;
}