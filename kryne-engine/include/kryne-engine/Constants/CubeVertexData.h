/**
 * @file
 * @author Max Godefroy
 * @date 03/08/2019
 */

#ifndef INC_3D_DEMOS_CUBE_VERTEX_DATA_H
#define INC_3D_DEMOS_CUBE_VERTEX_DATA_H

const uint32_t CUBE_AMOUNT_OF_VERTICES = 24;

const float CUBE_VERTEX_POSITIONS[] = {
         1,  1,  1,     -1,  1,  1,     -1, -1,  1,      1, -1,  1,
         1, -1,  1,     -1, -1,  1,     -1, -1, -1,      1, -1, -1,
         1,  1, -1,      1,  1,  1,      1, -1,  1,      1, -1, -1,
        -1,  1, -1,      1,  1, -1,      1, -1, -1,     -1, -1, -1,
        -1,  1,  1,     -1,  1, -1,     -1, -1, -1,     -1, -1,  1,
         1,  1, -1,     -1,  1, -1,     -1,  1,  1,      1,  1,  1,
};

const float CUBE_VERTEX_UV_COORDINATES[] = {
        2. / 4., 2. / 3.,       2. / 4., 1. / 3.,       1. / 4., 2. / 3.,
        1. / 4., 1. / 3.,       1. / 4., 2. / 3.,       2. / 4., 1. / 3.,
        2. / 4., 0. / 3.,       1. / 4., 0. / 3.,       2. / 4., 1. / 3.,
        1. / 4., 1. / 3.,       2. / 4., 1. / 3.,       1. / 4., 0. / 3.,
        0. / 4., 2. / 3.,       1. / 4., 2. / 3.,       0. / 4., 1. / 3.,
        1. / 4., 1. / 3.,       0. / 4., 1. / 3.,       1. / 4., 2. / 3.,

        4. / 4., 2. / 3.,       4. / 4., 1. / 3.,       3. / 4., 2. / 3.,
        3. / 4., 1. / 3.,       3. / 4., 2. / 3.,       4. / 4., 1. / 3.,
        2. / 4., 3. / 3.,       2. / 4., 2. / 3.,       1. / 4., 3. / 3.,
        1. / 4., 2. / 3.,       1. / 4., 3. / 3.,       2. / 4., 2. / 3.,
        3. / 4., 2. / 3.,       3. / 4., 1. / 3.,       2. / 4., 2. / 3.,
        2. / 4., 1. / 3.,       2. / 4., 2. / 3.,       3. / 4., 1. / 3.,
};

const float CUBE_VERTEX_NORMALS[] = {
         0,  0,  1,     0,  0,  1,     0,  0,  1,     0,  0,  1,
         0, -1,  0,     0, -1,  0,     0, -1,  0,     0, -1,  0,
         1,  0,  0,     1,  0,  0,     1,  0,  0,     1,  0,  0,
         0,  0, -1,     0,  0, -1,     0,  0, -1,     0,  0, -1,
        -1,  0,  0,    -1,  0,  0,    -1,  0,  0,    -1,  0,  0,
         0,  1,  0,     0,  1,  0,     0,  1,  0,     0,  1,  0,
};

const uint32_t CUBE_INDEXES[] {
     0,  1,  2,      0,  2,  3,
     4,  5,  6,      4,  6,  7,
     8,  9, 10,      8, 10, 11,
    12, 13, 14,     12, 14, 15,
    16, 17, 18,     16, 18, 19,
    20, 21, 22,     20, 22, 23,
};

#endif //INC_3D_DEMOS_CUBE_VERTEX_DATA_H
