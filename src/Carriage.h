#pragma once

#include "base.h"

class Carriage {
private:
    float width = 0;
    unsigned int height = 0;
    Vect position = Vect(0.0f);
    float border_radius;
    float carriage_speed;   
public:
    Carriage();
    Carriage(Vect position, const float width);
    ~Carriage() {}
    Vect get_position();
    unsigned int get_height();
    float get_width();
    Vect get_left_top_coord();    
    Vect get_right_bottom_coord();
    float get_border_radius();
    void set_position(Vect new_position);
    void shift_right();
    void shift_left();
};