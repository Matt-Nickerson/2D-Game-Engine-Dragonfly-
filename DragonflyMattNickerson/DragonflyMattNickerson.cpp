#include <cmath>
#include <thread>
#include <chrono>
#include <stdexcept>

#include "LogManager.h"
#include "WorldManager.h"
#include "GameManager.h"
#include "Vector.h"
#include "Object.h"
#include "ObjectList.h"
#include "EventStep.h"
#include "Clock.h"

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

    // Raw ObjectList behavior (independent of world)
    ObjectList L;
    TEST_ASSERT(L.getCount() == 0, "ObjectList starts empty");
    TEST_ASSERT(L.insert(a) == 0 && L.insert(b) == 0 && L.getCount() == 2, "insert() 2 objects");
    TEST_ASSERT(L[0] == a && L[1] == b, "operator[] returns inserted pointers");
    TEST_ASSERT(L.remove(a) == 0 && L.getCount() == 1, "remove() existing");
    TEST_ASSERT(L.remove(a) == -1, "remove() non-existent returns -1");
    L.clear(); TEST_ASSERT(L.getCount() == 0, "clear() empties list");

    // WorldManager add/remove via Object helpers
    int before = WM().getAllObjects().getCount();
    DummyObj* c = new DummyObj(Vector(0.f, 0.f)); // auto add
    int after = WM().getAllObjects().getCount();
    TEST_ASSERT(after == before + 1, "addToWorld (via ctor) increased world count");

    c->markForDelete(); WM().update();
    int afterDel = WM().getAllObjects().getCount();
    TEST_ASSERT(afterDel == before, "markForDelete + update removes object");

    // cleanup
    a->markForDelete(); b->markForDelete(); WM().update();
}

// ---------- WorldManager tests ----------
static void test_WorldManager() {
    df::LogManager::getInstance().writeLog("== WorldManager tests ==\n");

    // clear world
    {
        auto objs = WM().getAllObjects();
        for (int i = 0;i < objs.getCount();++i) if (objs[i]) objs[i]->markForDelete();
        WM().update();
        TEST_ASSERT(WM().getAllObjects().getCount() == 0, "world cleared");
    }

    DummyObj* a = new DummyObj(Vector(2.f, 2.f));
    DummyObj* b = new DummyObj(Vector(8.f, 1.f));
    int cnt = WM().getAllObjects().getCount();
    TEST_ASSERT(cnt == 2, "world count == 2 after inserts");

    auto dummies = WM().objectsOfType("Dummy");
    TEST_ASSERT(dummies.getCount() == 2, "objectsOfType(\"Dummy\") returns 2");

    WM().removeObject(a); // explicit remove
    TEST_ASSERT(WM().getAllObjects().getCount() == 1, "removeObject() decreased count");
    delete a; // already removed from world, safe to delete

    b->markForDelete(); WM().update();
    TEST_ASSERT(WM().getAllObjects().getCount() == 0, "deferred deletion processed");
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

    int getSeen() const { return seen; }  // <-- this was missing
};

static void test_GameManager_loop() {
    df::LogManager::getInstance().writeLog("== GameManager loop test ==\n");

    // Ensure world is empty before test
    auto objs = WM().getAllObjects();
    for (int i = 0;i < objs.getCount();++i) if (objs[i]) objs[i]->markForDelete();
    WM().update();

    StepProbe p(5);
    df::GameManager::getInstance().run(); // will stop after 5 steps via StepProbe
    TEST_ASSERT(p.getSeen() >= 5, "Game loop sent at least 5 EventStep events");
    // World will remove p in its destructor after run() returns
}

// ---------- main ----------
int main() {
    auto& LM = df::LogManager::getInstance();
    if (LM.startUp() != 0) return 1;
    LM.setFlush(true); // easier to watch the log while running

    WM().startUp();
    auto& GM = df::GameManager::getInstance();
    GM.startUp();

    test_Vector();
    test_Clock();
    test_Object_and_ObjectList();
    test_WorldManager();
    test_GameManager_loop();

    df::LogManager::getInstance().writeLog("== TEST SUMMARY: %d passed, %d failed ==\n", g_pass, g_fail);

    GM.shutDown();
    WM().shutDown();
    LM.shutDown();

    return (g_fail == 0) ? 0 : 2;
}
