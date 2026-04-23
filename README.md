# Tomb of Mask

## Description

**Tomb of Mask** is an arcade-style sliding maze game. The player dives into an ancient tomb, sliding through corridors, dodging enemies and traps, and reaching the exit to advance through 5 challenging levels.

Inspired by the mobile game *Tomb of the Mask*, movement works by sliding in one direction until hitting a wall — there is no jumping. Navigation relies on reading the level layout and reacting to enemies in real time.

**Subject**: [Game Design and Development]  
**Degree**: [Bachelor's Degree in Game Design and Development]  
**University**: [Universitat Politècnica de Catalunya]  
**Tutors**: [De Pedro Lombao Rodrigo]  
**GitHub**: https://github.com/YanAnd-team/404-Not-Found

---

## Team Members

| Role | Name | GitHub |
|------|------|--------|
| Art / Design | WEIHAOYAN | [https://github.com/yanweihao136699-art] |
| Programming / Audio | YIXIANGLU | [https://github.com/Renshengwuqu] |

Full task breakdown is available in (https://github.com/YanAnd-team/404-Not-Found/blob/main/Tasks.pdf)

---

## How to Play

### Controls

| Key | Action |
|-----|--------|
| Arrow Right | Slide right until hitting a wall |
| Arrow Left  | Slide left until hitting a wall  |
| Arrow Up    | Slide up until hitting a wall    |
| Arrow Down  | Slide down until hitting a wall  |
| Enter       | Confirm / Start game             |

### Objective

Slide through each level and reach the **exit tile** to complete it.  
Enemies and traps will reset the player and cost a life — lose all 3 lives and it is Game Over.

### Enemies & Traps

| Element | Behaviour |
|---------|-----------|
| Ghost | Bounces back and forth between walls |
| GhostPlus | Same as Ghost but periodically turns invisible |
| Fixed Spikes | Always dangerous on contact |
| Trigger Spikes | Deploy when the player is adjacent; retract after a moment |
| Gun Trap | Fires a bullet periodically in a fixed direction |

---

## Implemented Features

### Core Gameplay
- Player sliding movement in 4 directions (stops on wall contact, snaps to grid)
- 5 tile-based levels
- 2 enemy types with independent movement and animation
- 3 trap types with distinct behaviours
- Win condition: reach the exit tile
- Lose condition: 3 lives; contact with enemies or traps resets position and costs a life
- Camera follow with world boundary limits

### UI & Audio
- Score and lives HUD during gameplay
- Background music per screen (menu music and battle music)
- Sound effects: movement, player death, level victory
- Title screen, gameplay screen, win screen, game over screen
- Initial screen with project information

---

## Demo Video

[Watch the 1-minute demo video](https://youtu.be/your-video-id)

> The video shows: initial screen → title screen → gameplay → enemy/trap contact (game over) → return to title → successful run → win screen → return to title.

---

## Project Links

- **GitHub Repository**: https://github.com/YanAnd-team/404-Not-Found

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE.md) for details.

MIT License © 2026 WEIHAOYAN & YIXIANGLU
