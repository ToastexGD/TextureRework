#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/CharacterColorPage.hpp>


using namespace geode::prelude;

bool isUnlockedByDefault(int id, UnlockType type) {
    if (type == UnlockType::Cube) return id < 5;
    if (type == UnlockType::GJItem) return false;
    return id < 2;
}

class $modify(GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        auto lock = this->getChildByIDRecursive("username-lock");
        if (lock) lock->setVisible(false);

        return true;
    }

    void setupPage(int page, IconType type) {
        GJGarageLayer::setupPage(page, type);
        if (!m_iconSelection || !m_iconSelection->m_pages) return;

        auto gsm = GameStatsManager::sharedState();

        for (auto p : CCArrayExt<CCNode*>(m_iconSelection->m_pages)) {
            auto menu = p->getChildByType<CCMenu>(0);
            if (!menu) continue;

            for (auto btn : CCArrayExt<CCNode*>(menu->getChildren())) {
                auto icon = btn->getChildByType<GJItemIcon>(0);
                if (!icon) continue;

                auto id = icon->m_unlockID;
                auto ut = icon->m_unlockType;

                bool is221 = ut != UnlockType::Col1 && ut != UnlockType::Col2 &&
                gsm->getItemUnlockState(id, ut) == 0 && !isUnlockedByDefault(id, ut);

                for (auto s : CCArrayExt<CCSprite*>(icon->getChildren())) {
                    if (s != icon->m_player)
                    s->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(is221 ? "GJ_lockGray_001.png" : "GJ_lock_001.png"));
                }
            }
        }
    }
};

class $modify(CharacterColorPage) {
    void createColorMenu() {
        CharacterColorPage::createColorMenu();

        auto callback = geode::Function<void()>([this]() {
            auto menu = this->getChildByIDRecursive("buttons-menu");
            if (!menu) return;

            for (auto btn : CCArrayExt<CCNode*>(menu->getChildren())) {
                auto color = btn->getChildByType<ColorChannelSprite>(0);
                if (!color) continue;

                for (auto lock : CCArrayExt<CCSprite*>(color->getChildren()))
                    lock->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("GJ_lock_001.png"));
            }
        });
        geode::queueInMainThread(std::move(callback));
    }
};
