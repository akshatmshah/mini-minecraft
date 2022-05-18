# Mini Minecraft - Bishop Sycamore

Milestone 1
video: https://youtu.be/qpxpv06STD4

Milestone 2
video: https://youtu.be/1fxtWB2Jf3o

Milestone 3
video: https://youtu.be/BLmY7efIDSs

## Milestone 1

### Procedural Terrain - David Zhang

I implemented this feature by using different noise functions to determine the height of the blocks in each biome. First, I used perlin
noise to determine the biome. To determine the height of a column, I mixed the my mountainValue function, grasslandValue function and the noise to determine the biome, so that the biome heights were blended across borders. The mountainValue function used perlin noise, and I set the minimum output to 150 and the max to 250. The grasslandValue function used worley noise and perlin noise, and I set the minimum output to 128 and max to 168. Then with this column height, I set the blocks for each y value according to the instructions.

Some difficulties I encountered were blending the heights of the biomes across the borders between biomes and making sure the mountains were high and steep while the grasslands were flat.

### Efficient Terrain Rendering and Chunking - Akshat Shah

To implement this feature, I first needed to create the createVBOdata() function after making chunks inherit drawable and make sure that I interleave the data (position, normal, and color). First, I simply iterated over the entire chunk and obtained the current block. Then if this block wasn't empty, I would get all blocks in each of its directions. After iterating over these directions, if it was empty (which means we show the face) I update the VBO data in an interleaved format. After this, I simply called a function to create it that would utilize a drawable method I created to get the interleaved data. Within the terrain class, I created the generate chunk function by simply checking if the players position was within 16 of a chunk (that doesn't connect) and if so I created that chunk.

Some difficulties that I had during this was making sure the correct color was displaying. I realized that I messed up my lambert function and I went back and changed the variable according to what my shader program was using. Another issue I had was that my chunks weren't rendering correctly. This was because my w value was incorrect for the currently block position so it wasn't being processed correctly. I also got the size of my interleaved data wrong so when I was setting the offset, it would read the wrong part of the interleaved data. So I went back and made everything a vec4.

### Game Engine Tick Function and Player Physics - Claire Zhang

Player movement: I implemented player movement the same way we learned in class. This part went fairly smoothly except initially I did not have diagonal movement supported when 2 keys are pressed. I implemented jumping by having a function that determined if the player is on the ground by seeing if there was a block below the player's position. If yes and space was pressed, I added a y-component to the player's current velocity.

Collisions: I detected collisions using the gridmarch method taught in class. If a player would walk into a block the next tick, I changed the distance it could travel to be right before it hit that block. One issue I ran into was the player could not fit through gaps of one block width. This was because the corners I was grid marching from were the exact corners of the player and it is impossible to line it up perfectly. I fixed this by making moving in all the points I grid marched from, effectively making the player skinnier. Another issue I had was if I jumped and hit the block on the way up, it would read it as a collision and just get stuck in midair. I fixed this by making another function that would detect a collision while not in flightmode and in the air and if such collision happened, make y velocity 0. This effectively makes the player slide down the block it tried to jump on top of until it hits the ground.

Place/Remove block: I implemented this by seeing if there was a block within 3 units of the camera and if there was, either setting it to EMPTY or setting the block in front of it to STONE. It worked on my branch but not after we merged code, so I updated the VBO data by destorying and creating the portion of the terrain.

Jumping (a strugglebus saga):
It's rly glitchy lol
Also i need to fix camera

## Milestone 2

### Cave Systems - David Zhang

I implemented this feature by using a perlin noise function that takes in a 3D vector. For each block below y=128, if the output of the noise function is below a threshold, then the block will
be empty/lava. Otherwise, the block will be stone. When computing physics, I check if the player's body is in water/lava, and if it is, I multiply the player's velocity by 2/3. If the player is not on the ground in water/lava, they still experince gravity so they sink. If they hold space, they swim up at constant velocity. To implement the tinge when a player is in water/lava, I added a post-process shader with a fragment shader for water and a separate one for lava.

Some difficulties I had were making the caves look like minecraft caves. I just had to mess with the
threshold for a block to be empty/lava and modify the input of the noise function.

### Texture and Texture Animation - Claire Zhang

Split interleaved vbo data : Modified the chunk class. split up interleaved vbos into vbo for opaque blocks and vbo for transparent block, also did the same for idx data. Each vbo stores position, normal, and uv coordinates. z coordinate of uv coordinates has a 1 or 0 which indicates if the block should be animated or not. Had a bug with this where the game would crash upon launch
Texture : made texture class from code from hw4
Animation : added texture and time into lambert shader files. Each time paintGL is called, time is incremented and used in each call to paintGL() to create the uv coordinate offsets for the water and lava to give it a moving effect. I had trouble with the animation because the texture on a square would almost drag as opposed to moving across the blocks, but this was because I had chosen the wrong indices from the texture file which was not allowing the offsets to move across the image.
Draw transparent blocks : Changed create() in chink to update the opaque/transparent VBOs depending on what block is being generated. The VBO data is also loaded with opaque before transparent so the chunks are drawn in that order as well.

### Multithreaded Terrain Generation - Akshat Shah

I implemented multithreading chunk rendering. I first got the neighboring regions of the previous position before a tick and after a tick. Then, I found regions that were in the previous position that weren't in the current position regions and destroyed those vbo datas. Then I went over those in the current position regions (which we had a zone for already) that didn't have VBO data and created VBO workers. For those that didn't have a zone, I made a blocktypeworker. For block type workers I went over the zone and instatiated a chunk and then added each of these to a vector. For the VBO worker, I created the chunk and populated the struct data. After these finished, I went over and fixed the neighboring chunks and created the VBO workers for the chunks we set blocks for in the blocktypeworker. Finally, I went over all the chunks with datas populated and created the VBO data.

Some difficulties that I had during this was that I had to move the createChunk function from the terrain.cpp to the chunk.cpp. This made it we had to change the x and z coordinate because the position was different (now relative to the chunk). Another one was updating neighbors. I created a flag to check if the chunk was created in the blocktypeworker. So, when I was created the VBOworkers at the end, I could check the neighbor chunks and if this flag was true then I could create a VBOworker for that neighbor aswell.

# Milestone 3

## Akshat Shah

### L-Systems

I implemented something similar to what we learned in class about creating the grammar. I first created the river (for grammar) and turtle class that would actually go and generate the grammar alongside the starting and ending points. I had a map that mapped the string to a function and then I had another map that would map the string to some expanded version or to itself. Then I iterated over the intial axiom and expanded the string to get the expanded grammar.
So, I would then going through the grammar and set the radius and a waterlevel and if the turtle moved forward then we would take the starting and end points of the segment to carve the river.

Some trouble I had with this was with the distance function. For some reason, I kept getting my distance function wrong so it kept looking really funky. However, I think I solved this in office hours and go it to look pretty good.

### Day and Night Cycles

I used the procedural sky code from class. I made the sun rotate about the x axis according to time and created a time zone variable that would track what time of day the sun was currently at (so it would be easy to work with this in lighting). I added a new palette for the sun (so it goes from dark to light to dark like a sun would) , and I also made it so they smoothly change as it goes from day to night. Furthermore, with this I was able to make it so the light and direction is different as time goes on in the lambert fragment shader. Currently the lighting from the sun makes all the blocks seem a little more orange and as time goes on the colors and lighting will change to reflect the day.

Some difficulties I had with this was making it so it was smooth as it transitioned between the different zones, to make a smoother transition I used the smooth function based on the advice of my group partner.

### Post Process Camera Overlay

I made it so when the player is underwater that the camera would have some distortion to it (by using sin waves because why not) . This was very similar to the fun shader project and one of my group members had a variable setup for when animation would occur (which is only for lava and water), so I simply added to it and made it distort with time. I kept playing around with it and used my previous homeworks for inspiration until I got something that I liked.

### Water Waves

For water waves, I wanted to first change the Y positions with a sin function so I played around with this again until I got something that I really liked. I thought this part was really nice to do because it was again similar to the shader fun project. Furthermore, I wanted to add a reflective lighting to I tried modified the normal based off a sin wave. Then, I used the ideas from lecture for a blinn phong reflective model.

Because I added this after adding day and night, I really want it to reflect based off the sun, so it was really hard for me to debug at some points and see if it was reflecting off the water correctly. I had a lot of trouble with the normal :(. However, I think I was able to keep at it and figure it out.

### Decorative Blocks (Mushroom, Grass, Roses, etc)

Our group wanted to add blocks like grass, mushroom, and roses but we had to modify the VBO data that was sent because the faces weren't on the edges of the blocks. Instead, these blocks were in a cross-like pattern. So, I added a new struct that shifted the XPOS and ZPOS inward by 0.5 and then pushed this new updated data. With this, we were able to get blocks like these in.

Some trouble I had with this was that I kept getting out of range errors and I was having trouble figuring this out because of the multithreading. However, I kept taking things out one by one until I isolated the problem.

## Claire Zhang

### Procedurally Generated Grass/Mountain shading:

Used fbm function in the lambert shade file to make brown tinted grass to dynamically mix into the grass as well as darker shaded areas of stone to mix into the mountains.

### Procedurally Placed Assets:

Made helper functions to create mushrooms, different trees, cacti, and buildings. Some assets such as cacti and buildings take in a height which allow for varying heights. The assets are placed procedurally depending on which biome they belong in. Took some trial and error to get trees looking like their real counterparts, but was not too bad.

### Inventory:

Made a new GUI that opens and closes when you press I. Edited removeBlock and placeBlock in Player so that they return and take in (respectively) the removed or placed block. This block is then used to update the slots and signals keeping track of the number of each block. If the number of a block is 0, the user is unable to place that block. Added images of blocks to GUI as well. Inventory keeps track of the number of each block the player has in their inventory at all times. Some difficulties was the inventory counts would sometimes go down but it wouldn't look like I was placing any blocks of the specified type. This was a problem with place block, not inventory. Also had a minor bug where some radio buttons wouldn't click, but realized this was due to overlapping elements in the GUI.

### Distance Fog:

Added unifPlayer handle in the shaderprogram and a function to set the player’s position so the fog would update with the player's movement. Using u_Player in the shader, I interoplated the distance between the player and other elements in the terrain to have the fog disappear near the player. We placed the fog shading after the procedural sky code so the fog can take the sky code into account.

### Custom Textures:

Found a new texture pack that is more detailed. Also found new individual textures and added them to the texture file and changed the shading on some textures using photoshop to achieve the biome look we intended. Akshat made "x" blocks (flowers, small mushrooms, etc)

## David Zhang

### Additional Biomes:

In total, we had 8 biomes: grasslands, mountains, drifting forests, mesa, mushroomland, tropics, tundra and desert. I used trilinear interpolation to blend between the 8 biomes. For the tropics biome, the ocean floor uses perlin noise, while the islands use worley noise. I also placed palm trees based on a random function. For the drifting forests biomes, I used a mix between noise functions to create the surface, and then used another noise function for the floating islands, where the change in height also varies based on noise. Also the jagged bottom of the floating islands uses perlin noise. Based on a random function I placed birch and oak trees and flowers. For the tundra biome, I used a mix between noise functions and I used a noise function for ice instead of water. I also placed 3 different spruce trees based on a random function. For the grassland biome, I used a mix between noise functions. I placed grass and flowers based on noise functions and I placed trees based on a random function. For the mesa biome, the plateaus are created with worley noise and LERPed with the red sand desert below. The shades of the mesa use modulus based on the height. Acacia trees and shrubs are placed based on random functions. The desert biome uses FBM and and worley noise. Cactus and shrubs were placed with random functions. For the mushroom land, a random function is used for the mushroom trees and perlin noise is used for the small mushrooms. The mountain biome uses perlin noise, with snow at the peaks that drop down at slightly random depths.

### NPCS:

I added 7 NPCS: cows, sheep, chicken, horse, pig, squid and mooshrooms. I loaded an obj file and use a half-edge mesh to store the models. Each NPC of same animal shares the same model and texture data, but are drawn differently by applying transformations to the vertex positions. Different NPCs spawn in different biomes. For example the mooshrooms only spawn in the mushroomland, while many different animals spawn in grasslands. Their movement is controlled by "decisions" where for each decision, they rotate a random amount of degrees and move in that direction for a random number of ticks. Also half the decisions are not moving at all. Different NPCs have different movements too. For example the squid can swim. All the animals can jump over 1 block. The NPCs are stored per chunk, so that only NPCs in chunks near a player are rendered and ticked. NPCs can't move into unloaded chunks and when they move into a different chunk, their data is transfered to the other chunk. For each chunk, theres a 1/16 chance to spawn in a herd of animals. A herd varies in size from 1-4. If a chunk is chosen to spawn animals, a random block in the chunk is chosen and the 1st animal spawns on that block. Any successive animals spawn in random nearby blocks.
