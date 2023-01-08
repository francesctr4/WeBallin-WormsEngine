# Kirby's Pinball Land by WeBallin

### [WeBallin Repository](https://github.com/francesctr4/WeBallin)

### Current release version: Alpha (v0.1.0)

## Team Members

_**Art & Design:** Víctor Gil_
* **GitHub:** [Ludo-pixel](https://github.com/Ludo-pixel)

_**Code & Management:** Francesc Teruel_
* **GitHub:** [francesctr4](https://github.com/francesctr4)

_**Quality Assurance (QA):** Mario García_
* **GitHub:** [mariogs5](https://github.com/mariogs5)

## Game Description

Kirby's Pinball Land is a game developed for Game Boy, released on November 28, 1993.

The game is like a simple pinball game, when we start with a cannon we must choose between three levels and each level consists of three pinball boards. We must handle Kirby as if it were a normal pinball ball; we must take him to the top of the whole and we must be skillful, as there will be enemies on the boards that will prevent us from climbing.

At the end, we will face the final boss of the level. Defeating him will give us points that are saved for the next time we play. If Kirby falls at the end of the level we can bring him back up if we press the A button, otherwise we will spend one of the three lives available.

[Link to the original game](https://youtu.be/zwrw-k74TOU)

## Differences with the Original Game

- Different enemy spawns
- Less enemies than the original game
- Different ball spawn (cannon shoots the ball in the original game)
- The original game has more than 1 level
- All the enemies give the same amount of points
- The map distribution is different from the original
- The original game is in black and white
- There's a scoreboard in the original game
- There are different game modes and boss fights in the original game
- There are win and lose conditions in the original game

## Game Controls

### KEYBOARD ###
	
	Left arrow --> Activate left kickers
	Right arrow --> Activate right kickers
	
### DEBUG KEYS ###

	F1 --> Show colliders and make available a mouse joint with the Kirby Ball (hold left click on the collider)
	F2 --> Respawn Kirby Ball on the star (keeps momentum)
	F3 --> Set score to 0
	F4 --> Set score to 99999999
	
## Exciting Stuff

- We have implemented several enemies from all the different levels of the original game, with their respective animations.
- We have also painted the game, as the original game was released on black and white.

## Development Diary

In this section we will explain the development of the game and all the problems we have faced while we were doing it and how we have solved them:

### Development

- Firstly, we found the spritesheet of Kirby's Pinball Land on the internet.
- Then, we painted all the sprites and animations.
- We programmed the map and the chain of the walls.
- Then we did the revolute joint of the kickers, and solved some bugs.
- We applied the texture to the kickers.
- We programmed the player (Kirby Ball) with its appropiate texture.
- We added some music to the map.
- We programmed the enemies and its animations and colliders.
- We modified the collisions manager, and added score by hitting enemies.
- We added the debug keys and extra functionalities.
- Finally, we modified the coeficient of restitution of the enemies and the walls.

### Bugs

- The first bug we encountered was that, after scaling the map sprite, we forgot to scalate the chain of the walls. Then, the wall colliders were significantly smaller than what they should be. To solve this, we multiplied the chain vertices by a specific factor to scale it successfully.
- The second bug we encountered was that the kickers got their center of mass at the center of the object, not at the axis of rotation, so we modified the function that creates a rectangle and moved the center of mass to the axis.
- The third bug was also with the kickers, specifically with the right kicker, because the angle of rotation was diferent due to its orientation, so we had to modify the functions that create a revolute joint adding more parameters, in order to adjust the angle of rotation.
- The last but not the least bug we found was with the coeficient of restitution of the enemies, because the ball and the enemies share the function that creates its collider. We created two different functions to distinguish between collider of Kirby and collider of enemies (the difference between them is only the coeficient of restitution).
