#ifndef BATTLE_SYSTEM_OF_BOOK_OF_LEGENDS
#define BATTLE_SYSTEM_OF_BOOK_OF_LEGENDS

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>

using namespace std;

//Constants:

const int INITIATIVE_CONSTANT = 10; //			Number of initiative that is needed to be accumulated to make turn
const int REVIVE_CONSTANT = 10;	//				Percent of hp after revival
const int NUMBER_OF_WEAPON_TYPES = 10; //		Number of weapon types in a game

//Firstly basic classes for storing data:

class Ability;
class Ability_Active;
class Ability_Passive;
class Armor;
class Artifact;
class Attack;
class Consumable_Item;
class Effect;
class Hero;
class Item;
class Opponents;
class Party;
class Unit;
class Weapon;

//Secondly classes for implementing battle

class Battle;

class Effect
{
public:
	Effect(){};
	Effect(int duration, double value):duration(duration), value(value) {};

	bool expired(int turn) //			True if effect should not be applied on the turn
	{
		return turn > duration;
	}
	bool is_instant() //				Shows whether effect is instant or continious
	{
		return duration == 0;
	}
	bool is_buff() //					Shows whether effect is a positive buff
	{
		return positive;
	}

	void initialize_effect(Unit &, Unit &){}; //				Method used when initializing effect
	void apply_effect(Unit &, Unit &){}; //						Method used when applying effect
	void remove_effect(Unit &, Unit &){}; //					Method used when removing effect

	char duration; //						Duration of the effect
	double value; //						Modifier of effect
	bool positive; //						Shows whether effect is positive(buff) or negative(debuff)
};

class Ability
{
public:
	// Some information for the atmosphere
	string name;
	string background;
private:
	int value; //									It's for the game balance, I'm serious!				
};

class Item
{
public:
	// Some information for the atmosphere
	string name;
	string background;
private:
	int value; //									It's for the game balance, I'm serious!

	int cost; //									Cost of item in gold pieces
	vector <Ability*> abilities; //					List of abilities of an item
};

class Consumable_Item: public Item
{
public:
private:
	char capacity; //								Amount of items that can be placed in one stot
};

class Weapon: public Item
{
public:
	int get_hit_modifier();
	int get_type();
	int get_damage();
	double get_AP_modifier();
private:
	char to_hit_modifier; //						Displays the inprovement of to-hit chance while hitting with this weapon
	short damage; //									Damage, done by attack with this weapon
	double AP_modifier; //							Modifyer of action points needed to hit with this weapon
	bool twohanded; //								Displays whether the second hand can be used
	int type; //									Type of a weapon, look it up in a special table
};

class Artifact: public Item
{
public:
private:
	Ability_Passive *buff; //						Passive ability of the artifact
};

class Armor: public Item
{
public:
private:
	Ability_Passive *protection; //					Passive ability of protecting the wearer
};

class Unit
{
public:
	// Some information for the atmosphere
	string name;
	string background;

	// Public stats of the unit:

	short max_HP; //								Maximal hit points
	short max_MP; //								Maximal mana points
	char initiative; //								Initiative of a unit
	int value; //									It's for the game balance, I'm serious!

	// Modifiers of stats:
	void modify_current_initiative(double);	//	Adds argument to currrent initiative
	int modify_initiative(double);	//			Adds argument to initiative, returns amount of initiative over INITIATIVE_CONSTANT or amount of initiative below 1
	int modify_HP(double); //					Adds argument to current HP, returns amount of HP over max_HP
	int modify_MP(double); //					Adds argument to current MP, returns amount of MP over max_MP
	void modify_max_HP(double); //				Multiplies max_HP and HP by argument
	void modify_max_MP(double); //				Multiplies max_MP and MP by argument
	void modify_AP(double); //					Adds argument to AP
	void kill();
	void revive();
	void modify_hit_chance(double); //			Multiplies hit_chance by argument
	void modify_dodge_chance(double); //		Multiplies dodge_chance by argument
	void modify_covered(double); //				Multiplies covered by argument
	int modify_toughness(int); //				Adds argument to toughness of the unit, returnes 0 if modified toughness >= 0, modulo of toughness else.
	void modify_weapon_skill(short, short); //  Modifies weapon skill of the given weapon by given value

	// Getters of Unit:
	bool is_dead();
	int get_current_initiative();
	int get_HP();
	int get_MP();
	int get_AP();
	int get_hit_chance();
	int get_dodge_chance();
	int get_toughness();
	int get_weapon_skill(int);
	double get_covered();

	// Methods of a class:

	Ability_Active &choose_ability(Battle &); //			Describes the way bot choose ability from the list in battle, has to be redefined for player in Hero class
	Unit &choose_target_for_ability(Battle &, Ability_Active &); //			Describes the way bot choose target for ability in battle, has to be redefined for player in Hero class

	void show();

	//Use of other classes:

	vector <Ability_Active> abilities; //			List of abilities of a unit
	vector <Ability_Active> applied_abilities; //	List of active abilities, applied on a unit
	Weapon weapon; //								Weapon unit is carrying
	Armor armor; //									Armor unit is wearing
	vector <Artifact> artifacts; //					List of artifacts unit is carryings
	vector <Consumable_Item> consumables; //		List of consumable items

private:
	//Stats of a unit:

	short HP; //									Current hit points
	short MP; //									Current mana points
	short AP;//										Maximal action points

	char current_initiative; //						Current initiative in battle for queueing
	char hit_chance; //								Chance of a unit to hit, 0<hit_chance<100
	char dodge_chance; //							Chance of a unit to dodge, 0<dodge_chance<100
	int toughness; //								Amount of attack damage that will be substracted
	double covered; //								AP modifier to attack this unit
					//								Covered == 1 means unit is not covered at all
	
	short weapon_skill[NUMBER_OF_WEAPON_TYPES]; //	Stores weapon skills for every weapon type, skill = percent of weapon's to_hit bonus

	bool dead; //									Shows whether unit is dead in particular combat
};

class Ability_Passive: public Ability
{
public:
	void obtaining_ability(Unit*){}; //				Method, applied when obtainig abiity
	void discarding_ability(Unit*){}; //			Method, applied when discarding ability
private:
};

class Ability_Active: public Ability
{
public:
	Ability_Active(int return_value = 0):return_value(return_value){}; //							Constructor

	int get_manacost();
	int get_actioncost();
	int get_duration_counter();
	int get_return_value();
	Unit & get_ability_caster();

	bool is_instant(); //							Shows whether all the effects are instant
	bool is_buff(); //								Shows whether ability contains buff
	bool is_debuff(); //							Shows whether ability contains debuff
	bool expired(); //								True if ability should be removed

	void operator++(); //							Adds one to duration counter

	int initialize_ability(Unit &, Unit &, int turn = 0); //Used when ability is applied first time, begins with duration_counter = turn
	void apply_ability(Unit &); //					Used every turn in battle
	void remove_ability(Unit &); //					Used when ability is removed
private:
	short manacost; //								Amount of mana needed to use ability
	short actioncost; //							Amount of action points needed to use ability
	char duration_counter; //						Counter of a current stage of the effect

	vector <Effect> effects; //						Effects of particular ability
	Unit *ability_caster; //						Stores caster after initialization\

	int return_value; //							Uses to process special situations, can be changed in constructor ONLY. Zero by default
				   	  //							If return value is equal to 1, this abillity is Attack
					  //							If return value is equal to 2, this ability is Retreat
};

class Attack:public Ability_Active
{
public:
	Attack();
	int initialize_ability(Unit &, Unit &, int);
private:
};

class Hero: public Unit
{
public:
	void show();
private:
	// Specific PC attributes
	char alignment; //								Reflects alignment of a character
	char level; //									Level of a character
	int experience; //								Amount of XP gained
	char influence; //								Reflects the influence of the character on others

	vector <Item> loot; //							Stores hordes of loot
};

class Party
{
public:
	void modify_gold(int); //						Adds argument to current amount of gold
	vector <Hero> creatures;//						List of heroes in the party
private:	
	double alignment; //							Average alignment of a party
	int gold; //									Treasures of a party

	int value; //									It's for the game balance, I'm serious!
};

class Opponents
{
public:
	vector <Unit> creatures;//						List of enemies in the group
private:

	int gold; //									Amount of gold reward
	int experience; //								Amount of experience reward
	vector <Item> loot; //							Stores hordes of loot reward

	int value; //									It's for the game balance, I'm serious!
};

class Battle
{
public:
	Battle(Party&, Opponents&);
	int process_situation(); //						Check whether someone is dead and add units to battle queue if there are none
							 //						Returns 0 if party is dead, 2 if party wins, 1 if none of this
	int process_unit(); //							Processing top of battle queue while it has action points
							 //						Returns the code of special event, 0 - nothing special happened
	int go(); //									Is used to process battle, main method. 
			//										Returns the result of battle: 1 - party won, 0 - party died, 2 - party fled
private:
	Party *party_in_battle; //						Pointer to a party of adventurers
	Opponents *opponents_in_battle; //				Pointer to a party of enemies
	queue <Unit*> battle_queue; //					Queue of acting units
	vector <Ability_Active&> *current_abilities;//	Abilities of a selected unit	
};

#endif 