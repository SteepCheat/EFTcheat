


namespace structs {
	enum Bones : int
	{
		HumanBase = 0,
		HumanPelvis = 14,
		HumanLThigh1 = 15,
		HumanLThigh2 = 16,
		HumanLCalf = 17,
		HumanLFoot = 18,
		HumanLToe = 19,
		HumanRThigh1 = 20,
		HumanRThigh2 = 21,
		HumanRCalf = 22,
		HumanRFoot = 23,
		HumanRToe = 24,
		HumanSpine1 = 29,
		HumanSpine2 = 36,
		HumanSpine3 = 37,
		HumanLCollarbone = 89,
		HumanLUpperarm = 90,
		HumanLForearm1 = 91,
		HumanLForearm2 = 92,
		HumanLForearm3 = 93,
		HumanLPalm = 94,
		HumanRCollarbone = 110,
		HumanRUpperarm = 111,
		HumanRForearm1 = 112,
		HumanRForearm2 = 113,
		HumanRForearm3 = 114,
		HumanRPalm = 115,
		HumanNeck = 132,
		HumanHead = 133
	};

	struct BaseObject
	{
		uint64_t previousObjectLink; //0x0000
		uint64_t nextObjectLink; //0x0008
		uint64_t object; //0x0010
	};

	struct GameObjectManager
	{
		uint64_t LastTaggedNode; // 0x0
		uint64_t TaggedNodes; // 0x8
		uint64_t LastMainCameraTaggedNode; // 0x10
		uint64_t MainCameraTaggedNodes; // 0x18
		uint64_t LastActiveNode; // 0x20
		uint64_t ActiveNodes; // 0x28
	}; //Size: 0x0010

	class ListInternal
	{
	public:
		char pad_0x0000[0x20]; //0x0000
		uintptr_t* firstEntry; //0x0020 
	}; //Size=0x0028

}





class entity_list {
public:



	uintptr_t transform, entity;


};







class baseplayer : public entity_list {
private:
	uintptr_t _values;
public:


	
	bool is_local = false;
	void NoSwey()
	{
		uint64_t animation = read<UINT64>(entity + 0x198);
		if (animation)
		{
			uint64_t breath = animation + 0x28;
			breath = read<uint64_t>(breath);
			safe_write(breath + 0xA4, 0.f, float); //breath intensity

			uint64_t Walk = animation + 0x30;
			Walk = read<uint64_t>(Walk);
			safe_write(Walk + 0x44, 0.f, float); //walk intensity

			uint64_t MotionReact = animation + 0x38;
			MotionReact = read<uint64_t>(MotionReact);
			safe_write(MotionReact + 0xD0, 0.f, float);// motion

			uint64_t ForceReact = animation + 0x40;
			ForceReact = read<uint64_t>(ForceReact);
			safe_write(ForceReact + 0x30, 0.f, float);//force 

			uint64_t shot = animation + 0x48;
			shot = read<uint64_t>(shot);
			safe_write(shot + 0x68, 0.f, float); // shot
		}
	}



	void SpeedHack() {
		uintptr_t time = (0x17F8AE0 + (7 * 8));
		uint64_t time_scale = read<uint64_t>(U_Base + time);
		safe_write(time_scale + 0xFC, 2.0f, float);
	}
	
	void remove_recoil()
	{
		auto procedural_weapon_animation = read<uintptr_t>(entity + 0x198);
		if (procedural_weapon_animation)
		{
			safe_write(procedural_weapon_animation + 0x100, 1, int); // mask
		}
	}


	bool is_local_player() {
		bool local = safe_read(entity + 0x18, int);
		return local;
	}

	
	/*void unlimited_stamina()
	{
		uint64_t physical = safe_read<uintptr_t>(entity + 0x4D0);
		if (physical)
		{

			const uintptr_t body_stamina = safe_read<uintptr_t>(physical + 0x38);
			const uintptr_t hand_stamina = safe_read<uintptr_t>(physical + 0x40);

			safe_write<float>(body_stamina + 0x48, 1000.f);
			safe_write<float>(hand_stamina + 0x48, 1000.f);

		}
	}*/

	 
	class movement_context {
	public:
		enum EPhysicalCondition {
			None = 0,
			OnPainkillers = 1,
			LeftLegDamaged = 2,
			RightLegDamaged = 4,
			BoggedDown = 8,
			LeftArmDamaged = 16,
			RightArmDamaged = 32,
			Tremor = 64,
			UsingMeds = 128,
			HealingLegs = 256
		};



		void fall_speed(float speed)
		{
			safe_write((uintptr_t)this + 0x1F8, speed, float);//freefalltime
		}
	};

	movement_context* get_movement()
	{
		return read<movement_context*>(entity + 0x40);
	}
};





