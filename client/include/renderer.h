#pragma once
#include <gl2d/gl2d.h>
#include "Player.h"
#include "Bullet.h"

void renderPlayer(gl2d::Renderer2D& renderer, Player player, gl2d::Texture texture);
void renderBullet(gl2d::Renderer2D& renderer, Bullet bullet, gl2d::Texture texture);
void renderPlayerName(gl2d::Renderer2D& renderer, Player player, gl2d::Font font);
void renderMapTile(gl2d::Renderer2D& renderer, Tile& tile, gl2d::TextureAtlas& atlas, int x, int y);