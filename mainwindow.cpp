#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QKeyEvent>
#include <fstream>
#include <utility>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), play_automatic_(true)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);
    next_scene_ = new QGraphicsScene(this);
    hold_scene_ = new QGraphicsScene(this);

    timer_.setSingleShot(false);
    playing_timer_.setSingleShot(false);

    //*************************************************************************
    // Setting for connection.
    // Connection for dropping and display playing time.

    connect(&timer_, &QTimer::timeout,
            this, &MainWindow::make_drop_down_automatic);
    connect(&playing_timer_, &QTimer::timeout,
            this, &MainWindow::display_playing_time);

    // Connection for getting name of player.
    connect(ui->player_name_line_edit, &QLineEdit::returnPressed,
            this, &MainWindow::set_player_name);

    connect(ui->name_edit_push_button, &QPushButton::clicked,
            this, &MainWindow::set_player_name);

    // Connection for pause and quit game.
    connect(ui->pause_game_push_button, &QPushButton::clicked,
            this, &MainWindow::pause_game);
    connect(ui->close_game_push_button, &QPushButton::clicked,
            this, &MainWindow::quit_game);


    //*************************************************************************
    // Setting random engine ready for the first real call.

    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng);
    // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.


    //*************************************************************************
    // Initialize the game.

    // Setting for displaying on window.
    initialize_window();

    // Read score board information stored.
    get_high_scores();

    // Start the game.
    initialize_game();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//*****************************************************************************
// Function related to setup the game.

// Clear all the scene.
void MainWindow::clear_scene()
{
    scene_->clear();
    next_scene_->clear();
    hold_scene_->clear();
}

// Set up window for program
void MainWindow::initialize_window()
{
    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.

    // Setup position for playing area.
    ui->graphicsView->setGeometry(LEFT_MARGIN_PLAYING_VIEW,
                                  TOP_MARGIN_PLAYING_VIEW,
                                  BORDER_RIGHT_PLAYING_VIEW + 2,
                                  BORDER_DOWN_PLAYING_VIEW + 2);

    ui->graphicsView->setScene(scene_);

    scene_->setSceneRect(0, 0, BORDER_RIGHT_PLAYING_VIEW - 1,
                         BORDER_DOWN_PLAYING_VIEW - 1);


    //*************************************************************************
    // Setup position for display incoming tetromino.

    ui->next_tetromino_graphic_view->setGeometry(LEFT_MARGIN_NEXT_VIEW,
                                                 TOP_MARGIN_NEXT_VEW,
                                                 BORDER_RIGHT_NEXT_VIEW + 2,
                                                 BORDER_DOWN_NEXT_VIEW + 2);

    ui->next_tetromino_graphic_view->setScene(next_scene_);

    next_scene_->setSceneRect(0, 0, BORDER_RIGHT_NEXT_VIEW - 1,
                              BORDER_DOWN_NEXT_VIEW -1);


    //*************************************************************************
    // Setup position for display hold tetromino.

    ui->hold_graphic_view->setGeometry(LEFT_MARGIN_HOLD_VIEW,
                                       TOP_MARGI_HOLD_VIEW,
                                       BORDER_RIGHT_HOLD_VIEW + 2,
                                       BORDER_DOWN_HOLD_VIEW + 2);

    ui->hold_graphic_view->setScene(hold_scene_);

    hold_scene_->setSceneRect(0, 0, BORDER_RIGHT_HOLD_VIEW - 1,
                              BORDER_DOWN_HOLD_VIEW - 1);


    //*************************************************************************
    // Setup position for display lines removed.

    ui->lines_remove_label->setStyleSheet("QLabel { background-color : white; "
                                          "border : 1px solid grey; "
                                          "font : 12pt; color : violet }");

    ui->lines_remove_label->setGeometry(LEFT_MARGIN_LINES_LABEL,
                                        TOP_MARGIN_LINES_LABEL,
                                        BORDER_RIGHT_LINES_LABEL + 2,
                                        BORDER_DOWN_LINES_LABEL + 2);

    ui->lines_remove_label->setAlignment(Qt::AlignCenter);

    ui->lines_remove_label->setScaledContents(true);


    //*************************************************************************
    // Setup position for display score of player.

    ui->player_score_label->setStyleSheet("QLabel { background-color : white; "
                                          "border : 1px solid grey; "
                                          "font : 12pt; color : green; }");

    ui->player_score_label->setGeometry(LEFT_MARGIN_SCORE_LABEL,
                                        TOP_MARGIN_SCORE_LABEL,
                                        BORDER_RIGHT_SCORE_LABEL,
                                        BORDER_DOWN_SCORE_LABEL);

    ui->player_score_label->setAlignment(Qt::AlignCenter);

    ui->player_score_label->setScaledContents(true);


    //*************************************************************************
    // Setup position for display tetris points.

    ui->tetris_point_label->setStyleSheet("QLabel { background-color : white; "
                                          "border : 1px solid grey; "
                                          "font : 10pt; color : red; }");

    ui->tetris_point_label->setGeometry(LEFT_MARGIN_TETRIS_LABEL,
                                        TOP_MARGIN_TETRIS_LABEL,
                                        BORDER_RIGHT_TETRIS_LABEL,
                                        BORDER_DOWN_TETRIS_LABEL);

    ui->tetris_point_label->setAlignment(Qt::AlignCenter);

    ui->tetris_point_label->setScaledContents(true);


    //*************************************************************************
    // Setup display the score board.

    //*******************************************
    // Highest point player.

    // Player name
    ui->player_name_first_label->setStyleSheet("QLabel { background-color : white; "
                                               "color : black; }");

    ui->player_name_first_label->setAlignment(Qt::AlignCenter);

    ui->player_name_first_label->setScaledContents(true);

    // Score.
    ui->score_first_label->setStyleSheet("QLabel { background-color : white; "
                                         "color : black; }");

    ui->score_first_label->setAlignment(Qt::AlignCenter);

    ui->score_first_label->setScaledContents(true);

    // Playing time.
    ui->playing_time_first_label->setStyleSheet("QLabel { background-color : white; "
                                                "color : black; }");

    ui->playing_time_first_label->setAlignment(Qt::AlignCenter);

    ui->playing_time_first_label->setScaledContents(true);


    //*******************************************
    // Second highest player.

    // Player name.
    ui->player_name_second_label->setStyleSheet("QLabel { background-color : white; "
                                                "color : black; }");

    ui->player_name_second_label->setAlignment(Qt::AlignCenter);

    ui->player_name_second_label->setScaledContents(true);

    // Score.
    ui->score_second_label->setStyleSheet("QLabel { background-color : white; "
                                          "color : black; }");

    ui->score_second_label->setAlignment(Qt::AlignCenter);

    ui->score_second_label->setScaledContents(true);

    // Playing time.
    ui->playing_time_second_label->setStyleSheet("QLabel { background-color : white; "
                                                 "color : black; }");

    ui->playing_time_second_label->setAlignment(Qt::AlignCenter);

    ui->playing_time_second_label->setScaledContents(true);

    //*******************************************
    // Third highest player.

    // Player name
    ui->player_name_third_label->setStyleSheet("QLabel { background-color : white; "
                                               "color : black; }");

    ui->player_name_third_label->setAlignment(Qt::AlignCenter);

    ui->player_name_third_label->setScaledContents(true);

    // Score.
    ui->score_third_label->setStyleSheet("QLabel { background-color : white; "
                                         "color : black; }");

    ui->score_third_label->setAlignment(Qt::AlignCenter);

    ui->score_third_label->setScaledContents(true);

    // Playing time.
    ui->playing_time_third_label->setStyleSheet("QLabel { background-color : white; "
                                                "color : black; }");

    ui->playing_time_third_label->setAlignment(Qt::AlignCenter);

    ui->playing_time_third_label->setScaledContents(true);


    //*************************************************************************
    // Setup for display message to the player.

    ui->game_message_label->setStyleSheet("QLabel { border : 2px solid black; "
                                          "color : black; font : 8pt;}");
    ui->game_message_label->setAlignment(Qt::AlignCenter);
    ui->game_message_label->setScaledContents(true);


    // ************************************************************************
    // Set up for display playing time.

    ui->number_sec_lcd->setStyleSheet("background-color:blue;");
    ui->number_min_lcd->setStyleSheet("background-color:blue;");
    ui->number_hou_lcd->setStyleSheet("background-color:blue;");
}

// Setup value for start the game.
void MainWindow::initialize_game()
{
    clear_scene();

    // Initialize tetromino.
    curr_tetro_ = Tetromino();

    left_ = 0;
    right_ = 0;
    up_ = 0;
    bottom_ = 0;

    // Initailize next tetromino.
    next_tetro_  = Tetromino();

    // Initailize hold tetromino.
    hold_tetro_ = Tetromino();

    // Initialize grid.
    grid_ = std::vector<std::vector<Square>>(ROWS, std::vector<Square>(COLUMNS, Square()));

    // Time related information in the game.
    minute_ = 0;
    second_ = 0;
    hour_ = 0;

    ui->number_hou_lcd->display(0);
    ui->number_min_lcd->display(0);
    ui->number_sec_lcd->display(0);

    // Player related information in the game.
    playing_level_ = 0;
    total_lines_removed_ = 0;
    playing_points_ = 0;
    tetris_points_ = 0;
    playing_speed_ = STARTING_SPEED;
    num_turn_ = 0;

    // Setup information for the game.
    ui->start_game_push_button->setEnabled(true);
    ui->automatic_radio_button->setChecked(play_automatic_);
    ui->fall_button->setEnabled(true);
    ui->name_edit_push_button->setEnabled(true);
    ui->player_name_line_edit->setEnabled(true);

    ui->lines_remove_label->setText(QString("LINE\n0"));
    ui->player_score_label->setText(QString("SCORE 0"));
    ui->tetris_point_label->setText(QString("TETRIS\n0"));

    game_started_ = false;
    game_over_ = false;
    game_running_ = false;

    can_hold_ = true;
    is_hold_empty_ = true;
    hold_tetro_.type = 0;

    next_tetro_.type = distr(randomEng) % NUMBER_OF_TETROMINOS;

    int random_color_index = distr(randomEng) % NUM_CORLOR_IN_LEVEL;
    next_tetro_.color = COLOR_CODE_SET.at(playing_level_).at(random_color_index);
}

//*****************************************************************************
// Function handle key press by player

// Getting key command and move the tetromino.
void MainWindow::keyPressEvent(QKeyEvent *event)
{

    if (!timer_.isActive())
    {
        if (game_running_ && !play_automatic_ &&
                event->key() == Qt::Key_T)
        {
            // Drop new tetromino by press key board.
            on_fall_button_clicked();
        }

        return;
    }

    if (event->key() == Qt::Key_A ||
        event->key() == Qt::Key_4)
    {
        if (can_move_left())
        {
           // Move to the left one square.
           move_left();
        }

        return;
    }

    if (event->key() == Qt::Key_D ||
        event->key() == Qt::Key_6)
    {
        if (can_move_right())
        {
            // Move to the right one square.
            move_right();
        }

        return;
    }

    if (event->key() == Qt::Key_W ||
        event->key() == Qt::Key_8)
    {
        // Rotation
        rotate_counterclockwise();

        // Used for calculate points.
        num_turn_ += 1;

        return;
    }

    if (event->key() == Qt::Key_S ||
        event->key() == Qt::Key_5)
    {
        // Fall down 5 unit.
        move_soft_fall();
        return;
    }

    if (event->key() == Qt::Key_C ||
        event->key() == Qt::Key_7)
    {
        // Fall to the bottom.
        move_hard_fall();
        return;

    }

    if (event->key() == Qt::Key_F ||
        event->key() == Qt::Key_9)
    {
        // hold tetromino.
        exchange_tetromino();
        return;
    }

    if (event->key() == Qt::Key_R ||
        event->key() == Qt::Key_3)
    {
        // Reflection tetromino.
        reflect_vertical_axis();
        return;
    }

}


//*****************************************************************************
// Functions related to status of the game.

// Update grid calculate scores and continue play
// game.
void MainWindow::update_game()
{
    // Add new squares to the grid..
    update_grid();

    int num_row_remove = remove_full_row();

    // Calculate point and possible update for the
    // scoreboard.
    update_player_score(num_row_remove);
    update_score_board();

    // Continue playing.
    if (play_automatic_)
    {
        continue_game();
    }
    else
    {
        ui->fall_button->setEnabled(true);
    }
}

// Move the square of tetromino from the scene and replace
// by the square in that positon of the grid.
void MainWindow::update_grid()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);
        QBrush brush = QBrush(curr_tetro_.squares.at(i).block->brush());

        grid_.at(c.y).at(c.x).pos = Coord(c.x, c.y);
        grid_.at(c.y).at(c.x).block = scene_->addRect(0, 0, SQUARE_SIDE,
                                        SQUARE_SIDE, BLACK_PEN, brush);

        grid_.at(c.y).at(c.x).block->setPos(c.x * SQUARE_SIDE,
                                            c.y * SQUARE_SIDE);

        scene_->removeItem(curr_tetro_.squares.at(i).block);
    }
}

// Remove full row and move the grid down.
int MainWindow::remove_full_row()
{
    int total_remove_row = 0;
    int lowest_row_remove = -1;
    int num_row_remove = 0;

    do
    {
        move_grid_down(num_row_remove, lowest_row_remove);

        total_remove_row += num_row_remove;

        // Set up values for next round of finding.
        num_row_remove = 0;
        lowest_row_remove = -1;

        // Find complete row.
        for (int row = bottom_; row  >= up_; --row)
        {
            bool full_row = true;
            for (int col = 0; col < COLUMNS; ++col)
            {
                if (grid_.at(row).at(col).block == NULL)
                {
                    full_row = false;
                    break;
                }
            }

            if (full_row)
            {
                num_row_remove += 1;
                if (lowest_row_remove < row)
                {
                    lowest_row_remove = row;
                }
            }
        }
    }
    while (num_row_remove != 0);

    return total_remove_row;
}

// Move the grid down.
void MainWindow::move_grid_down(int num_row_remove, int lowest_row_remove)
{
    if (num_row_remove <= 0)
    {
        return;
    }

    // Remove complete row.
    for (int row = lowest_row_remove; row >= 0; --row)
    {
        for (int col = 0; col < COLUMNS; ++col)
        {
            // Check if there is a square above.
            bool square_above = true;

            if (row - num_row_remove < 0)
            {
                square_above = false;
            }
            else
            {
                if (grid_.at(row - num_row_remove).at(col).block == NULL)
                {
                    square_above = false;
                }
            }

            if (!square_above)
            {
                // Remove square in current position since no square will going
                // fill it position.
                if (grid_.at(row).at(col).block != NULL)
                {
                    scene_->removeItem(grid_.at(row).at(col).block);
                }

                grid_.at(row).at(col).block = NULL;
            }
            else
            {
                // There is a square above will move to current position the grid.

                // Remove square in current position on the grid.
                if (grid_.at(row).at(col).block != NULL)
                {
                    scene_->removeItem(grid_.at(row).at(col).block);
                }

                grid_.at(row).at(col).block = NULL;

                // Draw new square in current positon.
                QBrush brush = grid_.at(row - num_row_remove).at(col).block->brush();

                grid_.at(row).at(col).block = scene_->addRect(0, 0, SQUARE_SIDE,
                                                SQUARE_SIDE, BLACK_PEN, QBrush(brush));

                grid_.at(row).at(col).block->setPos(col * SQUARE_SIDE,
                                                    row * SQUARE_SIDE);
            }
        }
    }
}

// Coninue playing the game.
void MainWindow::continue_game()
{
    make_new_tetromino();

    // Finish the game if game is over.
    if (is_over())
    {
        make_appear_over();
        finish_game();
        return;
    }

    // Set up value for continue playing.
    can_hold_ = true;
    game_running_ = true;

    // Continue playing.
    make_appear();
    draw_next_tetromino();
    timer_.start(playing_speed_);
}

// Game is over when tetromino the get into playing area.
bool MainWindow::is_over()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);

        if (grid_.at(c.y).at(c.x).block!= NULL)
        {
            game_over_ = true;
        }
    }

    return game_over_;
}

// Setup when game is finish.
void MainWindow::finish_game()
{
    playing_timer_.stop();
    ui->game_message_label->setText("Game finish.");

    game_running_ = false;

    ui->start_game_push_button->setText("Play again");
    ui->start_game_push_button->setEnabled(true);
    ui->name_edit_push_button->setEnabled(true);
    ui->player_name_line_edit->setEnabled(true);

    // Store highest scores information to a file for next games.
    store_high_scores();
}

// Pause game but playing time clock will not stop.
void MainWindow::pause_game()
{
    if (!game_started_)
    {
        ui->game_message_label->setText("Game has not started.");
        return;
    }

    if (game_over_)
    {
        ui->game_message_label->setText("Game is over.");
        return;
    }

    if (game_running_)
    {
        ui->game_message_label->setText("Game pause.");
        ui->pause_game_push_button->setText("Resume game");

        timer_.stop();

        game_running_ = false;

    }
    else
    {
        ui->game_message_label->setText("Continue game.");
        ui->pause_game_push_button->setText("Pause");

        // If play automatic the tetromino start dropping.
        if (play_automatic_)
        {
            timer_.start(playing_speed_);
        }
        else
        {
            // If play manually but there is already tetromino
            // dropped.
            if (curr_tetro_.squares.front().block != NULL)
            {
                timer_.start(playing_speed_);
            }
        }

        game_running_ = true;
    }

}

// Store high score and close the window.
void MainWindow::quit_game()
{
    store_high_scores();
    close();
}


//*****************************************************************************
// Function related to player information.

// Calculate point after each drop.
int MainWindow::calculate_point(int num_row_remove, int num_turn)
{
    // For each tetromino drop player get 100 points.
    // The point will not be negative.
    int point = 100;

    // The player should turn as less as possible.
    // For each turn excepts the first three turn the
    // points is minus to 5 points.
    if (num_turn > 3)
    {
        if (point - (num_turn - 3) * 5 <= 0)
        {
            point = 0;
        }
        else
        {
            point -= 5 * num_turn;
        }
    }

    // Point earn from make a complete rows.
    if (num_row_remove < 4)
    {
        point += num_row_remove * 1000;
    }
    else
    {
        // More point from remove large number of
        // rows.
        point += num_row_remove * 2000;
    }

    return point;
}

// Update player score after each drop and update leve.
void MainWindow::update_player_score(int num_row_remove)
{
    // Calculate point.
    playing_points_ += calculate_point(num_row_remove, num_turn_);
    total_lines_removed_ += num_row_remove;

    if (num_row_remove >= 4)
    {
        tetris_points_ += 1;
    }

    ui->lines_remove_label->setText(QString("LINE\n") +
                                    QString::number(total_lines_removed_));

    ui->player_score_label->setText(QString("SCORE ") +
                                    QString::number(playing_points_));

    ui->tetris_point_label->setText(QString("TETRIS\n") +
                                    QString::number(tetris_points_));


    if (playing_points_ >= LEVEL_THRESHOLD.at(playing_level_))
    {
        update_level();
    }
}

// Update playing level.
void MainWindow::update_level()
{
    // Update level.
    if (playing_level_ < NUM_LEVELS - 1)
    {
        playing_level_ += 1;

        // Increasing fall speed.
        // Possible update rule of increasing
        playing_speed_ -= 70;

        QString level_message = "Level up. Level " +
                QString::number(playing_level_ + 1);

        ui->game_message_label->setText(level_message);
    }
    else
    {
        ui->game_message_label->setText("Maximum level.");
    }
}

// Get player name. If no thing is provided the player
// name is "No name".
void MainWindow::set_player_name()
{
    player_name_ = ui->player_name_line_edit->text().toStdString();
    ui->player_name_line_edit->setText("");

    QString player_name_message = QString("Welcome") +
            QString::fromStdString(' ' + player_name_) + QString('.');

    ui->game_message_label->setText(player_name_message);

    if (player_name_ == "")
    {
        player_name_ = "No name";
    }
}


//*****************************************************************************
// Functions related to generate and display tetromino.

// Create new tetromino for next drop.
void MainWindow::make_new_tetromino()
{
    // Getting information to make new tetromino.
    curr_tetro_.type = next_tetro_.type;
    curr_tetro_.color = next_tetro_.color;
    num_turn_ = 0;

    // Assign information for tetromino based on its typed.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).pos =
                Coord(COORD_INFO.at(curr_tetro_.type).at(i));

        bottom_ = BOTTOM_INFO.at(curr_tetro_.type);
        left_ = LEFT_INFO.at(curr_tetro_.type);
        right_ = RIGHT_INFO.at(curr_tetro_.type);
        up_ = UP_INFO.at(curr_tetro_.type);
    }

    // Set appearance position of new tetromino.
    set_appear_position();

    // Prepare information of the next tetromino.
    next_tetro_.type = distr(randomEng) % NUMBER_OF_TETROMINOS;
    int color_index = distr(randomEng) % NUM_CORLOR_IN_LEVEL;
    next_tetro_.color = COLOR_CODE_SET.at(playing_level_).at(color_index);
}

// Align appear position of tetromino in the center.
// Possible update for more appearing affect.
void MainWindow::set_appear_position()
{
    // Align to center.
    int deltaX = ((COLUMNS - 1) / 2) - (right_ - left_) / 2;

    // Set square position.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).pos.x += deltaX;
    }

    // Update related value.
    left_ += deltaX;
    right_ += deltaX;
}

// Draw tetromino on the playing area.
void MainWindow::make_appear()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);
        curr_tetro_.squares.at(i).block = scene_->addRect(0, 0, SQUARE_SIDE,
                                                          SQUARE_SIDE, BLACK_PEN,
                                                          QBrush(curr_tetro_.color));

        curr_tetro_.squares.at(i).block->setPos(c.x * SQUARE_SIDE,
                                                c.y * SQUARE_SIDE);
    }
}

// Draw part of tetromino when it can not get into
// playing area.
void MainWindow::make_appear_over()
{
    // Find how many square need to move up to fit in
    // playing area.
    int move_up = 1;
    while (bottom_ - move_up >= 0)
    {
        bool found_suitable_position = true;
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            Coord c(curr_tetro_.squares.at(i).pos);
            c.y -= move_up;

            // Check only part apper on the scene.
            if (c.y >= 0)
            {
                if (grid_.at(c.y).at(c.x).block!= NULL)
                {
                    found_suitable_position = false;
                    break;
                }
            }
        }

        if (found_suitable_position)
        {
            break;
        }

        // Try moving up one unit.
        move_up += 1;
    }

    // Move up tetromino for fit in playing area.
    if (bottom_ - move_up >= 0)
    {
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            Coord c(curr_tetro_.squares.at(i).pos);

            c.y -= move_up;

            if (c.y >= 0)
            {
                grid_.at(c.y).at(c.x).block =
                        scene_->addRect(0, 0,SQUARE_SIDE, SQUARE_SIDE,
                        BLACK_PEN, QBrush(curr_tetro_.squares.at(i).block->brush()));

                grid_.at(c.y).at(c.x).block->setPos(c.x * SQUARE_SIDE,
                                                    c.y * SQUARE_SIDE);
            }
        }
     }
}

// Drop tetrmonio by time out.
void MainWindow::make_drop_down_automatic()
{
    if (can_move_down())
    {
        move_down();
    }
    else
    {
        timer_.stop();
        update_game();
    }
}

// Draw incoming tetromino in the next scene.
void MainWindow::draw_next_tetromino()
{
    next_scene_->clear();

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        next_tetro_.squares.at(i).pos = Coord(COORD_INFO.at(next_tetro_.type).at(i));

        Coord c(next_tetro_.squares.at(i).pos);
        QBrush brush = QBrush(next_tetro_.color);

        // Align the tetromino.
        next_tetro_.squares.at(i).block =
                next_scene_->addRect(0, 0, SQUARE_SIDE / 1.2,
                                     SQUARE_SIDE / 1.2, BLACK_PEN, brush);

        next_tetro_.squares.at(i).block->setPos(c.x * SQUARE_SIDE + 30,
                                                c.y * SQUARE_SIDE + 10);
    }

}


//*****************************************************************************
// Functions related to condition of moving of tetromino.

// Check if possible moving down.
bool MainWindow::can_move_down()
{
    if (bottom_ + 1 >= ROWS)
    {
        return false;
    }

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);

        if (grid_.at(c.y + 1).at(c.x).block!= NULL)
        {
            return false;
        }
    }

    return true;
}

// Check if possible move to the left.
bool MainWindow::can_move_left()
{
    // The border is on the left.
    if (left_ - 1 < 0)
    {
        return false;
    }

    // There are other tetrominos on the left.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        int x_index = curr_tetro_.squares.at(i).pos.x;
        int y_index = curr_tetro_.squares.at(i).pos.y;

        if (grid_.at(y_index).at(x_index - 1).block!= NULL)
        {
            return false;
        }
    }

    // Can move to the left.
    return true;
}

// Check if possible move to the right.
bool MainWindow::can_move_right()
{
    // The border is on the right.
    if (right_ + 1 >= COLUMNS)
    {
        return false;
    }

    // There are other tetrominos on the right.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        int x_index = curr_tetro_.squares.at(i).pos.x;
        int y_index = curr_tetro_.squares.at(i).pos.y;

        if (grid_.at(y_index).at(x_index + 1).block!= NULL)
        {
            return false;
        }
    }

    // Can move to the left.
    return true;
}


//*****************************************************************************
// Functions related to move tetromino.


// Move teromino one square to the left.
void MainWindow::move_left()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).block->moveBy(-SQUARE_SIDE, 0);
        curr_tetro_.squares.at(i).pos.x -= 1;
    }

    left_ -= 1;
    right_ -= 1;
}

// Move tetromino one square to the right.
void MainWindow::move_right()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).block->moveBy(SQUARE_SIDE, 0);
        curr_tetro_.squares.at(i).pos.x += 1;
    }

    left_ += 1;
    right_ += 1;
}

// Move tetromino one square down.
void MainWindow::move_down()
{
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
       curr_tetro_.squares.at(i).block->moveBy(0, SQUARE_SIDE);
       curr_tetro_.squares.at(i).pos.y += 1;
    }

    bottom_ += 1;
    up_ += 1;
}

// Move tetromino down six square if possible.
// if not then move as low as possible.
void MainWindow::move_soft_fall()
{
    // Move lowst as possible.
    if (bottom_ + MOVE_SOFT >= ROWS)
    {
        move_hard_fall();
        return;
    }

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);

        for (int j = 1; j <= MOVE_SOFT; ++j)
        {
            if (grid_.at(c.y + j).at(c.x).block!= NULL)
            {
                // Move to lowest possible.
                move_hard_fall();
                return;
            }
        }
    }

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).block->moveBy(0, MOVE_SOFT * SQUARE_SIDE);
        curr_tetro_.squares.at(i).pos.y += MOVE_SOFT;
    }

    bottom_ += MOVE_SOFT;
    up_ += MOVE_SOFT;
}

// Move down as lowest as possible
void MainWindow::move_hard_fall()
{
    int deltaY = ROWS;

    // Find distance to move down.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);

        // Find how many step can go down.
        int j = 0;
        while (c.y + j < ROWS &&
               grid_.at(c.y + j).at(c.x).block == NULL)
        {
            j += 1;
        }

        j -= 1;

        if (deltaY > j)
        {
            deltaY = j;
        }
    }

    // Move to the surface of fallen tetrimions.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        curr_tetro_.squares.at(i).block->moveBy(0, deltaY * SQUARE_SIDE);
        curr_tetro_.squares.at(i).pos.y += deltaY;
    }

    // Update value.
    bottom_ += deltaY;
    up_ += deltaY;
}

// Calculate position of squares after rotation and check if rotation
// is possilbe.
bool MainWindow::rotation_calculation()
{
    std::vector<Coord> rotation_result;
    std::vector<std::pair<double, double>> coord_convert;
    std::pair<double, double> rotation_center = std::make_pair(0, 0);

    // Convert coordinates to real number.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(curr_tetro_.squares.at(i).pos);

        coord_convert.push_back(std::make_pair(c.x + 0.5, c.y + 0.5));

        rotation_center.first += coord_convert.at(i).first;
        rotation_center.second += coord_convert.at(i).second;
    }

    rotation_center.first /= NUM_SQUARE;
    rotation_center.second /= NUM_SQUARE;

    // Rotation and convert coordinate back to integer.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c;
        c.x = ceil(rotation_center.first +
            (coord_convert.at(i).second - rotation_center.second)) - 1;

        c.y = ceil(rotation_center.second -
            (coord_convert.at(i).first - rotation_center.first)) - 1;

        rotation_result.push_back(c);
    }

    bool can_rotate = true;

    // Possible update for automatic move after rotation for fit it
    // the grid.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(rotation_result.at(i));

        // There is wall on the left.
        if (c.x < 0)
        {
            can_rotate = false;
            break;
        }

        // There is wall on the right.
        if (c.x >= COLUMNS)
        {
            can_rotate = false;
            break;
        }

        // There is wall above.
        if (c.y < 0)
        {
            can_rotate = false;
            break;
        }

        // There is wall below.
        if (c.y >= ROWS)
        {
            can_rotate = false;
            break;
        }

        // There there is other tetromino.
        if (grid_.at(c.y).at(c.x).block!= NULL)
        {
            can_rotate = false;
            break;
        }
    }

    // Update position of square in tetromino.
    if (can_rotate)
    {
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            curr_tetro_.squares.at(i).pos.x = rotation_result.at(i).x;
            curr_tetro_.squares.at(i).pos.y = rotation_result.at(i).y;
        }
    }

    return can_rotate;
}

// Update value related to tetromino after rotation.
void MainWindow::rotate_counterclockwise()
{
    // Rotate 90 degree counter-clockwise.
    if (!rotation_calculation())
    {
        return;
    }

    // Update position information of tetromino.
    int new_bottom = -1;
    int new_up = ROWS;
    int new_left = COLUMNS;
    int new_right = -1;

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        if (curr_tetro_.squares.at(i).pos.y < new_up)
        {
            new_up = curr_tetro_.squares.at(i).pos.y;
        }

        if (curr_tetro_.squares.at(i).pos.y > new_bottom)
        {
            new_bottom = curr_tetro_.squares.at(i).pos.y;
        }

        if (curr_tetro_.squares.at(i).pos.x < new_left)
        {
            new_left = curr_tetro_.squares.at(i).pos.x;
        }

        if (curr_tetro_.squares.at(i).pos.x > new_right)
        {
            new_right = curr_tetro_.squares.at(i).pos.x;
        }


        curr_tetro_.squares.at(i).block->setPos(curr_tetro_.squares.at(i).pos.x * SQUARE_SIDE,
                                                curr_tetro_.squares.at(i).pos.y * SQUARE_SIDE);
    }


    // Assigning new values.
    bottom_ = new_bottom;
    up_ = new_up;
    right_ = new_right;
    left_ = new_left;
}

// Calculate position of the squares after reflection and check if
// reflection is possible.
bool MainWindow::reflect_calculation()
{
    std::vector<Coord> reflection_result;
    std::vector<double> coord_convert;
    double rotation_center = 0;

    // Convert coordinates to real number.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        coord_convert.push_back(curr_tetro_.squares.at(i).pos.x + 0.5);
    }

    rotation_center = (double(right_ + left_) / 2) + 0.5;

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c;
        c.x = 2 * rotation_center - coord_convert.at(i);
        c.y = curr_tetro_.squares.at(i).pos.y;

        reflection_result.push_back(c);
    }

    bool can_reflect = true;

    // Possible update for automatic move after reflection for fit it
    // the grid.
    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        Coord c(reflection_result.at(i));

        // There is wall on the left.
        if (c.x < 0)
        {
            can_reflect = false;
            break;
        }

        // There is wall on the right.
        if (c.x >= COLUMNS)
        {
            can_reflect = false;
            break;
        }

        // There is wall above.
        if (c.y < 0)
        {
            can_reflect = false;
            break;
        }

        // There is wall below.
        if (c.y >= ROWS)
        {
            can_reflect = false;
            break;
        }

        // There there is other tetromino.
        if (grid_.at(c.y).at(c.x).block!= NULL)
        {
            can_reflect = false;
            break;
        }
    }

    if (can_reflect)
    {
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            curr_tetro_.squares.at(i).pos.x = reflection_result.at(i).x;
        }
    }

    return can_reflect;
}

// Update value related to tetromino after reflection.
void MainWindow::reflect_vertical_axis()
{
    if (!reflect_calculation())
    {
        return;
    }

    // Update position related information of tetromino.
    int new_bottom = -1;
    int new_up = ROWS;
    int new_left = COLUMNS;
    int new_right = -1;

    for (int i = 0; i < NUM_SQUARE; ++i)
    {
        if (curr_tetro_.squares.at(i).pos.y < new_up)
        {
            new_up = curr_tetro_.squares.at(i).pos.y;
        }

        if (curr_tetro_.squares.at(i).pos.y > new_bottom)
        {
            new_bottom = curr_tetro_.squares.at(i).pos.y;
        }

        if (curr_tetro_.squares.at(i).pos.x < new_left)
        {
            new_left = curr_tetro_.squares.at(i).pos.x;
        }

        if (curr_tetro_.squares.at(i).pos.x > new_right)
        {
            new_right = curr_tetro_.squares.at(i).pos.x;
        }


        curr_tetro_.squares.at(i).block->setPos(curr_tetro_.squares.at(i).pos.x * SQUARE_SIDE,
                                                    curr_tetro_.squares.at(i).pos.y * SQUARE_SIDE);
    }

    // Assign new value.
    bottom_ = new_bottom;
    up_ = new_up;
    right_ = new_right;
    left_ = new_left;
}

// Exchange current playing tetromino to hold position and move
// hold tetromino to plaing area.
void MainWindow::exchange_tetromino()
{
    // In one drop can only hold one time.
    if (can_hold_)
    {
        // Exchange two tetromino.
        int temp_type =curr_tetro_.type;
        QColor temp_color = curr_tetro_.color;

        // Remove current playing tetromino.
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            scene_->removeItem(curr_tetro_.squares.at(i).block);
        }

        curr_tetro_.squares = std::vector<Square>(NUM_SQUARE, Square());

        // Move hold tetromino to the playing area.
        if (!is_hold_empty_)
        {
            // Store tetromino information.
            curr_tetro_.type = hold_tetro_.type;
            curr_tetro_.color = hold_tetro_.color;

            for (int i = 0; i < NUM_SQUARE; ++i)
            {
                curr_tetro_.squares.at(i).pos =
                        Coord(COORD_INFO.at(curr_tetro_.type).at(i));

                bottom_ = BOTTOM_INFO.at(curr_tetro_.type);
                left_ = LEFT_INFO.at(curr_tetro_.type);
                right_ = RIGHT_INFO.at(curr_tetro_.type);
                up_ = UP_INFO.at(curr_tetro_.type);
            }

            set_appear_position();

            // Drawing in the playingarea.
            for (int i = 0; i < NUM_SQUARE; ++i)
            {
                Coord c(curr_tetro_.squares.at(i).pos);
                curr_tetro_.squares.at(i).block = scene_->addRect(0, 0, SQUARE_SIDE,
                                                        SQUARE_SIDE, BLACK_PEN,
                                                        QBrush(curr_tetro_.color));

                curr_tetro_.squares.at(i).block->setPos(c.x * SQUARE_SIDE,
                                                        c.y * SQUARE_SIDE);
            }
        }
        else
        {
            continue_game();
        }

        hold_scene_->clear();

        hold_tetro_.type = temp_type;
        hold_tetro_.color = temp_color;

        // Draw tetromino in the hold scene.
        for (int i = 0; i < NUM_SQUARE; ++i)
        {
            hold_tetro_.squares.at(i).pos =
                    Coord(COORD_INFO.at(hold_tetro_.type).at(i));

            // Scale for fit in the scene.
            hold_tetro_.squares.at(i).block = hold_scene_->addRect(0, 0, SQUARE_SIDE / 2,
                                                            SQUARE_SIDE / 2, BLACK_PEN,
                                                            QBrush(hold_tetro_.color));

            Coord c(hold_tetro_.squares.at(i).pos);
            hold_tetro_.squares.at(i).block->setPos(c.x * SQUARE_SIDE / 1.4 + 10,
                                                    c.y * SQUARE_SIDE / 1.4 + 15);
        }

        // Update game control values.
        is_hold_empty_ = false;
        can_hold_ = false;
    }
}


//*****************************************************************************
// Functions related to button on main window.

// Change to automatic playing mode.
void MainWindow::on_automatic_radio_button_toggled(bool checked)
{
    ui->automatic_radio_button->setChecked(checked);
    play_automatic_ = true;

    if (game_started_ && game_running_)
    {
        ui->fall_button->setDisabled(true);

        // Toggled button when currently playing.
        if (!timer_.isActive())
        {
            // If there are no tetromino currently drop.
            if (curr_tetro_.squares.front().block == NULL)
            {
                continue_game();
            }
            else
            {
                timer_.start(playing_speed_);
            }
        }
    }

    ui->game_message_label->setText("Play automatically.");
}

// Change to manual playing mode.
void MainWindow::on_manual_radio_button_toggled(bool checked)
{
    ui->manual_radio_button->setChecked(checked);
    play_automatic_ = false;

    ui->game_message_label->setText("Play maually.");
}

// Start the game.
void MainWindow::on_start_game_push_button_clicked()
{
    initialize_game();

    ui->start_game_push_button->setDisabled(true);
    ui->name_edit_push_button->setDisabled(true);
    ui->player_name_line_edit->setDisabled(true);
    ui->game_message_label->setText("Game started.");

    game_started_ = true;
    game_running_ = true;

    playing_timer_.start(1000);

    if (play_automatic_)
    {
        ui->fall_button->setDisabled(true);
        continue_game();
    }

}

// Button for drop new tetromino when playing maunal mode.
void MainWindow::on_fall_button_clicked()
{
    // Cannot drop multiple tetriminos at the same time.

    if (game_started_)
    {
        ui->fall_button->setDisabled(true);
        continue_game();
    }
    else
    {
        ui->game_message_label->setText("Please start game.");
    }
}


//*****************************************************************************
// Fuctions related to score boards.

// Get information of high score player stored in file
// and display them on the scoreboard.
void MainWindow::get_high_scores()
{
    score_board_.clear();
    int num_rows = 0;

    std::ifstream file;
    file.open(HIGHEST_SCORES_FILE);

    if (file.is_open())
    {
        std::string line = "";
        std::getline(file, line);

        while (line != "")
        {
            std::vector<std::string> parts = split_line(line, ',');

            std::string player_name = "";
            int point = 0;
            int playing_time = 0;

            // If the file is in right format.
            if (parts.size() == 3)
            {
                num_rows += 1;

                player_name = parts.at(0);
                point = stoi(parts.at(1));
                playing_time = stoi(parts.at(2));
            }

            score_board_.push_back(std::make_pair(player_name,
                                std::make_pair(point, playing_time)));

            std::getline(file, line);
        }

        file.close();
    }

    // Adding for enough row.
    for (int lack_row = 0; lack_row < (HIGHEST_SCORES_DISPLAY_NUM - num_rows);
         ++lack_row)
    {
        score_board_.push_back(std::make_pair("",
                                std::make_pair(0, 0)));
    }

    // Sort score in order.
    sort_score_board();

    display_score_board();
}

// Sort score board in decreasing score order. If player has the same score
// then player has less playing time will have higher rank.
void MainWindow::sort_score_board()
{
    unsigned int num_row = score_board_.size();

    for (unsigned int i = 1; i < num_row; ++i)
    {
        for (unsigned int j = 0; j < num_row - i; ++j)
        {
            bool smaller_relation = false;

            // Compare scores.
            if (score_board_.at(j).second.first <
                score_board_.at(j + 1).second.first)
            {
                smaller_relation = true;
            }
            else
            {
                // Have the same scores.
                if (score_board_.at(j).second.first ==
                    score_board_.at(j + 1).second.first)
                {
                    // Compare playing time.
                    if (score_board_.at(j).second.second >
                        score_board_.at(j + 1).second.second)
                    {
                        smaller_relation = true;
                    }
                }
            }

            // Swap two elements.
            if (smaller_relation)
            {
                std::pair<std::string, std::pair<int, int>> temp_pair = score_board_.at(j);
                score_board_.at(j) = score_board_.at(j + 1);
                score_board_.at(j + 1) = temp_pair;
            }
        }
    }
}

// Update score board in real time when current player archive
// score higher than the score on the scoreboard.
void MainWindow::update_score_board()
{
    int rank = HIGHEST_SCORES_DISPLAY_NUM + 1;
    int curr_time = 3600 * hour_ + 60 * minute_ + second_;

    // Check if current player has scores higer than scores display
    // on score board.
    for (int i = HIGHEST_SCORES_DISPLAY_NUM - 1; i >= 0; --i)
    {
        if (playing_points_ > score_board_.at(i).second.first)
        {
            rank = i;
        }
        else
        {
            if (playing_points_ == score_board_.at(i).second.first)
            {
                if (curr_time < score_board_.at(i).second.second)
                {
                    rank = i;
                }
            }
        }
    }

    if (rank == HIGHEST_SCORES_DISPLAY_NUM + 1)
    {
        return;
    }

    // Move score on the score board.
    for (int i = HIGHEST_SCORES_DISPLAY_NUM - 1; i >= rank + 1; --i)
    {
        score_board_.at(i) = score_board_.at(i - 1);
    }

    // Update the score on score board.
    score_board_.at(rank) = std::make_pair(player_name_,
                            std::make_pair(playing_points_, curr_time));

    display_score_board();
}

// Display high score player information on the score board.
void MainWindow::display_score_board()
{
    std::vector<std::pair<QString, std::pair<QString, QString>>> message_display;

    message_display = make_display_information();

    ui->player_name_first_label->setText(message_display.at(0).first);
    ui->score_first_label->setText(message_display.at(0).second.first);
    ui->playing_time_first_label->setText(message_display.at(0).second.second);

    ui->player_name_second_label->setText(message_display.at(1).first);
    ui->score_second_label->setText(message_display.at(1).second.first);
    ui->playing_time_second_label->setText(message_display.at(1).second.second);

    ui->player_name_third_label->setText(message_display.at(2).first);
    ui->score_third_label->setText(message_display.at(2).second.first);
    ui->playing_time_third_label->setText(message_display.at(2).second.second);
}

// Getting information of high score player and make
// suitable format for display on the scoreboard.
std::vector<std::pair<QString, std::pair<QString, QString>>> MainWindow::make_display_information()
{
    std::vector<std::pair<QString, std::pair<QString, QString>>> message_display;
    for (int i = 0; i < HIGHEST_SCORES_DISPLAY_NUM; ++i)
    {
        QString name_display = "";
        QString point_display = "";
        QString time_display = "";

        // Name display message.
        if (score_board_.at(i).first == "")
        {
            name_display = "No name";
        }
        else
        {
            name_display = QString::fromStdString(score_board_.at(i).first);
        }

        // Point display message.
        if (score_board_.at(i).second.first == 0)
        {
            point_display = QString("No point");
        }
        else
        {
            point_display = QString::number(score_board_.at(i).second.first);
        }

        // Playing time display message.
        if (score_board_.at(i).second.second == 0)
        {
            time_display = QString("No time");
        }
        else
        {
            int hour = score_board_.at(i).second.second / 3600;
            int minute = (score_board_.at(i).second.second % 3600) / 60;
            int second = score_board_.at(i).second.second - (3600 * hour + 60 * minute);

            if (hour != 0)
            {
                time_display += QString::number(hour) + " hours ";
            }

            if (minute != 0)
            {
                time_display += QString::number(minute) + " minutes ";
            }

            time_display += QString::number(second) + " seconds";
        }

        message_display.push_back(std::make_pair(name_display,
                                std::make_pair(point_display, time_display)));
    }

    return message_display;
}

// Store high score for later display.
void MainWindow::store_high_scores()
{
    std::ofstream file;
    file.open(HIGHEST_SCORES_FILE);

    if (file.is_open())
    {
        for (int i = 0; i < HIGHEST_SCORES_DISPLAY_NUM; ++i)
        {
            std::string name = score_board_.at(i).first;
            int point = score_board_.at(i).second.first;
            int time_in_second = score_board_.at(i).second.second;

            std::string store_line = name + ',' + std::to_string(point) + ',' +
                    std::to_string(time_in_second) + '\n';

            file << store_line;
        }

        file.close();
    }
}

// Split line to read information from score board information stored.
std::vector<std::string> MainWindow::split_line(const std::string& line,
                                                const char& delimiter)
{
    std::vector<std::string> result;

    unsigned int index = 0;
    unsigned int prev_index = 0;
    while (index < line.size())
    {
        if (line.at(index) == delimiter)
        {
            std::string temp = "";
            if (prev_index == 0 && result.empty())
            {
                // First part.
                temp = line.substr(0, index);
            }
            else
            {
                // Part in the middle.
                temp = line.substr(prev_index + 1, index - prev_index - 1);
            }

            prev_index = index;
            result.push_back(temp);
        }

        index += 1;
    }

    if (prev_index != line.size() - 1)
    {
        result.push_back(line.substr(prev_index + 1));
    }

    return result;
}


//*****************************************************************************
// Function related to playing time.

// Display plaing time in hour minute and second.
void MainWindow::display_playing_time()
{
    second_ += 1;
    minute_ += second_ / 60;

    hour_ += minute_ / 60;
    minute_ %= 60;
    second_ %= 60;

    ui->number_min_lcd->display(minute_);
    ui->number_sec_lcd->display(second_);
    ui->number_hou_lcd->display(hour_);
}
