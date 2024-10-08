#include "Carriage.h"

Carriage::Carriage() 
{ 
    Carriage(Vect(0.0f, 0.0f), 0.0f);
}
Carriage::Carriage(Vect position, const float width) {
    this->position = Vect(position);
    this->width = width;
    this->height = 30;
    this->border_radius = 5;
    this->carriage_speed = 5;
}
Vect Carriage::get_position() { return this->position; }
unsigned int Carriage::get_height() { return this->height; }
float Carriage::get_width() { return this->width; }
Vect Carriage::get_left_top_coord() { return Vect(this->position.x - this->width / 2, this->position.y - this->height / 2); }
Vect Carriage::get_right_bottom_coord() { return Vect(this->position.x + this->width / 2, this->position.y + this->height / 2); }
float Carriage::get_border_radius() { return this->border_radius; }
void Carriage::shift_left() { this->position.x -= carriage_speed; }
void Carriage::shift_right() { this->position.x += carriage_speed; }