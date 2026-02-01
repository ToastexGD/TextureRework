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
        
        geode::queueInMainThread([this]() { // insane workaround to have buttons-menu created
            auto menuNode = this->getChildByIDRecursive("buttons-menu");
            if (!menuNode) return;
            
            auto menu = static_cast<CCMenu*>(menuNode);
            
            for (auto btnNode : CCArrayExt<CCNode*>(menu->getChildren())) {
                auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(btnNode);
                if (!button) continue;
                
                auto colorChannel = button->getChildByType<ColorChannelSprite>(0);
                if (!colorChannel) continue;
                
                for (auto childNode : CCArrayExt<CCNode*>(colorChannel->getChildren())) {
                    auto lockSprite = typeinfo_cast<CCSprite*>(childNode);
                    if (!lockSprite) continue;
                    
                    lockSprite->setDisplayFrame(
                        CCSpriteFrameCache::get()->spriteFrameByName("GJ_lock_001.png")
                    );
                }
            }
        });
    }
};
