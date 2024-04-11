#pragma once
#include <gl2d/gl2d.h>
#include "Player.h"
#include "Bullet.h"
#include "MapData.h"

void renderPlayer(gl2d::Renderer2D& renderer, Player player, gl2d::Texture texture);
void renderBullet(gl2d::Renderer2D& renderer, Bullet bullet, gl2d::Texture texture);
void renderPlayerName(gl2d::Renderer2D& renderer, Player player, gl2d::Font font);
void renderTile(gl2d::Renderer2D& renderer, Tile& tile, gl2d::TextureAtlas,int x,int y);