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

void renderMapTile(gl2d::Renderer2D& renderer, Tile& tile, gl2d::TextureAtlas& atlas, int x, int y) {

	//atlas x value = tile type % 14
	//atlas y value = tile type / 14 rounded down
	renderer.renderRectangle({ x * 16, y * 16, 16, 16 }, atlas.get((tile.type % 14), (tile.type / 14), false));
	
}