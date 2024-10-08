#pragma once

#include "base.h"

class BricksMatrix 
{
private:
	Vect4** matrix = nullptr;
	float brick_width;
	float brick_height;
	float top_boarder_coord;
	float bottom_boarder_coord;
	float left_boarder_coord;
	float right_boarder_coord;
	int bricks_columns;
	int bricks_rows;
	float border_radius;
	float bricks_columns_padding;
	float bricks_rows_padding;
	unsigned int score;
	unsigned int deleted_elem_count;
public:
	BricksMatrix();
	BricksMatrix(Vect world_size, 
		int bricks_columns, 
		int bricks_rows, 
		int bricks_columns_max, 
		int bricks_rows_max, 
		float bricks_columns_padding, 
		float bricks_rows_padding,
		unsigned int current_score);
	~BricksMatrix() {}
	void delete_matrix();
	Vect4** get_matrix();
	float get_brick_width();
	float get_brick_height();
	float get_top_boarder_coord();
	float get_bottom_boarder_coord();
	float get_left_boarder_coord();
	float get_right_boarder_coord();
	int get_bricks_columns();
	int get_bricks_rows();
	float get_border_radius();
	float get_bricks_columns_padding();
	float get_bricks_rows_padding();
	void destruction_elem(int i, int j);
	bool elem_is_empty(int i, int j);
	unsigned int get_score();
	unsigned int get_deleted_elem_count();
	void set_score(unsigned int value);
};
