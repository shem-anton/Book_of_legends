#include "Battle_system.h"

// File contains the basics of ability system of "Book of legends"

class Effect;

/*template for Effect:
class :public Effect //			Description
{
public:
	(int duration, double value):Effect(duration, value) {positive = ();};

	void initialize_effect(Unit & caster, Unit & target)
	{
		
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		
	}
};*/

// List of effects on characters:

class HP_Affect:public Effect //				Instantly adds value to current HP or duration turns increases current HP by value
{
public:
	HP_Affect(int duration, double value):Effect(duration, value), HP_override(0) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		HP_override += target.modify_HP(value);
		if(target.get_HP() <= 0)
			target.kill();
	}

	void apply_effect(Unit & caster, Unit & target)
	{
		HP_override += target.modify_HP(value);
		if(target.get_HP() <= 0)
			target.kill();
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		if(!target.is_dead())
		{
			target.modify_HP(HP_override);
			target.modify_HP((-1)*value);
		}
	}
private:
	short HP_override; //							Stores the amount of HP that was added over max_HP
};

class HP_Affect_temporary:public Effect //			Gives a temporary shield that absorbs damage, does nothing if value is non-positive
{
public:
	HP_Affect_temporary(int duration, double value):Effect(duration, value), HP_pool(0) {positive = true;};

	void initialize_effect(Unit & caster, Unit & target)
	{
		HP_override = target.modify_HP(value);
		HP_observations = target.get_HP();
	}

	void apply_ability(Unit & caster, Unit & target)
	{
		if(active)
		{
			if(target.get_HP() < HP_observations)
			{
				HP_pool += (HP_observations - target.get_HP());
				if(HP_pool <= value)
					target.modify_HP(HP_observations - target.get_HP());
				else
				{
					target.modify_HP(HP_observations - target.get_HP() - value + HP_pool);
					active = false;
					HP_pool = value;
				}
			}
			HP_observations = target.get_HP();
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_HP(HP_override);
		if(active)
			target.modify_HP(value - HP_pool);
	}

private:
	bool active; //						Shows whether shield have worked out
	short HP_observations; //			Stores last observed HP of a target
	short HP_pool; //					Stores the damage taken by shield
	short HP_override; //				Stores the amount of HP that was added over max_HP
};

class MP_Affect_temporary:public Effect //			Gives a temporary mana to use abilities, does nothing if value is non-positive
{
public:
	MP_Affect_temporary(int duration, double value):Effect(duration, value), MP_pool(0) {positive = true;};

	void initialize_effect(Unit & caster, Unit & target)
	{
		MP_override = target.modify_MP(value);
		MP_observations = target.get_MP();
	}

	void apply_ability(Unit & caster, Unit & target)
	{
		if(active)
		{
			if(target.get_MP() < MP_observations)
			{
				MP_pool += (MP_observations - target.get_MP());
				if(MP_pool <= value)
					target.modify_MP(MP_observations - target.get_MP());
				else
				{
					target.modify_MP(MP_observations - target.get_MP() - value + MP_pool);
					active = false;
					MP_pool = value;
				}
			}
			MP_observations = target.get_MP();
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_MP(MP_override);
		if(active)
			target.modify_MP(value - MP_pool);
	}

private:
	bool active; //						Shows whether all the mana was spent
	short MP_observations; //			Stores last observed MP of a target
	short MP_pool; //					Stores the amount of mana that was used
	short MP_override; //				Stores the amount of MP that was added over max_MP
};

class MP_Affect:public Effect //				Instantly adds value to current MP or duration turns increases current MP by value
{
public:
	MP_Affect(int duration, double value):Effect(duration, value), MP_override(0) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		MP_override += target.modify_MP(value);
	}

	void apply_effect(Unit & caster, Unit & target)
	{
		MP_override += target.modify_MP(value);
		if(target.get_MP() <= 0)
			target.kill();
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_MP(MP_override);
		target.modify_MP((-1)*value);
	}
private:
	short MP_override; //						Stores the amount of MP that was added over max_MP
};

class AP_Affect:public Effect //			Adds value to AP and decreases AP after duration
{
public:
	AP_Affect(int duration, double value):Effect(duration, value) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(!(*this).is_instant()) //		Duration != 0
		target.modify_AP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_AP((-1)*value);
	}
};

class max_HP_Affect:public Effect //			Multiply max_HP with value and change current HP so that percentage of health be the same for duration
{
public:
	max_HP_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_max_HP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_max_HP(1/value);
	}
};

class max_MP_Affect:public Effect //			Multiply max_MP with value and change current MP so that percentage of health be the same for duration
{
public:
	max_MP_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_max_MP(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_max_MP(1/value);
	}
};

class Hit_Chance_Affect:public Effect //			Multiplies hit chance of target by value for duration
{
public:
	Hit_Chance_Affect (int duration, double value):Effect(duration, value) 
	{		
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(!(*this).is_instant()) //		Duration != 0
		target.modify_hit_chance(value);
	}

		void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_hit_chance(1/value);
	}
};

class Dodge_Chance_Affect:public Effect //			Multiplies dodge chance of target by value for duration
{
public:
	Dodge_Chance_Affect (int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 1);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_dodge_chance(value);
	}

		void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_dodge_chance(1/value);
	}
};

class Covered_Affect:public Effect //			Multiplies field "covered" of a target by value for a duration
{
public:
	Covered_Affect(int duration, double value):Effect(duration, value) {positive = (value > 1); };

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_covered(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_covered(1/value);
	}
};

class Current_Initiative_Affect:public Effect //			Adds value to current initiative of a unit. Sets everything back after duration
{
public:
	Current_Initiative_Affect(int duration, double value):Effect(duration, value) {positive = (value > 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_current_initiative(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_current_initiative((-1)*value);
	}
};

class Initiative_Affect:public Effect //			Adds value to initiative for duration, initiative can not be modified to be less than 1 or more then INITIATIVE_CONSTANT
{
public:
	Initiative_Affect(int duration, double value):Effect(duration, value), initiative_override(0) 
	{		
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 0);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		initiative_override = target.modify_initiative(value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_initiative(initiative_override);
		target.modify_initiative((-1)*value);
	}
private:
	char initiative_override;
};

class Remove_Buff_Affect:public Effect //			Removes all buffs from target if value == 0, removes all debuffs otherwise. If not instant, sets them back if they are not expired
{
public:
	Remove_Buff_Affect(int duration, double value):Effect(duration, value) {positive = (value != 0);};

	void initialize_effect(Unit & caster, Unit & target) // Take away all buffs/debuffs
	{
		for(auto iter = target.applied_abilities.begin(); iter != target.applied_abilities.end(); ++iter)
			if(value)
			{
				if((*iter)->is_debuff()) // If is a debuff
				{
					taken_buffs.push_back(*(*iter)); //		Store it in taken_buffs
					(*iter)->remove_ability(target); //		Apply remove_ability
					target.applied_abilities.erase(iter); //Remove from applied_abilities
				}
			}
			else
			{
				if((*iter)->is_buff()) // If is a buff/debuff
				{
					taken_buffs.push_back(*(*iter)); //		Store it in taken_buffs
					(*iter)->remove_ability(target); //		Apply remove_ability
					target.applied_abilities.erase(iter); //Remove from applied_abilities
				};
			}
	}

	void apply_effect(Unit & caster, Unit & target) //	Ensure that their clock is still ticking
	{
		for(auto iter = taken_buffs.begin(); iter != taken_buffs.end(); ++iter)
		{
			++(*iter);
			if((iter)->expired())
				taken_buffs.erase(iter);
		}
	}

	void remove_effect(Unit & caster, Unit & target) //	Return unexpired ones
	{
		for(auto iter = taken_buffs.begin(); iter != taken_buffs.end(); ++iter)
			(*iter).initialize_ability((*iter).get_ability_caster(),target,(*iter).get_duration_counter());
	}
	vector <Ability_Active> taken_buffs;
};

class Silence_Affect:public Effect //			Prevents from using abilities except for Attack
{
public:
	Silence_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = false;
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		for(auto iterator = target.abilities.begin(); iterator != target.abilities.end(); ++iterator)
			if((*iterator)->get_return_value() != 1)
			{
				removed_abilities.push_back(*(*iterator)); //			Put ability into temporary storage
				target.abilities.erase(iterator); //				Remove ability from target
			}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		for(auto iterator = removed_abilities.begin(); iterator != removed_abilities.end(); ++iterator)
			target.abilities.push_back(&(*iterator));
		removed_abilities.clear();
	}

private:
	vector <Ability_Active> removed_abilities; //		Storage for temporary removed abilities
};

class Death_Affect:public Effect //			Kills target, sets current hp to 0, sets current_initiative to 0 if value == 0, revives with max hp if duration != 0, with REVIVE_CONSTANT% hp otherwise otherwise
{
public:
	Death_Affect(int duration, double value):Effect(duration, value) {positive = (value != 0);};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(value)
			// Revive
			if(target.is_dead())
		{
			target.revive();
			if(duration)
				target.modify_HP(target.max_HP);
			else
				target.modify_HP(target.max_HP * REVIVE_CONSTANT / 100);
		}
		else
		{
			// Kill
			HP_buffer = target.get_HP();
			target.modify_HP((-1) * target.get_HP());
			target.modify_current_initiative((-1) * target.get_current_initiative());
			target.kill();
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		if(value)
			// Unrevive
		{
			target.kill();
			target.modify_HP((-1) * target.get_HP());
		}
		else
			// Unkill
		if(target.is_dead())
		{
			target.revive();
			target.modify_HP(HP_buffer);
		}
	}
private:
	int HP_buffer; //			Stores HP of killed character until it is revived
};

class Toughness_Affect:public Effect //			Adds given value to toughness of target
{
public:
	Toughness_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 0);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		if(value < 0 && -value > target.get_toughness())
			actual_value_of_modifying_toughness = -target.get_toughness();
		else actual_value_of_modifying_toughness = value;
		target.modify_toughness(actual_value_of_modifying_toughness);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_toughness(-actual_value_of_modifying_toughness);
	}
private:
	unsigned char actual_value_of_modifying_toughness;
};

class Weapon_Skill_Affect:public Effect //			Modifies weapon skill of current weapon by value
{
public:
	Weapon_Skill_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = (value > 0);
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		target.modify_weapon_skill(target.weapon.get_type(), value);
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		target.modify_weapon_skill(target.weapon.get_type(), -value);
	}
};

class Passive_Ability_Affect:public Effect //			Removes temporary all passive abilities
{
public:
	Passive_Ability_Affect(int duration, double value):Effect(duration, value) 
	{
		if(duration == 0) // Must not be instant
			duration++;
		positive = false;
	};

	void initialize_effect(Unit & caster, Unit & target)
	{
		for(auto iter = target.passive_abilities.begin(); iter != target.passive_abilities.end(); ++iter)
		{
			iter->remove_ability(target);
			passive_abilities_disabled.push_back(*iter);
			target.passive_abilities.erase(iter);
		}
	}

	void remove_effect(Unit & caster, Unit & target)
	{
		for(auto iter = passive_abilities_disabled.begin(); iter != passive_abilities_disabled.end(); ++iter)
		{
			iter->initialize_ability(target);
			target.passive_abilities.push_back(*iter);
			passive_abilities_disabled.erase(iter);
		}
	}
private:
	vector <Ability_Passive> passive_abilities_disabled; //		Storage for removed abilities
};