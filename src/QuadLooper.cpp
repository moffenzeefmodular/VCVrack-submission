#include "plugin.hpp"

struct QuadLooper : Module {
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

    QuadLooper() {
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

struct QuadLooperXYDisplay : Widget {
    QuadLooper *module;
    Vec dragPos;
    bool dragging = false;

    void updateFromPos() {
        // Clamp dragPos inside pad
        dragPos.x = clamp(dragPos.x, 0.f, box.size.x);
        dragPos.y = clamp(dragPos.y, 0.f, box.size.y);

        // Update module params
        float x = dragPos.x / box.size.x;
        float y = 1.f - (dragPos.y / box.size.y);

        module->params[QuadLooper::X_POS_PARAM].setValue(x);
        module->params[QuadLooper::Y_POS_PARAM].setValue(y);
    }

    void onButton(const event::Button &e) override {
        if (!module) return;
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            dragging = true;

            // Compute local dragPos directly from event
            dragPos = e.pos;   // e.pos is already relative to this widget
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

        // Add delta directly (scaled by zoom)
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

        float px = module->params[QuadLooper::X_POS_PARAM].getValue() * box.size.x;
        float py = (1.f - module->params[QuadLooper::Y_POS_PARAM].getValue()) * box.size.y;

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

        // Dot
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, px, py, radius);
        nvgFillColor(args.vg, nvgRGB(230, 230, 230));
        nvgFill(args.vg);
    }
};



///////////////////////////
// MODULE WIDGET SETUP //
///////////////////////////

struct QuadLooperWidget : ModuleWidget {
    QuadLooperWidget(QuadLooper *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/panels/QuadLooper.svg")));

        // === 60x60 XY Pad, centered ===
        auto xy = new QuadLooperXYDisplay();
        xy->module = module;

        float squareSize = 60.f; // 60 x 60 mm
        float xOffset = (152.4f - squareSize) / 2.f; // ~46.2 mm
        float yOffset = (128.5f - squareSize) / 2.f; // ~34.25 mm

        xy->box.pos = mm2px(Vec(xOffset, yOffset));
        xy->box.size = mm2px(Vec(squareSize, squareSize));
        addChild(xy);

        // Outputs below the XY pad
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50, 110)), module, QuadLooper::X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(100, 110)), module, QuadLooper::Y_OUTPUT));
    }
};

Model* modelQuadLooper = createModel<QuadLooper, QuadLooperWidget>("QuadLooper");
