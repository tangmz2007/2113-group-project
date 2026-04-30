2113-group-project
main_final (Final Executable)
 |-- main_final.o
 |    |-- main_final.cpp
 |    |-- Dependencies: battle.h, map.h, player.h, savesystem.h, items.h, types.h
 |
 |-- battle.o (Battle System)
 |    |-- battle.cpp
 |    |-- Dependencies: battle.h, items.h, skills.h, types.h, monster.h
 |
 |-- map.o (Map System)
 |    |-- map.cpp
 |    |-- Dependencies: map.h, types.h
 |
 |-- monster.o (Monster System)
 |    |-- monster.cpp
 |    |-- Dependencies: monster.h, types.h
 |
 |-- skills.o (Skill System)
 |    |-- skills.cpp
 |    |-- Dependencies: skills.h, types.h, player.h
 |
 |-- savesystem.o (Save System)
 |    |-- savesystem.cpp
 |    |-- Dependencies: savesystem.h, map.h, player.h
 |
 |-- player.o (Player System)
 |    |-- player.cpp
 |    |-- Dependencies: player.h, skills.h, items.h
 |
 |-- items.o (Item System)
 |    |-- items.cpp
 |    |-- Dependencies: items.h, types.h
 |
 |-- types.o (Basic Type Definitions)
      |-- types.cpp
      |-- Dependencies: types.h
Introduction

This game is written in C++ with map generation, combat, player progression, and save/load support. The project is organized as a modular console application in which the player advances through a branching map, enters battles and special rooms, collects resources, and attempts to defeat a final boss.

Overview
* This project implements an experience in command-line environment. Its core gameplay loop combines exploration, combat, progression, and persistence, with the main program coordinating player creation, map traversal, room handling, victory conditions, defeat handling, and run continuation.
* The game uses a node-based map made of multiple layers and branching routes. As the player moves forward, each node may represent a monster fight, shop, campfire, chest and random ones, creating a structured but replayable run each time a new map is generated.

Main Features
* Procedurally generated branching map with visible paths and partially hidden room identities.
* Turn-based combat with energy, block, status effects, monster intents, and skill usage.
* Persistent player growth through experience, gold, skills, relics, potions, and room-based rewards.
* Special room systems including shops, campfires, chests, and random events.
* Save/load support for active runs, plus a same-map restart feature after defeat.

Gameplay Structure
* A run begins from the start node and proceeds layer by layer across a fixed-shape map whose room contents are assigned through randomized generation rules. Early layers avoid some high-risk room types, while later layers can include elite monsters and a final boss, which helps create a sense of progression in difficulty and reward.
​* At each step, the player resolves the current room before choosing among connected nodes in the next layer. This structure gives the player meaningful control over routing, risk management, and resource planning instead of relying only on random outcomes.

Combat System
* Combat is implemented as a turn-based system where the player spends energy to use learned skills, gains block to reduce incoming damage, and reacts to enemy intent shown before the enemy turn. The battle logic also supports poison, vulnerability, weakness, trapping effects, retained block, damage caps, and conditional relic triggers, which makes combat more tactical than a simple attack-exchange loop.
* Enemies are defined from reusable monster templates that specify their health, attack, block values, rewards, and action pools. Normal enemies, elites, and the boss are generated from different pools, giving the game a scalable combat structure across the run.
​
Progression Systems
* The player develops during a run through several interconnected systems: experience and leveling, skill learning and forgetting, potion inventory, relic collection, and gold spending. Campfires, chests, shops, and events all contribute to progression by offering healing, training, item rewards, or strategic trade-offs.
* This design encourages build variety. Some skills emphasize direct damage, some improve defense or healing, and others manipulate tempo through energy gain or status application, while relics add passive bonuses that can significantly change decision-making in and out of battle.

Project Structure
* The main loop is handled separately from combat, map generation, monster definitions, player management, items, skills, and save logic.

Design Strengths
* One notable strength of the project is its modular architecture. Shared enums and helpers are separated from gameplay systems, and major subsystems such as map generation, battles, and persistence are implemented in their own files, which improves readability and supports future expansion.
* Another strength is the balance between randomness and player agency. Randomized maps, enemy actions, and item rewards create replayability, while branching routes, room choices, skill management, and resource planning ensure that player decisions remain central to success.
