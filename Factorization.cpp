#pragma once

#include "Factorization.hpp"
#include <conio.h>
#include <cassert>
#include <chrono>
#include <algorithm>
#include <string>
#include <iostream>
#include <Windows.h>
#include <wchar.h>

namespace Factorization {

    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());

    constexpr char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* subscripts[] = { "₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉" };

    std::string ToBaseN(size_t n, const size_t base) {
        assert(2 <= base and base <= 36);
        std::string s;
        do {
            s.push_back(digits[n % base]);
            n /= base;
        } while (n);
        std::reverse(s.begin(), s.end());
        return s;
    }

    FactorsWithWeight::FactorsWithWeight(
        const std::vector<size_t> factors,
        const std::vector<double> weights,
        const std::string str)
        : m_factors(factors), m_weights(weights), m_string(str) {
        assert(factors.size() == weights.size());
        m_dist = std::discrete_distribution<size_t>{ weights.begin(), weights.end() };
    }
    size_t FactorsWithWeight::Choice(void) {
        return m_factors.at(m_dist(engine));
    }
    bool FactorsWithWeight::IsInFactors(const size_t number) const {
        return (std::find(m_factors.begin(), m_factors.end(), number) != m_factors.end());
    }
    std::vector<size_t> FactorsWithWeight::GetFactors(void) const{
        return m_factors;
    }
    std::string FactorsWithWeight::GetString(void) const {
        return m_string;
    }

    FactorsWithWeight FactorsEasy{
        {2, 3, 5},
        {1.0, 0.5, 0.25},
        "Easy"
    };
    FactorsWithWeight FactorsNormal{
        {2, 3, 5, 7, 11},
        {1.0, 0.8, 0.6, 0.4, 0.2},
        "Normal"
    };
    FactorsWithWeight FactorsHard{
        {2, 3, 5, 7, 11, 13},
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        "Hard"
    };
    FactorsWithWeight FactorsOni{
        {2, 3, 5, 7, 11, 13},
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        "Oni"
    };

    double FloatingEffect::CalcElapsed(void) const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_birth_time).count() / 1000.0;
    }
    FloatingEffect::FloatingEffect(size_t x, size_t y, size_t number)
        : m_init_x(x), m_init_y(y), m_number(number), m_birth_time(std::chrono::system_clock::now()) {
        m_particles.reserve(NUM_PARTICLES);
        for (size_t i = 0; i < NUM_PARTICLES; i++) {
            const double angle = (2.0 * 3.1415 / NUM_PARTICLES) * i;
            const double speed_c = 1.0 + static_cast<double>(rand() % 500) / 1000.0;
            Particle p;
            p.vx = std::cos(angle) * PARTICLE_SPEED * speed_c;
            p.vy = std::sin(angle) * PARTICLE_SPEED * 0.7 * speed_c - 10.0;
            p.c = CHARS[rand() % (sizeof(CHARS) / sizeof(char))];
            p.color_code = COLORS[rand() % (sizeof(COLORS) / sizeof(size_t))];
            m_particles.push_back(p);
        }
    }
    bool FloatingEffect::IsExpired(void) const {
        double elapsed = CalcElapsed();
        return elapsed > LIFE_SPAN;
    }
    void FloatingEffect::Print(void) const {
        const double elapsed = CalcElapsed();
        const double life_ratio = elapsed / LIFE_SPAN;
        // 加算描画
        size_t color_code;
        if (life_ratio < 0.33) color_code = 97;
        else if (life_ratio < 0.67) color_code = 93;
        else color_code = 91;
        printf("\033[%zu;%zuH\033[%zum+%zu!\033[39m",
               m_init_y, m_init_x, color_code, m_number);
        // パーティクル描画
        for (const Particle& p : m_particles) {
            const double particle_x = m_init_x + 1 + p.vx * elapsed;
            const double particle_y = m_init_y + 1 + p.vy * elapsed + 0.5 * GRAVITY * elapsed * elapsed;
            if (particle_x >= 1 and particle_y >= 7) {
                bool is_visible = true;
                if (life_ratio > 0.5) {
                    if (rand() % 100 < (life_ratio - 0.5) * 2 * 100) {
                        is_visible = false;
                    }
                }
                if (is_visible) {
                    printf("\033[%zu;%zuH\033[%zum%c\033[39m",
                        static_cast<size_t>(particle_y),
                        static_cast<size_t>(particle_x),
                        p.color_code,
                        p.c);
                }
            }
        }
    }
    
    void State::InitializeDisplay(void) const {
        printf("\033[2J\033[1;1H");
    }

    void StateTitle::Update(void) {
        if (_kbhit()) {
            const int c = _getch();
            if (c == 0x1b) m_game.Halt();
            else if (c == '1') m_game.SetFactorsWithWeight(FactorsEasy);
            else if (c == '2') m_game.SetFactorsWithWeight(FactorsNormal);
            else if (c == '3') m_game.SetFactorsWithWeight(FactorsHard);
            else if (c == '4') {
                ++m_key4_count;
                if (m_key4_count == 4) m_game.SetFactorsWithWeight(FactorsOni);
                else return;
            } else {
                m_key4_count = 0;
                return;
            }
            m_game.ChangeState(std::make_shared<StatePlay>(m_game));
            InitializeDisplay();
        }
    }
    void StateTitle::Draw(void) const {
        printf("\033[1;1H  0xFactorization!!");
        printf("\033[2;1H  Press Key to Select Difficulty");
        printf("\033[3;1H      \033[32mEasy  : '1'\033[39m");
        printf("\033[4;1H      \033[33mNormal: '2'\033[39m");
        printf("\033[5;1H      \033[31mHard  : '3'\033[39m");
        printf("\033[6;1H  Press Esc to Exit Game");
    }

    StatePlay::StatePlay(Game& game) : State(game) {
        CreateNewProblemNumber();
        m_start_time = std::chrono::system_clock::now();
    }
    void StatePlay::SoundAsync(const size_t freq, const size_t length) const {
        std::thread sound_thread([freq, length]() {
            Beep(static_cast<DWORD>(freq), static_cast<DWORD>(length));
        });
        sound_thread.detach();
        return;
    }
    void StatePlay::DivideProblemNumber(const size_t number) {
        assert(number != 0);
        // 間違えた場合はペナルティでスコア減算
        if (m_problem_number % number != 0) {
            SoundAsync(540, 100);
            m_game.SubScore(number);
            return;
        }
        // 実際に割る
        m_problem_number /= number;
        // ビープ音再生
        SoundAsync(880, 100);
        m_game.AddScore(number);
        // エフェクト生成
        FloatingEffect* effect = m_effect_allocator.Alloc();
        if (effect) {
            new (effect)FloatingEffect(rand() % 40 + 5, rand() % 10 + 15, number);
            m_active_effects.push_back(effect);
        }
        // 全て割り切ったときには新しい問題に入れ替え
        if (m_problem_number == 1) {
            m_game.SetProblemCount(m_game.GetProblemCount() + 1);
            if (m_game.GetFactorsWithWeight().GetString() == "Oni") {
                m_base = rand() % 35 + 2;
            }
            CreateNewProblemNumber();
        }
    }
    void StatePlay::CreateNewProblemNumber(void) {
        m_problem_number = 1;
        for (size_t i = 0; i < log2(m_game.GetProblemCount() + 2); i++) {
            m_problem_number *= m_game.GetFactorsWithWeight().Choice();
        }
    }
    size_t StatePlay::GetGameTime(void) const {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_start_time).count();
    }
    void StatePlay::Update(void) {
        // 入力受け取り
        if (_kbhit()) {
            int c = _getch();
            // 小文字は大文字に変換
            if (islower(c)) c -= 0x20;
            // Enterキーが押された時
            if (c == '\r') {
                if (not m_input_buffer.empty()) {
                    size_t answer = std::stoull(m_input_buffer, nullptr, static_cast<int>(m_base));
                    if (m_game.GetFactorsWithWeight().IsInFactors(answer)) {
                        DivideProblemNumber(answer);
                    } else {
                        SoundAsync(540, 100);
                    }
                    m_input_buffer.clear();
                }
            // アルファベット
            } else if ('0' <= c and c <= digits[m_base-1]) {
                m_input_buffer += static_cast<char>(c);
            // バックスペース
            } else if (c == '\b' and not m_input_buffer.empty()) {
                m_input_buffer.pop_back();
            }
        }
        // エフェクトの寿命を管理
        for (auto iter = m_active_effects.begin(); iter != m_active_effects.end();) {
            if ((*iter)->IsExpired()) {
                m_effect_allocator.Free(*iter);
                iter = m_active_effects.erase(iter);
            } else {
                iter++;
            }
        }
        // 制限時間を過ぎたら結果画面へ遷移
        if (GetGameTime() >= m_time_limit) {
            InitializeDisplay();
            m_game.ChangeState(std::make_shared<StateResult>(m_game));
            return;
        }
    }
    void StatePlay::Draw(void) const {
        // 残り時間表示
        if (m_time_limit - GetGameTime() <= 5) {
            printf("\033[1;1H  Remaining Time: \033[1;31m %zu \033[0m\033[K", m_time_limit - GetGameTime());
        } else {
            printf("\033[1;1H  Remaining Time: %zu\033[K", m_time_limit - GetGameTime());
        }
        // スコアの表示
        printf("\033[2;1H  Score: %zu\033[K", m_game.GetScore());
        // 問題の数字表示
        if (m_game.GetFactorsWithWeight().GetString() == "Oni") {
            printf("\033[3;1H  Problem: ( %s )%s%s\033[K", ToBaseN(m_problem_number, m_base).c_str(),
                    subscripts[m_base / 10], subscripts[m_base % 10]);
        } else {
            printf("\033[3;1H  Problem: 0x%s\033[K", ToBaseN(m_problem_number, m_base).c_str());
        }
        // 選択肢の因数表示
        std::string choices_str = "Choices: ";
        bool is_first = true;
        for (const size_t factor : m_game.GetFactorsWithWeight().GetFactors()) {
            if (not is_first) choices_str += ",";
            choices_str += ToBaseN(factor, m_base);
            is_first = false;
        }
        printf("\033[4;1H  %s\033[K", choices_str.c_str());
        // 回答の表示
        printf("\033[5;1H  Factor?  %s\033[K", m_input_buffer.c_str());
        // 表示のクリア
        for (size_t i = 6; i < 50; i++) {
            printf("\033[%zu;1H\033[K", i);
        }
        for (const FloatingEffect* effect : m_active_effects) {
            effect->Print();
        }
        fflush(stdout);
    }

    StateResult::StateResult(Game& game) : State(game) { }
    void StateResult::Update(void) {
        if (_kbhit()) {
            const int c = _getch();
            if(c == 'q') {
                InitializeDisplay();
                m_game.Initialize();
            }
        }
    }
    void StateResult::Draw(void) const {
        printf("\033[1;1H  You Solved \033[4m\033[36m%zu\033[39m\033[0m Problems!", m_game.GetProblemCount());
        printf("\033[2;1H  Your Score is \033[4m\033[36m%zu\033[39m\033[0m!!", m_game.GetScore());
        printf("\033[3;1H  Press 'q' to Return to Title!!");
    }

    Game::Game(void) {
        Initialize();
    }
    void Game::Halt(void) {
        m_is_halt = true;
    }
    bool Game::IsHalt(void) const {
        return m_is_halt;
    }
    void Game::ChangeState(std::shared_ptr<State> next_state) {
        m_current_state = next_state;
    }
    FactorsWithWeight Game::GetFactorsWithWeight(void) const {
        return m_factors_width_weight;
    }
    void Game::SetFactorsWithWeight(const FactorsWithWeight& factors) {
        m_factors_width_weight = factors;
    }
    size_t Game::GetProblemCount(void) const {
        return m_problem_count;
    }
    void Game::SetProblemCount(const size_t count) {
        m_problem_count = count;
    }
    size_t Game::GetScore(void) const{
        return m_score;
    }
    void Game::AddScore(const size_t score) {
        m_score += score;
    }
    void Game::SubScore(const size_t penalty) {
        // 符号なしのためオーバーフローを考慮
        m_score = (m_score - penalty > m_score) ? 0 : m_score - penalty;
    }
    bool Game::Update(void) {
        m_current_state->Update();
        return not IsHalt();
    }
    void Game::Draw(void) const {
        m_current_state->Draw();
    }
    void Game::Initialize(void) {
        m_factors_width_weight = FactorsEasy;
        m_problem_count = 0;
        m_score = 0;
        m_current_state = std::make_shared<StateTitle>(*this);
    }

} // namespace Factorization
