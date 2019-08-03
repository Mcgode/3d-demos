//
// Created by max on 03/08/2019.
//

#ifndef INC_3D_DEMOS_CUBE_VERTEX_DATA_H
#define INC_3D_DEMOS_CUBE_VERTEX_DATA_H

const int CUBE_AMOUNT_OF_VERTICES = 36;

const float CUBE_VERTEX_POSITIONS[] = {
         1, -1, -1,      1,  1, -1,      1, -1,  1,
         1,  1,  1,      1, -1,  1,      1,  1, -1,
        -1,  1, -1,     -1,  1,  1,      1,  1, -1,
         1,  1,  1,      1,  1, -1,     -1,  1,  1,
        -1, -1,  1,      1, -1,  1,     -1,  1,  1,
         1,  1,  1,     -1,  1,  1,      1, -1,  1,
        -1, -1,  1,     -1,  1,  1,     -1, -1, -1,
        -1,  1, -1,     -1, -1, -1,     -1,  1,  1,
        -1, -1, -1,      1, -1, -1,     -1, -1,  1,
         1, -1,  1,     -1, -1,  1,      1, -1, -1,
        -1, -1, -1,     -1,  1, -1,      1, -1, -1,
         1,  1, -1,      1, -1, -1,     -1,  1, -1,
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
        1, 0, 0,    1, 0, 0,    1, 0, 0,
        1, 0, 0,    1, 0, 0,    1, 0, 0,
        0, 1, 0,    0, 1, 0,    0, 1, 0,
        0, 1, 0,    0, 1, 0,    0, 1, 0,
        0, 0, 1,    0, 0, 1,    0, 0, 1,
        0, 0, 1,    0, 0, 1,    0, 0, 1,
        -1, 0, 0,    -1, 0, 0,    -1, 0, 0,
        -1, 0, 0,    -1, 0, 0,    -1, 0, 0,
        0, -1, 0,    0, -1, 0,    0, -1, 0,
        0, -1, 0,    0, -1, 0,    0, -1, 0,
        0, 0, -1,    0, 0, -1,    0, 0, -1,
        0, 0, -1,    0, 0, -1,    0, 0, -1,
};

#endif //INC_3D_DEMOS_CUBE_VERTEX_DATA_H