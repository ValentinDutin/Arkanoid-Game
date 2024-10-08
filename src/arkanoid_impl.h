#pragma once

#include "arkanoid.h"
#include "Carriage.h"
#include "BricksMatrix.h"

#define USE_ARKANOID_IMPL

class ArkanoidImpl : public Arkanoid
{
public:
    void reset(const ArkanoidSettings& settings) override;
    void update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed, const ArkanoidSettings& settings) override;
    void draw(ImGuiIO& io, ImDrawList& draw_list) override;

private:
    void objects_update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed);
    void ball_update(ArkanoidDebugData& debug_data);
    void carriage_update();
    void bricks_matrix_update();
    void objects_draw(ImGuiIO& io, ImDrawList& draw_list);
    void demo_add_debug_hit(ArkanoidDebugData& debug_data, const Vect& pos, const Vect& normal);
    void next_level(const ArkanoidSettings& settings);
    BricksMatrix bricks_matrix;
    Vect world_size = Vect(0.0f);
    Vect world_to_screen = Vect(0.0f);
    Vect ball_position = Vect(0.0f);
    Vect ball_velocity = Vect(0.0f);
    float ball_radius = 0.0f;
    float ball_initial_speed = 0.0f;
    Carriage carriage;
    unsigned int score = 0;
    unsigned int level = 1;
    bool game_over = false;
};
