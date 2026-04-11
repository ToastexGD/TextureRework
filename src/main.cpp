#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/CharacterColorPage.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>

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
                for (auto child : CCArrayExt<CCNode*>(icon->getChildren())) {
                    if (auto s = typeinfo_cast<CCSprite*>(child)) {
                        if (s != icon->m_player) {
                            s->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(is221 ? "GJ_lockGray_001.png" : "GJ_lock_001.png"));
                            s->setFlipX(false);
                            s->setFlipY(false);
                        }
                    }
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

                //ccnode so it doesn't crash when I cast it
                for (auto child : CCArrayExt<CCNode*>(color->getChildren())) {
                    if (auto lock = typeinfo_cast<CCSprite*>(child)) {
                        lock->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("GJ_lock_001.png"));
                        lock->setFlipX(false);
                        lock->setFlipY(false);
                    }
                }
            }
        });
        geode::queueInMainThread(std::move(callback));
    }
};

class $modify(MoreSearchLayer) {
    bool init() {
        if (!MoreSearchLayer::init()) return false;

        auto mainMenu = m_mainLayer->getChildByID("main-menu");
        if (!mainMenu) return true;

        auto legendary = mainMenu->getChildByID("legendary-filter-toggler");
        auto mythic = mainMenu->getChildByID("mythic-filter-toggler");
        auto legendaryLabel = m_mainLayer->getChildByID("legendary-filter-label");
        auto mythicLabel = m_mainLayer->getChildByID("mythic-filter-label");

        auto uncompleted = mainMenu->getChildByID("uncompleted-filter-toggler");
        auto completed = mainMenu->getChildByID("completed-filter-toggler");
        if (uncompleted && completed && legendary && mythic) {
            float leftX = uncompleted->getPositionX();
            float midX = completed->getPositionX();
            float rowY = legendary->getPositionY();

            legendary->setPosition(ccp(leftX, rowY));
            mythic->setPosition(ccp(midX, rowY));
        }

        auto uncompletedLabel = m_mainLayer->getChildByID("uncompleted-filter-label");
        auto completedLabel = m_mainLayer->getChildByID("completed-filter-label");
        if (uncompletedLabel && completedLabel && legendaryLabel && mythicLabel) {
            float leftX = uncompletedLabel->getPositionX();
            float midX = completedLabel->getPositionX();
            float rowY = legendaryLabel->getPositionY();

            legendaryLabel->setPosition(ccp(leftX, rowY));
            mythicLabel->setPosition(ccp(midX, rowY));
        }

        return true;
    }
};
