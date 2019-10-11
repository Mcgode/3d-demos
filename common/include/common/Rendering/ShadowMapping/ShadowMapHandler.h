//
// Created by max on 11/10/2019.
//

#ifndef INC_3D_DEMOS_SHADOW_MAP_HANDLER_H
#define INC_3D_DEMOS_SHADOW_MAP_HANDLER_H

#include <common/Window.h>
#include <common/3DObjects/HierarchicalNode.h>
#include <common/Light/DirectionalLight.h>
#include <common/Light/PointLight.h>
#include <map>
#include "DirectionalShadowMapRendering.h"

class ShadowMapHandler {

public:

    void renderShadowMaps(Window *window, std::vector<HierarchicalNode *> *rootNodes,
                          DirectionalLight *directionalLight, std::vector<PointLight *> *pointLights,
                          AdditionalParameters *params);

    GLuint getDirectionalShadowMap(DirectionalLight *directionalLight);

private:

    std::map<DirectionalLight *, DirectionalShadowMapRendering *> directionalShadowMaps;

};


#endif //INC_3D_DEMOS_SHADOW_MAP_HANDLER_H
