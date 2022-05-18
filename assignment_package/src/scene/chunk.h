#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include <drawable.h>
#include "noise.h"
#include "misc.h"
#include "npc.h"
#include "vboworker.h"
#include "model.h"
//using namespace std;
#include "river.h"

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.

// match BlockType to specific collection of UV coords
const static std::unordered_map<BlockType, std::unordered_map<Direction, glm::vec4, EnumHash>, EnumHash> blockFaceUVs {
    {GRASS, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(3.f/16.f, 15.f/16.f, 0, 0)},
                                                               {XNEG, glm::vec4(3.f/16.f, 15.f/16.f, 0, 0)},
                                                               {YPOS, glm::vec4(8.f/16.f, 13.f/16.f, 0, 0)},
                                                               {YNEG, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                               {ZPOS, glm::vec4(3.f/16.f, 15.f/16.f, 0, 0)},
                                                               {ZNEG, glm::vec4(3.f/16.f, 15.f/16.f, 0, 0)}}},

    {DIRT, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(2.f/16.f, 15.f/16.f, 0, 0)}}},

    {STONE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)},
                                                               {XNEG, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)},
                                                               {YPOS, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)},
                                                               {YNEG, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)},
                                                               {ZPOS, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)},
                                                               {ZNEG, glm::vec4(1.f/16.f, 15.f/16.f, 0, 0)}}},

    {LAVA, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 1.f/16.f, 0, 0)}}},

    {WATER, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 3.f/16.f, 0, 0)}}},

    {SNOW, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)}}},

    {BEDROCK, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(1.f/16.f, 14.f/16.f, 0, 0)}}},

    {SAND, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(2.f/16.f, 14.f/16.f, 0, 0)}}},

    {REDSAND, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(10.f/16.f, 14.f/16.f, 0, 0)}}},

    {WOOD, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(4.f/16.f, 14.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(4.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(5.f/16.f, 14.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(4.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(4.f/16.f, 14.f/16.f, 0, 0)}}},

    {LEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)}}},

    {ICE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(3.f/16.f, 11.f/16.f, 0, 0)}}},

    {MUSHROOM, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 8.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 8.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(13.f/16.f, 8.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(14.f/16.f, 9.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 8.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 8.f/16.f, 0, 0)}}},

    {STEM, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 7.f/16.f, 0, 0)}}},

    {BROWNSHROOM, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(14.f/16.f, 8.f/16.f, 0, 0)},
                                                                 {XNEG, glm::vec4(14.f/16.f, 8.f/16.f, 0, 0)},
                                                                 {YPOS, glm::vec4(14.f/16.f, 8.f/16.f, 0, 0)},
                                                                 {YNEG, glm::vec4(14.f/16.f, 9.f/16.f, 0, 0)},
                                                                 {ZPOS, glm::vec4(14.f/16.f, 8.f/16.f, 0, 0)},
                                                                 {ZNEG, glm::vec4(14.f/16.f, 8.f/16.f, 0, 0)}}},

    {SNOWLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(5.f/16.f, 12.f/16.f, 0, 0)}}},

    {WINDOW, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)},
                                                                  {XNEG, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)},
                                                                  {YPOS, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)},
                                                                  {YNEG, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)},
                                                                  {ZPOS, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)},
                                                                  {ZNEG, glm::vec4(10.f/16.f, 7.f/16.f, 0, 0)}}},

    {BLUE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(12.f/16.f, 4.f/16.f, 0, 0)}}},

    {PURPLE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)},
                                                                {XNEG, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YPOS, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YNEG, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZPOS, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZNEG, glm::vec4(11.f/16.f, 2.f/16.f, 0, 0)}}},

    {WHITE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)},
                                                                {XNEG, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YPOS, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YNEG, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZPOS, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZNEG, glm::vec4(10.f/16.f, 2.f/16.f, 0, 0)}}},

    {RED, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)},
                                                                {XNEG, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YPOS, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YNEG, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZPOS, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZNEG, glm::vec4(12.f/16.f, 2.f/16.f, 0, 0)}}},

    {YELLOW, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)},
                                                                {XNEG, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YPOS, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)},
                                                                {YNEG, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZPOS, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)},
                                                                {ZNEG, glm::vec4(13.f/16.f, 2.f/16.f, 0, 0)}}},

    {BLACK, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)},
                                                               {XNEG, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)},
                                                               {YPOS, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)},
                                                               {YNEG, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)},
                                                               {ZPOS, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)},
                                                               {ZNEG, glm::vec4(9.f/16.f, 2.f/16.f, 0, 0)}}},

    {GRAY, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(14.f/16.f, 4.f/16.f, 0, 0)}}},

    {ORANGE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(11.f/16.f, 4.f/16.f, 0, 0)}}},

    {BROWN, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 4.f/16.f, 0, 0)}}},

    {CACTUS, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(6.f/16.f, 11.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(6.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(5.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(6.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(6.f/16.f, 11.f/16.f, 0, 0)}}},

    {OBSIDIAN, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(5.f/16.f, 13.f/16.f, 0, 0)}}},

    {MYCELIUM, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 11.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(13.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(14.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(14.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(13.f/16.f, 11.f/16.f, 0, 0)}}},


    {SNOWDIRT, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(4.f/16.f, 11.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(4.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(2.f/16.f, 11.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(1.f/8.f, 15.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(4.f/16.f, 11.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(4.f/16.f, 11.f/16.f, 0, 0)}}},
  
    {ACACIA, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(7.f/16.f, 6.f/16.f, 0, 0)},
                                                                  {XNEG, glm::vec4(7.f/16.f, 6.f/16.f, 0, 0)},
                                                                  {YPOS, glm::vec4(7.f/16.f, 5.f/16.f, 0, 0)},
                                                                  {YNEG, glm::vec4(7.f/16.f, 5.f/16.f, 0, 0)},
                                                                  {ZPOS, glm::vec4(7.f/16.f, 6.f/16.f, 0, 0)},
                                                                  {ZNEG, glm::vec4(7.f/16.f, 6.f/16.f, 0, 0)}}},

    {ACACIALEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(3.f/16.f, 8.f/16.f, 0, 0)}}},

    {PALM, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(9.f/16.f, 6.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(9.f/16.f, 6.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(9.f/16.f, 5.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(9.f/16.f, 5.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(9.f/16.f, 6.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(9.f/16.f, 6.f/16.f, 0, 0)}}},

    {PALMLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(4.f/16.f, 4.f/16.f, 0, 0)}}},

    {BIRCH, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(8.f/16.f, 6.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(8.f/16.f, 6.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(8.f/16.f, 5.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(8.f/16.f, 5.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(8.f/16.f, 6.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(8.f/16.f, 6.f/16.f, 0, 0)}}},

    {BIRCHLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(5.f/16.f, 4.f/16.f, 0, 0)}}},

    {DARKWOOD, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(4.f/16.f, 8.f/16.f, 0, 0)},
                                                              {XNEG, glm::vec4(4.f/16.f, 8.f/16.f, 0, 0)},
                                                              {YPOS, glm::vec4(4.f/16.f, 7.f/16.f, 0, 0)},
                                                              {YNEG, glm::vec4(4.f/16.f, 7.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(4.f/16.f, 8.f/16.f, 0, 0)},
                                                              {ZNEG, glm::vec4(4.f/16.f, 8.f/16.f, 0, 0)}}},

    {GRASSDEC, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(7.f/16.f, 13.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(7.f/16.f, 13.f/16.f, 0, 0)}}},

    {ROSE, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(12.f/16.f, 15.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(12.f/16.f, 15.f/16.f, 0, 0)}}},

    {DAISY, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 15.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 15.f/16.f, 0, 0)}}},

    {REDMUSH, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(12.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(12.f/16.f, 14.f/16.f, 0, 0)}}},

    {BROWNMUSH, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(13.f/16.f, 14.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(13.f/16.f, 14.f/16.f, 0, 0)}}},

    {SHRUB, std::unordered_map<Direction, glm::vec4, EnumHash>{{XPOS, glm::vec4(7.f/16.f, 12.f/16.f, 0, 0)},
                                                              {ZPOS, glm::vec4(7.f/16.f, 12.f/16.f, 0, 0)}}}


};


//class Chunk;
// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.
typedef struct ChunkData ChunkData;
class NPC;
class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;
    const std::vector<uPtr<Model>> &models;

public:
    std::vector<GLuint> transIdx;
    std::vector<glm::vec4> trans;

    std::vector<GLuint> opaqIdx;
    std::vector<glm::vec4> opaq;

    bool generated;
    int x;
    int z;

    Chunk(OpenGLContext* context, int x, int z, const std::vector<uPtr<Model>> &models);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void createVBOdata();
    void createVBOdata(ChunkData*);
    void updateVBOdata(std::vector<glm::vec4> &vbo, Direction d, std::vector<GLuint> &idx, int &count, glm::vec4 currPos, BlockType t);
    void createVBO(std::vector<glm::vec4> &vbo_opq, std::vector<GLuint> &idx_opq, std::vector<glm::vec4> &vbo_trans, std::vector<GLuint> &idx_trans);
//    ChunkData chunkData;

    int grasslandValue(glm::vec2 uv);
    int driftingIslesValue(glm::vec2 uv);
    int mountainValue(glm::vec2 uv);
    int lavalandValue(glm::vec2 uv);
    int desertValue(glm::vec2 uv);
    int mushroomlandValue(glm::vec2 uv);
    int tundraValue(glm::vec2 uv);
    int islandValue(glm::vec2 uv);
    int mesaValue(glm::vec2 uv);

    bool cross(BlockType t);
    bool transparent(BlockType t);

    void CreateChunk(int x, int z);

    std::vector<uPtr<NPC>> npcs;

    void moveNPC(int npcId, Chunk*);
    void createRivers();
    void drawStuff();
    void drawBirch(int x, int z, int height);
    void drawOak(int x, int z, int height);
    void drawSmolSnowy(int x, int z, int height);
    void drawLorgeSnowy(int x, int z, int height);
    void drawTallSnowy(int x, int z, int height);
    void drawSmolAcacia(int x, int z, int height);
    void drawLorgeAcacia(int x, int z, int height);
    void drawPalm(int x, int z, int height);
    void drawRedShroom(int x, int z, int height);
    void drawBrownShroom(int x, int z, int height);
    void drawCactus(int x, int z, int height, int tallness, bool xDir);

    void drawSpiky(int x_start, int x_end, int z_start, int z_end, int y, BlockType b);
    void drawBoring(int x_start, int x_end, int z_start, int z_end, int y, int height, BlockType b);
    void drawSlanty(int x_start, int x_end, int z_start, int z_end, int y, int height, BlockType b);
};


