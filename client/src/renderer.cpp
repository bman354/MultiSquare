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

void renderMapTile(gl2d::Renderer2D& renderer, Tile& tile, gl2d::TextureAtlas& atlas, gl2d::Texture& texture, int x, int y) {

	//atlas x value = tile type % 14
	//atlas y value = tile type / 14 rounded down
	renderer.renderRectangle({ x * 32, y * 32, 32, 32 }, texture, Colors_White, {0,0}, 0.0f, {atlas.get((tile.type % 14), (tile.type / 14))});
	//renderRectangle(const Rect transforms, const Texture texture, const Color4f colors = {1,1,1,1}, const glm::vec2 origin = {}, const float rotationDegrees = 0, const glm::vec4 textureCoords = GL2D_DefaultTextureCoords)
}

void renderMap(gl2d::Renderer2D& renderer, gl2d::TextureAtlas& textureAtlas, gl2d::Texture& texture, MapData& map) {
	for (int y = 0; y < map.h; y++) {
		for (int x = 0; x < map.w; x++) {
			renderMapTile(renderer, map.get(x, y), textureAtlas, texture, x, y);
		}
	}
}