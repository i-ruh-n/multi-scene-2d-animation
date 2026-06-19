#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
// --- Global Settings ---
const int SCR_WIDTH = 1000;
const int SCR_HEIGHT = 600;
// Scene Manager (1 = Cox's Bazar, 2 = Sajek Valley)
int currentScene = 1;

// --- Scene 1 Variables (Cox's Bazar) ---
const int ROAD_HEIGHT = 100;
const int BUILDING_BASE = 100;
const int BEACH_HEIGHT = 40;
const int SHORE_START = 300;
const int HORIZON_LEVEL = 450;

bool isNight = false;
bool isRaining = false;

// --- Scene 2 Variables ---
bool s2_houseLights = false;
bool house1_Lit = true;
bool house2_Lit = true;
bool house3_Lit = true;
float windmillAngle = 0.0f;
float windmillSpeedScale = 1.0f; // 0.0f = stop, 1.0f = normal, 3.0f = fast
float s1_carSpeedScale = 1.0f;// 0.0f = stop, 1.0f = normal, 2.5f = fast
float waterWaveOffset = 0.0f;

// --- Structures (Blueprints) ---
struct Hotel { float x, w, h, r, g, b; int type; };
struct Car { float x, y, speed, maxSpeed; int lane; float r, g, b, width; int type; };
struct Cloud { float x, y, speed; };
struct RainDrop { float x, y, speed; };
struct Star { float x, y, alpha, glowSpeed; int direction; };
struct Boat { float x, y, speed; };
struct SandDot { float x, y, r, g, b; };

//structures for Scene 2
struct Bird {
    float x, y, speed;
    float scale; // Depth scaling
    float wingAngle; // For flapping
    int wingDir; // Flap direction multiplier
};

struct Tree {
    float x, y, size;
    int type; // 0 = Pine tree, 1 = cloud tree
};

struct Cottage {
    float x, y, width, height;
    bool lightOn; // Toggleable windowlight
};

struct Bush { float x, y, size; };
struct Grass { float x, y, height; };

// --- Data Lists ---
// Scene 1
vector<Hotel> city;
vector<Car> traffic;
vector<Cloud> clouds;
vector<SandDot> sandDots;
vector<Star> stars;
RainDrop rain[300];
Boat myBoat, farBoat;

// Scene 2
vector<Star> s2_stars;
vector<Tree> s2_trees;
vector<Cottage> s2_houses;
vector<Bird> s2_birds;
vector<Car> s2_traffic;
vector<Bush> s2_bushes;
vector<Grass> s2_grass;

// --- Helper Functions ---
void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y); // btm lefy
    glVertex2f(x + w, y);//btm right
    glVertex2f(x + w, y + h);//top right
    glVertex2f(x, y + h);//top left
    glEnd();
}

void drawCircle(float cx, float cy, float r, int segments) { // center xy radius segments (how many vertex)
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.14159f * float(i) / float(segments);
        glVertex2f(r * cosf(theta) + cx, r * sinf(theta) + cy);

    }
    glEnd();
}

void setShadedColor(float r, float g, float b, float dim, float shadeFactor = 1.0f) {
    glColor3f(r * dim * shadeFactor, g * dim * shadeFactor, b * dim * shadeFactor);
}

void drawBeautifulCloud(float x, float y, bool nightMode) {
    if (nightMode) glColor3f(0.5f, 0.5f, 0.6f);
    else glColor3f(1.0f, 1.0f, 1.0f);

    drawCircle(x, y, 30, 20);
    drawCircle(x + 35, y, 35, 20);
    drawCircle(x + 70, y, 30, 20);
    drawCircle(x + 15, y + 25, 25, 20);
    drawCircle(x + 50, y + 30, 28, 20);
}
// --- Scene 1 Drawing Functions ---
void drawModernHotel(Hotel h) {
    float dim = isNight ? 1.0f : 1.0f;
    setShadedColor(h.r, h.g, h.b, dim);
    drawRect(h.x, BUILDING_BASE, h.w, h.h);
    setShadedColor(h.r, h.g, h.b, dim, 0.7f);
    drawRect(h.x + h.w, BUILDING_BASE, 15, h.h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // transparecy alpha value math
    if (isNight) glColor4f(1.0f, 0.95f, 0.8f, 0.7f);
    else glColor4f(0.7f, 0.8f, 1.0f, 0.5f);

    for (float wy = BUILDING_BASE + 10; wy < BUILDING_BASE + h.h - 10; wy += 30) {
        drawRect(h.x + 5, wy, h.w - 10, 25); // left margin wy=curenheight -10 width 25 height
    }
    glDisable(GL_BLEND); // back to solid
}

void drawTraditionalHotel(Hotel h) {
    float dim = isNight ? 0.4f : 1.0f;
    setShadedColor(h.r, h.g, h.b, dim);
    drawRect(h.x, BUILDING_BASE, h.w, h.h);
    setShadedColor(h.r, h.g, h.b, dim, 0.6f);
    drawRect(h.x - 5, BUILDING_BASE + h.h - 15, h.w + 10, 20);

    for (float wy = BUILDING_BASE + 25; wy < BUILDING_BASE + h.h - 30; wy += 35) {
        for (float wx = h.x + 15; wx < h.x + h.w - 20; wx += 30) { // nich  theke upre bam theke dan
            setShadedColor(0.3f, 0.2f, 0.1f, dim);
            drawRect(wx - 3, wy - 3, 18, 24); // border
            if (isNight) glColor3f(1.0f, 0.8f, 0.3f);
            else glColor3f(0.2f, 0.2f, 0.3f);
            drawRect(wx, wy, 12, 18); // glass
        }
    }
}

void drawMotel(Hotel h) {
    float dim = isNight ? 1.0f : 1.0f;
    float floorH = h.h / 2.0f;

    setShadedColor(h.r, h.g, h.b, dim);
    drawRect(h.x, BUILDING_BASE, h.w, h.h);

    for (float wx = h.x + 15; wx < h.x + h.w - 30; wx += 50) { //new room +=50
        setShadedColor(0.4f, 0.2f, 0.1f, dim);
        drawRect(wx, BUILDING_BASE, 14, 28);
        if (isNight) glColor3f(1.0f, 0.9f, 0.5f);
        else glColor3f(0.3f, 0.3f, 0.5f);
        drawRect(wx + 18, BUILDING_BASE + 8, 20, 16); //window
    }

    setShadedColor(0.1f, 0.1f, 0.1f, dim, 0.5f);
    drawRect(h.x, BUILDING_BASE + floorH - 5, h.w, 5); //middle floor line
    setShadedColor(h.r, h.g, h.b, dim, 0.8f);
    drawRect(h.x - 5, BUILDING_BASE + floorH, h.w + 10, 5); // upper floor balcony

    for (float wx = h.x + 15; wx < h.x + h.w - 30; wx += 50) { // 2nd floor
        setShadedColor(0.4f, 0.2f, 0.1f, dim);
        drawRect(wx, BUILDING_BASE + floorH + 5, 14, 28); // door
        if (isNight) glColor3f(1.0f, 0.9f, 0.5f);
        else glColor3f(0.3f, 0.3f, 0.5f);
        drawRect(wx + 18, BUILDING_BASE + floorH + 13, 20, 16); //window
    }

    glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
    drawRect(h.x - 5, BUILDING_BASE + floorH + 25, h.w + 10, 3); // railing
    for (float wx = h.x; wx <= h.x + h.w; wx += 15) {
        drawRect(wx, BUILDING_BASE + floorH + 5, 2, 20); // rail bar
    }
}

void drawHotel(Hotel h) {
    if (h.type == 1) drawModernHotel(h);
    else if (h.type == 2) drawTraditionalHotel(h);
    else drawMotel(h);
}

void drawShip(Boat b, float scale) {
    float dim = isNight ? 0.6f : 1.0f;
    glColor3f(0.5f * dim, 0.1f * dim, 0.1f * dim);
    glBegin(GL_POLYGON);
    glVertex2f(b.x + 10 * scale, b.y - 8 * scale);
    glVertex2f(b.x + 80 * scale, b.y - 8 * scale);
    glVertex2f(b.x + 90 * scale, b.y);
    glVertex2f(b.x, b.y);
    glEnd(); //base body

    glColor3f(0.9f * dim, 0.9f * dim, 0.95f * dim);
    glBegin(GL_POLYGON);
    glVertex2f(b.x, b.y);
    glVertex2f(b.x + 90 * scale, b.y);
    glVertex2f(b.x + 100 * scale, b.y + 15 * scale);
    glVertex2f(b.x - 5 * scale, b.y + 15 * scale);
    glEnd(); // uopper deck and cabin shape

    glColor3f(0.8f * dim, 0.8f * dim, 0.9f * dim);
    drawRect(b.x + 10 * scale, b.y + 15 * scale, 50 * scale, 10 * scale);// lower cabin
    drawRect(b.x + 15 * scale, b.y + 25 * scale, 30 * scale, 8 * scale);//upper cabin

    glColor3f(1.0f * dim, 0.5f * dim, 0.0f * dim);
    drawRect(b.x + 35 * scale, b.y + 25 * scale, 6 * scale, 12 * scale);// chimny 1nd
    drawRect(b.x + 48 * scale, b.y + 15 * scale, 6 * scale, 15 * scale); // chimny 2nd

    if (isNight) glColor3f(1.0f, 1.0f, 0.0f);
    else glColor3f(0.2f, 0.3f, 0.4f);
    glPointSize(3.0f * scale); // window size
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; i++) glVertex2f(b.x + (15 * scale) + (i * 10 * scale), b.y + 20 * scale); // side window 4 per 10px
    glVertex2f(b.x + 20 * scale, b.y + 29 * scale);
    glVertex2f(b.x + 30 * scale, b.y + 29 * scale);
    glEnd();
}

void drawCar(Car c) {
    float dim = isNight ? 0.6f : 1.0f;
    float bodyY = c.y + 10; //wheels on car body

    // Draw wheels
    glColor3f(0.1f, 0.1f, 0.1f);
    drawRect(c.x + 5, c.y - 2, c.width, 5); // shadow
    drawCircle(c.x + 15, c.y + 10, 10, 12); // front
    drawCircle(c.x + c.width - 15, c.y + 10, 10, 12);//rear
    glColor3f(0.6f, 0.6f, 0.6f);
    drawCircle(c.x + 15, c.y + 10, 6, 8); // inner rim
    drawCircle(c.x + c.width - 15, c.y + 10, 6, 8);

    // Car Body based on type
    if (c.type == 1) { // SUV
        glColor3f(c.r * dim, c.g * dim, c.b * dim);
        drawRect(c.x, bodyY, c.width, 25);//body
        drawRect(c.x + 10, bodyY + 25, c.width - 20, 20); //  cabin
        glColor3f(0.7f * dim, 0.9f * dim, 1.0f * dim);
        drawRect(c.x + 15, bodyY + 28, 25, 14); // f window
        drawRect(c.x + 45, bodyY + 28, 25, 14);//r window
    }
    else if (c.type == 2) { // Sports
        glColor3f(c.r * dim, c.g * dim, c.b * dim);
        glBegin(GL_POLYGON);
        glVertex2f(c.x, bodyY);
        glVertex2f(c.x + c.width, bodyY);
        glVertex2f(c.x + c.width, bodyY + 22);
        glVertex2f(c.x + c.width * 0.5, bodyY + 28);
        glVertex2f(c.x + c.width * 0.2, bodyY + 28);
        glVertex2f(c.x, bodyY + 12);
        glEnd();
        glColor3f(0.1f, 0.1f, 0.1f);
        drawRect(c.x + c.width - 15, bodyY + 22, 15, 4); // spoilar on rear
    }
    else { // Sedan
        glColor3f(c.r * dim, c.g * dim, c.b * dim);
        drawRect(c.x - 5, bodyY, c.width + 10, 20); // lower
        drawRect(c.x + 15, bodyY + 20, c.width - 30, 15);//roof
        glColor3f(0.7f * dim, 0.9f * dim, 1.0f * dim);
        drawRect(c.x + 20, bodyY + 22, (c.width - 40) / 2 - 2, 10); // w left
        drawRect(c.x + 20 + (c.width - 40) / 2 + 2, bodyY + 22, (c.width - 40) / 2 - 2, 10); // w right
    }

    // --- HEADLIGHT (GL_TRIANGLE_FAN) ---
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blending for light color

        float originX = (c.lane == 0) ? c.x - 5 : c.x + c.width + 5;
        float originY = bodyY + 10; // beam center
        float beamLength = 150.0f;// distance
        float beamSpread = 30.0f; // angle degrees

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 1.0f, 0.8f, 0.6f); // Bright at origin
        glVertex2f(originX, originY);

        glColor4f(1.0f, 1.0f, 0.8f, 0.0f); // Fade to transparent
        int segments = 10;
        for (int i = 0; i <= segments; i++) { // edge with segments
            float angle;
            if (c.lane == 0) { // Beam left
                angle = 180.0f - (beamSpread / 2.0f) + (beamSpread * (float)i / segments);
            } else { // Beam right
                angle = -(beamSpread / 2.0f) + (beamSpread * (float)i / segments);
            }
            float rad = angle * 3.14159f / 180.0f;
            glVertex2f(originX + cos(rad) * beamLength, originY + sin(rad) * beamLength);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
}

// --- Scene 2 Drawing Functions (Valley) ---


void drawPineTree(float x, float y, float size, float dim) {
    // Trunk
    glColor3f(0.4f * dim, 0.2f * dim, 0.1f * dim);
    drawRect(x - 6 * size, y, 12 * size, 40 * size);

    // stacked triangles
    glColor3f(0.1f * dim, 0.45f * dim, 0.1f * dim);
    float base_y = y + 30 * size;
    for (int i = 0; i < 3; i++) {
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 25 * size * (1.0f - i * 0.15f), base_y + i * 20 * size);
        glVertex2f(x + 25 * size * (1.0f - i * 0.15f), base_y + i * 20 * size);
        glVertex2f(x, base_y + (i * 20 + 35) * size);
        glEnd();
    }
}

void drawDeciduousTree(float x, float y, float size, float dim) {
    // Trunk
    glColor3f(0.4f * dim, 0.2f * dim, 0.1f * dim);
    drawRect(x - 8 * size, y, 16 * size, 50 * size);

    // Thick canopy overlapping circles
    glColor3f(0.15f * dim, 0.5f * dim, 0.15f * dim);
    float cy = y + 55 * size;
    drawCircle(x, cy, 30 * size, 16);
    drawCircle(x - 20 * size, cy - 10 * size, 22 * size, 16);
    drawCircle(x + 20 * size, cy - 10 * size, 22 * size, 16);
    drawCircle(x - 15 * size, cy + 15 * size, 24 * size, 16);
    drawCircle(x + 15 * size, cy + 15 * size, 24 * size, 16);
}

void drawPoplarTree(float x, float y, float size, float dim) {
    // Trunk
    glColor3f(0.4f * dim, 0.2f * dim, 0.1f * dim);
    drawRect(x - 5 * size, y, 10 * size, 45 * size);

    // Tall narrow oval / elongated diamond canopy
    glColor3f(0.12f * dim, 0.48f * dim, 0.18f * dim);
    float base_y = y + 45 * size;
    glBegin(GL_POLYGON);
    glVertex2f(x, base_y + 80 * size); // peak
    glVertex2f(x + 15 * size, base_y + 50 * size);
    glVertex2f(x + 18 * size, base_y + 25 * size);
    glVertex2f(x, base_y);
    glVertex2f(x - 18 * size, base_y + 25 * size);
    glVertex2f(x - 15 * size, base_y + 50 * size);
    glEnd();
}

void drawTree(float x, float y, float size, int type) {
    float dim = isNight ? 0.35f : 1.0f; // Ambient Dimming Multiplier: 0.35f
    if (type == 0) drawPineTree(x, y, size, dim);
    else if (type == 1) drawDeciduousTree(x, y, size, dim);
    else drawPoplarTree(x, y, size, dim);
}

void drawWindmill(float x, float y) {
    float dim = isNight ? 0.35f : 1.0f;
    // Tower trunk
    glColor3f(0.7f * dim, 0.7f * dim, 0.7f * dim);
    glBegin(GL_POLYGON);
    glVertex2f(x - 8, y);
    glVertex2f(x + 8, y);
    glVertex2f(x + 3, y + 100);
    glVertex2f(x - 3, y + 100);
    glEnd();

    // Rotor assembly
    glPushMatrix();
    glTranslatef(x, y + 100, 0.0f);
    glRotatef(windmillAngle, 0, 0, 1);
    glColor3f(0.6f * dim, 0.6f * dim, 0.6f * dim);
    // 3-blade rotor
    for(int i = 0; i < 3; i++) {
        glRotatef(120, 0, 0, 1);
        glBegin(GL_TRIANGLES);
        glVertex2f(-5, 0);
        glVertex2f(5, 0);
        glVertex2f(0, 60);
        glEnd();
    }
    glPopMatrix();
}

void drawHouse(float x, float y, float width, float height, bool lit) {
    float dim = isNight ? 0.35f : 1.0f;

    // Main Body
    glColor3f(0.6f * dim, 0.4f * dim, 0.2f * dim);
    drawRect(x, y, width, height);

    // Roof
    glColor3f(0.8f * dim, 0.3f * dim, 0.2f * dim);
    glBegin(GL_POLYGON);
    glVertex2f(x - 10, y + height);
    glVertex2f(x + width + 10, y + height);
    glVertex2f(x + width / 2, y + height + 40);
    glEnd();

    // Door
    glColor3f(0.3f * dim, 0.15f * dim, 0.1f * dim);
    drawRect(x + width / 2 - 10, y, 20, 30);

    // Windows & Porch Light
    if (isNight && lit) {
        glColor3f(1.0f, 0.9f, 0.4f); // Glowing window
        drawRect(x + 10, y + 20, 20, 20);
        drawRect(x + width - 30, y + 20, 20, 20);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 0.0f, 0.6f);
        drawCircle(x + width / 2, y + height - 10, 15, 12);
        glDisable(GL_BLEND);
    } else {
        if (isNight) glColor3f(0.15f, 0.15f, 0.2f);
        else glColor3f(0.2f, 0.6f, 0.8f);
        drawRect(x + 10, y + 20, 20, 20);
        drawRect(x + width - 30, y + 20, 20, 20);
    }

    // Bulb
    glColor3f(0.8f * dim, 0.8f * dim, 0.8f * dim);
    drawCircle(x + width / 2, y + height - 10, 5, 8);
}

void drawBird(Bird b) {
    float dim = isNight ? 0.3f : 1.0f;

    glPushMatrix();
    glTranslatef(b.x, b.y, 0);
    glScalef(b.scale, b.scale, 1.0f); // Scaling Requirement

    // Tail Feathers
    glColor3f(0.15f * dim, 0.05f * dim, 0.0f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(-15, 0);
    glVertex2f(-25, 8);
    glVertex2f(-25, -8);
    glEnd();

    // Body Core
    glColor3f(0.2f * dim, 0.1f * dim, 0.0f * dim);
    drawRect(-15, -5, 30, 10);

    // Head
    drawCircle(15, 0, 6, 8);

    // Beak
    glColor3f(1.0f * dim, 0.8f * dim, 0.0f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(20, 2);
    glVertex2f(20, -2);
    glVertex2f(28, 0);
    glEnd();

    // Left WingFlapping (Rotation)
    glPushMatrix();
    glTranslatef(0, 5, 0);
    glRotatef(b.wingAngle, 0, 0, 1);
    glColor3f(0.25f * dim, 0.15f * dim, 0.05f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10, 0);
    glVertex2f(-20, 20);
    glVertex2f(5, 0);
    glEnd();
    glPopMatrix();

    // Right WingFlapping ( Rotation)
    glPushMatrix();
    glTranslatef(0, -5, 0);
    glRotatef(-b.wingAngle, 0, 0, 1);
    glColor3f(0.15f * dim, 0.05f * dim, 0.0f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10, 0);
    glVertex2f(-20, -20);
    glVertex2f(5, 0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

// --- Initialization ---
void initSand() {
    sandDots.clear();
    for (int i = 0; i < 1200; i++) {
        SandDot s;
        s.x = rand() % SCR_WIDTH;
        s.y = (rand() % BEACH_HEIGHT) + (SHORE_START - BEACH_HEIGHT);
        float var = (rand() % 10) / 100.0f;
        s.r = 0.8f - var; s.g = 0.7f - var; s.b = 0.5f - var;
        sandDots.push_back(s);
    }
}

void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCR_WIDTH, 0, SCR_HEIGHT);

    // --- Scene 1 Setup ---
    initSand();
    myBoat.x = -150; myBoat.y = SHORE_START + 50; myBoat.speed = 0.3f;
    farBoat.x = 200; farBoat.y = SHORE_START + 110; farBoat.speed = 0.15f;

    for (int i = 0; i < 100; i++) {
        Star s;
        s.x = rand() % SCR_WIDTH;
        s.y = (rand() % (SCR_HEIGHT - HORIZON_LEVEL - 50)) + HORIZON_LEVEL + 50;
        s.alpha = (rand() % 10) / 10.0f;
        s.glowSpeed = 0.01f + ((rand() % 20) / 1000.0f);
        s.direction = 1;
        stars.push_back(s);
        s2_stars.push_back(s); // Reuse for scene 2
    }

    city.push_back({20, 120, 200, 0.7f, 0.3f, 0.2f, 2});
    city.push_back({160, 90, 280, 0.3f, 0.5f, 0.7f, 1});
    city.push_back({270, 200, 80, 0.8f, 0.8f, 0.6f, 3});
    city.push_back({480, 100, 220, 0.2f, 0.6f, 0.6f, 1});
    city.push_back({600, 140, 160, 0.8f, 0.7f, 0.5f, 2});
    city.push_back({755, 100, 310, 0.3f, 0.6f, 0.5f, 1});
    city.push_back({880, 110, 190, 0.4f, 0.4f, 0.45f, 2});

    // Lane 0 = upper sub-lane, cars go LEFT (negative speed). Spaced exactly 350px apart
    traffic.push_back({950, ROAD_HEIGHT - 35, -2.5f, -2.5f, 0, 1.0f, 0.2f, 0.2f, 80, 0});  // Red Sedan
    traffic.push_back({600, ROAD_HEIGHT - 35, -2.5f, -2.5f, 0, 0.2f, 0.2f, 1.0f, 90, 1});  // Blue SUV
    traffic.push_back({250, ROAD_HEIGHT - 35, -2.5f, -2.5f, 0, 0.9f, 0.9f, 0.2f, 80, 0});  // Yellow Sedan
    // Lane 1 = lower sub-lane, cars go RIGHT (positive speed).
    traffic.push_back({50,  20, 2.5f, 2.5f, 1, 0.2f, 0.8f, 0.2f, 80, 0});   // Green Sedan
    traffic.push_back({400, 20, 2.5f, 2.5f, 1, 0.8f, 0.4f, 0.0f, 110, 2});   // Orange Sedan
    traffic.push_back({750, 20, 2.5f, 2.5f, 1, 1.0f, 1.0f, 1.0f, 80, 0});    // White Sedan

    clouds.push_back({100, 500, 0.5f});
    clouds.push_back({400, 550, 0.3f});
    clouds.push_back({700, 520, 0.6f});

    for (int i = 0; i < 300; i++) {
        rain[i].x = rand() % SCR_WIDTH;
        rain[i].y = rand() % SCR_HEIGHT;
        rain[i].speed = (rand() % 10) + 15;
    }

    // --- Scene 2 Setup ---
    s2_houses.push_back({120, 220, 100, 80, false});
    s2_houses.push_back({450, 220, 100, 80, false});
    s2_houses.push_back({780, 220, 100, 80, false});

    s2_birds.push_back({0, 500, 2.0f, 1.0f, 0, 1});
    s2_birds.push_back({-150, 450, 1.5f, 0.8f, 0, 1});
    s2_birds.push_back({-50, 550, 1.8f, 0.9f, 0, 1});
    s2_birds.push_back({-300, 480, 2.2f, 0.7f, 0, 1});

    // Scene 2: lane 0 = upper sub-lane going LEFT, spaced >=350px
    s2_traffic.push_back({950, 110, -2.5f, -2.5f, 0, 0.8f, 0.2f, 0.2f, 80, 1}); // Red SUV
    s2_traffic.push_back({600, 110, -2.5f, -2.5f, 0, 0.2f, 0.2f, 0.8f, 90, 0}); // Blue SUV
    s2_traffic.push_back({250, 110, -2.5f, -2.5f, 0, 0.9f, 0.9f, 0.2f, 80, 2}); // Yellow Sedan
    // Scene 2: lane 1 = lower sub-lane going RIGHT, spaced >=350px
    s2_traffic.push_back({ 50, 160,  2.5f,  2.5f, 1, 0.2f, 0.8f, 0.2f, 80, 1}); // Green SUV
    s2_traffic.push_back({400, 160,  2.5f,  2.5f, 1, 0.8f, 0.4f, 0.0f, 110, 0}); // Orange SUV
    s2_traffic.push_back({750, 160,  2.5f,  2.5f, 1, 0.9f, 0.9f, 0.9f, 80, 2}); // White Sedan

    s2_trees.clear();
    // Canal side buffer (Y=60 to 100, trees only)
    s2_trees.push_back({50, 60, 0.7f, 0});
    s2_trees.push_back({170, 60, 0.65f, 1});
    s2_trees.push_back({290, 60, 0.75f, 2});
    s2_trees.push_back({410, 60, 0.7f, 0});
    s2_trees.push_back({530, 60, 0.65f, 1});
    s2_trees.push_back({650, 60, 0.75f, 2});
    s2_trees.push_back({770, 60, 0.7f, 0});
    s2_trees.push_back({890, 60, 0.65f, 1});

    // Plateau forest (Y=200 to 350)
    s2_trees.push_back({100, 310, 0.8f, 0});
    s2_trees.push_back({300, 310, 0.75f, 1});
    s2_trees.push_back({600, 310, 0.8f, 2});
    s2_trees.push_back({900, 310, 0.75f, 0});

    // Grass & Bushes
    for(int i=0; i<25; i++) {
        s2_bushes.push_back({(float)(rand() % SCR_WIDTH), 320.0f + (rand() % 25), 10.0f + (rand() % 5)});
    }
}

// --- Display Functions ---
void drawScene1() {
    // 1. SKY
    if (isNight) {
        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.1f); glVertex2f(0, SCR_HEIGHT); glVertex2f(SCR_WIDTH, SCR_HEIGHT);
        glColor3f(0.1f, 0.1f, 0.2f); glVertex2f(SCR_WIDTH, HORIZON_LEVEL); glVertex2f(0, HORIZON_LEVEL);
        glEnd();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (size_t i = 0; i < stars.size(); i++) {
            glColor4f(1.0f, 1.0f, 1.0f, stars[i].alpha);
            glVertex2f(stars[i].x, stars[i].y);
        }
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 1.0f); drawCircle(900, 520, 30, 20); // Moon
    } else {
        glBegin(GL_QUADS);
        glColor3f(0.0f, 0.6f, 1.0f); glVertex2f(0, SCR_HEIGHT); glVertex2f(SCR_WIDTH, SCR_HEIGHT);
        glColor3f(0.6f, 0.8f, 1.0f); glVertex2f(SCR_WIDTH, HORIZON_LEVEL); glVertex2f(0, HORIZON_LEVEL);
        glEnd();
        glColor3f(1.0f, 0.9f, 0.0f); drawCircle(900, 520, 45, 20); // Sun
    }

    // 2. SEA
    if (isNight) glColor3f(0.0f, 0.0f, 0.3f);
    else glColor3f(0.0f, 0.4f, 0.8f);
    drawRect(0, SHORE_START, SCR_WIDTH, HORIZON_LEVEL - SHORE_START);

    // 3. SHIPS
    drawShip(farBoat, 0.5f);
    drawShip(myBoat, 1.0f);

    // 4. DISTANT GROUND
    if (isNight) glColor3f(0.1f, 0.2f, 0.1f);
    else glColor3f(0.2f, 0.5f, 0.2f);
    drawRect(0, BUILDING_BASE, SCR_WIDTH, SHORE_START - BUILDING_BASE);

    // 5. BEACH
    if (isNight) glColor3f(0.4f, 0.35f, 0.2f);
    else glColor3f(0.94f, 0.85f, 0.6f);
    drawRect(0, SHORE_START - BEACH_HEIGHT, SCR_WIDTH, BEACH_HEIGHT);
    if (!isNight) {
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (size_t i = 0; i < sandDots.size(); i++) {
            glColor3f(sandDots[i].r, sandDots[i].g, sandDots[i].b);
            glVertex2f(sandDots[i].x, sandDots[i].y);
        }
        glEnd();
    }

    // 6. CLOUDS
    for (size_t i = 0; i < clouds.size(); i++) drawBeautifulCloud(clouds[i].x, clouds[i].y, isNight);

    // 7. HOTELS
    for (size_t i = 0; i < city.size(); i++) drawHotel(city[i]);

    // 8. ROAD (drawn first so cars appear on top)
    glColor3f(0.3f, 0.3f, 0.3f);
    drawRect(0, 0, SCR_WIDTH, ROAD_HEIGHT);          // Road surface
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < SCR_WIDTH; i += 60) drawRect(i, ROAD_HEIGHT / 2 - 2, 30, 4); // Centre dashes
    glColor3f(0.6f, 0.6f, 0.6f);
    drawRect(0, ROAD_HEIGHT, SCR_WIDTH, 10);          // Top kerb stripe

    // 9. TRAFFIC
    for (size_t i = 0; i < traffic.size(); i++) drawCar(traffic[i]);

    // 10. RAIN
    if (isRaining) {
        glColor3f(0.8f, 0.8f, 1.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < 300; i++) {
            glVertex2f(rain[i].x, rain[i].y);
            glVertex2f(rain[i].x - 5, rain[i].y - 15);
        }
        glEnd();
    }
}

void drawScene2() {
    float dim = isNight ? 0.35f : 1.0f; // Ambient dimming multiplier (0.35f)

    // 1. SKY & CELESTIAL BACKGROUND (Deepest)
    glBegin(GL_QUADS);
    if (isNight) {
        glColor3f(0.02f, 0.02f, 0.1f); glVertex2f(0, SCR_HEIGHT); glVertex2f(SCR_WIDTH, SCR_HEIGHT);
        glColor3f(0.05f, 0.05f, 0.18f); glVertex2f(SCR_WIDTH, 200); glVertex2f(0, 200);
    } else {
        glColor3f(0.4f, 0.7f, 1.0f); glVertex2f(0, SCR_HEIGHT); glVertex2f(SCR_WIDTH, SCR_HEIGHT);
        glColor3f(0.7f, 0.9f, 1.0f); glVertex2f(SCR_WIDTH, 200); glVertex2f(0, 200);
    }
    glEnd();

    if (isNight) {
        // Star points
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (size_t i = 0; i < s2_stars.size(); i++) {
            glColor4f(1.0f, 1.0f, 1.0f, s2_stars[i].alpha);
            glVertex2f(s2_stars[i].x, s2_stars[i].y);
        }
        glEnd();
        glDisable(GL_BLEND);

        // Glowing Moon X=650, Y=530
        glColor3f(0.95f, 0.95f, 1.0f);
        drawCircle(650, 530, 35, 30);
    } else {
        // Bright Sun  X=650, Y=530
        glColor3f(1.0f, 0.9f, 0.0f);
        drawCircle(650, 530, 45, 30);
    }

    // 2. MOUNTAIN BACKDROP LAYER


    glColor3f(0.5f * dim, 0.5f * dim, 0.52f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 300); glVertex2f(200, 550); glVertex2f(400, 300);
    glVertex2f(250, 300); glVertex2f(500, 500); glVertex2f(700, 300);
    glVertex2f(550, 300); glVertex2f(800, 580); glVertex2f(1100, 300);
    glEnd();

    // Midground mountain group
    glColor3f(0.42f * dim, 0.42f * dim, 0.45f * dim);
    glBegin(GL_TRIANGLES);
    glVertex2f(-100, 300); glVertex2f(100, 450); glVertex2f(300, 300);
    glVertex2f(150, 300); glVertex2f(350, 480); glVertex2f(600, 300);
    glVertex2f(450, 300); glVertex2f(650, 420); glVertex2f(900, 300);
    glVertex2f(750, 300); glVertex2f(950, 460); glVertex2f(1200, 300);
    glEnd();

    // 3. DYNAMIC SOARING BIRDS LAYER
    for (size_t i = 0; i < s2_birds.size(); i++) {
        drawBird(s2_birds[i]);
    }

    // 4. ELEVATED walkway (Upper Land Field)
    glColor3f(0.28f * dim, 0.48f * dim, 0.18f * dim);
    glBegin(GL_QUADS);
    glVertex2f(0, 200); glVertex2f(SCR_WIDTH, 200);
    glVertex2f(SCR_WIDTH, 350); glVertex2f(0, 350);
    glEnd();

    // 5. STATIC FARMHOUSES
    drawHouse(120, 220, 100, 80, house1_Lit);
    drawHouse(450, 220, 100, 80, house2_Lit);
    drawHouse(780, 220, 100, 80, house3_Lit);

    // 6. walkway TREES
    for(size_t i = 0; i < s2_trees.size(); i++) {
        if (s2_trees[i].y >= 200) {
            drawTree(s2_trees[i].x, s2_trees[i].y, s2_trees[i].size, s2_trees[i].type);
        }
    }

    // 7. 4 HILLSIDE WINDMILLS
    drawWindmill(80, 280);
    drawWindmill(320, 290);
    drawWindmill(600, 285);
    drawWindmill(920, 280);

    // 8. ROADWAY AND TRAFFIC LANE (Concrete color)
    glColor3f(0.55f * dim, 0.55f * dim, 0.55f * dim);
    drawRect(0, 100, SCR_WIDTH, 100);

    // Broken center line
    glColor3f(0.9f * dim, 0.9f * dim, 0.9f * dim);
    for (int i = 0; i < SCR_WIDTH; i += 60) {
        drawRect(i, 148, 30, 4);
    }
    // Road curbs
    glColor3f(0.7f * dim, 0.7f * dim, 0.7f * dim);
    drawRect(0, 196, SCR_WIDTH, 4);
    drawRect(0, 100, SCR_WIDTH, 4);

    // 9. TWO-WAY TRAFFIC SYSTEM
    for (size_t i = 0; i < s2_traffic.size(); i++) {
        drawCar(s2_traffic[i]);
    }
    // 10. RIVER CANAL LAYER (Foreground boundary, distinct royal blue water with dynamic waves)
    glBegin(GL_QUADS);
    if (isNight) {
        glColor3f(0.0f, 0.1f, 0.25f); glVertex2f(0, 0);
        glColor3f(0.0f, 0.1f, 0.25f); glVertex2f(SCR_WIDTH, 0);
        glColor3f(0.0f, 0.2f, 0.4f); glVertex2f(SCR_WIDTH, 60);
        glColor3f(0.0f, 0.2f, 0.4f); glVertex2f(0, 60);
    } else {
        glColor3f(0.0f, 0.3f, 0.65f); glVertex2f(0, 0);
        glColor3f(0.0f, 0.3f, 0.65f); glVertex2f(SCR_WIDTH, 0);
        glColor3f(0.0f, 0.55f, 0.9f); glVertex2f(SCR_WIDTH, 60);
        glColor3f(0.0f, 0.55f, 0.9f); glVertex2f(0, 60);
    }
    glEnd();

    // Wavy dynamic water ripples (Simulates realistic flowing liquid)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int layer = 0; layer < 3; layer++) {
        float base_y = 15.0f + layer * 15.0f;
        float speed_mult = (layer == 0) ? 1.0f : ((layer == 1) ? -1.3f : 0.8f);
        float offset = waterWaveOffset * speed_mult;

        if (isNight) glColor4f(0.3f, 0.5f, 0.8f, 0.4f);
        else glColor4f(0.6f, 0.85f, 1.0f, 0.55f);

        glBegin(GL_LINE_STRIP);
        for (int x = 0; x <= SCR_WIDTH; x += 20) {
            float wy = base_y + sin((x + offset) * 0.04f) * 3.5f;
            glVertex2f(x, wy);
        }
        glEnd();
    }
    glDisable(GL_BLEND);

    //  CANAL-SIDE
    glColor3f(0.55f * dim, 0.55f * dim, 0.55f * dim);
    drawRect(0, 60, SCR_WIDTH, 40);

    //  CANAL-SIDE FOREST
    for(size_t i = 0; i < s2_trees.size(); i++) {
        if (s2_trees[i].y >= 60 && s2_trees[i].y < 100) {
            drawTree(s2_trees[i].x, s2_trees[i].y, s2_trees[i].size, s2_trees[i].type);
        }
    }
}

void display() {

    if (currentScene == 1) {
        if (isNight) glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        else glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    } else {
        if (isNight) glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        else glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    if (currentScene == 1) {
        drawScene1();
    } else {
        drawScene2();
    }

    // Draw Scene indicator text
    glColor3f(1, 1, 1);
    glRasterPos2f(10, SCR_HEIGHT - 20);
    string txt = "Press Tab to switch scene";
    for (char c : txt) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glutSwapBuffers();
}

void update(int value) {
    if (currentScene == 1) {
        // Scene 1 Updates
        myBoat.x += myBoat.speed; if (myBoat.x > SCR_WIDTH + 150) myBoat.x = -150;
        farBoat.x += farBoat.speed; if (farBoat.x > SCR_WIDTH + 80) farBoat.x = -80;

        for (size_t i = 0; i < stars.size(); i++) {
            if (stars[i].direction == 1) {
                stars[i].alpha += stars[i].glowSpeed;
                if (stars[i].alpha >= 1.0f) stars[i].direction = -1;
            } else {
                stars[i].alpha -= stars[i].glowSpeed;
                if (stars[i].alpha <= 0.0f) stars[i].direction = 1;
            }
        }

        if (isRaining) {
            for (int i = 0; i < 300; i++) {
                rain[i].y -= rain[i].speed;
                if (rain[i].y < 0) {
                    rain[i].y = SCR_HEIGHT; rain[i].x = rand() % SCR_WIDTH;
                }
            }
        }

        for (size_t i = 0; i < clouds.size(); i++) {
            clouds[i].x += clouds[i].speed;
            if (clouds[i].x > SCR_WIDTH + 50) clouds[i].x = -100;
        }

        // Traffic AI (2-pass propagation to fix cascading collision bugs with dynamic ordering)
        float safeDist = 130.0f;
        vector<float> s1_speeds(traffic.size());
        for (size_t i = 0; i < traffic.size(); i++) {
            s1_speeds[i] = traffic[i].maxSpeed * s1_carSpeedScale;
        }
        for (int pass = 0; pass < 2; pass++) {
            for (size_t i = 0; i < traffic.size(); i++) {
                for (size_t j = 0; j < traffic.size(); j++) {
                    if (i == j || traffic[i].lane != traffic[j].lane) continue;
                    if (traffic[i].lane == 1) { // moves right
                        if (traffic[j].x > traffic[i].x && (traffic[j].x - traffic[i].x) < safeDist) {
                            s1_speeds[i] = min(s1_speeds[i], s1_speeds[j]);
                        }
                    } else { // moves left
                        if (traffic[j].x < traffic[i].x && (traffic[i].x - traffic[j].x) < safeDist) {
                            s1_speeds[i] = max(s1_speeds[i], s1_speeds[j]);
                        }
                    }
                }
            }
        }
        for (size_t i = 0; i < traffic.size(); i++) {
            traffic[i].speed = s1_speeds[i];
            traffic[i].x += traffic[i].speed;
            if (traffic[i].lane == 1 && traffic[i].x > SCR_WIDTH) traffic[i].x = -150;
            if (traffic[i].lane == 0 && traffic[i].x < -150) traffic[i].x = SCR_WIDTH;
        }
    }
    else {
        // Scene 2 Updates

        // Windmill Rotation
        windmillAngle += 1.5f * windmillSpeedScale;
        if (windmillAngle >= 360.0f) windmillAngle -= 360.0f;

        // Water Wave Offset
        waterWaveOffset += 0.8f;
        if (waterWaveOffset >= 1000.0f) waterWaveOffset -= 1000.0f;

        // Stars
        for (size_t i = 0; i < s2_stars.size(); i++) {
            if (s2_stars[i].direction == 1) {
                s2_stars[i].alpha += s2_stars[i].glowSpeed;
                if (s2_stars[i].alpha >= 1.0f) s2_stars[i].direction = -1;
            } else {
                s2_stars[i].alpha -= s2_stars[i].glowSpeed;
                if (s2_stars[i].alpha <= 0.0f) s2_stars[i].direction = 1;
            }
        }

        // Traffic AI (Same 2-pass propagation logic for Scene 2)
        float safeDist = 130.0f;
        vector<float> s2_speeds(s2_traffic.size());
        for (size_t i = 0; i < s2_traffic.size(); i++) {
            s2_speeds[i] = s2_traffic[i].maxSpeed;
        }
        for (int pass = 0; pass < 2; pass++) {
            for (size_t i = 0; i < s2_traffic.size(); i++) {
                for (size_t j = 0; j < s2_traffic.size(); j++) {
                    if (i == j || s2_traffic[i].lane != s2_traffic[j].lane) continue;
                    if (s2_traffic[i].lane == 1) { // moves right
                        if (s2_traffic[j].x > s2_traffic[i].x && (s2_traffic[j].x - s2_traffic[i].x) < safeDist) {
                            s2_speeds[i] = min(s2_speeds[i], s2_speeds[j]);
                        }
                    } else { // moves left
                        if (s2_traffic[j].x < s2_traffic[i].x && (s2_traffic[i].x - s2_traffic[j].x) < safeDist) {
                            s2_speeds[i] = max(s2_speeds[i], s2_speeds[j]);
                        }
                    }
                }
            }
        }
        for (size_t i = 0; i < s2_traffic.size(); i++) {
            s2_traffic[i].speed = s2_speeds[i];
            s2_traffic[i].x += s2_traffic[i].speed;
            if (s2_traffic[i].lane == 1 && s2_traffic[i].x > SCR_WIDTH) s2_traffic[i].x = -150;
            if (s2_traffic[i].lane == 0 && s2_traffic[i].x < -150) s2_traffic[i].x = SCR_WIDTH;
        }

        // Birds Update (Flapping, Translation, Scaling)
        for (size_t i = 0; i < s2_birds.size(); i++) {
            s2_birds[i].x += s2_birds[i].speed;
            s2_birds[i].y += s2_birds[i].speed * 0.1f;
            s2_birds[i].scale = 1.0f - (s2_birds[i].x / SCR_WIDTH) * 0.6f;

            s2_birds[i].wingAngle += s2_birds[i].wingDir * 8.0f;
            if (s2_birds[i].wingAngle > 30.0f) s2_birds[i].wingDir = -1;
            if (s2_birds[i].wingAngle < -30.0f) s2_birds[i].wingDir = 1;

            if (s2_birds[i].x > SCR_WIDTH + 50 || s2_birds[i].scale < 0.1f) {
                s2_birds[i].x = -150 - (rand() % 100);
                s2_birds[i].y = 400 + (rand() % 100);
                s2_birds[i].scale = 1.0f;
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

// --- Input Handling ---
void handleKey(unsigned char key, int x, int y) {
    switch(key) {
        case 9: // Tab Key = Switch Scene
            currentScene = (currentScene == 1) ? 2 : 1;
            break;
        case 'N':
        case 'n':
            isNight = true;
            house1_Lit = true;
            house2_Lit = true;
            house3_Lit = true;
            break;
        case 'D':
        case 'd': isNight = false; break;
        case '0':
            if (currentScene == 2) {
                windmillSpeedScale = 0.0f;
            } else {
                s1_carSpeedScale = 0.0f;
            }
            break;
        case '1':
            if (currentScene == 2) {
                windmillSpeedScale = 1.0f;
            } else {
                s1_carSpeedScale = 1.0f;
            }
            break;
        case '2':
            if (currentScene == 2) {
                windmillSpeedScale = 3.0f;
            } else {
                s1_carSpeedScale = 2.5f;
            }
            break;
        case 27: exit(0); break; // Esc
    }
}

void handleMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (currentScene == 1) {
            // Scene 1 Mouse Interaction: Toggle rain on left click
            isRaining = !isRaining;
        }
        else if (currentScene == 2) {
            // Scene 2 Mouse Interaction: Only toggle house lights during night mode
            if (isNight) {
                float glY = SCR_HEIGHT - y;
                float glX = x;

                // Check bounding box of each farmhouse to toggle individual lights
                // House 1: X: 120 to 220, Y: 220 to 300
                if (glX >= 120 && glX <= 220 && glY >= 220 && glY <= 300) {
                    house1_Lit = !house1_Lit;
                }
                // House 2: X: 450 to 550, Y: 220 to 300
                else if (glX >= 450 && glX <= 550 && glY >= 220 && glY <= 300) {
                    house2_Lit = !house2_Lit;
                }
                // House 3: X: 780 to 880, Y: 220 to 300
                else if (glX >= 780 && glX <= 880 && glY >= 220 && glY <= 300) {
                    house3_Lit = !house3_Lit;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("City & Rural View");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(handleKey);
    glutMouseFunc(handleMouse); // Register mouse callback

    glutMainLoop();
    return 0;
}
