#include "plugin.hpp"

struct QUADLOOPERXY : Module {
    enum ParamIds {
        X_POS_PARAM,
        Y_POS_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        X_OUTPUT,
        Y_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    QUADLOOPERXY() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(X_POS_PARAM, 0.f, 1.f, 0.f, "X Position");
        configParam(Y_POS_PARAM, 0.f, 1.f, 0.f, "Y Position");
    }

    void process(const ProcessArgs &args) override {
        float x = params[X_POS_PARAM].getValue();
        float y = params[Y_POS_PARAM].getValue();

        outputs[X_OUTPUT].setVoltage(x * 10.f);
        outputs[Y_OUTPUT].setVoltage(y * 10.f);
    }
};

////////////////////////////////
// XY PAD DISPLAY
////////////////////////////////
struct QuadLooperXYDisplay : Widget {
    QUADLOOPERXY *module;
    Vec dragPos;
    bool dragging = false;

    void updateFromPos() {
        dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
        dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

        float x = dragPos.x / box.size.x;
        float y = 1.f - (dragPos.y / box.size.y);

        module->params[QUADLOOPERXY::X_POS_PARAM].setValue(x);
        module->params[QUADLOOPERXY::Y_POS_PARAM].setValue(y);
    }

    void onButton(const event::Button &e) override {
        if (!module) return;
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            dragging = true;

            // e.pos is already relative to this widget
            dragPos = e.pos;
            dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
            dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

            updateFromPos();
            e.consume(this);
        }
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE) {
            dragging = false;
        }
    }

    void onDragMove(const event::DragMove &e) override {
        if (!module || !dragging) return;

        dragPos = dragPos.plus(e.mouseDelta.div(getAbsoluteZoom()));

        dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
        dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

        updateFromPos();
    }

    void draw(const DrawArgs &args) override {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, nvgRGB(18, 18, 18));
        nvgFill(args.vg);

        if (!module) return;

        float radius = 6.f;

        float px = module->params[QUADLOOPERXY::X_POS_PARAM].getValue() * box.size.x;
        float py = (1.f - module->params[QUADLOOPERXY::Y_POS_PARAM].getValue()) * box.size.y;

        px = clamp(px, radius, box.size.x - radius);
        py = clamp(py, radius, box.size.y - radius);

        // Crosshair
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, px, 0);
        nvgLineTo(args.vg, px, box.size.y);
        nvgMoveTo(args.vg, 0, py);
        nvgLineTo(args.vg, box.size.x, py);
        nvgStrokeColor(args.vg, nvgRGB(60, 60, 60));
        nvgStroke(args.vg);

        // Cursor dot
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, px, py, radius);
        nvgFillColor(args.vg, nvgRGB(230, 230, 230));
        nvgFill(args.vg);
    }
};

////////////////////////////////
// MODULE WIDGET
////////////////////////////////
struct QuadLooperWidget : ModuleWidget {
    QuadLooperWidget(QUADLOOPERXY *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/panels/QUADLOOPERXY.svg")));

        // === 40x40 XY Pad, centered ===
        auto xy = new QuadLooperXYDisplay();
        xy->module = module;

        float squareSize = 40.f; // 40 x 40 mm
        float xOffset = (152.4f - squareSize) / 2.f; // center horizontally
        float yOffset = (128.5f - squareSize) / 2.f; // center vertically

        xy->box.pos = mm2px(Vec(xOffset, yOffset));
        xy->box.size = mm2px(Vec(squareSize, squareSize));
        addChild(xy);

        // Outputs below the XY pad, relative to pad
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(56.2f, 95.f)), module, QUADLOOPERXY::X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(96.2f, 95.f)), module, QUADLOOPERXY::Y_OUTPUT));
    }
};

Model* modelQuadLooper = createModel<QUADLOOPERXY, QuadLooperWidget>("QUADLOOPERXY");
