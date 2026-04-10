#include "plugin.hpp"

static const int NUM_PARTICLES = 800;
static const int NUM_TYPES     = 16;

struct Particle {
    float x, y;
    float rot;
    float rotSpeed;     // rad/s
    float wobblePhase;  // initial phase for sinusoidal horizontal wobble
    float wobbleFreq;   // Hz
    float wobbleAmp;    // px amplitude
    float speedMult;    // multiplier on global downward speed (Y only)
    float sizeMult;     // multiplier on global size
    float colorHue;     // [0,1] hue for color-per-particle types
};

// ── per-type draw functions (local coords: pre-translated & pre-rotated) ──────
// colorHue is passed through for types that use per-particle color.

static void pDrawSnowflake(NVGcontext* vg, float r, float) {
    nvgStrokeColor(vg, nvgRGBAf(.88f, .94f, 1.f, .85f));
    nvgStrokeWidth(vg, std::max(1.f, r * .13f));
    nvgLineCap(vg, NVG_ROUND);
    for (int i = 0; i < 6; i++) {
        float a = i * M_PI / 3.f;
        float ca = cosf(a), sa = sinf(a);
        nvgBeginPath(vg); nvgMoveTo(vg, 0, 0); nvgLineTo(vg, r*ca, r*sa); nvgStroke(vg);
        float bx = .6f*r*ca, by = .6f*r*sa;
        float ba = a + M_PI*.5f, bl = r*.38f;
        nvgBeginPath(vg);
        nvgMoveTo(vg, bx - .5f*bl*cosf(ba), by - .5f*bl*sinf(ba));
        nvgLineTo(vg, bx + .5f*bl*cosf(ba), by + .5f*bl*sinf(ba));
        nvgStroke(vg);
    }
    nvgBeginPath(vg); nvgCircle(vg, 0, 0, std::max(1.f, r*.1f));
    nvgFillColor(vg, nvgRGBAf(.88f, .94f, 1.f, .85f)); nvgFill(vg);
}

static void pDrawStar(NVGcontext* vg, float r, float) {
    float ri = r * .42f;
    nvgBeginPath(vg);
    for (int i = 0; i < 10; i++) {
        float a  = i * M_PI / 5.f - M_PI * .5f;
        float cr = (i & 1) ? ri : r;
        if (i == 0) nvgMoveTo(vg, cr*cosf(a), cr*sinf(a));
        else        nvgLineTo(vg, cr*cosf(a), cr*sinf(a));
    }
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(1.f, .88f, .18f, .92f)); nvgFill(vg);
}

// Raindrop: tip at top (trailing), fat round end at bottom (leading).
// The overlay rotates it to match actual velocity direction before calling this.
static void pDrawRaindrop(NVGcontext* vg, float r, float) {
    float w = r * .6f, bot = r * .5f;
    nvgBeginPath(vg);
    nvgMoveTo(vg, 0, -r);                              // tip at top
    nvgBezierTo(vg,  w, -r*.2f,  w, bot, 0, bot);     // right side, round bottom
    nvgBezierTo(vg, -w,  bot,   -w, -r*.2f, 0, -r);   // left side, back to tip
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(.3f, .62f, 1.f, .72f)); nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBAf(.6f, .82f, 1.f, .45f));
    nvgStrokeWidth(vg, std::max(.5f, r*.07f)); nvgStroke(vg);
}

static void pDrawLeaf(NVGcontext* vg, float r, float) {
    float w = r * .52f;
    nvgBeginPath(vg);
    nvgMoveTo(vg,  0, -r);
    nvgBezierTo(vg,  w, -r*.5f,  w,  r*.5f, 0,  r);
    nvgBezierTo(vg, -w,  r*.5f, -w, -r*.5f, 0, -r);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(.35f, .78f, .18f, .82f)); nvgFill(vg);
    nvgBeginPath(vg); nvgMoveTo(vg, 0, -r); nvgLineTo(vg, 0, r);
    nvgStrokeColor(vg, nvgRGBAf(.2f, .48f, .1f, .72f));
    nvgStrokeWidth(vg, std::max(.5f, r*.09f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
}

static void pDrawHeart(NVGcontext* vg, float r, float) {
    float s = r / 14.f;
    nvgBeginPath(vg);
    for (int i = 0; i <= 40; i++) {
        float t  = -M_PI + 2.f * M_PI * i / 40.f;
        float px = 16.f * powf(sinf(t), 3.f) * s;
        float py = -(13.f*cosf(t) - 5.f*cosf(2*t) - 2.f*cosf(3*t) - cosf(4*t)) * s - 2.f*s;
        if (i == 0) nvgMoveTo(vg, px, py);
        else        nvgLineTo(vg, px, py);
    }
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(1.f, .22f, .42f, .88f)); nvgFill(vg);
}

static void pDrawEmber(NVGcontext* vg, float r, float colorHue) {
    // colorHue [0,1] maps: deep red → saturated red → orange → yellow
    // HSL hue: 0.0 (red) through 0.167 (yellow); dark reds use low lightness
    float hue   = colorHue * 0.167f;
    float light = 0.38f + colorHue * 0.38f;   // deep red=dark, yellow=bright
    NVGcolor hot  = nvgHSLA(hue, 1.f, light + 0.20f, 255);
    NVGcolor cool = nvgHSLA(hue, 1.f, light,          0);
    NVGpaint g = nvgRadialGradient(vg, 0, 0, r*.08f, r, hot, cool);
    nvgBeginPath(vg); nvgEllipse(vg, 0, 0, r, r*.48f);
    nvgFillPaint(vg, g); nvgFill(vg);
}

// Cheeseburger: bun top, patty, cheese, bun bottom stacked
static void pDrawCheeseburger(NVGcontext* vg, float r) {
    float bw = r * .90f; // half-width of bun
    float sw = std::max(1.f, r * .07f);
    // Top bun (dome)
    nvgBeginPath(vg);
    nvgArc(vg, 0, -r*.18f, bw, (float)M_PI, 0, NVG_CW);
    nvgLineTo(vg, bw, -r*.18f); nvgLineTo(vg, -bw, -r*.18f);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(.87f, .60f, .18f, .95f)); nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBAf(.62f, .38f, .10f, .7f));
    nvgStrokeWidth(vg, sw); nvgStroke(vg);
    // Sesame seeds (dots on top bun)
    nvgFillColor(vg, nvgRGBAf(.95f, .85f, .55f, .80f));
    nvgBeginPath(vg); nvgEllipse(vg, -r*.28f, -r*.72f, r*.07f, r*.04f); nvgFill(vg);
    nvgBeginPath(vg); nvgEllipse(vg,  r*.18f, -r*.78f, r*.07f, r*.04f); nvgFill(vg);
    nvgBeginPath(vg); nvgEllipse(vg, -r*.05f, -r*.88f, r*.07f, r*.04f); nvgFill(vg);
    // Cheese slice (yellow, slightly wider, peeking out)
    nvgBeginPath(vg); nvgRect(vg, -bw*.95f, -r*.18f, bw*1.9f, r*.20f);
    nvgFillColor(vg, nvgRGBAf(1.f, .82f, .10f, .92f)); nvgFill(vg);
    // Patty
    nvgBeginPath(vg); nvgRect(vg, -bw*.85f, r*.02f, bw*1.7f, r*.24f);
    nvgFillColor(vg, nvgRGBAf(.42f, .20f, .08f, .95f)); nvgFill(vg);
    // Bottom bun
    nvgBeginPath(vg); nvgRect(vg, -bw, r*.26f, bw*2.f, r*.30f);
    nvgFillColor(vg, nvgRGBAf(.87f, .60f, .18f, .95f)); nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBAf(.62f, .38f, .10f, .7f));
    nvgStrokeWidth(vg, sw); nvgStroke(vg);
}

// Hot dog: bun with sausage inside
static void pDrawHotDog(NVGcontext* vg, float r) {
    float bw = r * .95f;
    float sw = std::max(1.f, r * .07f);
    // Bun (tan elongated oval)
    nvgBeginPath(vg); nvgEllipse(vg, 0, 0, bw, r*.38f);
    nvgFillColor(vg, nvgRGBAf(.87f, .65f, .30f, .95f)); nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBAf(.62f, .40f, .12f, .7f));
    nvgStrokeWidth(vg, sw); nvgStroke(vg);
    // Sausage (reddish-pink, slightly smaller)
    nvgBeginPath(vg); nvgEllipse(vg, 0, 0, bw*.82f, r*.21f);
    nvgFillColor(vg, nvgRGBAf(.82f, .28f, .18f, .95f)); nvgFill(vg);
    // Mustard zigzag
    nvgBeginPath(vg);
    nvgMoveTo(vg, -bw*.60f, 0);
    nvgLineTo(vg, -bw*.30f, -r*.10f); nvgLineTo(vg, 0,       r*.10f);
    nvgLineTo(vg,  bw*.30f, -r*.10f); nvgLineTo(vg, bw*.60f, r*.10f);
    nvgStrokeColor(vg, nvgRGBAf(1.f, .85f, .0f, .90f));
    nvgStrokeWidth(vg, std::max(1.f, r*.10f));
    nvgLineCap(vg, NVG_ROUND); nvgLineJoin(vg, NVG_ROUND); nvgStroke(vg);
}

// French fries: red carton with golden fries sticking up
static void pDrawFrenchFries(NVGcontext* vg, float r) {
    float cw = r * .72f;  // half-width of carton
    float sw = std::max(1.f, r * .06f);
    // Carton body (red trapezoid, wider at top)
    nvgBeginPath(vg);
    nvgMoveTo(vg, -cw,       -r*.10f);
    nvgLineTo(vg,  cw,       -r*.10f);
    nvgLineTo(vg,  cw*.78f,   r*.56f);
    nvgLineTo(vg, -cw*.78f,   r*.56f);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(.88f, .12f, .12f, .95f)); nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBAf(.55f, .05f, .05f, .7f));
    nvgStrokeWidth(vg, sw); nvgStroke(vg);
    // Golden arches logo hint (two small yellow arcs on carton)
    nvgBeginPath(vg);
    nvgArc(vg, -cw*.18f, r*.22f, r*.13f, (float)M_PI, 0, NVG_CW);
    nvgArc(vg,  cw*.18f, r*.22f, r*.13f, (float)M_PI, 0, NVG_CW);
    nvgStrokeColor(vg, nvgRGBAf(1.f, .85f, .0f, .85f));
    nvgStrokeWidth(vg, std::max(1.f, r*.09f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
    // Fries (golden rectangles sticking up above carton)
    NVGcolor fry = nvgRGBAf(.98f, .80f, .18f, .95f);
    NVGcolor fryDark = nvgRGBAf(.80f, .55f, .08f, .95f);
    float fryW = r * .10f;
    float tops[5]   = {-r*.90f, -r*.78f, -r*1.00f, -r*.72f, -r*.88f};
    float centers[5] = {-cw*.55f, -cw*.22f, 0.f, cw*.22f, cw*.55f};
    for (int i = 0; i < 5; i++) {
        NVGcolor c = (i & 1) ? fryDark : fry;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, centers[i] - fryW, tops[i], fryW*2.f, -tops[i] - r*.08f, fryW*.4f);
        nvgFillColor(vg, c); nvgFill(vg);
    }
}

// Fast food: cheeseburger, hot dog, or french fries depending on colorHue
static void pDrawFastFood(NVGcontext* vg, float r, float colorHue) {
    if      (colorHue < 0.33f) pDrawCheeseburger(vg, r);
    else if (colorHue < 0.67f) pDrawHotDog(vg, r);
    else                       pDrawFrenchFries(vg, r);
}

static void pDrawBubble(NVGcontext* vg, float r, float) {
    nvgBeginPath(vg); nvgCircle(vg, 0, 0, r);
    nvgStrokeColor(vg, nvgRGBAf(.55f, .8f, 1.f, .62f));
    nvgStrokeWidth(vg, std::max(1.f, r*.12f)); nvgStroke(vg);
    nvgBeginPath(vg); nvgEllipse(vg, -r*.22f, -r*.28f, r*.2f, r*.13f);
    nvgFillColor(vg, nvgRGBAf(1.f, 1.f, 1.f, .45f)); nvgFill(vg);
}

// Cats and Dogs: alternates between a cat and a dog silhouette per particle.
// colorHue < 0.5 → cat (orange tabby), colorHue >= 0.5 → dog (golden brown).
static void pDrawCat(NVGcontext* vg, float r) {
    NVGcolor black = nvgRGBAf(.08f, .08f, .08f, .92f);
    NVGcolor white = nvgRGBAf(.95f, .95f, .95f, .92f);
    NVGcolor pink  = nvgRGBAf(1.f,  .70f, .75f, .85f);

    // Body (black)
    nvgBeginPath(vg); nvgEllipse(vg, 0, r*.15f, r*.42f, r*.26f);
    nvgFillColor(vg, black); nvgFill(vg);
    // White chest patch
    nvgBeginPath(vg); nvgEllipse(vg, 0, r*.18f, r*.22f, r*.16f);
    nvgFillColor(vg, white); nvgFill(vg);

    // Head (black)
    nvgBeginPath(vg); nvgCircle(vg, 0, -r*.22f, r*.25f);
    nvgFillColor(vg, black); nvgFill(vg);
    // White muzzle
    nvgBeginPath(vg); nvgEllipse(vg, 0, -r*.12f, r*.13f, r*.09f);
    nvgFillColor(vg, white); nvgFill(vg);

    // Left ear (black)
    nvgBeginPath(vg);
    nvgMoveTo(vg, -r*.16f, -r*.44f); nvgLineTo(vg, -r*.30f, -r*.62f); nvgLineTo(vg, -r*.04f, -r*.47f);
    nvgClosePath(vg); nvgFillColor(vg, black); nvgFill(vg);
    // Right ear (black)
    nvgBeginPath(vg);
    nvgMoveTo(vg, r*.16f, -r*.44f); nvgLineTo(vg, r*.30f, -r*.62f); nvgLineTo(vg, r*.04f, -r*.47f);
    nvgClosePath(vg); nvgFillColor(vg, black); nvgFill(vg);
    // Inner ears (pink)
    nvgBeginPath(vg);
    nvgMoveTo(vg, -r*.17f, -r*.46f); nvgLineTo(vg, -r*.26f, -r*.58f); nvgLineTo(vg, -r*.08f, -r*.49f);
    nvgClosePath(vg); nvgFillColor(vg, pink); nvgFill(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, r*.17f, -r*.46f); nvgLineTo(vg, r*.26f, -r*.58f); nvgLineTo(vg, r*.08f, -r*.49f);
    nvgClosePath(vg); nvgFillColor(vg, pink); nvgFill(vg);

    // Tail (black)
    nvgBeginPath(vg);
    nvgMoveTo(vg, r*.40f, r*.20f);
    nvgBezierTo(vg, r*.65f, r*.10f, r*.70f, -r*.25f, r*.50f, -r*.45f);
    nvgStrokeColor(vg, black); nvgStrokeWidth(vg, std::max(1.f, r*.1f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
}

static void pDrawDog(NVGcontext* vg, float r) {
    NVGcolor col  = nvgRGBAf(.70f, .48f, .22f, .88f);
    NVGcolor dark = nvgRGBAf(.48f, .30f, .10f, .88f);
    NVGcolor snout= nvgRGBAf(.88f, .70f, .45f, .88f);

    // Body
    nvgBeginPath(vg); nvgEllipse(vg, 0, r*.15f, r*.45f, r*.28f);
    nvgFillColor(vg, col); nvgFill(vg);
    // Head
    nvgBeginPath(vg); nvgCircle(vg, 0, -r*.20f, r*.26f);
    nvgFillColor(vg, col); nvgFill(vg);
    // Left floppy ear
    nvgBeginPath(vg); nvgEllipse(vg, -r*.24f, -r*.10f, r*.10f, r*.20f);
    nvgFillColor(vg, dark); nvgFill(vg);
    // Right floppy ear
    nvgBeginPath(vg); nvgEllipse(vg, r*.24f, -r*.10f, r*.10f, r*.20f);
    nvgFillColor(vg, dark); nvgFill(vg);
    // Snout
    nvgBeginPath(vg); nvgEllipse(vg, 0, -r*.08f, r*.14f, r*.09f);
    nvgFillColor(vg, snout); nvgFill(vg);
    // Nose dot
    nvgBeginPath(vg); nvgCircle(vg, 0, -r*.13f, r*.04f);
    nvgFillColor(vg, nvgRGBAf(.15f,.1f,.05f,.9f)); nvgFill(vg);
    // Tail
    nvgBeginPath(vg);
    nvgMoveTo(vg, r*.42f, r*.10f);
    nvgBezierTo(vg, r*.58f, r*.0f, r*.55f, -r*.20f, r*.42f, -r*.30f);
    nvgStrokeColor(vg, col); nvgStrokeWidth(vg, std::max(1.f, r*.1f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
}

static void pDrawCatsAndDogs(NVGcontext* vg, float r, float colorHue) {
    if (colorHue < .5f) pDrawCat(vg, r * 1.45f);
    else                pDrawDog(vg, r * 1.70f);
}

// Confetti: brightly colored square (hue varies per particle)
static void pDrawConfetti(NVGcontext* vg, float r, float colorHue) {
    float s = r * .78f;
    nvgBeginPath(vg); nvgRect(vg, -s, -s, s*2, s*2);
    nvgFillColor(vg, nvgHSLA(colorHue, 1.f, .58f, 210)); nvgFill(vg);
    // White highlight strip
    nvgBeginPath(vg); nvgRect(vg, -s, -s, s*2, s*.35f);
    nvgFillColor(vg, nvgRGBAf(1.f, 1.f, 1.f, .22f)); nvgFill(vg);
}

// Men: stick figure in a spread-eagle falling pose
static void pDrawMan(NVGcontext* vg, float r, float) {
    NVGcolor col = nvgRGBAf(1.f, 1.f, 1.f, .90f);
    float sw = std::max(1.5f, r * .13f);
    nvgStrokeColor(vg, col);
    nvgStrokeWidth(vg, sw);
    nvgLineCap(vg, NVG_ROUND);

    // Head
    nvgBeginPath(vg); nvgCircle(vg, 0, -r*.58f, r*.18f);
    nvgFillColor(vg, col); nvgFill(vg);

    // Torso
    nvgBeginPath(vg); nvgMoveTo(vg, 0, -r*.40f); nvgLineTo(vg, 0, r*.18f); nvgStroke(vg);

    // Left arm (raised, spread out)
    nvgBeginPath(vg); nvgMoveTo(vg, 0, -r*.24f); nvgLineTo(vg, -r*.42f, -r*.52f); nvgStroke(vg);
    // Right arm
    nvgBeginPath(vg); nvgMoveTo(vg, 0, -r*.24f); nvgLineTo(vg,  r*.42f, -r*.52f); nvgStroke(vg);

    // Left leg
    nvgBeginPath(vg); nvgMoveTo(vg, 0, r*.18f); nvgLineTo(vg, -r*.36f, r*.62f); nvgStroke(vg);
    // Right leg
    nvgBeginPath(vg); nvgMoveTo(vg, 0, r*.18f); nvgLineTo(vg,  r*.36f, r*.62f); nvgStroke(vg);
}

// Acid rain: neon-green elongated drop, glowing
static void pDrawAcidRain(NVGcontext* vg, float r, float) {
    float w = r * .38f, bot = r * .45f;
    // Glow halo
    NVGpaint glow = nvgRadialGradient(vg, 0, 0, r*.1f, r*.9f,
        nvgRGBAf(.2f, 1.f, .1f, .55f), nvgRGBAf(.1f, .8f, .0f, 0.f));
    nvgBeginPath(vg); nvgEllipse(vg, 0, 0, r*.9f, r*.9f);
    nvgFillPaint(vg, glow); nvgFill(vg);
    // Drop body: tip at top, round at bottom (same orientation as raindrop)
    nvgBeginPath(vg);
    nvgMoveTo(vg, 0, -r);
    nvgBezierTo(vg,  w, -r*.2f,  w, bot, 0, bot);
    nvgBezierTo(vg, -w,  bot,   -w, -r*.2f, 0, -r);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(.18f, 1.f, .08f, .88f)); nvgFill(vg);
    // Inner highlight streak
    nvgBeginPath(vg);
    nvgMoveTo(vg, r*.08f, -r*.6f); nvgLineTo(vg, r*.08f, r*.05f);
    nvgStrokeColor(vg, nvgRGBAf(.7f, 1.f, .6f, .45f));
    nvgStrokeWidth(vg, std::max(.5f, r*.06f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
}

// Polka dot: solid filled circle, hue varies per particle
static void pDrawPolkaDot(NVGcontext* vg, float r, float colorHue) {
    nvgBeginPath(vg); nvgCircle(vg, 0, 0, r);
    nvgFillColor(vg, nvgHSLA(colorHue, 1.f, .55f, 218)); nvgFill(vg);
}

static void pDrawPetal(NVGcontext* vg, float r, float colorHue) {
    // Pointed tip at top, rounded base at bottom — classic petal silhouette
    float w = r * .42f;
    nvgBeginPath(vg);
    nvgMoveTo(vg, 0, -r);                               // pointed tip
    nvgBezierTo(vg,  w, -r*.55f,  w*.9f,  r*.30f, 0,  r*.58f); // right curve, round base
    nvgBezierTo(vg, -w*.9f, r*.30f, -w, -r*.55f, 0, -r);        // left curve, back to tip
    nvgClosePath(vg);
    // Vary hue gently from pink to lavender using colorHue
    NVGcolor fill = nvgHSLA(0.88f + colorHue * 0.08f, .85f, .72f, 210);
    nvgFillColor(vg, fill); nvgFill(vg);
    // Centre vein
    nvgBeginPath(vg);
    nvgMoveTo(vg, 0, -r*.82f); nvgLineTo(vg, 0, r*.42f);
    nvgStrokeColor(vg, nvgRGBAf(1.f, .80f, .90f, .40f));
    nvgStrokeWidth(vg, std::max(.5f, r*.06f));
    nvgLineCap(vg, NVG_ROUND); nvgStroke(vg);
}

// Meteor: head at origin, tail trailing upward.
// The overlay rotates this to align with velocity direction before calling.
static void pDrawMeteor(NVGcontext* vg, float r, float) {
    float tl = r * 3.8f;
    NVGpaint tail = nvgLinearGradient(vg, 0, 0, 0, -tl,
        nvgRGBAf(1.f, .55f, .05f, .85f), nvgRGBAf(1.f, .2f, .0f, 0.f));
    nvgBeginPath(vg);
    nvgMoveTo(vg, -r*.22f, 0); nvgLineTo(vg, r*.22f, 0); nvgLineTo(vg, 0, -tl);
    nvgClosePath(vg); nvgFillPaint(vg, tail); nvgFill(vg);
    nvgBeginPath(vg); nvgCircle(vg, 0, 0, r*.44f);
    nvgFillColor(vg, nvgRGBAf(.92f, .08f, .05f, 1.f)); nvgFill(vg);
}

// Ash: irregular grayscale polygon, shape and shade stable per particle via colorHue seed.
static void pDrawAsh(NVGcontext* vg, float r, float colorHue) {
    float gray  = .30f + colorHue * .52f;           // dark ash → lighter ash
    float alpha = .45f + colorHue * .40f;

    int nsides = 4 + (int)(colorHue * 3.99f);       // 4-7 sides

    nvgBeginPath(vg);
    for (int i = 0; i < nsides; i++) {
        // Stable per-vertex offsets derived from colorHue
        float t1 = fmodf(colorHue * (i * 11.37f + 3.71f), 1.f);
        float t2 = fmodf(colorHue * (i *  7.29f + 1.93f), 1.f);
        float angle  = (float)i / nsides * 2.f * M_PI + (t1 - .5f) * M_PI / nsides;
        float radius = r * (.55f + t2 * .45f);
        if (i == 0) nvgMoveTo(vg, radius * cosf(angle), radius * sinf(angle));
        else        nvgLineTo(vg, radius * cosf(angle), radius * sinf(angle));
    }
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBAf(gray, gray, gray, alpha)); nvgFill(vg);
}

using DrawFn = void (*)(NVGcontext*, float, float);
static const DrawFn DRAW_TABLE[NUM_TYPES] = {
    pDrawSnowflake,   // 0  Snowflake
    pDrawRaindrop,    // 1  Raindrop
    pDrawAcidRain,    // 2  Acid Rain
    pDrawLeaf,        // 3  Leaf
    pDrawPetal,       // 4  Petal
    pDrawAsh,         // 5  Ash
    pDrawEmber,       // 6  Ember
    pDrawStar,        // 7  Star
    pDrawMeteor,      // 8  Meteor
    pDrawBubble,      // 9  Bubble
    pDrawConfetti,    // 10 Confetti
    pDrawPolkaDot,    // 11 Polka Dots
    pDrawHeart,       // 12 Heart
    pDrawMan,         // 13 Men
    pDrawCatsAndDogs, // 14 Cats and Dogs
    pDrawFastFood,    // 15 Fast Food
};

// Types that should be direction-locked to velocity angle instead of random spin
static bool isDirectionLocked(int type) {
    return type == 1  // Raindrop
        || type == 2  // Acid Rain
        || type == 8; // Meteor
}

// ── overlay widget ─────────────────────────────────────────────────────────

struct ParticleOverlay : Widget {
    // Back-pointer: if the scene deletes us before ParticlesWidget does,
    // we null this out so the widget destructor knows not to double-free.
    ParticleOverlay** ownerPtr = nullptr;

    ~ParticleOverlay() override {
        if (ownerPtr) *ownerPtr = nullptr;
    }

    std::vector<Particle> particles;
    bool  active           = false;
    bool  showMouseTracker = true;
    float particleOpacity  = 1.f;
    Vec   mousePos         = {};
    float alpha         = 0.f;
    float size          = 8.f;
    float speed         = 120.f;
    float drift         = 0.f;
    int   ptype         = 0;
    int   numActive     = NUM_PARTICLES / 2;
    int   prevNumActive = 0;
    bool  needsReset    = true;
    double prevTime     = 0.0;

    ParticleOverlay() {
        box.pos = Vec(0, 0);
        particles.resize(NUM_PARTICLES);
    }

    void initParticle(Particle& p, float w, float h, bool randomY) {
        p.x         = random::uniform() * w;
        p.y         = randomY ? random::uniform() * h
                              : -size * 2.f - random::uniform() * h * .1f;
        p.rot       = random::uniform() * 2.f * M_PI;
        p.rotSpeed  = (random::uniform() - .5f) * 6.f;
        p.wobblePhase = random::uniform() * 2.f * M_PI;
        p.wobbleFreq  = .15f + random::uniform() * 1.85f;
        float u       = random::uniform();
        p.wobbleAmp   = u * u * 45.f;                     // skewed low, max 45 px
        p.speedMult   = .35f + random::uniform() * 1.3f;  // 0.35–1.65×
        p.sizeMult    = .55f + random::uniform() * .9f;   // 0.55–1.45×
        p.colorHue    = random::uniform();                 // [0,1] for color types
    }

    void resetParticles(float w, float h) {
        for (auto& p : particles) initParticle(p, w, h, true);
        needsReset = false;
    }

    void step() override {
        Widget::step();
        if (APP && APP->scene) box.size = APP->scene->box.size;

        // Track cursor in overlay-local (scene) coordinates
        if (APP && APP->window && APP->window->win) {
            double xpos, ypos;
            glfwGetCursorPos(APP->window->win, &xpos, &ypos);
            float scale = APP->window->pixelRatio;
            mousePos = Vec((float)xpos / scale, (float)ypos / scale);
        }

        double t  = glfwGetTime();
        float  dt = (prevTime > 0.0) ? clamp((float)(t - prevTime), 0.f, 0.05f) : 0.f;
        prevTime  = t;

        float target = active ? 1.f : 0.f;
        float rate   = 3.f;
        if (alpha < target) alpha = std::min(target, alpha + rate * dt);
        else if (alpha > target) alpha = std::max(target, alpha - rate * dt);

        if (alpha <= 0.f) return;

        float w = box.size.x, h = box.size.y;
        if (needsReset) { resetParticles(w, h); return; }

        // Reinit particles newly brought into the active range
        if (numActive > prevNumActive) {
            for (int i = prevNumActive; i < numActive; i++)
                initParticle(particles[i], w, h, false);
        }
        prevNumActive = numActive;

        float pad = size * 4.f;
        // Wobble: three mutually irrational frequencies per particle with mixed
        // sin/cos and independent phase offsets → organic, non-repeating lateral drift.
        float wobbleScale = 0.15f + (speed / 500.f) * 0.35f;
        float tf = (float)t;
        for (int i = 0; i < numActive; i++) {
            Particle& p = particles[i];
            float f1 = p.wobbleFreq;
            float f2 = p.wobbleFreq * 1.618f;  // golden ratio
            float f3 = p.wobbleFreq * 2.732f;  // 1 + sqrt(3)
            float wv = p.wobbleAmp * wobbleScale * (
                0.45f * f1 * 2.f * M_PI * cosf(2.f * M_PI * f1 * tf + p.wobblePhase) +
                0.35f * f2 * 2.f * M_PI * sinf(2.f * M_PI * f2 * tf + p.wobblePhase * 2.09f) +
                0.20f * f3 * 2.f * M_PI * cosf(2.f * M_PI * f3 * tf + p.wobblePhase * 4.33f)
            );
            p.x   += (drift + wv) * dt;
            p.y   += speed * p.speedMult * dt;
            p.rot += p.rotSpeed * dt;

            if (p.x < -pad)    p.x += w + pad * 2.f;
            if (p.x > w + pad) p.x -= w + pad * 2.f;
            if (p.y > h + pad) initParticle(p, w, h, false);
        }
    }

    void draw(const DrawArgs& args) override {
        if (alpha <= 0.f) return;
        NVGcontext* vg = args.vg;

        // Direction-locked types rotate to match velocity angle so they
        // always face the way they're actually travelling.
        bool locked = isDirectionLocked(ptype);

        nvgSave(vg);
        nvgGlobalAlpha(vg, alpha * particleOpacity);
        for (int i = 0; i < numActive; i++) {
            const Particle& p = particles[i];
            float r   = size * p.sizeMult;
            // Rotation formula for direction-locked types:
            //   atan2f(-drift, speed*mult) orients the shape so its "forward"
            //   tip faces the direction of travel in NVG's y-down coord system.
            float rot = locked
                ? atan2f(-drift, speed * p.speedMult)
                : p.rot;
            nvgSave(vg);
            nvgTranslate(vg, p.x, p.y);
            nvgRotate(vg, rot);
            DRAW_TABLE[ptype](vg, r, p.colorHue);
            nvgRestore(vg);
        }
        nvgRestore(vg);

        // Mouse tracker ring — drawn at full opacity regardless of particle fade
        if (showMouseTracker) {
            nvgSave(vg);
            // Outer ring
            nvgBeginPath(vg);
            nvgCircle(vg, mousePos.x, mousePos.y, 24.f);
            nvgStrokeColor(vg, nvgRGBAf(0.f, 1.f, 0.f, .90f));
            nvgStrokeWidth(vg, 3.5f);
            nvgStroke(vg);
            // Inner dot
            nvgBeginPath(vg);
            nvgCircle(vg, mousePos.x, mousePos.y, 3.f);
            nvgFillColor(vg, nvgRGBAf(0.f, 1.f, 0.f, .90f));
            nvgFill(vg);
            nvgRestore(vg);
        }
    }
};

// ── module ─────────────────────────────────────────────────────────────────

struct Particles : Module {
    enum ParamId {
        TYPE_PARAM,
        SIZE_PARAM,
        SPEED_PARAM,
        DENSITY_PARAM,
        ANGLE_PARAM,
        ONOFF_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        TYPECV_INPUT,
        SIZECV_INPUT,
        SPEEDCV_INPUT,
        DENSITYCV_INPUT,
        ANGLECV_INPUT,
        ONOFFCV_INPUT,
        INPUTS_LEN
    };
    enum OutputId { OUTPUTS_LEN };
    enum LightId  { ONOFFLED_LIGHT, LIGHTS_LEN };

    bool  showMouseTracker = true;
    float particleOpacity  = 1.f;   // 0.1–1.0

    json_t* dataToJson() override {
        json_t* rootJ = json_object();
        json_object_set_new(rootJ, "showMouseTracker", json_boolean(showMouseTracker));
        json_object_set_new(rootJ, "particleOpacity",  json_real(particleOpacity));
        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override {
        json_t* v = json_object_get(rootJ, "showMouseTracker");
        if (v) showMouseTracker = json_boolean_value(v);
        json_t* o = json_object_get(rootJ, "particleOpacity");
        if (o) particleOpacity = clamp((float)json_real_value(o), 0.1f, 1.f);
    }

    Particles() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configSwitch(TYPE_PARAM, 0.f, 15.f, 0.f, "Type",
            {"Snowflake","Raindrop","Acid Rain","Leaf","Petal","Ash","Ember",
             "Star","Meteor","Bubble","Confetti","Polka Dots",
             "Heart","Men","Cats and Dogs","Fast Food"});
        configParam(SIZE_PARAM,    0.f, 1.f, 0.25f, "Size",      "%", 0.f, 100.f);
        configParam(SPEED_PARAM,   0.f, 1.f, 0.15f, "Gravity",   "%", 0.f, 100.f);
        configParam(DENSITY_PARAM, 0.f, 1.f, 0.50f, "Density",   "%", 0.f, 100.f);
        configParam(ANGLE_PARAM,   0.f, 1.f, 0.60f, "Direction", "%", 0.f, 100.f);
        configSwitch(ONOFF_PARAM,  0.f, 1.f, 0.f,  "On/Off", {"Off", "On"});
        getParamQuantity(ONOFF_PARAM)->randomizeEnabled = false;
        configInput(TYPECV_INPUT,    "Type CV");
        configInput(SIZECV_INPUT,    "Size CV");
        configInput(SPEEDCV_INPUT,   "Gravity CV");
        configInput(DENSITYCV_INPUT, "Density CV");
        configInput(ANGLECV_INPUT,   "Direction CV");
        configInput(ONOFFCV_INPUT,   "On/Off CV");
    }

    void process(const ProcessArgs& args) override {
        bool on = params[ONOFF_PARAM].getValue() > 0.5f;
        lights[ONOFFLED_LIGHT].setBrightnessSmooth(on ? 1.f : 0.f, args.sampleTime, 3.f);
    }
};

// ── widget ─────────────────────────────────────────────────────────────────

// Non-momentary LEDBezel so each click latches on/off
struct LEDBezelToggle : LEDBezel {
    LEDBezelToggle() { momentary = false; }
};

struct ParticlesWidget : ModuleWidget {
    ParticleOverlay* overlay  = nullptr;
    bool prevOnOffCV = false;  // tracks last gate state for rising-edge detection

    ParticlesWidget(Particles* module) {
        setModule(module);
        setPanel(createPanel(
            asset::plugin(pluginInstance, "res/panels/Particles.svg"),
            asset::plugin(pluginInstance, "res/panels/Particles-dark.svg")
        ));

        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // On/Off: physical LED push button (latch) + LED light overlay
        Vec btnPos = mm2px(Vec(29.616, 95.619));
        addParam(createParamCentered<LEDBezelToggle>(btnPos, module, Particles::ONOFF_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(btnPos, module, Particles::ONOFFLED_LIGHT));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.024, 20.645)), module, Particles::TYPE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.616, 20.645)), module, Particles::SIZE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.024, 58.132)), module, Particles::SPEED_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.024, 95.619)), module, Particles::DENSITY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.616, 58.142)), module, Particles::ANGLE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.024, 38.854)), module, Particles::TYPECV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.616, 38.854)), module, Particles::SIZECV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.024, 76.341)), module, Particles::SPEEDCV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.024, 113.828)), module, Particles::DENSITYCV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.616, 76.351)), module, Particles::ANGLECV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.616, 113.828)), module, Particles::ONOFFCV_INPUT));

        // Insert before menuBar so particles render above rack but behind browser/menu
        overlay = new ParticleOverlay;
        overlay->ownerPtr = &overlay;
        if (APP && APP->scene) {
            if (APP->scene->menuBar) {
                APP->scene->addChildBelow(overlay, APP->scene->menuBar);
            } else {
                APP->scene->addChild(overlay);
            }
        }
    }

    ~ParticlesWidget() {
        if (overlay) {
            overlay->ownerPtr = nullptr;  // we're taking over, disable the back-pointer
            if (APP && APP->scene) APP->scene->removeChild(overlay);
            delete overlay;
            overlay = nullptr;
        }
    }

    void step() override {
        ModuleWidget::step();
        if (!overlay) return;

        Particles* p = dynamic_cast<Particles*>(module);
        if (!p) { overlay->active = false; return; }

        bool on = p->params[Particles::ONOFF_PARAM].getValue() > .5f;
        if (p->inputs[Particles::ONOFFCV_INPUT].isConnected()) {
            bool gateHigh = p->inputs[Particles::ONOFFCV_INPUT].getVoltage() > 1.f;
            if (gateHigh && !prevOnOffCV) {
                // Rising edge: toggle the button state
                on = !on;
                APP->engine->setParamValue(p, Particles::ONOFF_PARAM, on ? 1.f : 0.f);
            }
            prevOnOffCV = gateHigh;
        } else {
            prevOnOffCV = false;
        }
        if (on && overlay->alpha <= 0.f) overlay->needsReset = true;
        overlay->active = on;

        if (!on && overlay->alpha <= 0.f) return;

        // Type (0–15)
        float typeKnob = p->params[Particles::TYPE_PARAM].getValue();
        float typeCV   = p->inputs[Particles::TYPECV_INPUT].isConnected()
            ? p->inputs[Particles::TYPECV_INPUT].getVoltage() / 10.f : 0.f;
        float typeCtrl = clamp(typeKnob / 15.f + typeCV, 0.f, 1.f);
        overlay->ptype = clamp((int)(typeCtrl * 15.f + .5f), 0, NUM_TYPES - 1);

        // Size: 1–60 px base radius
        float sizeKnob = p->params[Particles::SIZE_PARAM].getValue();
        float sizeCV   = p->inputs[Particles::SIZECV_INPUT].isConnected()
            ? p->inputs[Particles::SIZECV_INPUT].getVoltage() / 10.f : 0.f;
        overlay->size = 1.f + clamp(sizeKnob + sizeCV, 0.f, 1.f) * 29.f;

        // Gravity: 10–1000 px/s downward only (Y axis)
        float speedKnob = p->params[Particles::SPEED_PARAM].getValue();
        float speedCV   = p->inputs[Particles::SPEEDCV_INPUT].isConnected()
            ? p->inputs[Particles::SPEEDCV_INPUT].getVoltage() / 10.f : 0.f;
        overlay->speed = 10.f + clamp(speedKnob + speedCV, 0.f, 1.f) * 990.f;

        // Direction: ±600 px/s horizontal drift only (X axis)
        float angleKnob = p->params[Particles::ANGLE_PARAM].getValue();
        float angleCV   = p->inputs[Particles::ANGLECV_INPUT].isConnected()
            ? p->inputs[Particles::ANGLECV_INPUT].getVoltage() / 10.f : 0.f;
        overlay->drift = (clamp(angleKnob + angleCV, 0.f, 1.f) - .5f) * 1200.f;

        // Density: 10–400 particles (knob centre ≈ 200, the original count)
        float densityKnob = p->params[Particles::DENSITY_PARAM].getValue();
        float densityCV   = p->inputs[Particles::DENSITYCV_INPUT].isConnected()
            ? p->inputs[Particles::DENSITYCV_INPUT].getVoltage() / 10.f : 0.f;
        float densityCtrl = clamp(densityKnob + densityCV, 0.f, 1.f);
        overlay->numActive = clamp((int)(10 + densityCtrl * 790.f), 1, NUM_PARTICLES);
        overlay->showMouseTracker = p->showMouseTracker;
        overlay->particleOpacity  = p->particleOpacity;
    }

    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        Particles* p = dynamic_cast<Particles*>(module);
        if (!p) return;

        menu->addChild(new MenuSeparator);

        // Opacity slider
        struct OpacityQuantity : Quantity {
            Particles* module;
            OpacityQuantity(Particles* m) : module(m) {}
            void setValue(float v) override { module->particleOpacity = clamp(v, 0.1f, 1.f); }
            float getValue() override { return module->particleOpacity; }
            float getDefaultValue() override { return 1.f; }
            float getMinValue() override { return 0.1f; }
            float getMaxValue() override { return 1.f; }
            float getDisplayValue() override { return getValue() * 100.f; }
            void setDisplayValue(float v) override { setValue(v / 100.f); }
            std::string getLabel() override { return "Particle Opacity"; }
            std::string getUnit() override { return "%"; }
        };
        struct OpacitySlider : ui::Slider {
            OpacitySlider(Particles* m) {
                quantity = new OpacityQuantity(m);
                box.size.x = 200.f;
            }
            ~OpacitySlider() { delete quantity; }
        };
        menu->addChild(new OpacitySlider(p));

        menu->addChild(createBoolMenuItem("Mouse Tracker", "",
            [=]() { return p->showMouseTracker; },
            [=](bool v) { p->showMouseTracker = v; }
        ));
    }
};


Model* modelParticles = createModel<Particles, ParticlesWidget>("Particles");
