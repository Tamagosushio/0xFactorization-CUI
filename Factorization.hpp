#pragma once
#include <memory>

#include "PoolAllocator.hpp"
#include <vector>
#include <chrono>
#include <string>
#include <random>
#include <thread>
#include <atomic>

namespace Factorization {

    class Game;

    std::string ToBaseN(size_t n, const size_t base);

    class FactorsWithWeight {
    private:
        std::vector<size_t> m_factors;
        std::vector<double> m_weights;
        std::string m_string;
        std::discrete_distribution<size_t> m_dist;
    public:
        FactorsWithWeight(void) {}
        FactorsWithWeight(
            const std::vector<size_t> factors,
            const std::vector<double> weight,
            const std::string str);
        size_t Choice(void);
        bool IsInFactors(const size_t number) const;
        std::vector<size_t> GetFactors(void) const;
        std::string GetString(void) const;
    };

    class FloatingEffect {
    private:
        size_t m_init_x;
        size_t m_init_y;
        size_t m_number;
        std::chrono::system_clock::time_point m_birth_time;
        struct Particle {
            double vx, vy;
            char c;
            size_t color_code;
        };
        std::vector<Particle> m_particles;
        static constexpr double LIFE_SPAN = 1.5;       // 表示される時間
        static constexpr size_t NUM_PARTICLES = 12;    // 生成するパーティクルの数
        static constexpr double PARTICLE_SPEED = 15.0; // パーティクルの射出速度
        static constexpr double GRAVITY = 19.6;        // パーティクルにかかる重力加速度
        static constexpr char CHARS[] = { '*', '+', '.', '\'' }; // パーティクルの文字
        static constexpr size_t COLORS[] = {91, 93, 96, 97};     // 赤,黄,シアン,白
        double CalcElapsed(void) const;
    public:
        FloatingEffect(size_t x, size_t y, size_t number);
        bool IsExpired(void) const;
        void Print(void) const;
    };

    class State {
    protected:
        Game& m_game;
        /// コンソール上の文字を全消去し、カーソルを(1,1)へ移動
        void InitializeDisplay(void) const;
    public:
        State(Game& game) : m_game(game) {}
        ~State() = default;
        virtual void Update(void) = 0;
        virtual void Draw(void) const = 0;
    };

    class StateTitle : public State {
    private:
        size_t m_key4_count = 0;
    public:
        StateTitle(Game& game) : State(game) {}
        void Update(void);
        void Draw(void) const;
    };

    class StatePlay: public State {
    private:
        size_t m_time_limit = 60;                               // ゲームの制限時間
        size_t m_problem_number = 1;                            // 問題の数字
        std::chrono::system_clock::time_point m_start_time;     // ゲームが始まった時間
        PoolAllocator<FloatingEffect, 64> m_effect_allocator;   // エフェクトのアロケーター
        std::vector<FloatingEffect*> m_active_effects;          // 現在有効になっているエフェクト
        std::string m_input_buffer;                             // 回答入力のバッファー
        size_t m_base = 16;                                     // 問題回答の基数 
        void SoundAsync(const size_t freq, const size_t length) const;
        void DivideProblemNumber(const size_t number);
        void CreateNewProblemNumber(void);
        size_t GetGameTime(void) const;
    public:
        StatePlay(Game& game);
        void Update(void);
        void Draw(void) const;
    };

    class StateResult: public State {
    public:
        StateResult(Game& game);
        void Update(void);
        void Draw(void) const;
    };

    class Game {
    private:
        FactorsWithWeight m_factors_width_weight;// 重み付き因数
        std::shared_ptr<State> m_current_state;  // 現在の状態
        size_t m_problem_count = 0;              // 現在の問題数
        size_t m_score;                          // ゲームスコア
        bool m_is_halt;
    public:
        Game(void);
        void Halt(void);
        bool IsHalt(void) const;
        void ChangeState(std::shared_ptr<State> next_state);
        FactorsWithWeight GetFactorsWithWeight(void) const;
        void SetFactorsWithWeight(const FactorsWithWeight& factors);
        size_t GetProblemCount(void) const;
        void SetProblemCount(const size_t count);
        size_t GetScore(void) const;
        void AddScore(const size_t score);
        void SubScore(const size_t penalty);
        bool Update(void);
        void Draw(void) const;
        void Initialize(void);
    };


} // namespace Factorization

