#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <math.h>
#include <array>
#include <string>



const int numbers[10][15] = {
    { 1, 1, 1,  1, 0, 1,  1, 0, 1,  1, 0, 1,  1, 1, 1 },
    { 1, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0 },
    { 1, 1, 1,  0, 0, 1,  1, 1, 1,  1, 0, 0,  1, 1, 1 },
    { 1, 1, 1,  0, 0, 1,  0, 1, 0,  0, 0, 1,  1, 1, 1 },
    { 0, 0, 1,  0, 1, 1,  1, 0, 1,  1, 1, 1,  0, 0, 1 },
    { 1, 1, 1,  1, 0, 0,  1, 1, 1,  0, 0, 1,  1, 1, 1 },
    { 1, 1, 1,  1, 0, 0,  1, 1, 1,  1, 0, 1,  1, 1, 1 },
    { 1, 1, 1,  0, 0, 1,  0, 1, 1,  0, 0, 1,  0, 0, 1 },
    { 1, 1, 1,  1, 0, 1,  1, 1, 1,  1, 0, 1,  1, 1, 1 },
    { 1, 1, 1,  1, 0, 1,  1, 1, 1,  0, 0, 1,  1, 1, 1 }
};

const float colors[16][3] = {
    { 245, 215, 66 },
    { 230, 147, 39 },
    { 245, 125, 56 },
    { 245, 97, 56 },

    { 255, 65, 13 },
    { 255, 13, 49 },
    { 255, 13, 102 },
    { 255, 13, 227 },

    { 223, 13, 255 },
    { 194, 13, 255 },
    { 150, 13, 255 },
    { 100, 13, 255 },

    { 30, 13, 255 },
    { 13, 81, 255 },
    { 13, 194, 255 },
    { 13, 81, 255 }

};


void DrawCube(int x, int y, int size, float color[3]) {

   
    glColor3f(color[0], color[1], color[2]);
    glVertex2i(x, y);
    glVertex2i(x, y + size);
    glVertex2i(x + size, y);

    glColor3f(color[0], color[1], color[2]);
    glVertex2i(x + size, y + size);
    glVertex2i(x, y + size);
    glVertex2i(x + size, y);
}


void DrawNumbers(std::vector<int> indexes, int x_centre, int y_centre, int font_size, float color[3]) {
    int text_size = indexes.size();
    font_size = int(float(font_size) / 4.0f);
    int width = (text_size * 3 + (text_size - 1)) * font_size;
    int height = text_size * 5;

    int x = x_centre - (width / 2);
    int y = y_centre - (height / 2);

    for (int i = 0; i < text_size; i++) {
        for (int i2 = 0; i2 < 5; i2++) {
            for (int i3 = 0; i3 < 3; i3++) {
                int index = (i2 * 3) + i3;
                if (numbers[indexes[i]][index] == 1) {
                    DrawCube(x + (i3 * font_size), y + (i2 * font_size), font_size, color);
                }
            }
        }
        x += 4 * font_size;
    }
}

//......................................................................

const int window_size = 600;

const int left[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const int right[16] = { 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 };
const int up[16] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
const int down[16] = { 12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3 };

int board[16] = { 0 };
int empty[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
int empty_count = 16;

int row_movement[65536][4] = { {} };


int multipliers[4] = { 4096, 256, 16, 1 };

const int randomness = 100000;
int track_random = 0;
bool place_four[randomness] = { false };

float power = 0;
float score_powers[100];

//......................................................................

void PrintBoard() {

    for (int i = 0; i < 4; i++) {
        for (int i2 = 0; i2 < 4; i2++) {
            int index = (i * 4) + i2;
            std::cout << board[index];
            if (i2 != 3) {
                std::cout << " | ";
            }
        }

        if (i != 3) {
            std::cout << "\n--+---+---+---\n";
        }
        else {
            std::cout << "\n\n\n";
        }


    }


}

int RowToIndex(int row[4]) {
    return row[0] * 4096 + row[1] * 256 + row[2] * 16 + row[3];
}


void AddRandom() {
    if (empty_count == 0) {
        return;
    }
    int random_value = rand() % empty_count;

    int value = 1;
    if (place_four[track_random] == true) {
        value = 2;
    }

    track_random++;
    if (track_random == randomness) {
        track_random = 0;
    }

    board[empty[random_value]] = value;

}


void FillRowMovement(int array[4], int depth) {

    if (depth == 4) {

        int new_array[4] = { 0, 0, 0, 0 };

        int selected = -1;
        int index = 0;
        int jump_count = 0;

        for (int i = 0; i < 4; i++) {
            if (array[i] == 0) {
                jump_count++;
            }
            else if (array[i] == selected) {
                jump_count++;
                new_array[i - jump_count]++;
                new_array[i] = 0;
                selected = -1;
            }
            else {
                new_array[i - jump_count] = array[i];

                if (jump_count > 0) {
                    new_array[i] = 0;
                }

                selected = array[i];
            }
        }

        for (int i = 0; i < 4; i++) {
            row_movement[RowToIndex(array)][i] = new_array[i];

        }

        return;
    }

    int new_depth = depth + 1;
    for (int i = 0; i < 16; i++) {
        array[depth] = i;
        FillRowMovement(array, new_depth);
    }
}


void BoardMovement(const int movement_array[16]) {
    empty_count = 0;
    for (int i = 0; i < 16; i += 4) {
        int row_index = 0;
        for (int i2 = 0; i2 < 4; i2++) {
            int index = i + i2;
            row_index += board[movement_array[index]] * multipliers[i2];
        }


        for (int i3 = 0; i3 < 4; i3++) {

            int value = row_movement[row_index][i3];

            board[movement_array[i + i3]] = value;

            if (value == 0) {
                empty[empty_count] = movement_array[i + i3];
                empty_count++;
            }

        }

    }
}

void MoveUsingNumber(int value) {
    switch (value) {
    case 0:
        BoardMovement(left);
        return;
    case 3:
        BoardMovement(right);
        return;
    case 2:
        BoardMovement(up);
        return;
    case 1:
        BoardMovement(down);
        return;
    }
}

float GetScore() {
    int score = 0;
    int highest = board[0];


    for (int i = 0; i < 4; i++) {
        for (int i2 = 0; i2 < 4; i2++) {
            int value = i * 4 + i2;
            int value_pos = board[value];

            
            if (value_pos == 0) {
                score += 10;
                continue;

            }

            if (i != 0 || i == 3) {
                score += value_pos;
            }
            if (i2 == 0 || i2 == 3) {
                score += value_pos;
            }


        }
    }
    return std::pow(score, 1.3);
}

int max_depth = 6;
float ExpectiMax(int depth, float change, bool is_player) {



    if (depth == max_depth) {
        return GetScore() * change;
    }

    int new_board[16];
    std::copy(std::begin(board), std::end(board), std::begin(new_board));



    float new_depth = depth + 1;
    if (is_player) {

        float highest = -1000000;
        int index = 0;
        for (int i = 0; i < 4; i++) {

            MoveUsingNumber(i);
            if (true) {
                float score = ExpectiMax(new_depth, change, false);
                if (score > highest) {
                    highest = score;
                    if (depth == 0) {
                        index = i;
                    }
                }

                std::copy(std::begin(new_board), std::end(new_board), std::begin(board));
            }
        }
        if (depth == 0) {
            return float(index);
        }
        return highest;
    }



    float sum = 0;
    float total = 0;
    for (int i = 0; i < 16; i++) {

        if (board[i] != 0) {
            continue;
        }


        board[i] = 1;
        sum += ExpectiMax(new_depth, change * 0.9f, true);

        board[i] = 2;
        sum += ExpectiMax(new_depth, change * 0.1f, true);

        board[i] = 0;
    }
    return sum;


}


int main(void)
{

    std::srand(std::time(NULL));

    

    for (int i = 0; i < randomness; i++) {
        if (rand() % 10 == 0) {
            place_four[i] = true;
            continue;
        }
    }

    AddRandom();
    AddRandom();
    AddRandom();


    int filler[4];
    FillRowMovement(filler, 0);

    GLFWwindow* window;



    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_size, window_size, "enge dingen", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    gluOrtho2D(0.0, window_size, window_size, 0.0);
    int pressed = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);


        pressed -= 1;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && pressed < 0) {

            power += 0.05f;
            std::cout << power << "\n";

            for (int i = 0; i < 100; i++) {
                score_powers[i] = pow(i, power);
            }

            for (int i = 0; i < 16; i++) {
                board[i] = 0;
            }

            AddRandom();
            AddRandom();
            AddRandom();
            pressed = 20;


        }

        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && pressed < 0) {

            for (int i = 0; i < 16; i++) {
                board[i] = 0;
            }

            AddRandom();
            AddRandom();
            AddRandom();
            pressed = 20;
        }

        AddRandom();
       
        glBegin(GL_TRIANGLES);

        for (int i = 0; i < 4; i++) {
            for (int i2 = 0; i2 < 4; i2++) {
                int board_value = board[(i * 4) + i2];
                int base = pow(2, board_value);

                std::vector<int> text;

                int divider = 1;
                while (divider < base) {
                    int new_char = (base / divider) % 10;
                    text.insert(text.begin(), new_char);
                    divider *= 10;
                }
                int quarter = window_size / 4;

                int x = quarter / 2 + (quarter * i2);
                int y = quarter / 2 + (quarter * i);

                if (base != 1) {
                    float r = (1.0f / 256.0f) * colors[board_value][0];
                    float g = (1.0f / 256.0f) * colors[board_value][1];
                    float b = (1.0f / 256.0f) * colors[board_value][2];

                    float color[3] = { r, g, b };
                    float color2[3] = { 1, 1, 1 };



                    DrawCube(i2 * quarter, i * quarter, quarter, color);
                    DrawNumbers(text, x, y, 20, color2);
                }
            }
        }



        MoveUsingNumber(ExpectiMax(0, 1, true));



        glEnd();



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
