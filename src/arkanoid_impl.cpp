#include "arkanoid_impl.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <string>

#ifdef USE_ARKANOID_IMPL
Arkanoid* create_arkanoid()
{
    return new ArkanoidImpl();  
}
#endif
using namespace std;

void ArkanoidImpl::reset(const ArkanoidSettings &settings)
{
    world_size.x = settings.world_size[0];
    world_size.y = settings.world_size[1];
    ball_initial_speed = settings.ball_speed;
    ball_radius = settings.ball_radius;
    ball_velocity = Vect(0, -sqrt(2 * pow(ball_initial_speed, 2)));
    carriage = Carriage(Vect(world_size.x * 0.5f, world_size.y * 0.9f), settings.carriage_width);
    ball_position = Vect(carriage.get_position().x, carriage.get_position().y - ball_radius - carriage.get_height() / 2);
    bricks_matrix.delete_matrix();
    bricks_matrix = BricksMatrix(world_size, 
        settings.bricks_columns_count, 
        settings.bricks_rows_count, 
        settings.bricks_columns_max, 
        settings.bricks_rows_max, 
        settings.bricks_columns_padding, 
        settings.bricks_rows_padding,
        0);
    game_over = false;
    level = 1;
}

void ArkanoidImpl::next_level(const ArkanoidSettings& settings)
{
    if (settings.ball_speed * (1 + 0.1 * level) > settings.ball_speed_max) {
        ball_initial_speed = settings.ball_speed_max;
    }
    else
    {
        ball_initial_speed = settings.ball_speed * (1 + 0.1 * level);
    }
    if (settings.ball_radius / (level + 1) < settings.ball_radius_min) {
        ball_radius = settings.ball_radius_min;
    }
    else
    {
        ball_radius = settings.ball_radius / (level + 1);
    }
    ball_velocity = Vect(0, -sqrt(2 * pow(ball_initial_speed, 2)));
    if (settings.carriage_width * (1 - 0.1 * level) < settings.carriage_width_min)
    {
        carriage = Carriage(Vect(world_size.x * 0.5f, world_size.y * 0.9f), settings.carriage_width_min);
    }
    else
    {
        carriage = Carriage(Vect(world_size.x * 0.5f, world_size.y * 0.9f), settings.carriage_width * (1 - 0.1 * level));
    }
    ball_position = Vect(carriage.get_position().x, carriage.get_position().y - ball_radius - carriage.get_height() / 2);
    score = bricks_matrix.get_score();
    bricks_matrix.delete_matrix();
    bricks_matrix = BricksMatrix(world_size,
        settings.bricks_columns_count + 5 * level,
        settings.bricks_rows_count + 2 * level,
        settings.bricks_columns_max,
        settings.bricks_rows_max,
        settings.bricks_columns_padding + 5 * level,
        settings.bricks_rows_padding + 5 * level,
        score);
    game_over = false;
    level++;
}

void ArkanoidImpl::update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed, const ArkanoidSettings &settings)
{
    if (!game_over)
    {
        if (io.KeysDown[GLFW_KEY_LEFT] || io.KeysDown[GLFW_KEY_A]) {
            carriage.shift_left();
        }
        if (io.KeysDown[GLFW_KEY_RIGHT] || io.KeysDown[GLFW_KEY_D]) {
            carriage.shift_right();
        }
        objects_update(io, debug_data, elapsed);
        if (bricks_matrix.get_deleted_elem_count() == bricks_matrix.get_bricks_columns() * bricks_matrix.get_bricks_rows())
        {
            next_level(settings);
        }
    }
    else
    {
        if (io.KeysDown[GLFW_KEY_SPACE])
        {
            reset(settings);
        }
    }
}

void ArkanoidImpl::draw(ImGuiIO& io, ImDrawList& draw_list)
{
    if (!game_over)
    {
        objects_draw(io, draw_list);
    }
    else
    {
        string game_over_text = "Game Over\nPress Space to restart game";
        draw_list.AddText(world_to_screen * 0.5f * world_size, ImColor(100, 100, 100), game_over_text.c_str(), game_over_text.c_str() + game_over_text.length());
    }
}

float distance(Vect point_1, Vect point_2)
{
    return sqrt(pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2));
}

void ArkanoidImpl::objects_update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed)
{
    world_to_screen = Vect(io.DisplaySize.x / world_size.x, io.DisplaySize.y / world_size.y);
    ball_position += ball_velocity * elapsed;
    ball_update(debug_data);
    carriage_update();
    bricks_matrix_update();
}

void ArkanoidImpl::ball_update(ArkanoidDebugData& debug_data)
{
    if (ball_position.x < ball_radius)
    {
        ball_position.x += (ball_radius - ball_position.x) * 2.0f;
        ball_velocity.x *= -1.0f;

        demo_add_debug_hit(debug_data, Vect(0, ball_position.y), Vect(1, 0));
    }
    else if (ball_position.x > (world_size.x - ball_radius))
    {
        ball_position.x -= (ball_position.x - (world_size.x - ball_radius)) * 2.0f;
        ball_velocity.x *= -1.0f;

        demo_add_debug_hit(debug_data, Vect(world_size.x, ball_position.y), Vect(-1, 0));
    }

    if (ball_position.y < ball_radius)
    {
        ball_position.y += (ball_radius - ball_position.y) * 2.0f;
        ball_velocity.y *= -1.0f;

        demo_add_debug_hit(debug_data, Vect(ball_position.x, 0), Vect(0, 1));
    }
    if (ball_position.y - ball_radius > world_size.y)
    {
        game_over = true;
    }
}

void ArkanoidImpl::carriage_update()
{
    if (ball_position.y + ball_radius >= carriage.get_position().y - carriage.get_height() / 2 &&
        ball_position.x + ball_radius >= carriage.get_position().x - carriage.get_width() / 2 &&
        ball_position.x - ball_radius <= carriage.get_position().x + carriage.get_width() / 2 &&
        ball_position.y - ball_radius <= carriage.get_position().y + carriage.get_height() / 2)
    {
        if (ball_position.y > carriage.get_position().y - carriage.get_height() / 2)
        {
            if (ball_position.x > carriage.get_position().x)
            {
                ball_position.x += (((carriage.get_position().x + carriage.get_width() / 2) + ball_radius) - ball_position.x) * 2.0f;
                if (ball_velocity.x < 0)
                {
                    ball_velocity.x *= -1.0f;
                }
            }
            else
            {
                ball_position.x -= (ball_position.x - ((carriage.get_position().x - carriage.get_width() / 2) - ball_radius)) * 2.0f;
                if (ball_velocity.x > 0)
                {
                    ball_velocity.x *= -1.0f;
                }
            }
        }
        else
        {
            if (ball_position.x > carriage.get_position().x)
            {
                float ball_velocity_modul = sqrt(pow(ball_velocity.x, 2) + pow(ball_velocity.y, 2));
                float angle;
                if ((ball_position.x - carriage.get_position().x) / (carriage.get_width() / 2) > 1)
                {
                    angle = 1;
                }
                else
                {
                    angle = acos((ball_position.x - carriage.get_position().x) / (carriage.get_width() / 2));
                }
                ball_velocity = Vect(cos(angle) * ball_velocity_modul, -sin(angle) * ball_velocity_modul);
            }
            else if (ball_position.x <= carriage.get_position().x)
            {
                float ball_velocity_modul = sqrt(pow(ball_velocity.x, 2) + pow(ball_velocity.y, 2));
                float angle;
                if ((carriage.get_position().x - ball_position.x) / (carriage.get_width() / 2) > 1)
                {
                    angle = 1;
                }
                else
                {
                    angle = acos((carriage.get_position().x - ball_position.x) / (carriage.get_width() / 2));
                }
                ball_velocity = Vect(-cos(angle) * ball_velocity_modul, -sin(angle) * ball_velocity_modul);
            }
            else
            {
                ball_velocity.x *= -1.0f;
            }
            ball_position.y -= (ball_position.y - ((carriage.get_position().y - carriage.get_height() / 2) - ball_radius)) * 2.0f;
        }
    }
}

void ArkanoidImpl::bricks_matrix_update()
{
    if (ball_position.y - ball_radius <= bricks_matrix.get_bottom_boarder_coord() &&
        ball_position.y + ball_radius >= bricks_matrix.get_top_boarder_coord() &&
        ball_position.x + ball_radius >= bricks_matrix.get_left_boarder_coord() &&
        ball_position.x - ball_radius <= bricks_matrix.get_right_boarder_coord())
    {
        int ball_i_index = floor((ball_position.x - bricks_matrix.get_left_boarder_coord() + bricks_matrix.get_bricks_columns_padding() / 2) / (bricks_matrix.get_brick_width() + bricks_matrix.get_bricks_columns_padding()));
        int ball_j_index = floor((ball_position.y - bricks_matrix.get_top_boarder_coord() + bricks_matrix.get_bricks_rows_padding() / 2) / (bricks_matrix.get_brick_height() + bricks_matrix.get_bricks_rows_padding()));
        int computing_area = 1;
        if (ball_radius > min(bricks_matrix.get_brick_height() + bricks_matrix.get_bricks_rows_padding(), bricks_matrix.get_brick_width() + bricks_matrix.get_bricks_columns_padding()));
        {
            computing_area = 1 + ceil(ball_radius / 
                min(bricks_matrix.get_brick_height() + bricks_matrix.get_bricks_rows_padding(), 
                    bricks_matrix.get_brick_width() + bricks_matrix.get_bricks_columns_padding()));
        }
        if (ball_i_index >= 0 &&
            ball_j_index >= 0 &&
            ball_i_index < bricks_matrix.get_bricks_columns() &&
            ball_j_index < bricks_matrix.get_bricks_rows() &&
            !bricks_matrix.elem_is_empty(ball_i_index, ball_j_index))
        {
            if (ball_position.y - ball_radius <= bricks_matrix.get_matrix()[ball_i_index][ball_j_index].w &&
                ball_position.y + ball_radius >= bricks_matrix.get_matrix()[ball_i_index][ball_j_index].y &&
                ball_position.x - ball_radius <= bricks_matrix.get_matrix()[ball_i_index][ball_j_index].z &&
                ball_position.x + ball_radius >= bricks_matrix.get_matrix()[ball_i_index][ball_j_index].x)
            {
                if (ball_position.y > bricks_matrix.get_matrix()[ball_i_index][ball_j_index].w)
                {
                    ball_j_index++;
                }
                else if (ball_position.x > bricks_matrix.get_matrix()[ball_i_index][ball_j_index].z)
                {
                    ball_i_index++;

                }
                else if (ball_position.y < bricks_matrix.get_matrix()[ball_i_index][ball_j_index].y)
                {
                    ball_j_index--;
                }
                else if (ball_position.x < bricks_matrix.get_matrix()[ball_i_index][ball_j_index].x)
                {
                    ball_i_index--;
                }
            }
        }
        for (int index = computing_area; index >= 1; index--)
        {
            if (ball_i_index >= 0 &&
                ball_j_index - index >= 0 &&
                ball_i_index < bricks_matrix.get_bricks_columns() &&
                ball_j_index - index < bricks_matrix.get_bricks_rows() &&
                !bricks_matrix.elem_is_empty(ball_i_index, ball_j_index - index))
            {
                if (ball_position.y - ball_radius <= bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].w &&
                    ball_position.x >= bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].x &&
                    ball_position.x <= bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].z)
                {                    
                    ball_position.y += fabs(bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].w + ball_radius - ball_position.y) * 2.0f;
                    ball_velocity.y *= -1.0f;
                    bricks_matrix.destruction_elem(ball_i_index, ball_j_index - index);
                }
                else if (ball_position.x < bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].x &&
                    ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].x,
                            bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].w)))
                {
                    ball_velocity = Vect(ball_velocity.y, ball_velocity.x);
                    bricks_matrix.destruction_elem(ball_i_index, ball_j_index - index);
                }
                else if (ball_position.x > bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].z &&
                    ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].z,
                            bricks_matrix.get_matrix()[ball_i_index][ball_j_index - index].w)))
                {
                    ball_velocity = Vect(-ball_velocity.y, -ball_velocity.x);
                    bricks_matrix.destruction_elem(ball_i_index, ball_j_index - index);
                }
            }
            if (ball_i_index - index >= 0 &&
                ball_j_index >= 0 &&
                ball_i_index - index < bricks_matrix.get_bricks_columns() &&
                ball_j_index < bricks_matrix.get_bricks_rows() &&
                !bricks_matrix.elem_is_empty(ball_i_index - index, ball_j_index))
            {
                if (ball_position.x - ball_radius <= bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].z &&
                    ball_position.y >= bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].y &&
                    ball_position.y <= bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].w)
                {
                    ball_position.x += ((bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].z + ball_radius) - ball_position.x) * 2.0f;
                    ball_velocity.x *= -1.0f;
                    bricks_matrix.destruction_elem(ball_i_index - index, ball_j_index);
                }
                else if (ball_position.y < bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].y &&
                    ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].z,
                            bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].y)))
                {
                    ball_velocity = Vect(ball_velocity.y, ball_velocity.x);
                    bricks_matrix.destruction_elem(ball_i_index - index, ball_j_index);
                }
                else if (ball_position.y > bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].w &&
                    ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].z,
                            bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index].w)))
                {
                    ball_velocity = Vect(-ball_velocity.y, -ball_velocity.x);
                    bricks_matrix.destruction_elem(ball_i_index - index, ball_j_index);
                }
            }
            if (ball_i_index >= 0 &&
                ball_j_index + index >= 0 &&
                ball_i_index < bricks_matrix.get_bricks_columns() &&
                ball_j_index + index < bricks_matrix.get_bricks_rows() &&
                !bricks_matrix.elem_is_empty(ball_i_index, ball_j_index + index))
            {
                if (ball_position.y + ball_radius >= bricks_matrix.get_matrix()[ball_i_index][ball_j_index + index].y)
                {
                    ball_position.y -= fabs(ball_position.y - bricks_matrix.get_matrix()[ball_i_index][ball_j_index + index].y + ball_radius) * 2.0f;
                    ball_velocity.y *= -1.0f;
                    bricks_matrix.destruction_elem(ball_i_index, ball_j_index + index);
                }
            }
            if (ball_i_index + index >= 0 &&
                ball_j_index >= 0 &&
                ball_i_index + index < bricks_matrix.get_bricks_columns() &&
                ball_j_index < bricks_matrix.get_bricks_rows() &&
                !bricks_matrix.elem_is_empty(ball_i_index + index, ball_j_index))
            {
                if (ball_position.x + ball_radius >= bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index].x)
                {
                    ball_position.x -= (ball_position.x - (bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index].x - ball_radius)) * 2.0f;
                    ball_velocity.x *= -1.0f;
                    bricks_matrix.destruction_elem(ball_i_index + index, ball_j_index);
                }
            }
            for (int index_2 = computing_area; index_2 >= 1; index_2--)
            {
                if (ball_i_index - index >= 0 &&
                    ball_j_index - index_2 >= 0 &&
                    ball_i_index - index < bricks_matrix.get_bricks_columns() &&
                    ball_j_index - index_2 < bricks_matrix.get_bricks_rows() &&
                    !bricks_matrix.elem_is_empty(ball_i_index - index, ball_j_index - index_2))
                {
                    if (ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index - index_2].z,
                            bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index - index_2].w)))
                    {
                        ball_velocity = Vect(-ball_velocity.y, -ball_velocity.x);
                        bricks_matrix.destruction_elem(ball_i_index - index, ball_j_index - index_2);
                    }
                }
                if (ball_i_index - index >= 0 &&
                    ball_j_index + index_2 >= 0 &&
                    ball_i_index - index < bricks_matrix.get_bricks_columns() &&
                    ball_j_index + index_2 < bricks_matrix.get_bricks_rows() &&
                    !bricks_matrix.elem_is_empty(ball_i_index - index, ball_j_index + index_2))
                {
                    if (ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index + index_2].z,
                            bricks_matrix.get_matrix()[ball_i_index - index][ball_j_index + index_2].y)))
                    {
                        ball_velocity = Vect(ball_velocity.y, ball_velocity.x);
                        bricks_matrix.destruction_elem(ball_i_index - index, ball_j_index + index_2);
                    }
                }
                if (ball_i_index + index >= 0 &&
                    ball_j_index + index_2 >= 0 &&
                    ball_i_index + index < bricks_matrix.get_bricks_columns() &&
                    ball_j_index + index_2 < bricks_matrix.get_bricks_rows() &&
                    !bricks_matrix.elem_is_empty(ball_i_index - index, ball_j_index + index_2))
                {
                    if (ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index + index_2].x,
                            bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index + index_2].y)))
                    {
                        ball_velocity = Vect(-ball_velocity.y, -ball_velocity.x);
                        bricks_matrix.destruction_elem(ball_i_index + index, ball_j_index + index_2);
                    }
                }
                if (ball_i_index + index >= 0 &&
                    ball_j_index - index_2 >= 0 &&
                    ball_i_index + index < bricks_matrix.get_bricks_columns() &&
                    ball_j_index - index_2 < bricks_matrix.get_bricks_rows() &&
                    !bricks_matrix.elem_is_empty(ball_i_index + index, ball_j_index - index_2))
                {
                    if (ball_radius >= distance(ball_position,
                        Vect(bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index - index_2].x,
                            bricks_matrix.get_matrix()[ball_i_index + index][ball_j_index - index_2].w)))
                    {
                        ball_velocity = Vect(ball_velocity.y, ball_velocity.x);
                        bricks_matrix.destruction_elem(ball_i_index + index, ball_j_index - index_2);
                    }
                }
            }
        }
    }
}


void ArkanoidImpl::objects_draw(ImGuiIO& io, ImDrawList &draw_list)
{
    Vect screen_pos = ball_position * world_to_screen;    
    float screen_radius = ball_radius * world_to_screen.x;   
    string score_text = "Score " + to_string(bricks_matrix.get_score());
    draw_list.AddRectFilled(world_to_screen * carriage.get_left_top_coord(), 
        world_to_screen * carriage.get_right_bottom_coord(), 
        ImColor(200, 100, 100), carriage.get_border_radius(), 0);
    draw_list.AddText(0.05f * world_size * world_to_screen, ImColor(100, 100, 100), score_text.c_str(), score_text.c_str() + score_text.length());
    draw_list.AddCircleFilled(screen_pos, screen_radius, ImColor(100, 255, 100));
    for (int i = 0; i < bricks_matrix.get_bricks_columns(); i++)
    {
        for(int j = 0; j < bricks_matrix.get_bricks_rows(); j++)
        {
            draw_list.AddRectFilled(world_to_screen * Vect(bricks_matrix.get_matrix()[i][j].x, bricks_matrix.get_matrix()[i][j].y), 
                world_to_screen * Vect(bricks_matrix.get_matrix()[i][j].z, bricks_matrix.get_matrix()[i][j].w), 
                ImColor(200, 200, 100), 
                bricks_matrix.get_border_radius(),
                0);
        }
    }
}

void ArkanoidImpl::demo_add_debug_hit(ArkanoidDebugData& debug_data, const Vect& world_pos, const Vect& normal)
{
    ArkanoidDebugData::Hit hit;
    hit.screen_pos = world_pos * world_to_screen;
    hit.normal = normal;
    debug_data.hits.push_back(std::move(hit));
}

