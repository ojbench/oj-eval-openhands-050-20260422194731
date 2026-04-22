#pragma once

#include <exception>
#include <optional>

class InvalidOperation : public std::exception {
public:
    const char* what() const noexcept override { return "invalid operation"; }
};

struct PlayInfo {
    int dummyCount = 0;
    int magnifierCount = 0;
    int converterCount = 0;
    int cageCount = 0;
};

class GameState {
public:
    enum class BulletType { Live, Blank };
    enum class ItemType { Dummy, Magnifier, Converter, Cage };

    GameState()
        : currentPlayer(0) {
        hp[0] = 5; hp[1] = 5;
        liveCount = blankCount = 0;
        topKnown.reset();
        usedCageThisTurn[0] = usedCageThisTurn[1] = false;
        activeCageEffect[0] = activeCageEffect[1] = false;
    }

    void fireAtOpponent(BulletType topBulletBeforeAction) {
        if (totalBullets() == 0) { consumeNoop(); return; }
        consumeTop(topBulletBeforeAction);
        if (topBulletBeforeAction == BulletType::Live) {
            hp[other(currentPlayer)] -= 1;
            if (isGameOver()) return; // game ends immediately
        }
        endTurnAfterShot();
    }

    void fireAtSelf(BulletType topBulletBeforeAction) {
        if (totalBullets() == 0) { consumeNoop(); return; }
        consumeTop(topBulletBeforeAction);
        if (topBulletBeforeAction == BulletType::Live) {
            hp[currentPlayer] -= 1;
            if (isGameOver()) return;
            endTurnAfterShot();
        } else {
            // blank -> continue turn, no end-turn
        }
    }

    void useDummy(BulletType topBulletBeforeUse) {
        auto& info = playerInfo(currentPlayer);
        if (info.dummyCount <= 0) throw InvalidOperation();
        // consume item
        info.dummyCount -= 1;
        if (totalBullets() == 0) return; // nothing to consume if no bullets
        consumeTop(topBulletBeforeUse); // consume bullet, reveal type given
        // does not end turn
    }

    void useMagnifier(BulletType topBulletBeforeUse) {
        auto& info = playerInfo(currentPlayer);
        if (info.magnifierCount <= 0) throw InvalidOperation();
        info.magnifierCount -= 1;
        // reveal top bullet type
        if (totalBullets() == 0) { topKnown.reset(); return; }
        topKnown = topBulletBeforeUse;
    }

    void useConverter(BulletType topBulletBeforeUse) {
        auto& info = playerInfo(currentPlayer);
        if (info.converterCount <= 0) throw InvalidOperation();
        info.converterCount -= 1;
        if (totalBullets() == 0) { topKnown.reset(); return; }
        // top before use is provided; flip it and adjust counts accordingly
        if (topBulletBeforeUse == BulletType::Live) {
            if (liveCount > 0) { liveCount -= 1; blankCount += 1; }
            topKnown = BulletType::Blank;
        } else {
            if (blankCount > 0) { blankCount -= 1; liveCount += 1; }
            topKnown = BulletType::Live;
        }
    }

    void useCage() {
        auto& info = playerInfo(currentPlayer);
        if (usedCageThisTurn[currentPlayer]) throw InvalidOperation();
        if (info.cageCount <= 0) throw InvalidOperation();
        info.cageCount -= 1;
        usedCageThisTurn[currentPlayer] = true;
        activeCageEffect[currentPlayer] = true;
    }

    void reloadBullets(int liveCnt, int blankCnt) {
        liveCount = liveCnt;
        blankCount = blankCnt;
        topKnown.reset();
    }

    void reloadItem(int playerId, ItemType item) {
        auto& info = playerInfo(playerId);
        int total = info.dummyCount + info.magnifierCount + info.converterCount + info.cageCount;
        if (total >= 2) return; // capacity limit
        switch (item) {
            case ItemType::Dummy: info.dummyCount += 1; break;
            case ItemType::Magnifier: info.magnifierCount += 1; break;
            case ItemType::Converter: info.converterCount += 1; break;
            case ItemType::Cage: info.cageCount += 1; break;
        }
    }

    double nextLiveBulletProbability() const {
        const int tot = totalBullets();
        if (tot <= 0) return 0.0;
        if (topKnown.has_value()) {
            return (*topKnown == BulletType::Live) ? 1.0 : 0.0;
        }
        return static_cast<double>(liveCount) / static_cast<double>(tot);
    }

    double nextBlankBulletProbability() const {
        const int tot = totalBullets();
        if (tot <= 0) return 0.0;
        if (topKnown.has_value()) {
            return (*topKnown == BulletType::Blank) ? 1.0 : 0.0;
        }
        return static_cast<double>(blankCount) / static_cast<double>(tot);
    }

    int winnerId() const {
        if (hp[0] <= 0 && hp[1] <= 0) return -1; // should not happen
        if (hp[0] <= 0) return 1;
        if (hp[1] <= 0) return 0;
        return -1;
    }

private:
    int hp[2]{};
    int currentPlayer;
    int liveCount = 0;
    int blankCount = 0;
    std::optional<BulletType> topKnown; // known top bullet type if any

    PlayInfo pinfo[2];
    bool usedCageThisTurn[2]{};
    bool activeCageEffect[2]{};

    int totalBullets() const { return liveCount + blankCount; }

    static int other(int x) { return x ^ 1; }

    PlayInfo& playerInfo(int id) { return pinfo[id]; }
    const PlayInfo& playerInfo(int id) const { return pinfo[id]; }

    void consumeTop(BulletType topType) {
        // consume a bullet of the given type from counts; clear topKnown
        if (topType == BulletType::Live) {
            if (liveCount > 0) liveCount -= 1; // robust to inconsistent input
        } else {
            if (blankCount > 0) blankCount -= 1;
        }
        topKnown.reset();
    }

    bool isGameOver() const { return hp[0] <= 0 || hp[1] <= 0; }

    void endTurnAfterShot() {
        // Called when a shot would normally end the shooter's turn
        if (isGameOver()) return;
        if (activeCageEffect[currentPlayer]) {
            // consume cage effect and keep the turn
            activeCageEffect[currentPlayer] = false;
            return;
        }
        // switch player and reset per-turn flags
        currentPlayer = other(currentPlayer);
        usedCageThisTurn[currentPlayer] = false; // ensure next player's flag is reset for their turn
        // Also reset for the player whose turn just ended so that when their next turn begins, it's false
        usedCageThisTurn[other(currentPlayer)] = false;
    }

    static void consumeNoop() {}
};
