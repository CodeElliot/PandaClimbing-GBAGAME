#include "bn_core.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_keypad.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_panda.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_numbers.h"
#include "bn_sprite_tiles_items_panda_action.h"
#include "bn_sprite_tiles_items_items_action.h"
#include "bn_regular_bg_items_background.h"
#include "bn_regular_bg_items_menu.h"
#include "bn_regular_bg_items_copyright.h"
#include "bn_regular_bg_items_gameover.h"
#include "bn_regular_bg_items_intro.h"
#include "bn_regular_bg_items_bestscore.h"
// 随机数
#include "bn_seed_random.h"
// 文字
#include "bn_sprite_text_generator.h"
#include "hanamin_sprite_font.h"
// 音效
#include "bn_sound_items.h"
#include "bn_sound.h"
// 存档
#include "bn_sram.h"
// 颜色相关
#include "bn_colors.h"
#include "bn_color_effect.h"
#include "bn_bg_palette_actions.h"
#include "bn_bg_palettes_actions.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_palette_actions.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_bg_palette_color_hbe_ptr.h"
#include "bn_bg_palettes_transparent_color_hbe_ptr.h"
// 音乐
#include "bn_dmg_music_items_back.h"
#include "bn_dmg_music.h"
/**
 * 声明各种参数
 */
uint8_t game_scene = 0;
// 玩家移动标识 0---静止，1---上下移动 2---攻击
uint8_t player_status = 0;
// 虫子移动速度
float bug_val = 0.1;
// 玩家朝向 :0--面朝右，1---面朝左
uint8_t direction = true;
// 当前分数
uint16_t score = 0;
// 历史最高分
uint16_t best_score = 0;
// 倒计时
int16_t clock_time = 71;
// 最终得分
uint16_t final_score;
// 时钟计时flag
uint8_t clock_flag = 0;
// 游戏是否第一次启动
uint8_t life = 0;
// 虫子横坐标数组
int bugs_x_array[8] = {-88, -52, -18, 18, 52, 88, 52, 88};
// 虫子纵坐标数组
int bugs_y_array[8] = {90, 90, 90, 90, 90, 90, 90, 90};
// 虫子翻转标志
bool bugs_flip_array[8] = {false, true, false, true, false, false, true, false};
// 击中标志
uint8_t shoot_flag = 0;
// 熊猫失败
uint8_t panda_fail = 0;
// 道具移动标识
int clock_moving = 0;
// 初始化随机数种子
bn::seed_random random_num;
// 物品宽度与高度
bn::fixed item_width = 16;
bn::fixed item_height = 16;
// 熊猫
bn::sprite_ptr *panda;
// 熊猫动作
bn::sprite_animate_action<3> *panda_idle;
// 虫子
bn::sprite_ptr *bug1;
bn::sprite_ptr *bug2;
bn::sprite_ptr *bug3;
bn::sprite_ptr *bug4;
bn::sprite_ptr *bug5;
bn::sprite_ptr *bug6;
bn::sprite_ptr *bug7;
bn::sprite_ptr *bug8;
// 虫子动作
bn::sprite_animate_action<2> *bug_idle1;
bn::sprite_animate_action<2> *bug_idle2;
bn::sprite_animate_action<2> *bug_idle3;
bn::sprite_animate_action<2> *bug_idle4;
bn::sprite_animate_action<2> *bug_idle5;
bn::sprite_animate_action<2> *bug_idle6;
bn::sprite_animate_action<2> *bug_idle7;
bn::sprite_animate_action<2> *bug_idle8;
// 虫子数组
bn::sprite_ptr *bugs_array[8];
// 虫子掉落状态数组
uint8_t bugs_fall_status_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// 时钟道具
bn::sprite_ptr *clock_item;
// 数字
bn::sprite_ptr *number1;
bn::sprite_ptr *number2;
bn::sprite_ptr *number3;
bn::sprite_ptr *number4;
// 时钟
bn::sprite_ptr *time1;
bn::sprite_ptr *time2;
bn::sprite_ptr *time3;
/**用于存储数据*/
struct
{
    uint16_t history_score;

} sram_data;
/**
 * 声明各种方法
 */
void scene_menu();
void scene_start();
void scene_over();
void handle_button();
void init_bugs();
void bugs_move();
void check_bug_position();
void change_scene(bn::regular_bg_ptr back_scen, int origion_num);
void update_score();
void update_time();
void init_numbers();
void countdown();
void hide_sprites(bool show);
void scene_score();
void scene_intro();
int8_t check_collison(bn::sprite_ptr *p1, bn::sprite_ptr *p2, bn::fixed w, bn::fixed h);
void init_sprites_position();
void check_attack_or_falldown();
void panda_fall_down();
void check_bugs_val();
void check_create_item();
void create_item_clock();
void clock_item_move();
int16_t create_random_item_position();

/**
 * 创建虫子 从左到右x坐标：-88，-52，-18，18，52，88
 */
void init_bugs()
{
    bug1 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(-88, 90));
    bug2 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(-52, 90));
    bug3 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(-18, 90));
    bug4 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(18, 90));
    bug5 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(88, 90));
    bug6 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(88, 90));
    bug7 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(88, 90));
    bug8 = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(88, 90));
    clock_item = new bn::sprite_ptr(bn::sprite_items::items.create_sprite(create_random_item_position(), -90));
    // 初始化虫子数组
    bugs_array[0] = bug1;
    bugs_array[1] = bug2;
    bugs_array[2] = bug3;
    bugs_array[3] = bug4;
    bugs_array[4] = bug5;
    bugs_array[5] = bug6;
    bugs_array[6] = bug7;
    bugs_array[7] = bug8;
    // 初始化虫子图片
    bug2->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(2));
    bug3->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(4));
    bug4->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(6));
    bug5->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(0));
    bug6->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(2));
    bug7->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(4));
    bug8->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(6));
    clock_item->set_tiles(bn::sprite_items::items.tiles_item().create_tiles(8));
    // 根据位置设置图片翻转
    for (int i = 0; i < 8; i++)
    {
        bugs_array[i]->set_horizontal_flip(bugs_flip_array[i]);
        bugs_array[i]->set_x(bugs_x_array[i]);
        bugs_array[i]->set_y(bugs_y_array[i]);
    }
    // 初始化动作
    bug_idle1 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug1, 8, bn::sprite_tiles_items::items_action, 0, 1));
    bug_idle2 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug2, 8, bn::sprite_tiles_items::items_action, 2, 3));
    bug_idle3 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug3, 8, bn::sprite_tiles_items::items_action, 4, 5));
    bug_idle4 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug4, 8, bn::sprite_tiles_items::items_action, 6, 7));
    bug_idle5 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug5, 8, bn::sprite_tiles_items::items_action, 0, 1));
    bug_idle6 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug6, 8, bn::sprite_tiles_items::items_action, 2, 3));
    bug_idle7 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug7, 8, bn::sprite_tiles_items::items_action, 4, 5));
    bug_idle8 = new bn::sprite_animate_action<2>(bn::create_sprite_animate_action_forever(*bug8, 8, bn::sprite_tiles_items::items_action, 6, 7));
}
/**
 * 初始化精灵位置
 */
void init_sprites_position()
{

    bug1->set_position(bugs_x_array[0], bugs_y_array[0]);
    bug2->set_position(bugs_x_array[1], bugs_y_array[1]);
    bug3->set_position(bugs_x_array[2], bugs_y_array[2]);
    bug4->set_position(bugs_x_array[3], bugs_y_array[3]);
    bug5->set_position(bugs_x_array[4], bugs_y_array[4]);
    bug6->set_position(bugs_x_array[5], bugs_y_array[5]);
    bug7->set_position(bugs_x_array[6], bugs_y_array[6]);
    bug8->set_position(bugs_x_array[7], bugs_y_array[7]);
    clock_item->set_position(create_random_item_position(), -90);

    number1->set_position(-66, -70);
    number2->set_position(-74, -70);
    number3->set_position(-82, -70);
    number4->set_position(-90, -70);

    number1->set_scale(1);
    number2->set_scale(1);
    number3->set_scale(1);
    number4->set_scale(1);

    number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number4->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    for (int i = 0; i < 8; i++)
    {
        bugs_array[i]->set_horizontal_flip(bugs_flip_array[i]);
    }

    panda->set_position(-13, -50);
    panda->set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(0));
    direction = true;
}
/**
 * 随机生成虫子位置
 */
void create_random_bug_position()
{
    int16_t y_temp = 80;
    for (int i = 0; i < 8; i++)
    {
        // 随机生成横坐标
        int rand_x = random_num.get_int(0, 100) % 6;
        switch (rand_x)
        {
        case 0:
            bugs_x_array[i] = -88;
            bugs_flip_array[i] = false;
            break;
        case 1:
            bugs_x_array[i] = -52;
            bugs_flip_array[i] = true;
            break;
        case 2:
            bugs_x_array[i] = -18;
            bugs_flip_array[i] = false;
            break;
        case 3:
            bugs_x_array[i] = 18;
            bugs_flip_array[i] = true;

            break;
        case 4:
            bugs_x_array[i] = 52;
            bugs_flip_array[i] = false;
            break;
        case 5:
            bugs_x_array[i] = 88;
            bugs_flip_array[i] = true;
            break;
        default:
            break;
        }
        // 比较是否有相同虫子横坐标，有则设置后者坐标加一个身位
        for (int j = 0; j < i; j++)
        {

            if (bugs_x_array[i] == bugs_x_array[j])
            {
                y_temp += 20;
            }
        }
        bugs_y_array[i] = y_temp;
    }
}
/**
 * 判断是否需要加快移动速度
 */
void check_bugs_val()
{
    switch (score)
    {
    case 0:
        bug_val = 0.1;
        break;
    case 100:
        bug_val = 0.2;
        break;
    case 200:
        bug_val = 0.3;
        break;
    case 400:
        bug_val = 0.4;
        break;
    case 500:
        bug_val = 0.5;
        break;
    case 600:
        bug_val = 0.6;
        break;
    case 700:
        bug_val = 0.7;
        break;
    case 800:
        bug_val = 0.8;
        break;
    case 900:
        bug_val = 0.9;
        break;
    case 1000:
        bug_val = 1;
        break;
    case 1200:
        bug_val = 1.2;
        break;
    case 1400:
        bug_val = 1.4;
        break;
    case 1600:
        bug_val = 1.6;
        break;
    default:
        break;
    }
}
/**
 *是否创建时钟道具
 */
void check_create_item()
{
    if (clock_time < 25 and clock_item->y() == -90)
    {
        create_item_clock();
    }
}
/**
 * 生成道具时钟
 */
void create_item_clock()
{

    int clock_flag = random_num.get_int(0, 100);
    if (clock_flag % 4 == 0)
    {
        clock_item_move();
    }
}
/**
 * 道具时钟移动
 */
void clock_item_move()
{
    if (clock_moving == 0)
    {
        clock_moving = 1;
    }
    clock_item->set_y(clock_item->y() + 0.5);
    if (clock_item->y() > 90)
    {
        clock_item->set_y(-90);
        clock_item->set_x(create_random_item_position());
        clock_moving = 0;
    }
}
/**
 * 随机生成道具位置
 */
int16_t create_random_item_position()
{
    // 随机生成横坐标
    uint8_t rand_x = random_num.get_int(0, 100) % 2;
    int16_t pos_x = 0;
    switch (rand_x)
    {
    case 0:
        pos_x = -35;
        break;
    case 1:
        pos_x = 35;

        break;
    default:
        break;
    }
    return pos_x;
}
/**
 * 创建分数精灵
 */
void init_numbers()
{
    // 分数
    number1 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(-66, -70));
    number2 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(-74, -70));
    number3 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(-82, -70));
    number4 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(-90, -70));
    // 时间
    time1 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(116, -70));
    time2 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(108, -70));
    time3 = new bn::sprite_ptr(bn::sprite_items::numbers.create_sprite(100, -70));
    life = 1;
    number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
    number4->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(0));
}
/**
 * 虫子移动
 */
void bugs_move()
{
    for (int i = 0; i < 8; i++)
    {
        // 未掉落则向上移动,否则掉落
        if (bugs_fall_status_array[i] == 0)
        {
            bugs_array[i]->set_y(bugs_array[i]->y() - bug_val);
        }
        else
        {
            bugs_array[i]->set_y(bugs_array[i]->y() + 2);
        }
    }
}
/**
 * 判断虫子的y坐标是否超出屏幕上方
 */
void check_bug_position()
{
    for (int i = 0; i < 8; i++)
    {
        if (bugs_array[i]->y() < -90 or (bugs_array[i]->y() > 90 and bugs_fall_status_array[i] == 1))
        {
            bugs_fall_status_array[i] = 0;
            int rand_x = random_num.get_int(0, 100) % 6;
            switch (rand_x)
            {
            case 0:
                bugs_array[i]->set_x(-88);
                bugs_array[i]->set_horizontal_flip(false);
                break;
            case 1:
                bugs_array[i]->set_x(-52);
                bugs_array[i]->set_horizontal_flip(true);
                break;
            case 2:
                bugs_array[i]->set_x(-18);
                bugs_array[i]->set_horizontal_flip(false);
                break;
            case 3:
                bugs_array[i]->set_x(18);
                bugs_array[i]->set_horizontal_flip(true);

                break;
            case 4:
                bugs_array[i]->set_x(52);
                bugs_array[i]->set_horizontal_flip(false);
                break;
            case 5:
                bugs_array[i]->set_x(88);
                bugs_array[i]->set_horizontal_flip(true);
                break;
            default:
                break;
            }
            bugs_array[i]->set_y(90);
        }
    }
}
/**
 * 碰撞检测
 */
int8_t check_collison(bn::sprite_ptr *p1, bn::sprite_ptr *p2, bn::fixed w, bn::fixed h)
{
    if (p1->x() - 16 < p2->x() - 8 + w and p1->x() > p2->x() - 8 - w and p1->y() - 16 < p2->y() - 8 + h and p1->y() > p2->y() - 8 - h)
    {

        return 1;
    }
    return 0;
}
/**
 * 玩家移动
 */
void handle_button()
{ // 左移
    if (panda->x() > -83 and bn::keypad::left_pressed())
    {
        bn::sound_items::select3.play(bn::fixed(0.9));
        panda->set_horizontal_flip(!direction);
        direction = !direction;
        // 中间特殊情况
        if (panda->x() == -13 or panda->x() == 57)
        {
            panda->set_x(panda->x() - 22);
            // 判断玩家是否吃到道具
            if (check_collison(panda, clock_item, item_width - 8, item_height - 8) == 1)
            {
                clock_moving = 0;
                clock_time += 25;
                clock_item->set_y(-90);
            }
            panda->set_x(panda->x() - 22);
        }
        else
        {
            panda->set_x(panda->x() - 26);
        }
    }
    // 右移
    if (panda->x() < 83 and bn::keypad::right_pressed())
    {
        bn::sound_items::select3.play(bn::fixed(0.9));
        panda->set_horizontal_flip(!direction);
        direction = !direction;
        // 中间特殊情况
        if (panda->x() == 13 or panda->x() == -57)
        {
            panda->set_x(panda->x() + 22);
            // 判断玩家是否吃到道具
            if (check_collison(panda, clock_item, item_width - 8, item_height - 8) == 1)
            {
                bn::sound_items::eat.play(bn::fixed(0.9));
                clock_moving = 0;
                clock_time += 25;
                clock_item->set_y(-90);
            }
            panda->set_x(panda->x() + 22);
        }
        else
        {
            panda->set_x(panda->x() + 26);
        }
    }
    // 上移
    if (panda->y() > -64 and bn::keypad::up_held())
    {
        player_status = 1;
        panda->set_y(panda->y() - 1);
    }
    // 下移
    if (panda->y() < 64 and bn::keypad::down_held())
    {
        player_status = 1;
        panda->set_y(panda->y() + 1);
    }
    // 攻击
    if (bn::keypad::b_held() and panda->y() < 64)
    {
        player_status = 2;
        panda->set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(1));
        panda->set_y(panda->y() + 4);
    }
    if (bn::keypad::b_pressed())
    {
        bn::sound_items::xiu.play(bn::fixed(0.9));
    }
    if (bn::keypad::up_released() or bn::keypad::down_released() or bn::keypad::b_released())
    {
        player_status = 0;
        panda->set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(0));
    }
}
/**
 * 场景切换缓冲
 */
void change_scene(bn::regular_bg_ptr back_scen, int origion_num)
{
    bn::bg_palette_ptr back_palette = back_scen.palette();

    bn::sprite_palette_ptr panda_palette = panda->palette();
    bn::sprite_palette_ptr bug1_palette = bug1->palette();
    bn::sprite_palette_ptr bug2_palette = bug2->palette();
    bn::sprite_palette_ptr bug3_palette = bug3->palette();
    bn::sprite_palette_ptr bug4_palette = bug4->palette();
    bn::sprite_palette_ptr bug5_palette = bug5->palette();
    bn::sprite_palette_ptr bug6_palette = bug6->palette();
    bn::sprite_palette_ptr bug7_palette = bug7->palette();
    bn::sprite_palette_ptr bug8_palette = bug8->palette();

    bn::sprite_palette_ptr number1_palette = number1->palette();
    bn::sprite_palette_ptr number2_palette = number2->palette();
    bn::sprite_palette_ptr number3_palette = number3->palette();
    bn::sprite_palette_ptr number4_palette = number4->palette();

    bn::sprite_palette_ptr time1_palette = time1->palette();
    bn::sprite_palette_ptr time2_palette = time2->palette();
    bn::sprite_palette_ptr time3_palette = time3->palette();
    //
    panda_palette.set_fade(bn::colors::black, origion_num);
    back_palette.set_fade(bn::colors::black, origion_num);
    bug1_palette.set_fade(bn::colors::black, origion_num);
    bug2_palette.set_fade(bn::colors::black, origion_num);
    bug3_palette.set_fade(bn::colors::black, origion_num);
    bug4_palette.set_fade(bn::colors::black, origion_num);
    bug5_palette.set_fade(bn::colors::black, origion_num);
    bug6_palette.set_fade(bn::colors::black, origion_num);
    bug7_palette.set_fade(bn::colors::black, origion_num);
    bug8_palette.set_fade(bn::colors::black, origion_num);
    number1_palette.set_fade(bn::colors::black, origion_num);
    number2_palette.set_fade(bn::colors::black, origion_num);
    number3_palette.set_fade(bn::colors::black, origion_num);
    number4_palette.set_fade(bn::colors::black, origion_num);
    time1_palette.set_fade(bn::colors::black, origion_num);
    time2_palette.set_fade(bn::colors::black, origion_num);
    time3_palette.set_fade(bn::colors::black, origion_num);
    for (int i = 0; i < 20; i++)
    {
        bn::fixed fade_intensity = back_palette.fade_intensity();
        switch (origion_num)
        {
        case 1:
            back_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            panda_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug1_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug2_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug3_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug4_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug5_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug6_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug7_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            bug8_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            number1_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            number2_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            number3_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            number4_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            time1_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            time2_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            time3_palette.set_fade_intensity(bn::max(fade_intensity - 0.05, bn::fixed(0)));
            break;
        case 0:
            back_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            panda_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug1_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug2_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug3_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug4_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug5_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug6_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug7_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            bug8_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            number1_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            number2_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            number3_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            number4_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            time1_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            time2_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            time3_palette.set_fade_intensity(bn::min(fade_intensity + 0.05, bn::fixed(1)));
            break;
        default:
            break;
        }
        bn::core::update();
    }
}
/**
 * 计时器
 */
void update_time()
{
    if (clock_time < 100)
    {
        int8_t ge = clock_time % 10;
        int8_t shi = clock_time / 10;
        time1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        time2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
    }
    else if (clock_time < 1000)
    {
        int8_t ge = clock_time % 10;
        int8_t shi = clock_time % 100 / 10;
        int8_t bai = clock_time / 100;
        time1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        time2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
        time3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(bai));
    }
    else
    {
        time1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        time2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        time3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
    }
}
/**
 * 计分器
 */
void update_score()
{
    if (score < 100)
    {
        int8_t ge = score % 10;
        int8_t shi = score / 10;
        number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
    }
    else if (score < 1000)
    {
        int8_t ge = score % 10;
        int8_t shi = score % 100 / 10;
        int8_t bai = score / 100;
        number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
        number3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(bai));
    }
    else if (score < 10000)
    {
        int8_t ge = score % 10;
        int8_t shi = score % 100 / 10;
        int8_t bai = score % 1000 / 100;
        int8_t qian = score / 1000;
        number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
        number3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(bai));
        number4->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(qian));
    }
    else
    {
        number1->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        number2->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        number3->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        number4->set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
    }
}
/**
 * 更新倒计时
 */
void countdown()
{
    clock_flag = clock_flag % 60;
    if (clock_flag == 0)
    {
        clock_time -= 1;
    }
    clock_flag += 1;
}
/**
 * 隐藏/显示部分精灵
 */
void hide_or_show_sprites(bool show)
{
    clock_time = 70;
    panda->set_visible(show);
    time1->set_visible(show);
    time2->set_visible(show);
    time3->set_visible(show);
    number1->set_visible(show);
    number2->set_visible(show);
    number3->set_visible(show);
    number4->set_visible(show);

    clock_item->set_visible(show);
    bug1->set_visible(show);
    bug2->set_visible(show);
    bug3->set_visible(show);
    bug4->set_visible(show);
    bug5->set_visible(show);
    bug6->set_visible(show);
    bug7->set_visible(show);
    bug8->set_visible(show);
}
/**
 * 攻击碰撞检测
 */
void check_attack_or_falldown()
{
    for (int i = 0; i < 8; i++)
    {
        // 发生碰撞
        if (check_collison(panda, bugs_array[i], item_width, item_height))
        {
            // 1.发动了攻击

            if (player_status == 2 and bugs_fall_status_array[i] == 0 and panda->y() < 64)
            {
                bugs_fall_status_array[i] = 1;
                score += 5;
                best_score += 5;
            }
            // 2.未发动攻击
            else if (player_status != 2 and bugs_fall_status_array[i] == 0)
            {
                // 游戏结束,熊猫掉落
                panda_fall_down();
            }
        }
    }
    update_score();
}
/**
 * 游戏结束,熊猫掉落
 */
void panda_fall_down()
{
    if (panda_fail == 0)
    {
        // 熊猫移动到空隙
        switch (panda->x().integer())
        {
        case -83:
            panda->set_x(-96);
            break;
        case -57:
            panda->set_x(-35);
            break;
        case -13:
            panda->set_x(-35);
            break;
        case 13:
            panda->set_x(35);
            break;
        case 57:
            panda->set_x(35);
            break;
        case 83:
            panda->set_x(96);
            break;
        default:
            break;
        }

        panda_fail = 1;
    }
    panda->set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(3));
    panda->set_y(panda->y() + 2);
    if (panda->y() > 64)
    {
        panda_fail = 0;
        game_scene = 3;
    }
}
/**
 * 游戏菜单
 */
void scene_menu()
{
    bn::regular_bg_ptr background = bn::regular_bg_items::menu.create_bg(8, 48);
    change_scene(background, 1);
    bn::sprite_ptr icon = bn::sprite_items::panda.create_sprite(-55, 8);
    icon.set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(4));
    if (life == 0)
    {
        bn::sram::read(sram_data);
        // 读取存档
        if (sram_data.history_score > 0 and sram_data.history_score < 10000)
        {
            best_score = sram_data.history_score;
        }
        else
        {
            best_score = 0;
            sram_data.history_score = 0;
        }
    }
    while (game_scene == 1)
    {
        if (bn::keypad::up_pressed() and icon.y() > 8)
        {
            bn::sound_items::select3.play(bn::fixed(0.9));

            icon.set_y(icon.y() - 20);
        }
        if (bn::keypad::down_pressed() and icon.y() < 48)
        {
            bn::sound_items::select3.play(bn::fixed(0.9));

            icon.set_y(icon.y() + 20);
        }
        // 选项
        if (bn::keypad::start_pressed() or bn::keypad::a_pressed())
        {
            switch ((icon.y()).integer())
            {
                // 开始游戏
            case 8:
                game_scene = 2;
                icon.set_visible(false);
                // 初始化随机数
                random_num.set_seed(*(int *)(0x04000108));
                break;
                // 最佳得分
            case 28:
                game_scene = 4;
                icon.set_visible(false);
                break;
                // 玩法说明
            case 48:
                game_scene = 5;
                icon.set_visible(false);
                break;
            default:
                break;
            }
        }
        bn::core::update();
    }
    change_scene(background, 0);
}

/**
 * 游戏开始
 */
void scene_start()
{
    score = 0;
    panda_fail = 0;
    player_status = 0;
    clock_moving = 0;

    bugs_y_array[0] = 80;
    bugs_y_array[1] = 80;
    bugs_y_array[2] = 80;
    bugs_y_array[3] = 80;
    bugs_y_array[4] = 80;
    bugs_y_array[5] = 80;
    bugs_y_array[6] = 80;
    bugs_y_array[7] = 80;

    bn::regular_bg_ptr background = bn::regular_bg_items::background.create_bg(8, 48);
    change_scene(background, 1);
    // 创建熊猫 :从右到左横坐标【83，57，13，-13，-57，-83】
    // 随机生成虫子位置
    create_random_bug_position();
    // 判断是否需要初始化
    if (life == 0)
    {
        init_bugs();
        panda = new bn::sprite_ptr(bn::sprite_items::panda.create_sprite(-13, -50));
        panda_idle = new bn::sprite_animate_action<3>(bn::create_sprite_animate_action_forever(*panda, 3, bn::sprite_tiles_items::panda_action, 0, 1, 2));
        init_numbers();
        life = 1;
    }
    else
    {
        // 初始化精灵位置
        init_sprites_position();
    }

    panda->set_horizontal_flip(direction);
    // 显示精灵
    hide_or_show_sprites(true);
    // 显示文字
    bn::sprite_text_generator text_generator(hanamin_sprite_font);
    bn::vector<bn::sprite_ptr, 128> text_sprites;
    text_generator.generate(-120, -68, "得分", text_sprites);
    // 创建时钟图标
    bn::sprite_ptr clock_icon = bn::sprite_items::items.create_sprite(90, -70);
    clock_icon.set_tiles(bn::sprite_items::items.tiles_item().create_tiles(9));
    // 播放音乐
    bn::dmg_music_master_volume master_volume = bn::dmg_music_master_volume::HALF;
    bn::dmg_music::set_master_volume(master_volume);
    bn::dmg_music_items::back.play();

    while (game_scene == 2)
    {

        // 监听按键输入
        handle_button();
        // 虫子移动
        bugs_move();
        if (panda_fail == 1)
        {
            panda_fall_down();
        }
        // 创建道具
        check_create_item();
        // 道具移动
        if (clock_moving == 1)
        {
            clock_item_move();
        }
        // 改变虫子速度
        check_bugs_val();
        // 检查虫子的y坐标
        check_bug_position();
        // 更新虫子动作
        bug_idle1->update();
        bug_idle2->update();
        bug_idle3->update();
        bug_idle4->update();
        bug_idle5->update();
        bug_idle6->update();
        bug_idle7->update();
        bug_idle8->update();
        // 非静止则更新动作
        if (player_status != 0)
        {
            panda_idle->update();
        }
        // 更新时间
        update_time();
        // 更新分数
        check_attack_or_falldown();
        // 更新倒计时
        countdown();
        // 时间到
        if (clock_time < 0)
        {
            game_scene = 3;
            clock_time = 71;
            player_status = 0;
        }
        // 更新核心组件
        bn::core::update();
    }
    bn::dmg_music::pause();

    change_scene(background, 0);
    // 隐藏精灵
    hide_or_show_sprites(false);
}
/**
 * 游戏结束
 */
void scene_over()
{
    bn::regular_bg_ptr background = bn::regular_bg_items::gameover.create_bg(8, 48);
    // 设置位置
    number1->set_position(25, 10);
    number2->set_position(9, 10);
    number3->set_position(-7, 10);
    number4->set_position(-23, 10);

    number1->set_scale(2);
    number2->set_scale(2);
    number3->set_scale(2);
    number4->set_scale(2);
    // 显示得分
    number1->set_visible(true);
    number2->set_visible(true);
    number3->set_visible(true);
    number4->set_visible(true);
    change_scene(background, 1);

    bn::sprite_ptr icon = bn::sprite_items::panda.create_sprite(-13, 48);
    icon.set_horizontal_flip(true);
    icon.set_tiles(bn::sprite_items::panda.tiles_item().create_tiles(0));
    // 判断是否需要写入最高成绩
    // 写入存档
    if (sram_data.history_score < score)
    {
        sram_data.history_score = score;
        bn::sram::write(sram_data);
    }
    best_score = sram_data.history_score;
    while (game_scene == 3)
    {
        if (bn::keypad::left_pressed())
        {
            bn::sound_items::select3.play(bn::fixed(0.9));

            icon.set_x(-13);
            icon.set_horizontal_flip(true);
        }
        else if (bn::keypad::right_pressed())
        {
            bn::sound_items::select3.play(bn::fixed(0.9));
            icon.set_x(13);
            icon.set_horizontal_flip(false);
        }
        if (bn::keypad::a_pressed() or bn::keypad::start_pressed())
        {
            switch (icon.x().integer())
            {
            case -13:
                game_scene = 2;
                break;
            case 13:
                game_scene = 1;
                break;
            default:
                break;
            }
        }
        bn::core::update();
    }
    number1->set_visible(false);
    number2->set_visible(false);
    number3->set_visible(false);
    number4->set_visible(false);
    change_scene(background, 0);
}
/**
 * 最佳成绩
 */
void scene_score()
{
    bn::regular_bg_ptr background = bn::regular_bg_items::bestscore.create_bg(8, 48);
    change_scene(background, 1);
    // 设置数字
    bn::sprite_ptr num1 = bn::sprite_items::numbers.create_sprite(0, 0);
    bn::sprite_ptr num2 = bn::sprite_items::numbers.create_sprite(0, 0);
    bn::sprite_ptr num3 = bn::sprite_items::numbers.create_sprite(0, 0);
    bn::sprite_ptr num4 = bn::sprite_items::numbers.create_sprite(0, 0);

    // 设置位置
    num1.set_position(25, 10);
    num2.set_position(9, 10);
    num3.set_position(-7, 10);
    num4.set_position(-23, 10);

    num1.set_scale(2);
    num2.set_scale(2);
    num3.set_scale(2);
    num4.set_scale(2);

    if (best_score < 100)
    {
        int8_t ge = best_score % 10;
        int8_t shi = best_score / 10;
        num1.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        num2.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
    }
    else if (best_score < 1000)
    {
        int8_t ge = best_score % 10;
        int8_t shi = best_score % 100 / 10;
        int8_t bai = best_score / 100;
        num1.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        num2.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
        num3.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(bai));
    }
    else if (best_score < 10000)
    {
        int8_t ge = best_score % 10;
        int8_t shi = best_score % 100 / 10;
        int8_t bai = best_score % 1000 / 100;
        int8_t qian = best_score / 1000;
        num1.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(ge));
        num2.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(shi));
        num3.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(bai));
        num4.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(qian));
    }
    else
    {
        num1.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        num2.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        num3.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
        num4.set_tiles(bn::sprite_items::numbers.tiles_item().create_tiles(9));
    }

    while (game_scene == 4)
    {
        if (bn::keypad::a_pressed() or bn::keypad::b_pressed() or bn::keypad::start_pressed())
        {
            game_scene = 1;
        }
        bn::core::update();
    }
    num1.set_visible(false);
    num2.set_visible(false);
    num3.set_visible(false);
    num4.set_visible(false);
    change_scene(background, 0);
}
/**
 * 游戏说明
 */
void scene_intro()
{
    bn::regular_bg_ptr background = bn::regular_bg_items::intro.create_bg(8, 48);
    change_scene(background, 1);
    while (game_scene == 5)
    {
        if (bn::keypad::a_pressed() or bn::keypad::b_pressed() or bn::keypad::start_pressed())
        {
            game_scene = 1;
        }
        bn::core::update();
    }
    change_scene(background, 0);
}
/**
 * 游戏copyright
 */
void scene_copyright()
{
    bn::regular_bg_ptr background = bn::regular_bg_items::copyright.create_bg(8, 48);
    change_scene(background, 1);
    for (int i = 0; i < 30; i++)
    {
        bn::core::update();
    }
    game_scene = 1;
    change_scene(background, 0);
}
int main()
{
    bn::core::init();
    while (true)
    {
        switch (game_scene)
        {
        case 0:
            scene_copyright();
            break;
        case 1:
            scene_menu();
            break;
        case 2:
            scene_start();
            break;
        case 3:
            scene_over();
            break;
        case 4:
            scene_score();
            break;
        case 5:
            scene_intro();
            break;
        default:
            break;
        }
        bn::core::update();
    }
}
