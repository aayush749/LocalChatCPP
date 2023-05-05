#include <LCClient/Application/Application.h>
#include <LCClient/LCClient.h>

#include <filesystem>

// Texture loader setup
TextureLoader GLOBAL_TEX_LOADER;

std::array<const char*, (size_t)TextureType::COUNT> texturePaths = {
	"../../../../LocalChat/LCClient/res/images/chat/chat_bubble_in.png",
	"../../../../LocalChat/LCClient/res/images/chat/chat_bubble_out.png"
};

std::filesystem::path CONFIG_FILE_PATH = std::filesystem::path(getenv("USERPROFILE")).concat("/Documents/LocalChatCpp/config/config.ini");

LCClient GLOBAL_CLIENT(CONFIG_FILE_PATH);

int main()
{
	Application app;
	app.Start();
	app.Update();
}