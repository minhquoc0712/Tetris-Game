#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <random>
#include <QTimer>
#include <QGraphicsRectItem>

namespace Ui {
class MainWindow;
}

// Coordinates of each squares in tetromino or in the grid with x axis
// is to the right and y direction is to below.
struct Coord
{
    int x = 0;
    int y = 0;

    Coord():
        x(0), y(0)
    {
    }

    Coord(int p_x, int p_y):
        x(p_x), y(p_y)
    {
    }
};

// The square in each tetromino or in the grid.
struct Square
{
    // Displaying square on window.
    QGraphicsRectItem* block = NULL;

    // Position of the square.
    Coord pos = Coord();
};


// Tetromino information
struct Tetromino
{
    int type = 0;

    // Squares in tetromino.
    std::vector<Square> squares = std::vector<Square>(4, Square());

    // Color of tetromino.
    QColor color = Qt::white;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;

private slots:

    // Functions related to setup the game.
    void initialize_window();
    void initialize_game();
    void clear_scene();

    // Functions related to status of the game.
    void update_game();
    void update_grid();
    int remove_full_row();
    void move_grid_down(int num_row_remove, int lowest_row_remove);
    void continue_game();
    bool is_over();
    void finish_game();
    void pause_game();
    void quit_game();

    // Function related to player information.
    void set_player_name();
    int calculate_point(int num_row_remove, int num_turn);
    void update_player_score(int num_row_remove);
    void update_level();

    // Functions related to generate and display tetromino.
    void make_new_tetromino();
    void set_appear_position();
    void make_appear();
    void make_appear_over();
    void draw_next_tetromino();

    // Functions related to condition of moving of tetromino.
    bool can_move_down();
    bool can_move_left();
    bool can_move_right();

    // Functions related to move tetromino.
    void make_drop_down_automatic();
    void move_left();
    void move_right();
    void move_down();
    void move_soft_fall();
    void move_hard_fall();
    bool rotation_calculation();
    void rotate_counterclockwise();
    bool reflect_calculation();
    void reflect_vertical_axis();
    void exchange_tetromino();

    // Functions related to button on main window.
    void on_automatic_radio_button_toggled(bool checked);
    void on_manual_radio_button_toggled(bool checked);
    void on_start_game_push_button_clicked();
    void on_fall_button_clicked();

    // Fuctions related to score boards.
    void get_high_scores();
    void sort_score_board();
    void update_score_board();
    void display_score_board();
    std::vector<std::pair<QString, std::pair<QString, QString>>> make_display_information();
    void store_high_scores();
    std::vector<std::string> split_line(const std::string& line,
                                        const char& delimiter);

    // Function related to playing time.
    void display_playing_time();


private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    QGraphicsScene* next_scene_;

    QGraphicsScene* hold_scene_;

    // Constants describing scene coordinates

    // Position of the playing area.
    const int LEFT_MARGIN_PLAYING_VIEW = 100;
    const int TOP_MARGIN_PLAYING_VIEW = 150;
    const int BORDER_DOWN_PLAYING_VIEW = 480;
    const int BORDER_RIGHT_PLAYING_VIEW = 240;

    // Position display of next tetromino.
    const int LEFT_MARGIN_NEXT_VIEW = 226;
    const int TOP_MARGIN_NEXT_VEW = 80;
    const int BORDER_RIGHT_NEXT_VIEW = 114;
    const int BORDER_DOWN_NEXT_VIEW = 60;

    // Position display of hold tetromino
    const int LEFT_MARGIN_HOLD_VIEW = 350;
    const int TOP_MARGI_HOLD_VIEW = 150;
    const int BORDER_RIGHT_HOLD_VIEW = 70;
    const int BORDER_DOWN_HOLD_VIEW = 60;

    // Position display total lines removed.
    const int LEFT_MARGIN_LINES_LABEL = 100;
    const int TOP_MARGIN_LINES_LABEL = 80;
    const int BORDER_RIGHT_LINES_LABEL = 114;
    const int BORDER_DOWN_LINES_LABEL = 60;

    // Position display playing score.
    const int LEFT_MARGIN_SCORE_LABEL = 110;
    const int TOP_MARGIN_SCORE_LABEL = 20;
    const int BORDER_RIGHT_SCORE_LABEL = 220;
    const int BORDER_DOWN_SCORE_LABEL = 50;

    // Position display tetris score.
    const int LEFT_MARGIN_TETRIS_LABEL = 350;
    const int TOP_MARGIN_TETRIS_LABEL = 220;
    const int BORDER_RIGHT_TETRIS_LABEL = 72;
    const int BORDER_DOWN_TETRIS_LABEL = 60;

    // Size of a tetromino component
    const int SQUARE_SIDE = 20;

    // numver of square in each tetromino.
    const int NUM_SQUARE = 4;

    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT_PLAYING_VIEW / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN_PLAYING_VIEW / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};
    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.


    //*************************************************************************
    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    //*************************************************************************
    // Information about the shape of tetrominos when tetromino is place in the
    // upper left corner of the grid ant x axis direction is to the right and
    // y axis direction is to the bottom.

    // Index of position int the grid of each square in each interomino.
    const std::vector<Coord> horizotal_tetro = {Coord(0, 0), Coord(1, 0),
                                                Coord(2, 0), Coord(3, 0)};

    const std::vector<Coord> left_corner_tetro = {Coord(0, 0), Coord(0, 1),
                                                  Coord(1, 1), Coord(2, 1)};

    const std::vector<Coord> right_corner_tetro = {Coord(0, 1), Coord(1, 1),
                                                   Coord(2, 1), Coord(2, 0)};

    const std::vector<Coord> square_tetro = {Coord(0, 0), Coord(0, 1),
                                             Coord(1, 1), Coord(1, 0)};

    const std::vector<Coord> step_up_right_tetro = {Coord(0, 1), Coord(1, 1),
                                                    Coord(1, 0), Coord(2, 0)};

    const std::vector<Coord> pyramid_tetro = {Coord(0, 1), Coord(1, 1),
                                              Coord(1, 0), Coord(2, 1)};

    const std::vector<Coord> step_up_left_tetro = {Coord(0, 0), Coord(1, 0),
                                                   Coord(1, 1), Coord(2, 1)};


    // Collect all information to one place.
    const std::vector<std::vector<Coord>> COORD_INFO = {horizotal_tetro,
                                                left_corner_tetro, right_corner_tetro,
                                                square_tetro, step_up_right_tetro,
                                                pyramid_tetro, step_up_left_tetro};

    // Information about the outer most postion of each tetromino in each
    // direction when the tetromino is place in the position specified in the
    // comment above.
    const std::vector<int> BOTTOM_INFO = {0, 1, 1, 1, 1, 1, 1};
    const std::vector<int> LEFT_INFO = {0, 0, 0, 0, 0, 0, 0};
    const std::vector<int> RIGHT_INFO = {3, 2, 2, 1, 2, 2, 2};
    const std::vector<int> UP_INFO = {0, 0, 0, 0, 0, 0, 0};


    //*************************************************************************
    // Information about the level in the game. There are are six levels.

    // Number of levels in the game.
    const int NUM_LEVELS = 8;

    // The score player need to upgrade to the next level.
    const std::vector<int> LEVEL_THRESHOLD = {5000, 50000, 100000, 150000, 200000,
                                              300000, 400000, 500000};

    // Number of color of tetromino in each level.
    int NUM_CORLOR_IN_LEVEL = 5;

    // The color of tetromino is change for each level.
    const std::vector<std::vector<QString>> COLOR_CODE_SET =
    {{"#0444BF", "#0584F2", "#0AAFF1", "#EDF259", "#A79674"},
    {"#04060F", "#03353E", "#0294A5", "#A79C93", "#C1403D"},
    {"#BE3B45", "#F07995", "#F3F1F3", "#A58E87", "#BE302B"},
    {"#A4A4BF", "#16235A", "#2A3457", "#888C46", "#F2EAED"},
    {"#E0E8F0", "#51A2D9", "#53C0F0", "#B9E5F3", "#8A140E"},
    {"#55D9C0", "#C7F6EC", "#107050", "#02231C", "#4DD8AD"},
    {"#BD3E85", "#182657", "#121F40", "#D59B2D", "#8D541E"},
    {"#C2D3DA", "#81A3A7", "#585A56", "#F1F3F2", "#272424"}};

    // The droping speed of tetromino in the first level.
    const int STARTING_SPEED = 650;


    //*************************************************************************
    // Constant related to the scoreboard.

    // File name to get the highest scores.
    // Remeber to build the proram in the same folder of this file.
    const std::string HIGHEST_SCORES_FILE = "highest_scores.txt";

    const int HIGHEST_SCORES_DISPLAY_NUM = 3;

    //*************************************************************************
    // Other constant.

    // Border for square in tetromino and in grid.
    const QPen BLACK_PEN = QPen(Qt::black);

    // Move down six square if possible in soft fall movement.
    const int MOVE_SOFT = 6;

    //*************************************************************************

    // Attributes in the class.

    //*******************************************
    // Time related attributes.

    // For tetronimo continuous moving
    QTimer timer_;

    // For calculate time of playing.
    QTimer playing_timer_;

    // For display playing time.
    int minute_ = 0;
    int second_ = 0;
    int hour_ = 0;

    //*******************************************
    // Tetrominos related attributes.

    // Tetromino moved by the player.
    Tetromino curr_tetro_;

    // Infomation of moved tetromino get from
    // the information specified above.
    int bottom_;
    int left_;
    int right_;
    int up_;

    // Next tetromino droped.
    Tetromino next_tetro_;

    // Hold tetromino.
    Tetromino hold_tetro_;

    //*******************************************
    // Attribute of the grid of playing area.
    std::vector<std::vector<Square>> grid_;

    //*******************************************
    // Control game related attributes.

    bool game_started_ = false;
    bool game_running_ = false;
    bool game_over_ = false;
    bool play_automatic_ = true;
    bool can_hold_ = true;
    bool is_hold_empty_ = true;

    //*******************************************
    // Attributes related to player.

    // For storing point and update level.
    std::string player_name_ = "";

    // Calculate and store points.
    int playing_level_ = 0;
    int playing_points_ = 0;
    int total_lines_removed_ = 0;
    int tetris_points_ = 0;
    int num_turn_ = 0;

    // Change by each level.
    int playing_speed_ = 0;


    //*******************************************
    // Scoreboards related attribute.

    // Highest scores read from HIGHEST_SCORES_FILE.
    // First element in the pair represents the player name.
    // First part of the second element represents the points and
    // the second part of second element is time of playing in second.
    std::vector<std::pair<std::string, std::pair<int, int>>> score_board_;

};

#endif // MAINWINDOW_HH
