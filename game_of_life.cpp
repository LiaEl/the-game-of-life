#include <iostream>
#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>
#include <iomanip> 
#include <limits>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

class TheGame
{
    int size_of_board;
    std::vector<std::pair<int, int>> live_cells;
    
    std::vector<std::vector<char>> board;
    std::vector<std::vector<int>> current_around_count;
    
    void increase_around(std::vector<std::vector<int>> & around_count, int x, int y);
    void decrease_around(std::vector<std::vector<int>> & around_count, int x, int y);
    
    public:
    int get_board_size();
    void display_board();
    void revive_cell();
    void assign_live_cells();
    void revive_glyder();
    bool next_step();

    TheGame(int size)
    : size_of_board(size), board(size, std::vector<char>(size, '-')), current_around_count(size, std::vector<int>(size, 0))
    {
        std::cout << "The Game starts ... " << std::endl;
    }
};

int TheGame::get_board_size()
{
    return this->size_of_board;
}
void TheGame::display_board()
{
    for (int i = 0; i < this->size_of_board; i++)
    {
        for (int j = 0; j < this->size_of_board; j++)
        {
            std::cout << this->board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void TheGame::assign_live_cells()
{
    for (int i = 0; i < this->live_cells.size(); i++)
    {
        this->board[this->live_cells[i].first][this->live_cells[i].second] = '*';
        this->increase_around(this->current_around_count, this->live_cells[i].first, this->live_cells[i].second);
    }
}

void TheGame::revive_glyder()
{
    this->live_cells.clear();
    this->live_cells.push_back(std::make_pair(2, 0));
    this->live_cells.push_back(std::make_pair(2, 1));
    this->live_cells.push_back(std::make_pair(2, 2));
    this->live_cells.push_back(std::make_pair(0, 1));
    this->live_cells.push_back(std::make_pair(1, 2));
}

void TheGame::revive_cell()
{
    int x, y;
    std::stringstream ss;
    std::string input;
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
        this->live_cells.push_back(std::make_pair(x, y));
        break;
    }
}

void TheGame::increase_around(std::vector<std::vector<int>> & around_count, int x, int y)
{
    // DEBUG PRINTS
    // std::cout << "Increasing (" << x << ", " << y << ")" << std::endl;
    bool is_under = x < (this->size_of_board - 1);
    bool is_above = x > 0;
    bool is_left = y > 0;
    bool is_right = y < (this->size_of_board - 1);
    
    if (is_under) around_count[x + 1][y] += 1;
    if (is_above) around_count[x - 1][y] += 1;
    if (is_left) around_count[x][y - 1] += 1;
    if (is_right) around_count[x][y + 1] += 1;
    if (is_under && is_left) around_count[x + 1][y - 1] += 1;
    if (is_under && is_right) around_count[x + 1][y + 1] += 1;
    if (is_above && is_left) around_count[x - 1][y - 1] += 1;
    if (is_above && is_right) around_count[x - 1][y + 1] += 1;
}

void TheGame::decrease_around(std::vector<std::vector<int>> & around_count, int x, int y)
{
    // DEBUG PRINTS
    // std::cout << "Decreasing (" << x << ", " << y << ")" << std::endl;
    bool is_under = x < (this->size_of_board - 1);
    bool is_above = x > 0;
    bool is_left = y > 0;
    bool is_right = y < (this->size_of_board - 1);
    
    if (is_under) around_count[x + 1][y] -= 1;
    if (is_above) around_count[x - 1][y] -= 1;
    if (is_left) around_count[x][y - 1] -= 1;
    if (is_right) around_count[x][y + 1] -= 1;
    if (is_under && is_left) around_count[x + 1][y - 1] -= 1;
    if (is_under && is_right) around_count[x + 1][y + 1] -= 1;
    if (is_above && is_left) around_count[x - 1][y - 1] -= 1;
    if (is_above && is_right) around_count[x - 1][y + 1] -= 1;

}

bool TheGame::next_step()
{
    bool something_changed = false;
    std::vector<std::vector<char>> new_board = this->board;
    std::vector<std::vector<int>> new_around_count = this->current_around_count;
    std::vector<std::pair<int, int>> new_live_cells = this->live_cells;

    for (int i = 0; i < this->size_of_board; i++)
    {
        for (int j = 0; j < this->size_of_board; j++)
        {
            std::pair<int, int> temp = std::make_pair(i, j);
            if (((this->current_around_count[i][j] <= 1) || (this->current_around_count[i][j] >= 4)) && std::count(this->live_cells.begin(),this-> live_cells.end(), temp))
            {
                // DEBUG PRINTS
                // std::cout << "Around cell (" << i << ", " << j << ") is " << around_count[i][j] << " other cells" << std::endl;
                new_board[i][j] = '-';
                new_live_cells.erase(remove(new_live_cells.begin(), new_live_cells.end(), temp));
                decrease_around(new_around_count, i, j);
                something_changed = true;
            }

            if (((this->current_around_count[i][j] == 3) && !(std::count(this->live_cells.begin(), this->live_cells.end(), temp))))
            {
                // DEBUG PRINTS
                // std::cout << "Around cell (" << i << ", " << j << ") is " << around_count[i][j] << " other cells" << std::endl;
                new_board[i][j] = '*';
                new_live_cells.push_back(temp);
                increase_around(new_around_count, i, j);
                something_changed = true;
            }
        }
    }
    this->board = new_board;
    this->live_cells = new_live_cells;
    this->current_around_count = new_around_count;
    // ------- UNCOMMENT TO DEBUG ------
    // std::cout << "--Live cells--" << std::endl;
    // for(int i = 0; i < live_cells.size(); i++)
    // {
    //     std::cout << "(" << live_cells[i].first << ", " << live_cells[i].second << ") " << std::endl;
    // }
    // std::cout << "--------------" << std::endl;

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
    int size_of_board;
    say_hello();
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

        if (size_of_board > 30)
        {
            std::cout << "Your number is too large for the screen, we recommend using numbers no more than 30" << std::endl;
            continue;
        }
        break;
    }

    TheGame new_game(size_of_board);
    
    while (true)
    {
        std::string ans;
        std::cout << ">> Choose: \n>> 1 - to revive one more cell \n>> 2 - to start the game\n>> 3 - to run prepared glyder\n: ";
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
            new_game.revive_cell();
            continue;
        }
        if (ans[0] == '2')
        {
            new_game.assign_live_cells();
            break;   
        }
        if (ans[0] == '3')
        {
            if(new_game.get_board_size() < 3)
            {
                std::cout << "Sorry, game board is too small to run the glyder, please, choose other options." << std::endl;
                continue;
            }
            std::cout << "If you revived any cells, they will be killed to run glyder." << std::endl;
            new_game.revive_glyder();
            new_game.assign_live_cells();
            break;   
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Undefined input:" << ans << ". Try again." << std::endl;
            continue;
        }
    }

    while (true)
    {
        new_game.display_board();
        std::cout << ">> Choose: \n>> 1 - to play game automatically \n>> 2 - to see one next step\n: ";
        char way;
        std::cin >> way;
        bool something_changed = false;
        if (way == '2')
        {
            something_changed = new_game.next_step();
            if(!something_changed)
            {
                std::cout << ">> END OF THE GAME << " << std::endl;
                break;
            }
            continue;
        }
        if (way == '1')
        {
            do
            {
                new_game.display_board();
                something_changed = new_game.next_step();
                
                #ifdef _WIN32
                Sleep(1000);
                #else
                sleep(1);
                #endif
            } while(something_changed);
            std::cout << ">> END OF THE GAME << " << std::endl;
            break;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Undefined symbol:" << way << ". Try again." << std::endl;
            continue;
        }
    }
    return 0;
}