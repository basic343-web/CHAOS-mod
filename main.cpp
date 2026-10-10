#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/Notification.hpp>
#include <cstdlib>

using namespace geode::prelude;

static void resetChaos(PlayLayer* pl) {
    CCDirector::get()->getScheduler()->setTimeScale(1.f);
    if (pl && pl->m_objectLayer) pl->m_objectLayer->setScale(1.f);
}

class $modify(ChaosPlayLayer, PlayLayer) {
    struct Fields {
        float timer = 0.f;
        float effectLeft = 0.f;
        bool active = false;
    };

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!Mod::get()->getSettingValue<bool>("enabled")) return;

        auto f = m_fields.self();
        float interval = static_cast<float>(Mod::get()->getSettingValue<double>("interval"));

        // dt viene escalado por el timescale, asi que usamos el real aprox.
        float realDt = dt / std::max(0.1f, CCDirector::get()->getScheduler()->getTimeScale());

        if (f->active) {
            f->effectLeft -= realDt;
            if (f->effectLeft <= 0.f) {
                resetChaos(this);
                f->active = false;
                f->timer = 0.f;
            }
            return;
        }

        f->timer += realDt;
        if (f->timer < interval) return;

        // Elegir efecto al azar
        int roll = std::rand() % 4;
        switch (roll) {
            case 0:
                CCDirector::get()->getScheduler()->setTimeScale(1.6f);
                Notification::create("CHAOS: VELOCIDAD x1.6", NotificationIcon::Warning, 1.5f)->show();
                break;
            case 1:
                CCDirector::get()->getScheduler()->setTimeScale(0.6f);
                Notification::create("CHAOS: CAMARA LENTA", NotificationIcon::Info, 1.5f)->show();
                break;
            case 2:
                if (m_objectLayer) m_objectLayer->setScale(1.5f);
                Notification::create("CHAOS: ZOOM", NotificationIcon::Info, 1.5f)->show();
                break;
            default:
                if (m_objectLayer) m_objectLayer->setScale(0.6f);
                Notification::create("CHAOS: ZOOM OUT", NotificationIcon::Info, 1.5f)->show();
                break;
        }

        f->active = true;
        f->effectLeft = 3.f;
    }

    void resetLevel() {
        resetChaos(this);
        m_fields->active = false;
        m_fields->timer = 0.f;
        PlayLayer::resetLevel();
    }

    void onQuit() {
        resetChaos(this);
        PlayLayer::onQuit();
    }

    void levelComplete() {
        resetChaos(this);
        PlayLayer::levelComplete();
    }
};
