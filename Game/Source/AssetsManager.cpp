#include"AssetsManager.h"
#include"App.h"
#include"External/PhysFS/include/physfs.h"

AssetsManager::AssetsManager() : Module() 
{
	PHYSFS_init(nullptr);
}
AssetsManager::~AssetsManager() 
{
	PHYSFS_deinit();
}
bool AssetsManager::Start() 
{
	PHYSFS_mount("Assets.zip",NULL, 0);
	return true;
}
bool AssetsManager::DeleteBuffer() {
	delete[] buffer;
	return true;
}
char* AssetsManager::GetLastBuffer() {
	return buffer;
}
int AssetsManager::MakeLoad(const char* fileName) 
{
	int ret = 0;
	PHYSFS_file* data_file = PHYSFS_openRead(fileName);

	if (data_file != nullptr)
	{
		int file_lenght = PHYSFS_fileLength(data_file);
		buffer = new char[(int)file_lenght];
		int readed = PHYSFS_read(data_file, buffer, 1, (int)file_lenght);
		if (readed != file_lenght)
		{
			delete[] buffer;
		}
		ret = readed;
		PHYSFS_close(data_file);
	}
	
	
	return ret;
}
SDL_RWops* AssetsManager::Load(const char* fileName) 
{
	int size = MakeLoad(fileName);
	return SDL_RWFromConstMem(buffer, size);
}
