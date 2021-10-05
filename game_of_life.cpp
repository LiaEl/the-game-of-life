#define MIN_SIZE_OF_BOARD 3
#define MAX_SIZE_OF_BOARD 30
#define TIME_TO_SLEEP 300

#include <algorithm>
#include <chrono>
#include <iomanip> 
#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <thread>
#include <vector>

class TheGame
{
    int size_of_board;
    std::set<std::pair<int, int>> live_cells; // array of current live cells
    std::vector<std::vector<int>> current_around_count; // map of current cell's score
    
    public:
    int get_board_size();
    bool is_live_cells();
    void display_board();
    void revive_new_cell();
    void update_board();
    void revive_glyder();
    bool next_step();

    TheGame(int size)
    : size_of_board(size), current_around_count(size, std::vector<int>(size, 0))
    {
        std::cout << ">>> The Game starts ... <<<" << std::endl;
    }
};

/** 
 * When a cell revives or dies, this function adds or subtracts the score of its neighbors
 *
 * @param around_count Map of cells with score.
 * @param x, y Coordinates of the cell  
 * @param update Could be 1 or -1 to. This parameter shows how we need to change the score of the neighbors of the cell.
 */ 
void update_around(std::vector<std::vector<int>> & around_count, int x, int y, int update)
{
    #ifdef DEBUG
    std::cout << "Updating (" << x << ", " << y << ") to " << update << std::endl;
    #endif

    /* Check which neighbors the cell has*/
    bool is_under = (unsigned int)x < (around_count.size() - 1);
    bool is_above = x > 0;
    bool is_left = y > 0;
    bool is_right = (unsigned int)y < (around_count.size() - 1);
    
    /* Update all the neighbors*/
    if (is_under) around_count[x + 1][y] += update;
    if (is_above) around_count[x - 1][y] += update;
    if (is_left) around_count[x][y - 1] += update;
    if (is_right) around_count[x][y + 1] += update;
    if (is_under && is_left) around_count[x + 1][y - 1] += update;
    if (is_under && is_right) around_count[x + 1][y + 1] += update;
    if (is_above && is_left) around_count[x - 1][y - 1] += update;
    if (is_above && is_right) around_count[x - 1][y + 1] += update;
}

/** 
 * Get board size
 * 
 * @return Size of the board
*/ 
int TheGame::get_board_size()
{
    return this->size_of_board;
}

/**
 * Check if at least one cell is alive
 * 
 * @return false if there is no alive cells
 */
bool TheGame::is_live_cells()
{
    return !live_cells.empty();
}

/**
 * Display the current state of the game
 */
void TheGame::display_board()
{
    for (int i = 0; i < this->size_of_board; i++)
    {
        for (int j = 0; j < this->size_of_board; j++)
        {
            if(this->live_cells.find({i, j}) != this-> live_cells.end())
            {
                std::cout << "* ";
            }
            else
            {
                std::cout << "- ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
 * Put all revived cells on the board by increasing their's neighbor's score
 */
void TheGame::update_board()
{
    for (const auto& elem: this->live_cells)
    {
        update_around(this->current_around_count, elem.first, elem.second, 1);
    }
}

/**
 * Revive built-in pattern (glyder)
 */
void TheGame::revive_glyder()
{
    this->live_cells.clear();
    this->live_cells.insert({2, 0});
    this->live_cells.insert({2, 1});
    this->live_cells.insert({2, 2});
    this->live_cells.insert({0, 1});
    this->live_cells.insert({1, 2});
}

/**
 * Ask user to revive new cell
 */
void TheGame::revive_new_cell()
{
    int x, y;
    while (true)
    {
        std::cout << "Please, enter coordinates, X and Y, for a live cell: ";
        std::cin >> x >> y;
        while(std::cin.fail()) 
        {
            std::cout << "Something goes wrong, try to enter coodrinates in format \"x y\"" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cin >> x >> y;
        }   
        if((x >= this->size_of_board) || (y >= this->size_of_board))
        {
            std::cout << "Please, enter integer numbers more than 0 and less than " << this->size_of_board << std::endl;
            continue;
        }
        this->live_cells.insert({x, y});
        break;
    }
}

/**
 * Calculate next step of the game
 * 
 * @return False if there were no changes on the board in this step
 */
bool TheGame::next_step()
{
    bool something_changed = false;
    auto next_around_count = this->current_around_count; 
    auto next_live_cells = this->live_cells;

    for (int i = 0; i < this->size_of_board; i++)
    {
        for (int j = 0; j < this->size_of_board; j++)
        {
            auto temp = std::make_pair(i, j);
            // If the cell is alive, but the number of its neighbors is too small or large for life
            // it dies
            if ((this->live_cells.find(temp) != this-> live_cells.end()) && ((this->current_around_count[i][j] <= 1) || (this->current_around_count[i][j] >= 4)))
            {
                #ifdef DEBUG
                std::cout << "Around cell (" << i << ", " << j << ") is " << current_around_count[i][j] << " other cells" << std::endl;
                #endif

                next_live_cells.erase(temp);
                update_around(next_around_count, i, j, -1);
                something_changed = true;
            }

            // If the cell is not alive, but the number of its neighbors meets the condition of revitalization
            // it revives
            if ((this->live_cells.find(temp) == this-> live_cells.end()) && ((this->current_around_count[i][j] == 3)))
            {
                #ifdef DEBUG
                std::cout << "Around cell (" << i << ", " << j << ") is " << current_around_count[i][j] << " other cells" << std::endl;
                #endif

                next_live_cells.insert(temp);
                update_around(next_around_count, i, j, 1);
                something_changed = true;
            }
        }
    }
    this->live_cells = next_live_cells;
    this->current_around_count = next_around_count;

    #ifdef DEBUG
    std::cout << "--Live cells--" << std::endl;
    for(const auto& elem: this->live_cells)
    {
        std::cout << "(" << elem.first << ", " << elem.second << ") " << std::endl;
    }
    std::cout << "--------------" << std::endl;
    #endif

    return something_changed;
}

void say_hello()
{
    std::cout << std::setfill('*') << std::setw(46) << std::left << "*" <<std::endl;
    std::cout << std::setfill(' ') << std::setw(45) << std::left << "* Hi there! Welcome to Game of Life!" << std::setw(47) << "*\n* To start The Game, revive a few cells and " << std::setw(47) << "*\n* see what happens" << "*\n";
    std::cout << std::setfill('*') << std::setw(46) << std::left << "*" <<std::endl;
}

int main()
{
    say_hello();

    int size_of_board;
    while (true)
    {
        std::cout << "Please, enter the size of the side of the square board: ";
        std::cin >> size_of_board;
        if(std::cin.fail()) 
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Not integer input. Try again." << std::endl;
            continue;
        }   

        if (size_of_board > MAX_SIZE_OF_BOARD)
        {
            std::cout << "Your number is too large for the screen, we recommend using numbers no more than " << MAX_SIZE_OF_BOARD << std::endl;
            continue;
        }
        break;
    }

    TheGame new_game(size_of_board);
    
    while (true)
    {
        std::string ans;
        std::cout << ">> Choose: \n>> 1 - to revive one more cell \n>> 2 - to start the game\n>> 3 - to run prepared glyder, previously revived cells will be killed\n: ";
        std::cin >> ans;
        if(ans.size() != 1)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Wrong input:" << ans << ". Try again." << std::endl;
            continue;
        }
        if (ans[0] == '1')
        {
            new_game.revive_new_cell();
            continue;
        }
        if (ans[0] == '2')
        {
            if(!new_game.is_live_cells())
            {
                std::cout << "You have not revived a single cell, the game will not be interesting." << std::endl;
                continue;
            }
            new_game.update_board();
            break;   
        }
        if (ans[0] == '3')
        {
            if(new_game.get_board_size() < MIN_SIZE_OF_BOARD)
            {
                std::cout << "Sorry, game board is too small to run the glyder, please, choose other options." << std::endl;
                continue;
            }
            new_game.revive_glyder();
            new_game.update_board();
            break;   
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Wrong input:" << ans << ". Try again." << std::endl;
            continue;
        }
    }

    while (true)
    {
        new_game.display_board();
        std::cout << ">> Choose: \n>> 1 - to play game automatically \n>> 2 - to see one next step\n: ";
        std::string way;
        std::cin >> way;
        if(way.size() != 1)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Wrong input:" << way << ". Try again." << std::endl;
            continue;
        }

        // if after the step nothing has changed then it indicates end of the game
        bool something_changed = false;
        if (way[0] == '1')
        {
            do
            {
                new_game.display_board();
                something_changed = new_game.next_step();

                // time for user to see the step
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));

            } while(something_changed);
            std::cout << ">> END OF THE GAME << " << std::endl;
            break;
        }
        if (way[0] == '2')
        {
            if(!new_game.next_step())
            {
                std::cout << ">> END OF THE GAME << " << std::endl;
                break;
            }
            continue;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Wrong input:" << way << ". Try again." << std::endl;
            continue;
        }
    }
    return 0;
}