#include <LCClient/Application/Application.h>

// Texture loader setup
TextureLoader GLOBAL_TEX_LOADER;
std::array<const char*, (size_t)TextureType::COUNT> texturePaths = {
	"../../../../LocalChat/LCClient/res/images/chat/chat_bubble_in.png",
	"../../../../LocalChat/LCClient/res/images/chat/chat_bubble_out.png"
};

int main()
{
	Application app;
	app.Start();
	app.Update();
}