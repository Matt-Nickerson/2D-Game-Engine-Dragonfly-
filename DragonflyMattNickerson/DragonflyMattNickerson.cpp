#include <cmath>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "LogManager.h"
#include "WorldManager.h"
#include "GameManager.h"
#include "InputManager.h"
#include "DisplayManager.h"

#include "Vector.h"
#include "Object.h"
#include "ObjectList.h"
#include "EventStep.h"
#include "EventOut.h"
#include "EventCollision.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "Clock.h"

// ====== Test Config ======
#define RUN_MANUAL_INPUT_TEST 0  // set to 1 to manually test keyboard/mouse

// ---------- test helpers ----------
static int g_pass = 0, g_fail = 0;
#define TEST_ASSERT(cond, msg) \
  do { \
    if (cond) { df::LogManager::getInstance().writeLog("[PASS] %s\n", msg); ++g_pass; } \
    else      { df::LogManager::getInstance().writeLog("[FAIL] %s\n", msg); ++g_fail; } \
  } while(0)

static void sleep_ms(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

// ---------- Vector tests ----------
static void test_Vector() {
    df::LogManager::getInstance().writeLog("== Vector tests ==\n");

    Vector v0; TEST_ASSERT(v0.getX() == 0.f && v0.getY() == 0.f, "default (0,0)");
    Vector v(3.f, 4.f);
    TEST_ASSERT(std::fabs(v.getMagnitude() - 5.f) < 1e-5f, "magnitude 3-4-5");
    v.normalize(); TEST_ASSERT(std::fabs(v.getMagnitude() - 1.f) < 1e-4f, "normalize => |v|=1");
    v.scale(10.f); TEST_ASSERT(std::fabs(v.getMagnitude() - 10.f) < 1e-3f, "scale by 10");
    Vector w(1.f, 2.f); Vector s = v + w;
    TEST_ASSERT(std::fabs(s.getX() - (v.getX() + 1.f)) < 1e-5f && std::fabs(s.getY() - (v.getY() + 2.f)) < 1e-5f, "operator+");
    v.setXY(7.f, -2.f); TEST_ASSERT(v.getX() == 7.f && v.getY() == -2.f, "setXY");
}

// ---------- Clock tests ----------
static void test_Clock() {
    df::LogManager::getInstance().writeLog("== Clock tests ==\n");
    df::Clock c;
    sleep_ms(3);
    long long d1 = c.delta();  // reset baseline
    TEST_ASSERT(d1 >= 2000, "delta() >= ~2ms after sleep(3ms)");
    sleep_ms(2);
    long long s1 = c.split();  // does not reset
    TEST_ASSERT(s1 >= 1500, "split() >= ~1.5ms after sleep(2ms)");
}

// ---------- Object & ObjectList tests ----------
class DummyObj : public Object {
public:
    DummyObj(const Vector& p) {
        setType("Dummy");
        setPosition(p);
        // Default sensible values
        setSolidness(Solidness::HARD);
        setAltitude(0);
        setVelocityX(0.f);
        setVelocityY(0.f);
    }
};

static void test_Object_and_ObjectList() {
    df::LogManager::getInstance().writeLog("== Object & ObjectList tests ==\n");

    // Object id/type/position
    DummyObj* a = new DummyObj(Vector(1.f, 2.f));
    DummyObj* b = new DummyObj(Vector(5.f, 9.f));
    TEST_ASSERT(a->getId() != b->getId(), "unique Object ids");
    TEST_ASSERT(a->getType() == "Dummy", "type set/get");
    a->setPosition(Vector(3.f, 4.f));
    TEST_ASSERT(std::fabs(a->getPosition().getX() - 3.f) < 1e-5f && std::fabs(a->getPosition().getY() - 4.f) < 1e-5f, "position set/get");

    // NEW: solidness, altitude, velocity
    a->setSolidness(Solidness::SOFT);
    TEST_ASSERT(a->getSolidness() == Solidness::SOFT, "solidness get/set");
    a->setAltitude(7);
    TEST_ASSERT(a->getAltitude() == 7, "altitude get/set");
    a->setVelocityX(1.5f); a->setVelocityY(-0.5f);
    TEST_ASSERT(std::fabs(a->getVelocityX() - 1.5f) < 1e-6f && std::fabs(a->getVelocityY() + 0.5f) < 1e-6f,
        "velocity x/y get/set");

    // Raw ObjectList behavior
    ObjectList L;
    TEST_ASSERT(L.getCount() == 0, "ObjectList starts empty");
    TEST_ASSERT(L.insert(a) == 0 && L.insert(b) == 0 && L.getCount() == 2, "insert() 2 objects");
    TEST_ASSERT(L[0] == a && L[1] == b, "operator[] returns inserted pointers");
    TEST_ASSERT(L.remove(a) == 0 && L.getCount() == 1, "remove() existing");
    TEST_ASSERT(L.remove(a) == -1, "remove() non-existent returns -1");
    L.clear(); TEST_ASSERT(L.getCount() == 0, "clear() empties list");

    // WorldManager add/remove via Object helpers
    int before = WM().getAllObjects().getCount();
    DummyObj* c = new DummyObj(Vector(0.f, 0.f)); // auto add (if your Object() adds itself)
    int after = WM().getAllObjects().getCount();
    TEST_ASSERT(after == before + 1, "addToWorld (via ctor) increased world count");

    c->markForDelete(); WM().update();
    int afterDel = WM().getAllObjects().getCount();
    TEST_ASSERT(afterDel == before, "markForDelete + update removes object");

    // cleanup
    a->markForDelete(); b->markForDelete(); WM().update();
}

// ---------- WorldManager movement/bounds/collision ----------
class CollisionProbe : public Object {
public:
    int out_count = 0;
    int col_count = 0;

    CollisionProbe(const char* t, const Vector& p, Solidness s) {
        setType(t);
        setPosition(p);
        setSolidness(s);
        setAltitude(0);
        setVelocityX(0); setVelocityY(0);
    }

    int onEvent(const Event& e) override {
        if (auto* o = dynamic_cast<const EventOut*>(&e)) {
            (void)o;
            ++out_count;
            df::LogManager::getInstance().writeLog("[CollisionProbe %s id=%d] OUT\n",
                getType().c_str(), getId());
            return 1;
        }
        if (auto* c = dynamic_cast<const EventCollision*>(&e)) {
            (void)c;
            ++col_count;
            df::LogManager::getInstance().writeLog("[CollisionProbe %s id=%d] COLLISION\n",
                getType().c_str(), getId());
            return 1;
        }
        return 0;
    }
};

class DrawProbe : public Object {
public:
    static std::vector<int> seen_ids;

    explicit DrawProbe(int alt, const Vector& p) {
        setType("DrawProbe");
        setPosition(p);
        setAltitude(alt);
        setSolidness(Solidness::SPECTRAL);
    }

    int draw() override {
        seen_ids.push_back(getId());
        return 0;
    }
};
std::vector<int> DrawProbe::seen_ids;

static void test_WorldManager_features() {
    df::LogManager::getInstance().writeLog("== WorldManager movement/bounds/collision tests ==\n");

    // clear world
    {
        auto objs = WM().getAllObjects();
        for (int i = 0; i < objs.getCount(); ++i) if (objs[i]) objs[i]->markForDelete();
        WM().update();
        TEST_ASSERT(WM().getAllObjects().getCount() == 0, "world cleared");
    }

    // 1) Out-of-bounds generates EventOut and blocks movement
    {
        CollisionProbe* edge = new CollisionProbe("Edge", Vector(0, 0), Solidness::HARD);
        edge->setVelocityX(-1.f); edge->setVelocityY(0.f);  // attempt to move to x=-1
        WM().update();
        TEST_ASSERT(edge->out_count >= 1, "EventOut delivered when moving out of bounds");
        TEST_ASSERT(static_cast<int>(edge->getPosition().getX()) == 0, "movement blocked at world edge");
        edge->markForDelete(); WM().update();
    }

    // 2) Collision HARD vs HARD blocks and sends EventCollision to both
    {
        CollisionProbe* A = new CollisionProbe("A", Vector(5, 5), Solidness::HARD);
        CollisionProbe* B = new CollisionProbe("B", Vector(6, 5), Solidness::HARD);
        A->setVelocityX(+1.f); 
        WM().update();
        TEST_ASSERT(A->col_count >= 1 && B->col_count >= 1, "EventCollision received by both HARD objects");
        TEST_ASSERT(static_cast<int>(A->getPosition().getX()) == 5, "HARD vs HARD blocked");
        A->markForDelete(); B->markForDelete(); WM().update();
    }

    // 3) Collision SPECTRAL vs HARD allows pass-through but still sends EventCollision
    {
        CollisionProbe* A = new CollisionProbe("A", Vector(10, 10), Solidness::SPECTRAL);
        CollisionProbe* B = new CollisionProbe("B", Vector(11, 10), Solidness::HARD);
        A->setVelocityX(+1.f);
        WM().update();
        TEST_ASSERT(A->col_count >= 1 && B->col_count >= 1, "EventCollision sent for spectral pass-through");
        TEST_ASSERT(static_cast<int>(A->getPosition().getX()) == 11, "SPECTRAL passed through");
        A->markForDelete(); B->markForDelete(); WM().update();
    }

    // 4) Draw order by altitude (lowest first)
    {
        DrawProbe::seen_ids.clear();
        DrawProbe* low = new DrawProbe(0, Vector(1, 1));
        DrawProbe* mid = new DrawProbe(5, Vector(2, 1));
        DrawProbe* high = new DrawProbe(10, Vector(3, 1));
        WM().draw();

        TEST_ASSERT(DrawProbe::seen_ids.size() == 3, "draw() visited 3 objects");
        TEST_ASSERT(DrawProbe::seen_ids[0] == 0 &&
            DrawProbe::seen_ids[1] == 5 &&
            DrawProbe::seen_ids[2] == 10,
            "objects drawn in ascending altitude");
        low->markForDelete(); mid->markForDelete(); high->markForDelete(); WM().update();
    }
}

// ---------- GameManager loop test ----------
class StepProbe : public Object {
    int limit;
    int seen = 0;
public:
    explicit StepProbe(int steps) : limit(steps) {
        setType("StepProbe");
    }

    int onEvent(const Event& e) override {
        if (auto* s = dynamic_cast<const EventStep*>(&e)) {
            ++seen;
            df::LogManager::getInstance().writeLog(
                "[StepProbe %d] saw step=%d\n", getId(), s->getStepCount());
            if (seen >= limit) df::GameManager::getInstance().setGameOver(true);
            return 1;
        }
        return 0;
    }

    int getSeen() const { return seen; }
};

static void test_GameManager_loop() {
    df::LogManager::getInstance().writeLog("== GameManager loop test ==\n");
    auto objs = WM().getAllObjects();
    for (int i = 0;i < objs.getCount();++i) if (objs[i]) objs[i]->markForDelete();
    WM().update();

    StepProbe p(5);
    df::GameManager::getInstance().run(); // will stop after 5 steps via StepProbe
    TEST_ASSERT(p.getSeen() >= 5, "Game loop sent at least 5 EventStep events");
}

// ---------- Display/Input smoke tests ----------
static void test_Display_smoke() {
    df::LogManager::getInstance().writeLog("== DisplayManager smoke ==\n");
    // Draw a tiny HUD for 2 frames just to ensure it runs
    auto& DM = DisplayManager::getInstance();
    for (int i = 0; i < 2; ++i) {
        DM.drawString(Vector(1, 1), "Dragonfly text OK", Justify::LEFT);
        DM.swapBuffers();
        sleep_ms(33);
    }
    TEST_ASSERT(true, "DisplayManager drew 2 frames");
}

#if RUN_MANUAL_INPUT_TEST
class InputCatcher : public Object {
public:
    int kb_count = 0, mouse_count = 0;
    int onEvent(const Event& e) override {
        if (dynamic_cast<const EventKeyboard*>(&e)) { ++kb_count; return 1; }
        if (dynamic_cast<const EventMouse*>(&e)) { ++mouse_count; return 1; }
        return 0;
    }
};

static void test_Input_manual() {
    df::LogManager::getInstance().writeLog("== InputManager manual test (press a key and click) ==\n");
    InputCatcher c;
    df::GameManager::getInstance().setGameOver(false);

    // run for ~3 seconds
    df::Clock clk;
    while (clk.split() < 3'000'000) {
        InputManager::getInstance().getInput();
        // do nothing else
        sleep_ms(33);
    }
    TEST_ASSERT(c.kb_count >= 1, "Keyboard event seen (manual)");
    TEST_ASSERT(c.mouse_count >= 1, "Mouse event seen (manual)");
}
#endif

// ---------- main ----------
int main() {
    auto& LM = df::LogManager::getInstance();
    if (LM.startUp() != 0) return 1;
    LM.setFlush(true); // easier to watch the log while running

    WM().startUp();
    auto& GM = df::GameManager::getInstance();
    GM.startUp();
    DisplayManager::getInstance().startUp();   // needed for draw()
    df::InputManager::getInstance().startUp();     // needed for input()

    test_Vector();
    test_Clock();
    test_Object_and_ObjectList();
    test_WorldManager_features();
    test_GameManager_loop();
    test_Display_smoke();
#if RUN_MANUAL_INPUT_TEST
    test_Input_manual();
#endif

    df::LogManager::getInstance().writeLog("== TEST SUMMARY: %d passed, %d failed ==\n", g_pass, g_fail);

    // shutdown in reverse
#if RUN_MANUAL_INPUT_TEST
#endif
    df::InputManager::getInstance().shutDown();
    DisplayManager::getInstance().shutDown();
    GM.shutDown();
    WM().shutDown();
    LM.shutDown();

    return (g_fail == 0) ? 0 : 2;
}
