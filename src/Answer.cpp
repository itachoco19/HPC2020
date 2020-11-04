//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  (C)HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Answer.hpp"
#include <vector>
hpc::Vector2 target;
double* distance_map;

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
    double min = 900.0;
    double add_point;
    for(int i = -1; i < 2; ++i)
    {
        if(x + i < 0 || x + i > 49) continue;
        for(int j = -1; j < 2; ++j)
        {
            if(y + j < 0 || y + j > 49) continue;
            min = map[x + i + (y + j) * Parameter::StageWidth] < min ? map[(x + i) + (y + j) * Parameter::StageWidth] : min;
        }
    }
    enum hpc::Terrain now_ground = aStage.terrain(Vector2(static_cast<float>(x), static_cast<float>(y)));
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
    
    map[x + Parameter::StageWidth * y] = min + add_point;
}

void calc_distance(const Stage& aStage, double* map, int start_point)
{
    int x = start_point % Parameter::StageWidth;
    int y = start_point / Parameter::StageWidth;
    for(int i = 1; ; ++i)
    {
        if(x + i > 49 && x - i < 0 && y + i < 0 && i + y > 49) break;
        if(x - i >= 0)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                around_minimam(x - i, j, map, aStage);
            }
        }
        if(x + i < 50)
        {
            for(int j = y - i; j < y + i; ++j)
            {
                around_minimam(x + i, j, map, aStage);
            }
        }
        if(y - i >= 0)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                around_minimam(j, y - i, map, aStage);
            }
        }
        if(y + i < 50)
        {
            for(int j = x - i; j < x + i; ++j)
            {
                around_minimam(j, y + i, map, aStage);
            }
        }
    }
}

Vector2 setTarget(const Stage& stage)
{
    Vector2 result;
    double min;
    int start_pos = stage.rabbit().pos().x + stage.rabbit().pos().y * Parameter::StageWidth;
    distance_map = new double(Parameter::StageHeight * Parameter::StageWidth);
    for(int i = 0; i < Parameter::StageHeight * Parameter::StageWidth;++i)distance_map[i] = 1000.0; 
    std::vector<hpc::Scroll> scrolls;
    std::vector<double> scrolls_distance;
    for(auto itr = stage.scrolls().begin(); itr != stage.scrolls().end(); ++itr)
    {
        if(!(itr->isGotten()))
        {
            scrolls.push_back(*itr);
        }
    }
    distance_map[start_pos] = 0.0;
    calc_distance(stage, distance_map, start_pos);
    result = scrolls.begin()->pos();
    min = distance_map[static_cast<int>(scrolls.begin()->pos().x) + static_cast<int>(scrolls.begin()->pos().y) * Parameter::StageWidth];
    for(auto itr = scrolls.begin() + 1; itr != scrolls.end(); ++itr)
    {
        double tmp = distance_map[static_cast<int>(itr->pos().x) + static_cast<int>(itr->pos().y) * Parameter::StageWidth];
        if(tmp < min)
        {
            result = itr->pos();
            min = tmp;
        }
    }
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
    if(static_cast<int>(pos.x) == static_cast<int>(target.x) && static_cast<int>(pos.y) == static_cast<int>(target.y))
    {
        target = setTarget(aStage);
    }

    return pos;
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
