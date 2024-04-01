#include <gl2d/gl2d.h>
#include "renderer.h"

void renderPlayer(gl2d::Renderer2D &renderer, Player player, gl2d::Texture texture){
	renderer.renderRectangle({ player.pos, player.playerSize.x,player.playerSize.y },texture);
}

//TODO scale text size to screen size
void renderPlayerName(gl2d::Renderer2D& renderer, Player player, gl2d::Font font) {
	renderer.renderText({ player.pos.x + (player.playerSize.x / 2), player.pos.y + player.playerSize.y }, player.name.c_str(), font, Colors_Orange, 0.5f);
}

void renderBullet(gl2d::Renderer2D& renderer, Bullet bullet, gl2d::Texture texture) {
	renderer.renderRectangle({ bullet.pos, bullet.width, bullet.height }, texture);
}

