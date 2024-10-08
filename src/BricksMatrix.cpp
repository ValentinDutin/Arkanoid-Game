#include "BricksMatrix.h"
#include <iostream>

using namespace std;
BricksMatrix::BricksMatrix() {
matrix= nullptr;
brick_width = 0.0f;
brick_height = 0.0f;
top_boarder_coord = 0.0f;
bottom_boarder_coord = 0.0f;
left_boarder_coord = 0.0f;
right_boarder_coord = 0.0f;
bricks_columns = 0;
bricks_rows = 0;
border_radius = 0.0f;
bricks_columns_padding = 0.0f;
bricks_rows_padding = 0.0f;
score = 0;
}

BricksMatrix::BricksMatrix(Vect world_size, 
    int bricks_columns, 
    int bricks_rows, 
    int bricks_columns_max, 
    int bricks_rows_max, 
    float bricks_columns_padding, 
    float bricks_rows_padding,
    unsigned int current_score)
{
    cout << "1: " << world_size.x << ", 2: " << world_size.y << ", 3: "<< bricks_columns << ", 4:" << bricks_rows << ", 5: " << bricks_columns_max << ", 6: " << bricks_rows_max << ", 7:" << bricks_columns_padding << ", 8: " << bricks_rows_padding << endl;
    this->score = current_score;
    this->deleted_elem_count = 0;
    this->bricks_columns = bricks_columns;
    this->bricks_rows = bricks_rows;
    this->border_radius = 0;
    this->bricks_columns_padding = bricks_columns_padding;
    this->bricks_rows_padding = bricks_rows_padding;
    this->brick_width = (1.0 * world_size.x - this->bricks_columns_padding * (this->bricks_columns + 1)) / bricks_columns_max;
    this->brick_height = (0.5 * world_size.y - this->bricks_rows_padding * (this->bricks_rows - 1)) / bricks_rows_max;
    this->top_boarder_coord = 0.1 * world_size.y + (0.5 * world_size.y - (this->brick_height * this->bricks_rows + this->bricks_rows_padding * (this->bricks_rows - 1))) / 2;
    this->left_boarder_coord = (world_size.x - (this->brick_width * this->bricks_columns + this->bricks_columns_padding * (this->bricks_columns - 1))) / 2;
    this->bottom_boarder_coord = 0.7 * world_size.y - this->top_boarder_coord;
    this->right_boarder_coord = world_size.x - this->left_boarder_coord;
    this->matrix = new Vect4 * [this->bricks_columns];
    int columns_padding_counter = 0;
    for (int i = 0; i < this->bricks_columns; i++)
    {
        int rows_padding_counter = 0;
        this->matrix[i] = new Vect4[this->bricks_rows];
        for (int j = 0; j < this->bricks_rows; j++)
        {
            this->matrix[i][j] = Vect4(this->left_boarder_coord + this->brick_width * i + this->bricks_columns_padding * columns_padding_counter,
                this->top_boarder_coord + this->brick_height * j + this->bricks_rows_padding * rows_padding_counter,
                this->left_boarder_coord + this->brick_width * (i + 1) + this->bricks_columns_padding * columns_padding_counter,
                this->top_boarder_coord + this->brick_height * (j + 1) + this->bricks_rows_padding * rows_padding_counter);
            rows_padding_counter++;
        }
        columns_padding_counter++;
    }
}

void BricksMatrix::delete_matrix()
{
    if (this->matrix != nullptr)
    {
        for (int i = 0; i < this->bricks_columns; i++)
        {
            delete[] matrix[i];
        }
        delete[] matrix;
        cout << "Matrix is delete (function delete)" << endl;
    }
}
Vect4** BricksMatrix::get_matrix()
{
    return this->matrix;
    /*Vect4** result = new Vect4 * [this->bricks_columns];
    for (int i = 0; i < this->bricks_columns; i++) {
        result[i] = new Vect4[this->bricks_rows];
        for (int j = 0; j < this->bricks_rows; j++) {
            result[i][j].x = this->matrix[i][j].x;
            result[i][j].y = this->matrix[i][j].y;
            result[i][j].z = this->matrix[i][j].z;
            result[i][j].w = this->matrix[i][j].w;
        }
    }
    return result;*/
}

float BricksMatrix::get_brick_width() { return this->brick_width; }
float BricksMatrix::get_brick_height() { return this->brick_height; }
float BricksMatrix::get_top_boarder_coord() { return this->top_boarder_coord; }
float BricksMatrix::get_bottom_boarder_coord() { return this->bottom_boarder_coord; }
float BricksMatrix::get_left_boarder_coord() { return this->left_boarder_coord; }
float BricksMatrix::get_right_boarder_coord() { return this->right_boarder_coord; }
int BricksMatrix::get_bricks_columns() { return this->bricks_columns; }
int BricksMatrix::get_bricks_rows() { return bricks_rows; }
float BricksMatrix::get_border_radius() { return this->border_radius; }
float BricksMatrix::get_bricks_columns_padding() { return this->bricks_columns_padding; }
float BricksMatrix::get_bricks_rows_padding() { return this->bricks_rows_padding; }
unsigned int BricksMatrix::get_score() { return this->score; }
void BricksMatrix::set_score(unsigned int value) { this->score = value; }
unsigned int BricksMatrix::get_deleted_elem_count() { return this->deleted_elem_count; }
void BricksMatrix::destruction_elem(int i, int j)
{
    this->matrix[i][j].x = 0.0f;
    this->matrix[i][j].y = 0.0f;
    this->matrix[i][j].z = 0.0f;
    this->matrix[i][j].w = 0.0f;
    cout << "delete elem (" << i << ", " << j << ")" << endl;
    this->score++;
    this->deleted_elem_count++;
}

bool BricksMatrix::elem_is_empty(int i, int j)
{
    if (i < 0 || i >= this->bricks_columns || j < 0 || j >= this->bricks_rows) {
        return true;
    }
    return this->matrix[i][j].x == 0 && this->matrix[i][j].y == 0 && this->matrix[i][j].z == 0 && this->matrix[i][j].w == 0;
}