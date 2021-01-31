struct rgb {
    double r;  // a fraction between 0 and 1
    double g;  // a fraction between 0 and 1
    double b;  // a fraction between 0 and 1
};

struct hsv {
    double h;  // angle in degrees
    double s;  // a fraction between 0 and 1
    double v;  // a fraction between 0 and 1
};

rgb hsv2rgb(hsv in);