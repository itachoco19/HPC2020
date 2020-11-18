//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  (C)HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------
 
#include "Answer.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <random>
#include <cmath>
 
#define INF 1000000000000.0
 
hpc::Vector2 target;
double* distance_map = nullptr;
std::vector<int> target_list;
int list_index = 0;
std::vector<std::vector<int>> all_map;
std::vector<std::vector<int>> start_to_scroll;

void map_initialize(double* map, int mapsize)
{
    for(int i = 0; i < mapsize; ++i) map[i] = INF;
}
 
//------------------------------------------------------------------------------
namespace hpc {
 
//------------------------------------------------------------------------------
/// コンストラクタ
/// @detail 最初のステージ開始前に実行したい処理があればここに書きます
Answer::Answer()
{
}
 
//------------------------------------------------------------------------------
/// デストラクタ
/// @detail 最後のステージ終了後に実行したい処理があればここに書きます
Answer::~Answer()
{
}


void around_minimam(int x, int y, double* map, const Stage& aStage)
{
    double min = INF;
    double add_point;
    for(int i = -1; i <= 1; ++i)
    {
        if(x + i < 0 || x + i > 49) continue;
        for(int j = -1; j <= 1; ++j)
        {
            double correction = std::sqrt(1.25);
            if(y + j < 0 || y + j > 49) continue;
            if(j == 0 || i == 0) correction = 1.0;
            min = map[x + i + (y + j) * Parameter::StageWidth] * correction < min ? map[(x + i) + (y + j) * Parameter::StageWidth] * correction : min;
        }
    }
    hpc::Terrain now_ground = aStage.terrain(Vector2(static_cast<float>(x), static_cast<float>(y)));
    if(now_ground == hpc::Terrain::Plain)
    {
        add_point = 1.0;
    }
    else if(now_ground == hpc::Terrain::Bush)
    {
        add_point = 1.6;
    }
    else if(now_ground == hpc::Terrain::Sand)
    {
        add_point = 3.0;
    }
    else
    {
        add_point = 10.0;
    }
     
    map[x + Parameter::StageWidth * y] = (min + add_point);
}

void counter_clockwise(const Stage& aStage, double* map, int start_x, int start_y)
{
    int x = start_x;
    int y = start_y;
    int i = 0;
    for(i = 0; ; ++i)
    {
        if(x + i > 49 && x - i < 0 && y - i < 0 && i + y > 49) break;
        if(x - i >= 0)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(x - i, j, map, aStage);
            }
        }
        if(y + i < 50)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(j, y + i, map, aStage);
            }
        }
        if(x + i < 50)
        {
            for(int j = y + i - 1; j >= y - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(x + i, j, map, aStage);
            }
        }
        if(y - i >= 0)
        {
            for(int j = x + i - 1; j >= x - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(j, y - i, map, aStage);
            }
        }
    }
    for(; i >= 0 ; --i)
    {
        if(y - i >= 0)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(j, y - i, map, aStage);
            }
        }
        if(x + i < 50)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(x + i, j, map, aStage);
            }
        }
        if(y + i < 50)
        {
            for(int j = x + i - 1; j >= x - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(j, y + i, map, aStage);
            }
        }
        if(x - i >= 0)
        {
            for(int j = y + i - 1; j >= y - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(x - i, j, map, aStage);
            }
        }
    }
}

void clockwise(const Stage& aStage, double* map, const int start_x, const int start_y)
{
    int x = start_x;
    int y = start_y;
    int i = 0;
    for(i = 0; ; ++i)
    {
        if(x + i > 49 && x - i < 0 && y - i < 0 && i + y > 49) break;
        
        if(y - i >= 0)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(j, y - i, map, aStage);
            }
        }
        if(x + i < 50)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(x + i, j, map, aStage);
            }
        }
        if(y + i < 50)
        {
            for(int j = x + i - 1; j >= x - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(j, y + i, map, aStage);
            }
        }
        if(x - i >= 0)
        {
            for(int j = y + i - 1; j >= y - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(x - i, j, map, aStage);
            }
        }
    }
    
    for(; i >= 0 ; --i)
    {
        if(x - i >= 0)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(x - i, j, map, aStage);
            }
        }
        if(y + i < 50)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                if(j < 0) j = 0;
                else if(j > 49) break;
                around_minimam(j, y + i, map, aStage);
            }
        }
        if(x + i < 50)
        {
            for(int j = y + i - 1; j >= y - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(x + i, j, map, aStage);
            }
        }
        if(y - i >= 0)
        {
            for(int j = x + i - 1; j >= x - i; --j)
            {
                if(j > 49) j = 49;
                else if(j < 0) break;
                around_minimam(j, y - i, map, aStage);
            }
        }
    }
}

std::vector<int> aster_func(int start_x, int start_y, int target_x, int target_y, double* f_map, double* n_map, const Stage& aStage)
{
    double add_point;
    bool undone_flag = true;
    std::deque<std::pair<int, double>> open_list;
    std::vector<int> result;
    bool open_flag[Parameter::StageHeight * Parameter::StageWidth];
    int parent_list[Parameter::StageWidth * Parameter::StageHeight];
    double astarmap[Parameter::StageWidth * Parameter::StageHeight];
    for(int i = 0; i < Parameter::StageWidth * Parameter::StageHeight; ++i)
    {
        open_flag[i] = true;
        parent_list[i] = -1;
        astarmap[i] = 9.0 * INF;
    }
    int start_point = start_x + start_y * Parameter::StageWidth;
    int goal_point = target_x + target_y * Parameter::StageWidth;
    int now_point = start_point;
    astarmap[now_point] = f_map[now_point] + n_map[now_point];
    open_list.push_back(std::pair<int, double>(now_point, f_map[now_point] + n_map[now_point]));

    while(undone_flag)
    {
        if(open_list.empty())
        {
            break;
        }
        now_point = open_list.front().first;
        open_list.pop_front();
        if(now_point == goal_point)
        {
            break;
        }
        if(!open_flag[now_point])continue;

        
        hpc::Terrain now_ground = aStage.terrain(Vector2(static_cast<float>(now_point % Parameter::StageWidth), static_cast<float>(now_point / Parameter::StageWidth)));
        if(now_ground == hpc::Terrain::Plain)
        {
            add_point = 1.0;
        }
        else if(now_ground == hpc::Terrain::Bush)
        {
            add_point = 1.6;
        }
        else if(now_ground == hpc::Terrain::Sand)
        {
            add_point = 3.0;
        }
        else
        {
            add_point = 10.0;
        }
            
        std::vector<std::pair<int, double>> list;
        list.clear();
        for(int i = -1; i <= 1; ++i)
        {
            int x = i + now_point % Parameter::StageWidth; 
            if(x < 0 || x > 49) continue;
            for(int j = -1; j <= 1; ++j)
            {
                double cost = (i == 0) || (j == 0) ? 1.0 : 1.4;
                int y = j + now_point / Parameter::StageWidth;
                if(y < 0 || y > 49) continue;
                if((i == 0 && j == 0)) continue;
                if(astarmap[now_point] - n_map[now_point] + add_point * cost + n_map[x + y * Parameter::StageWidth] < astarmap[x + y * Parameter::StageWidth])
                {
                    list.push_back(std::pair<int, double>(x + y * Parameter::StageWidth, astarmap[now_point] - n_map[now_point] + add_point * cost + n_map[x + y * Parameter::StageWidth]));
                    astarmap[x + y * Parameter::StageWidth] = astarmap[now_point] - n_map[now_point] + add_point * cost + n_map[x + y * Parameter::StageWidth];
                    open_flag[x + y * Parameter::StageWidth] = true;
                    parent_list[x + (y) * Parameter::StageWidth] = now_point;
                }
            }
        }
        open_flag[now_point] = false;
        
        for(auto itr : list)
        {
            open_list.push_back(itr);
        }
        std::sort(open_list.begin(), open_list.end(), [](std::pair<int, double>a, std::pair<int, double>b){return a.second < b.second;});
    }
    now_point = goal_point;
    while(now_point != start_point)
    {
        result.push_back(now_point);
        now_point = parent_list[now_point];
    }
    return result;
}

void calc_distance(const Stage& aStage, double* map, int start_x, int start_y)
{
    int x = start_x;
    int y = start_y;
    int maplength = Parameter::StageWidth * Parameter::StageHeight;
    double* clockwise_map = new double[Parameter::StageHeight * Parameter::StageWidth];
    double* counter_clockwise_map = new double[Parameter::StageWidth * Parameter::StageHeight];
    map_initialize(clockwise_map, Parameter::StageHeight * Parameter::StageWidth);
    map_initialize(counter_clockwise_map, Parameter::StageWidth * Parameter::StageHeight);
    clockwise_map[x + y * Parameter::StageWidth] = 0.0;
    counter_clockwise_map[x + y * Parameter::StageWidth] = 0.0;
    for(int i = 0; i < 2; ++i)
    {
        counter_clockwise(aStage, counter_clockwise_map, x, y);
        clockwise(aStage, clockwise_map, x, y);
    }
    for(int i = 0; i < maplength; ++i)
    {
        if(clockwise_map[i] < counter_clockwise_map[i])
        {
            map[i] = clockwise_map[i];
        }
        else
        {
            map[i] = counter_clockwise_map[i];
        }
        
    }
    delete[] clockwise_map;
    delete[] counter_clockwise_map;
}

void create_distance_map(const Stage& aStage, int target_x, int target_y, int start_x, int start_y, double* result_map)
{
    int stage_length = Parameter::StageWidth * Parameter::StageHeight;
    std::vector<int> tmp;
    double* map1 = new double[stage_length];
    double* map2 = new double[stage_length];
    map_initialize(map1, stage_length);
    map_initialize(map2, stage_length);
    calc_distance(aStage, map1, target_x, target_y);
    calc_distance(aStage, map2, static_cast<int>(aStage.rabbit().pos().x), static_cast<int>(aStage.rabbit().pos().y));

    map_initialize(result_map, Parameter::StageHeight * Parameter::StageWidth);
    tmp = aster_func(start_x, start_y, target_x, target_y, map2, map1, aStage);
    for(int i = 0; i < tmp.size(); ++i)
    {
        result_map[tmp[i]] = static_cast<double>(i);
    }
    
    delete[] map1;
    delete[] map2;
}
 
Vector2 setTarget(const Stage& aStage)
{
    Vector2 result;
    double min;
    std::vector<hpc::Vector2> scrolls;
    int start_pos = static_cast<int>(aStage.rabbit().pos().x) + static_cast<int>(aStage.rabbit().pos().y) * Parameter::StageWidth;
    if(distance_map != nullptr) delete[] distance_map;
    distance_map = new double[Parameter::StageHeight * Parameter::StageWidth];
    for(int i = 0; i < Parameter::StageHeight * Parameter::StageWidth;++i)distance_map[i] = INF;
    for(auto itr = aStage.scrolls().begin(); itr != aStage.scrolls().end(); ++itr)
    {
        if(!(itr->isGotten()))
        {
            Vector2 tmp = Vector2(itr->pos().x, itr->pos().y);
            scrolls.push_back(tmp);
        }
    }
    distance_map[start_pos] = 5.0;
    calc_distance(aStage, distance_map, start_pos % Parameter::StageWidth, start_pos / Parameter::StageWidth);
    result = Vector2(scrolls.begin()->x, scrolls.begin()->y);
    min = distance_map[static_cast<int>(scrolls.begin()->x) + static_cast<int>(scrolls.begin()->y) * Parameter::StageWidth];
    for(auto itr = scrolls.begin(); itr != scrolls.end(); ++itr)
    {
        double tmp = distance_map[static_cast<int>(itr->x) + static_cast<int>(itr->y) * Parameter::StageWidth];
        if(tmp < min)
        {
            result = *itr;
            min = tmp;
        }
    }
    
    create_distance_map(aStage, static_cast<int>(result.x), static_cast<int>(result.y), static_cast<int>(aStage.rabbit().pos().x), static_cast<int>(aStage.rabbit().pos().y), distance_map);
    return result;
}
 
bool is_reachble(int x, int y, float jump_length, Vector2 now_pos)
{
    bool result = false;
    if(std::pow(static_cast<double>(x) - now_pos.x, 2.0) + std::pow(static_cast<double>(y) - now_pos.y, 2.0) <= std::pow(jump_length, 2.0))
    {
        result = true;
    }
    else if(std::pow(static_cast<double>(x + 1) - now_pos.x, 2.0) + std::pow(static_cast<double>(y) - now_pos.y, 2.0) <= std::pow(jump_length, 2.0))
    {
        result = true;
    }
    else if(std::pow(static_cast<double>(x) - now_pos.x, 2.0) + std::pow(static_cast<double>(y + 1) - now_pos.y, 2.0) <= std::pow(jump_length, 2.0))
    {
        result = true;
    }
    else if(std::pow(static_cast<double>(x + 1) - now_pos.x, 2.0) + std::pow(static_cast<double>(y + 1) - now_pos.y, 2.0) <= std::pow(jump_length, 2.0))
    {
        result = true;
    }
    return result;
}

std::vector<int> yakinamashi(const Stage& aStage, int points_count, double* aDistmap, std::vector<double> aStartCost)
{
    std::vector<int> point;
    std::vector<int> result;
    if(points_count == 1) 
    {
        result.push_back(0);
        return result;
    }
    
    double turn = 0.0;
    double temp = 0.0;
    const double heat = 10000.0;
    double before = 0.0;
    int before_state = 0;
    char flag[points_count];
    for(int i = 0; i < points_count; ++i) flag[i] = 1;
    Random rnd = Random(RandomSeed::DefaultSeed());
    point.clear();
    for(int i = 0; i < points_count; ++i)
    {
        if(i == 0)
        {
            double min = INF;
            int lindex = 0;
            for(int j = 0; j < points_count; ++j)
            {
                if(min > aStartCost[j])
                {
                    min = aStartCost[j];
                    lindex = j;
                }
            }
            point.push_back(lindex);
            turn = aStartCost[lindex];
            flag[lindex] = 0;
            before_state = lindex;
        }
        else
        {
            double min = INF;
            int minIndex = 0;
            for(int j = 0; j < points_count; ++j)
            {
                if(min >= aDistmap[before_state * Parameter::MaxScrollCount + j] && flag[j] == 1)
                {
                    min = aDistmap[before_state * Parameter::MaxScrollCount + j];
                    minIndex = j;
                }
            }
            flag[minIndex] = 0;
            turn += min;
            point.push_back(minIndex);
            before_state = minIndex;
        }
    }
    result = point;
    before = turn;
    for(temp = heat; temp >= 0.001; temp *= 0.9999)
    {
        turn = 0.0;
        double diff = heat / 1000.0;
        for(double T = temp; T >= 0.0; T -= diff)
        {
            int lh = rnd.randMinMax(0, points_count - 1);
            int rh = rnd.randMinMax(0, points_count - 1);
            int tmp = 0;
            tmp = point.at(lh);
            point[lh] = point.at(rh);
            point[rh] = tmp;
        }
        for(int i = 0; i < points_count; ++i)
        {
            if(i == 0)
            {
                turn = aStartCost.at(point.at(0));
            }
            else
            {
                turn += aDistmap[point.at(i - 1) * Parameter::MaxScrollCount + point.at(i)];
            }
            
        }
        if(before > turn)
        {
            before = turn;
            result = point;
            continue;
        }
        if(std::exp((turn - before) - temp) < rnd.randFloat())
        {
            before = turn;
            result = point;
            continue;
        }
        point = result;
    }
    
    return result;
}

std::vector<int> create_list(const Stage& aStage)
{
    double* map = new double[Parameter::StageHeight * Parameter::StageWidth];
    double* lDistanceMap = new double[(Parameter::MaxScrollCount) * Parameter::MaxScrollCount];
    std::vector<double> start_cost;
    std::vector<int> result;
    int scrolls_count = aStage.scrolls().count();
    for(int i = 0; i < Parameter::MaxScrollCount * Parameter::MaxScrollCount; ++i) lDistanceMap[i] = INF;
    for(int i = 0; i < aStage.scrolls().count(); ++i)
    {
        start_cost.push_back(start_to_scroll[i].size());
    } 
    for(int i = 0; i < aStage.scrolls().count(); ++i)
    {
        for(int j = 0; j < aStage.scrolls().count(); ++j)
        {
            if(i == j) continue;
            map_initialize(map, Parameter::StageHeight * Parameter::StageWidth);
            for(int k = 0; k < all_map[j + i * aStage.scrolls().count()].size(); ++k)
            {
                map[all_map[j + i * aStage.scrolls().count()][k]] = k;
            }
            lDistanceMap[i * Parameter::MaxScrollCount + j] = all_map[j + i * aStage.scrolls().count()].size();
        }
    }

    result = yakinamashi(aStage, scrolls_count, lDistanceMap, start_cost);

    delete[] map;
    delete[] lDistanceMap;
    return result;
}

void setNextTarget(const Stage& aStage)
{
    //if(distance_map == nullptr) distance_map = new double[Parameter::StageHeight * Parameter::StageWidth];
    map_initialize(distance_map, Parameter::StageWidth * Parameter::StageHeight);
    if(list_index == 0)
    {
        target = aStage.scrolls()[target_list[0]].pos();
        for(int i = 0; i < start_to_scroll[target_list[0]].size(); ++i)
        {
            distance_map[start_to_scroll[target_list[0]][i]] = i;
        }
    }
    else
    {
    
        target = aStage.scrolls()[target_list[list_index]].pos();
        for(int i = 0; i < all_map[target_list[list_index] + target_list[list_index - 1] * aStage.scrolls().count()].size(); ++i)
        {
            distance_map[all_map[target_list[list_index] + target_list[list_index - 1] * aStage.scrolls().count()][i]] = i;
        }
    }
    ++list_index;
}

Vector2 next_jump_point(const Stage& aStage)
{
    Vector2 now_point = aStage.rabbit().pos();
    hpc::Terrain ground = aStage.terrain(aStage.rabbit().pos());
    float ground_effect;
    if(ground == Terrain::Plain)
    {
        ground_effect = 1.0;
    }
    else if(ground == Terrain::Bush)
    {
        ground_effect = 0.6;
    }
    else if(ground == Terrain::Sand)
    {
        ground_effect = 0.15;
    }
    else
    {
        ground_effect = 0.05;
    }
    float power = aStage.rabbit().power() * ground_effect;
    double min;
    Vector2 result;
    std::vector<std::pair<double, Vector2>> list;
    int min_x = static_cast<int>(now_point.x - power);
    int max_x = static_cast<int>(now_point.x + power);
    int min_y = static_cast<int>(now_point.y - power);
    int max_y = static_cast<int>(now_point.y + power);
    if(min_x < 0) min_x = 0;
    if(max_x > 49) max_x = 49;
    if(min_y < 0) min_y = 0;
    if(max_y > 49) max_y = 49;
    int now_x = static_cast<int>(now_point.x);
    int now_y = static_cast<int>(now_point.y);
    for(int i = -1; i < 2; ++i)
    {
        if(now_x + i > 49 || now_x + i < 0) continue;
        for(int j = -1; j < 2; ++j)
        {
            if(static_cast<int>(now_point.y) + j > 49 || static_cast<int>(now_point.y) + j < 0) continue;
            if(i == 0 && j == 0) continue;
            list.push_back(std::pair<double, Vector2>(distance_map[now_x + i + (now_y+j) * Parameter::StageWidth], Vector2(now_x + i, now_y + j)));
        }
    }
    for(int i = min_x; i <= max_x; ++i)
    {
        for(int j = min_y; j <= max_y; ++j)
        {
            if(now_x == i && now_y == j) continue;
            if(is_reachble(i, j, power, now_point))
            {
                list.push_back(std::pair<double, Vector2>(distance_map[i + j * Parameter::StageWidth], Vector2(i, j)));
            }
        }
    }
    min = list.at(0).first;
    result = list.at(0).second;
    for(auto itr = list.begin(); itr != list.end(); ++itr)
    {
        if(min > itr->first)
        {
            min = itr->first;
            result = itr->second;
        }
    }
    result = Vector2(result.x + 0.5, result.y + 0.5);
    return result;
}

void set_map(const Stage& aStage, int map_count)
{
    double tmp_map[Parameter::StageWidth * Parameter::StageHeight];
    double tmp_map2[Parameter::StageHeight * Parameter::StageWidth];
    
    for(int i = 0; i < aStage.scrolls().count(); ++i)
    {
        Vector2 start_pos = aStage.rabbit().pos();
        Vector2 end_pos = aStage.scrolls()[i].pos();
        calc_distance(aStage, tmp_map, static_cast<int>(end_pos.x), static_cast<int>(end_pos.y));
        calc_distance(aStage, tmp_map2, static_cast<int>(start_pos.x), static_cast<int>(start_pos.y));
        start_to_scroll.push_back(aster_func(static_cast<int>(start_pos.x), static_cast<int>(start_pos.y), static_cast<int>(end_pos.x), static_cast<int>(end_pos.y), tmp_map2, tmp_map, aStage));
    }
    for(int i = 0; i < map_count; ++i)
    {
        for(int j = 0; j < map_count; ++j)
        {
            if(i == j)
            {
                all_map.push_back(std::vector<int>(0));
                continue;
            }
            Vector2 start_pos = aStage.scrolls()[i].pos();
            Vector2 end_pos = aStage.scrolls()[j].pos();
            calc_distance(aStage, tmp_map, static_cast<int>(end_pos.x), static_cast<int>(end_pos.y));
            calc_distance(aStage, tmp_map2, static_cast<int>(start_pos.x), static_cast<int>(start_pos.y));
            all_map.push_back(aster_func(static_cast<int>(start_pos.x), static_cast<int>(start_pos.y), static_cast<int>(end_pos.x), static_cast<int>(end_pos.y), tmp_map2, tmp_map, aStage));
        }
    }
}
 
//------------------------------------------------------------------------------
/// 各ステージ開始時に呼び出される処理
/// @detail 各ステージに対する初期化処理が必要ならここに書きます
/// @param aStage 現在のステージ
void Answer::initialize(const Stage& aStage)
{
    list_index = 0;
    all_map.clear();
    start_to_scroll.clear();
    distance_map = new double[Parameter::StageWidth * Parameter::StageHeight];
    map_initialize(distance_map, Parameter::StageHeight * Parameter::StageWidth);
    set_map(aStage, aStage.scrolls().count());
    target_list = create_list(aStage);
    setNextTarget(aStage);
    
    //target = setTarget(aStage);
}
 
//------------------------------------------------------------------------------
/// 毎フレーム呼び出される処理
/// @detail 移動先を決定して返します
/// @param aStage 現在のステージ
/// @return 移動の目標座標
Vector2 Answer::getTargetPos(const Stage& aStage)
{
    auto pos = aStage.rabbit().pos();
    Vector2 result;
    if(static_cast<int>(pos.x) == static_cast<int>(target.x) && static_cast<int>(pos.y) == static_cast<int>(target.y))
    {
        setNextTarget(aStage);
        //target = setTarget(aStage);
    }
    result = next_jump_point(aStage);
    return result;
}
 
//------------------------------------------------------------------------------
/// 各ステージ終了時に呼び出される処理
/// @detail 各ステージに対する終了処理が必要ならここに書きます
/// @param aStage 現在のステージ
void Answer::finalize(const Stage& aStage)
{
    delete[] distance_map;
    distance_map = nullptr;
}
 
} // namespace
// EOF