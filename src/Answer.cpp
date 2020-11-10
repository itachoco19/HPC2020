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
#include <cmath>
 
#define INF 10000000.0
 
hpc::Vector2 target;
double* distance_map = nullptr;
 
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

void map_initialize(double* map, int mapsize)
{
    for(int i = 0; i < mapsize; ++i) map[i] = INF;
}

void around_minimam(int x, int y, double* map, const Stage& aStage)
{
    double min = INF;
    double add_point;
    for(int i = -1; i < 2; ++i)
    {
        if(x + i < 0 || x + i > 49) continue;
        for(int j = -1; j < 2; ++j)
        {
            double correction = 1.5;
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
        add_point = 1.5;
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
    for(int i = 0; ; ++i)
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
}

void clockwise(const Stage& aStage, double* map, const int start_x, const int start_y)
{
    int x = start_x;
    int y = start_y;
    for(int i = 0; ; ++i)
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
}

void calc_distance(const Stage& aStage, double* map, int start_point)
{
    int x = start_point % Parameter::StageWidth;
    int y = start_point / Parameter::StageWidth;
    int maplength = Parameter::StageWidth * Parameter::StageHeight;
    double* clockwise_map = new double[Parameter::StageHeight * Parameter::StageWidth];
    double* counter_clockwise_map = new double[Parameter::StageWidth * Parameter::StageHeight];
    map_initialize(clockwise_map, Parameter::StageHeight * Parameter::StageWidth);
    map_initialize(counter_clockwise_map, Parameter::StageWidth * Parameter::StageHeight);
    clockwise_map[x + y * Parameter::StageWidth] = 0.0;
    counter_clockwise_map[x + y * Parameter::StageWidth] = 0.0;
    counter_clockwise(aStage, counter_clockwise_map, x, y);
    clockwise(aStage, clockwise_map, x, y);
    int start = static_cast<int>(aStage.rabbit().pos().x) + static_cast<int>(aStage.rabbit().pos().y) * Parameter::StageWidth;
    if(clockwise_map[start] < counter_clockwise_map[start])
    {
        for(int i = 0; i < maplength; ++i)
        {
            map[i] = clockwise_map[i];
        }
    }
    else
    {
        for(int i = 0; i < maplength; ++i)
        {
            map[i] = counter_clockwise_map[i];
        }
    }
    delete[] clockwise_map;
    delete[] counter_clockwise_map;
}

void create_distance_map(const Stage& aStage, int target_x, int target_y)
{
    int stage_length = Parameter::StageWidth * Parameter::StageHeight;
    double* map1 = new double[stage_length];
    map_initialize(map1, stage_length);
    map1[target_x + target_y * Parameter::StageWidth] = 0.0;
    calc_distance(aStage, map1, target.x + target_y * Parameter::StageWidth);
    for(int i = 0; i < stage_length; ++i)
    {
        distance_map[i] = map1[i];
    }
    delete[] map1;
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
    distance_map[start_pos] = 0.0;
    calc_distance(aStage, distance_map, start_pos);
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
    for(int i = 0; i < Parameter::StageWidth * Parameter::StageHeight; ++i)
    {
        distance_map[i] = INF;
    }
    distance_map[static_cast<int>(result.x) + static_cast<int>(result.y) * Parameter::StageWidth] = 0.0;
    calc_distance(aStage, distance_map, static_cast<int>(result.x) + static_cast<int>(result.y) * Parameter::StageWidth);
    //create_distance_map(aStage, static_cast<int>(result.x), static_cast<int>(result.y));
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
 
Vector2 next_jump_point(const Stage& aStage)
{
    Vector2 now_point = aStage.rabbit().pos();
    float power = aStage.rabbit().power();
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
    for(int i = min_x; i <= max_x; ++i)
    {
        for(int j = min_y; j <= max_y; ++j)
        {
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
 
//------------------------------------------------------------------------------
/// 各ステージ開始時に呼び出される処理
/// @detail 各ステージに対する初期化処理が必要ならここに書きます
/// @param aStage 現在のステージ
void Answer::initialize(const Stage& aStage)
{
    target = setTarget(aStage);
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
        target = setTarget(aStage);
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
}
 
} // namespace
// EOF