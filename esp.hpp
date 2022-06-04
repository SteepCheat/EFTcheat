#pragma once

baseplayer local_player;

struct unity_string
{
	char buffer[256];
};

uint64_t GetComponentFromGameObject(const char* compname)
{
	char* name;
	uint64_t testicles = safe_read(fps_camera + 0x30, uint64_t);
	if (!testicles)return 0;

	for (int i = 0x8; i < 0x1000; i += 0x10)
	{
		uint64_t Fields = safe_read(safe_read(testicles + i, uint64_t) + 0x28, uint64_t);
		if (!Fields)return 0;
		uint64_t NameChain = safe_read(safe_read(safe_read(Fields, uint64_t), uint64_t) + 0x48, uint64_t);
		if (!NameChain)return 0;
		name = safe_read(NameChain, unity_string).buffer;
		if (strcmp(name, compname) == 0)
		{
			return Fields;
		}
	}

	return 0;
}

BOOL SetZoomB(float magnificationFactor)
{

	if (magnificationFactor < 1.0)
	{
		return FALSE;
	}

	int xDlg = (int)((float)GetSystemMetrics(
		SM_CXSCREEN) * (1.0 - (1.0 / magnificationFactor)) / 2.0);
	int yDlg = (int)((float)GetSystemMetrics(
		SM_CYSCREEN) * (1.0 - (1.0 / magnificationFactor)) / 2.0);

	return MagSetFullscreenTransform(magnificationFactor, xDlg, yDlg);
}

BOOL SetZoom(float magnificationFactor)
{

	if (magnificationFactor < 1.0)
	{
		return FALSE;
	}
	int xDlg = (int)((float)GetSystemMetrics(
		SM_CXSCREEN) * (1.0 - (1.0 / magnificationFactor)) / 2.0);
	int yDlg = (int)((float)GetSystemMetrics(
		SM_CYSCREEN) * (1.0 - (1.0 / magnificationFactor)) / 2.0);

	return MagSetFullscreenTransform(magnificationFactor, xDlg, yDlg);
}

void Zoom(void) {

	if ((MagInitialize)()) {

		while (true) {

			if ((GetAsyncKeyState)(Vars::Misc::zoombind) & 0x8000)
			{
				if (!Vars::Misc::zoomed) {
					SetZoomB((float)Vars::Misc::zoomvalue);
					Vars::Misc::zoomed = true;
				}
				
			}
			else {
				if (Vars::Misc::zoomed) {
					SetZoomB((float)1);
					Vars::Misc::zoomed = false;
				}
				Sleep(1);
			}
			Sleep(1);
		}
		Sleep(1);
	}

}


void GearChams(uint64_t in)
{
	uint64_t h1 = safe_read(in + 0xA8, uint64_t);
	uint64_t pSkinsDict = safe_read(h1 + 0x38, uint64_t);
	uint32_t SkinsCount = safe_read(pSkinsDict + 0x40, uint32_t);
	if (!SkinsCount || SkinsCount > 10000)
		return;
	uint64_t SkinEntries = safe_read(pSkinsDict + 0x18, uint64_t);
	for (int i = 0; i < SkinsCount; i++)
	{
		uint64_t pBodySkins = safe_read(SkinEntries + 0x30 + (0x18 * i), uint64_t);
		uint64_t pLodsArray = safe_read(pBodySkins + 0x18, uint64_t);
		uint32_t LodsCount = safe_read(pLodsArray + 0x18, uint32_t);
		if (LodsCount > 10000)
			continue;

		for (int j = 0; j < LodsCount; j++)
		{
			uint64_t pLodEntry = safe_read(pLodsArray + 0x20 + (j * 0x8), uint64_t);
			if (j == 1)
				pLodEntry = safe_read(pLodEntry + 0x20, uint64_t);

			uint64_t SkinnedMeshRenderer = safe_read(pLodEntry + 0x20, uint64_t);
			uint64_t pMaterialDictionary = safe_read(SkinnedMeshRenderer + 0x10, uint64_t);
			uint32_t MaterialCount = safe_read(pMaterialDictionary + 0x150, uint32_t);
			if (MaterialCount > 0 && MaterialCount < 6)
			{
				uint64_t MaterialDictionaryBase = safe_read(pMaterialDictionary + 0x140, uintptr_t);

				for (int k = 0; k < MaterialCount; k++)
					safe_write(MaterialDictionaryBase + (k * 0x50), 0, uintptr_t);
			}
		}
	}
}


class World
{
private:
	struct list_info
	{
		int size;
		uintptr_t base;
	};
public:


	void get_player_list()
	{

		uint64_t onlineusers = safe_read(reinterpret_cast<uintptr_t>(this) + 0x88, uintptr_t);
		list_info info{};
		info.base = safe_read(onlineusers + 0x10, uintptr_t);
		info.size = safe_read(onlineusers + 0x18, uintptr_t);
		if (info.size < 1)
		{
			Vars::Misc::test = false;
			
		}
		else
		{
			Vars::Misc::test = true;

		}


		constexpr auto BUFFER_SIZE = 128;
		uint64_t player_buffer[BUFFER_SIZE];
		writevall(info.base + 0x20, player_buffer, sizeof(uint64_t) * info.size);


		for (int i = 0; i < info.size; i++)
		{
			uint64_t instance = player_buffer[i];

			baseplayer current_player;
			current_player.entity = instance;

			if (current_player.is_local_player())
			{
				current_player.is_local = true;
				local_player = current_player;
			}

			if (Vars::Misc::Chams) {
				GearChams(instance);
			}
		}
	}



};





struct base_object_t
{
	uint64_t previousObjectLink; //0x0000
	uint64_t nextObjectLink; //0x0008
	uint64_t object; //0x0010
};

struct game_object_manager_t
{
	uint64_t lastTaggedObject; //0x0
	uint64_t taggedObjects; //0x8
	uint64_t lastMainCameraTagged; // 0x10
	uint64_t MainCameraTagged; // 0x18
	uint64_t lastActiveObject; //0x20
	uint64_t activeObjects; // 0x28
};


uint64_t GetObjectFromList(uint64_t listPtr, uint64_t lastObjectPtr, const char* objectName)
{
	char* name;
	uint64_t classNamePtr = 0x0;
	using structs::BaseObject;
	BaseObject activeObject = safe_read(listPtr, BaseObject);
	BaseObject lastObject = safe_read(lastObjectPtr, BaseObject);

	if (activeObject.object != 0x0)
	{
		while (activeObject.object != 0 && activeObject.object != lastObject.object)
		{
			classNamePtr = safe_read(activeObject.object + 0x60, uint64_t);
			name = safe_read(classNamePtr + 0x0, unity_string).buffer;
			if (strcmp(name, objectName) == 0)
			{
				return activeObject.object;
			}

			activeObject = safe_read(activeObject.nextObjectLink, BaseObject);
		}
	}
	if (lastObject.object != 0x0)
	{
		classNamePtr = safe_read(lastObject.object + 0x60, uint64_t);
		name = safe_read(classNamePtr + 0x0, unity_string).buffer;
		if (strcmp(name, objectName) == 0)
		{
			return lastObject.object;
		}
	}

	return uint64_t();
}


class GameObjectManager
{
private:
public:

	World* get_game_world()
	{
		char name[256];
		auto camera_objects = read<std::array<uintptr_t, 2>>(reinterpret_cast<uintptr_t>(this) + 0x28);
		base_object_t activeObject = safe_read(camera_objects[0], base_object_t);
		uintptr_t class_name_ptr = 0x00;
		int  game_world_count = 1000;
		for (int i = 0; i < game_world_count; i++)
		{
			class_name_ptr = read<uintptr_t>(activeObject.object + 0x60);
			writevall(class_name_ptr + 0x0, &name, sizeof(name));
			if (strcmp(name, _("GameWorld")) == 0)
			{
				auto unk1 = read<uintptr_t>(activeObject.object + 0x30);
				auto unk2 = read<uintptr_t>(unk1 + 0x18);
				return read<World*>(unk2 + 0x28);
			}
			activeObject = read<base_object_t>(activeObject.nextObjectLink);
		}
	}

};

World* world{};
class entity_list_t
{
public: 
	GameObjectManager* game_object_manager{};

	std::vector<uintptr_t> players_list{};







	void update()
	{

		game_object_manager = read<GameObjectManager*>(U_Base + 0x17F8D28);
		world = game_object_manager->get_game_world();
		if (world)
		{
			world->get_player_list();

			if (Vars::Misc::test)
			{

				gom = read<uintptr_t>(U_Base + 0x17F8D28);
				auto tagged_objects = read<std::array<uint64_t, 2>>(gom + 0x18);
				fps_camera = GetObjectFromList(tagged_objects[0], !tagged_objects[1], _("FPS Camera"));
			}
		}
	}


	void run()
    {
	
		if (local_player.entity)
		{
			if (Vars::Misc::no_recoil)
			{
				local_player.remove_recoil();
			}

			if (Vars::Misc::speedHack)
			{
				local_player.SpeedHack();
			}

			if (Vars::Misc::no_sway)
				local_player.NoSwey();

			/*if (Vars::Misc::infinite_stamine)
				local_player.unlimited_stamina();*/


			/*if (Vars::Misc::NightVision)
			{
				auto NightVision = GetComponentFromGameObject(_("NightVision"));
				safe_write<int>(NightVision + 0xD8, 1);
			}

			if (!Vars::Misc::NightVision)
			{
				auto NightVision = GetComponentFromGameObject(_("NightVision"));
				safe_write<int>(NightVision + 0xD8, 0);
			}*/

			if (Vars::Misc::ThermalVision)
			{
				auto ThermalVision = GetComponentFromGameObject(_("ThermalVision"));
				safe_write(ThermalVision + 0xE0, true, bool);
			}

			if (!Vars::Misc::ThermalVision)
			{
				auto ThermalVision = GetComponentFromGameObject(_("ThermalVision"));
				safe_write(ThermalVision + 0xE0, false, bool);
			}

			if (Vars::Misc::Visor)
			{
				auto nVision3 = GetComponentFromGameObject(_("VisorEffect"));
				safe_write(nVision3 + 0xB8, 0.f);

			}
		

		
	}
}

} entity_list;