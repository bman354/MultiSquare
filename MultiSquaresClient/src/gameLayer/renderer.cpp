#include <gl2d/gl2d.h>
#include "renderer.h"

void renderPlayer(gl2d::Renderer2D &renderer, Player player, gl2d::Texture texture){
	renderer.renderRectangle({ player.pos, player.playerSize.x,player.playerSize.y },texture);
}

void renderPlayerName(gl2d::Renderer2D& renderer, Player player, gl2d::Font font) {
	renderer.renderText({ player.pos.x, player.pos.y }, player.name.c_str(), font, Colors_Orange);
}

void renderBullet(gl2d::Renderer2D& renderer, Bullet bullet, gl2d::Texture texture) {
	renderer.renderRectangle({ bullet.pos, bullet.width, bullet.height }, texture);
}

