#include "config.h"
#include "noise.h"
#include "world.h"
#include "world.h"
#include "stdlib.h"

//populate chunk. P and Q are the unscaled xy coords for the chunk
//func takes(x,y,z,w,*arg)
void create_world(int p, int q, world_func func, void *arg) {

    //it looks like padding is superstitiously added so there's always a little overlap
    int pad = 1;
    for (int dx = -pad; dx < CHUNK_SIZE + pad; dx++) {
        for (int dz = -pad; dz < CHUNK_SIZE + pad; dz++) {
            //in this iter, dx and dz are the xy coords, relative to the chunk.

            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
            float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
            int mh = g * 32 + 16;
            int h = f * mh;
            int block_type = GRASS;
            int t = 12;
            if (h <= t) {
                h = t;
                block_type = 2;
            }
            // sand and grass terrain
            //fill from y=0 up to the level you're standing on.
            for (int y = 0; y < h; y++) {
                func(x, y, z, block_type * flag, arg);
            }
            if (block_type == GRASS) {
                if (SHOW_PLANTS) {
                    // grass
                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                        func(x, h, z, 17 * flag, arg);
                    }
                    // flowers
                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                        int block_type = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                        func(x, h, z, block_type * flag, arg);
                    }
                }
                // trees
                //don't draw trees on chunk boundries
                int safe_to_grow_trees =  !(dx - 4 < 0 || dz - 4 < 0 ||
                    dx + 4 >= CHUNK_SIZE || dz + 4 >= CHUNK_SIZE);

                if (SHOW_TREES && safe_to_grow_trees && simplex2(x, z, 6, 0.5, 2) > 0.84) {
                    gen_cone(x, h, z, func, arg);
                }
            }
            // clouds
            if (SHOW_CLOUDS) {
                for (int y = 64; y < 72; y++) {
                    if (simplex3(
                        x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        func(x, y, z, 16 * flag, arg);
                    }
                }
            }
        }
    }
}

//tree generation function. Calls func with args(x,y,z, block_id, arg)
void gen_tree(int x, int h, int z, world_func func, void *arg){
    for (int y = h + 3; y < h + 8; y++) {
        for (int ox = -3; ox <= 3; ox++) {
            for (int oz = -3; oz <= 3; oz++) {
                int d = (ox * ox) + (oz * oz) +
                    (y - (h + 4)) * (y - (h + 4));
                if (d < 11) {
                    func(x + ox, y, z + oz, 15, arg);
                }
            }
        }
    }
    for (int y = h; y < h + 7; y++) {
        func(x, y, z, 5, arg);
    }
}

void gen_cone(int x, int h, int z, world_func func, void *arg){
    float randomVal = simplex2(x, z, 6, 0.5, 2);
    int baseSize = randomVal * 3 + 3;
    for(int y = 0; y <= baseSize*2 ; y++){
        for(int ox = -1* baseSize; ox < baseSize; ox++){
            for(int oz = -1* baseSize; oz < baseSize; oz++){
                if(abs(ox) + abs(oz) < baseSize - y/2){
                    //inside of cone
                    //func(x + ox, h+y, z + oz, 0, arg);
                    if(y == 0){
                        //cone floor
                        func(x + ox, h+y - 1, z + oz, 2, arg);
                    }
                }
                else if(abs(ox) + abs(oz) == baseSize - y/2){
                    //cone wall
                    func(x + ox, h+y, z + oz, 3, arg);
                }
            }
        }
    }
}
