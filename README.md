# Enemy Shooter  
Author: Suhaib Anwaar 

![Enemy Shooter Banner](https://github.com/SuhaibAnwaar/Enemy-Shooter/blob/0d2932f28f38ccbec52766f494bbb4c081648c18/Enemy%20Shooter%202.png)

## 🎮 Play the Game
**[► Play on itch.io](https://suhaib-a.itch.io/enemyshooter)**

Overview
- Complete action game built in Unreal Engine using C++.
- Real-time AI systems with priority-based decision making, multi-sensory perception (sight + sound), level progression, parkour movement, and a multi-ammo weapon system.
- Objective: collect keys to open doors, survive increasingly difficult levels filled with enemies that patrol, detect, chase, and shoot the player. Use parkour and weapons (standard, ice, fire) to survive and reach the level end.

Demo flow
1. Start a level.
2. Explore and use parkour moves to traverse the environment.
3. Locate and collect all required keys.
4. Enemies patrol until they detect the player by sight or sound — then they chase and shoot.
5. Defeat all enemies and reach the exit door to progress.
6. If the player dies, respawn at the most recent checkpoint.

Core Features
- AI perception and decision making
  - Sight and hearing sensors for enemies.
  - Priority-based decision system: e.g., investigate sound > chase visible player > return to patrol.
  - Enemies patrol by default, chase and shoot when they detect the player.
- Multi-ammo weapon system
  - Standard ammo: normal projectile, basic damage.
  - Ice ammo: projectile that damages and slows enemies on hit.
  - Fire ammo: projectile that deals damage over time (burn).
  - Ammo switching and pickup ammo
- Level progression and objectives
  - Multiple levels, each with keys required to unlock the level door.
  - Checkpoints per level so players do not restart from the beginning after death.
  - Movement used as both exploratory and evasive mechanic during combat.
- Health & win/lose conditions
  - Player health bar; enemies deal damage via projectiles.
  - Win by collecting keys and reaching level exit; lose when health reaches zero.

Technical Summary
- Engine: Unreal Engine (C++ project).
- AI: Custom perception components (sight + sound), behavior selection using a priority decision system
- Weapons: C++ projectile classes for each ammo type with distinct damage/effect logic.

## 📸 Screenshots

![Gameplay 1](https://github.com/SuhaibAnwaar/Enemy-Shooter/blob/0d2932f28f38ccbec52766f494bbb4c081648c18/Enemy%20Shooter%203.png)

![Gameplay 2](https://github.com/SuhaibAnwaar/Enemy-Shooter/blob/0d2932f28f38ccbec52766f494bbb4c081648c18/Enemy%20Shooter%204.png)

![Gameplay 3](https://github.com/SuhaibAnwaar/Enemy-Shooter/blob/0d2932f28f38ccbec52766f494bbb4c081648c18/Enemy%20Shooter%205.png)

Future Work & Improvements (suggestions)
- Weapons & Combat
  - Add more ammo types (explosive, EMP, stun), weapon upgrades, and alternate-firing modes.
  - Implement weapon mods (piercing, AoE, homing) and a pickup/upgrade economy.
- AI & Behavior
  - Replace or augment priority system with Behavior Trees + Blackboard for richer tactics.
  - Add squad-based AI: flanking, cover usage, suppressive fire, and coordinated assaults.
  - Add memory/state persistence (last known player position, search patterns).
- Level & Gameplay
  - Procedural or semi-procedural level elements for replayability.
  - More complex key/door puzzles and optional objectives.
  - More parkour moves: grappling, zip-lines, sliding, and advanced traversal combos.
- UX & Persistence
  - Full save/load system with persistent inventory and unlocked upgrades.
  - Dynamic difficulty scaling based on player performance.
- Multiplayer & Co-op
  - Add local/online cooperative play or PvP modes
- Analytics & Telemetry
  - Integrate gameplay analytics to track balance, choke points, and AI engagement patterns.
