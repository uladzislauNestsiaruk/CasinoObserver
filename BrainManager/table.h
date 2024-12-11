#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <memory>
#include <random>
#include <vector>

#include "../card.h"
#include "constants.h"
#include "gambler.h"

class Table {
  public:
    Table(size_t decks_amount = 1)
        : whose_move_{0}, start_phase_{true},
          random_generator_{std::random_device{}()} {}

    explicit Table(GameType type) : Table() { table_type_ = type; }

    void ReshuffleDeck();

    Card GetTopCard() {
        Card top = deck_.back();
        deck_.pop_back();
        return top;
    }

    // returns first player in the order that still in game
    std::shared_ptr<Gambler> GetNextPlayer() {
        while (!players_[whose_move_]->GetGameStatus()) {
            ++whose_move_;
            whose_move_ %= players_.size();
        }

        return players_[whose_move_];
    }

    virtual void GameItaration() = 0;

    virtual void OneStep() = 0;

    virtual void AddPlayer(const Gambler &player);

    virtual void RemovePlayer(const Gambler &player);

    virtual ~Table() {}

  protected:
    GameType table_type_;
    bool start_phase_; // This flag indiactes is game on the table just started,
                       // it made for convinient card distribution animation
                       // implementation
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<Gambler>> players_;

  private:
    std::vector<Card> deck_;
    std::mt19937 random_generator_;
};

class PokerTable : public Table {
  public:
  private:
};

// Player by zero index is always a dealer
class BlackjackTable : public Table {
  public:
    BlackjackTable() : Table(3) {}

    void GameItaration() override;

    void OneStep() override;

    ~BlackjackTable() override {}
};

#endif // !CASINOOBSERVER_BRAINMANAGER_TABLE_H
