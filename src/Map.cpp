
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

#include "BaseNPC.h"
#include "BaseEnemy.h"
#include "BaseCompanion.h"
#include "Event.h"

#include <math.h>
#include "CombatManager.h"
#include "DialogueManager.h"
#include "EventManager.h"

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded) {

        DrawLayers(false);

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            //L09 TODO 7: Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
                
                Vector2D camPosTile = GetCameraPositionInTiles();
                Vector2D limits = GetCameraLimitsInTiles(camPosTile);
                
                for (int i = camPosTile.getX(); i < limits.getX(); i++) {
                    for (int j = camPosTile.getY(); j < limits.getY(); j++) {

                        // L07 TODO 9: Complete the draw function

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);

                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                           
                            //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            
                            if (tileSet != nullptr) {
                                
                                int drawGid = gid;

                                // If this gid is animated, compute current frame based on global timer
                                auto itAnim = tileSet->animations.find(gid);
                                if (itAnim != tileSet->animations.end() && !itAnim->second.empty()) {
                                    const auto& frames = itAnim->second;
                                    // sum durations
                                    int totalMs = 0;
                                    for (const auto& f : frames) totalMs += f.second;
                                    if (totalMs > 0) {
                                        double t = std::fmod(animationTimerMs, (double)totalMs);
                                        int acc = 0;
                                        for (const auto& f : frames) {
                                            acc += f.second;
                                            if (t < acc) {
                                                drawGid = f.first; // frame global gid
                                                break;
                                            }
                                        }
                                    }
                                }

                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, (int)mapCoord.getX(), (int)mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX((float)(x * mapData.tileWidth));
    ret.setY((float)(y * mapData.tileHeight));

    return ret;
}

Vector2D Map::WorldToMap(int x, int y)
{
    Vector2D ret(0, 0);
    ret.setX((float)(x / mapData.tileWidth));
    ret.setY((float)(y / mapData.tileHeight));

    return ret;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
        ret = true;
    }

    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    if (gid == 0) return nullptr;

    TileSet* set = nullptr;
    for (const auto& tileset : mapData.tilesets) {
        set = tileset;
        if (gid >= tileset->firstGid && gid < tileset->firstGid + tileset->tileCount) {
            break;
        }
    }
    return set;
}

// L10: TODO 7: Create a method to get the map size in pixels
Vector2D Map::GetMapSizeInPixels()
{
    Vector2D sizeInPixels;
    sizeInPixels.setX((float)(mapData.width * mapData.tileWidth));
    sizeInPixels.setY((float)(mapData.height * mapData.tileHeight));
    return sizeInPixels;
}

Vector2D Map::GetMapSizeInTiles()
{
    return Vector2D((float)mapData.width, (float)mapData.height);
}

// Method to get the navigation layer from the map
MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;
        }
    }

    return nullptr;
}

//L15 TODO 2: Define a method to load entities from the map XML
void Map::LoadEntities(std::shared_ptr<Player>& player, SceneID sceneID) {
    isReloading = true;
    //Iterate the object groups
    for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {
        //Check if the object group is "Entities"
        if (objectGroupNode.attribute("name").as_string() == std::string("Entities")) {

            //Iterate the objects
            for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {

                //Get the entity type and position
                std::string entityType = objectNode.attribute("type").as_string();
                Vector2D pos = Vector2D(objectNode.attribute("x").as_float(), objectNode.attribute("y").as_float());
                float x = objectNode.attribute("x").as_float();
                float y = objectNode.attribute("y").as_float();
                int ID = objectNode.attribute("id").as_int();
                std::string entity_Name = objectNode.attribute("name").as_string();
                // Create entity based on type
                if (entityType == "Player")
                {
                    // Create Player entity
                    if (player == nullptr) {
                        player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));
                        player->position = Vector2D(pos.getX() + 32, pos.getY() + 32);
                        player->Start();
                        LOG("Player created at %f, %f.", pos.getX(), pos.getY());
                    }
                    //If the player already exists, just set its position
                    else {
                        player->SetPosition(Vector2D(pos.getX(), pos.getY()));
                        LOG("Player positioned at %f, %f.", pos.getX(), pos.getY());
                    }
                    int HP;
                    int WizardJoined = false;
                    int CorneliusJoined = false;
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();
                        if (name == "HP")
                        {
                            HP = propertyNode.attribute("value").as_int();
                            LOG("player HP: %d", player->HP);
                            /*player->HP = HP;*/
                        }
                        if (name == "magicPoints")
                        {
                            magicPoints = propertyNode.attribute("value").as_int(); //map has magicPoints
                            PendingChangesCheckAndSetter(EntityType::PLAYER, Component::MAGICPOINTS, 0, magicPoints, sceneID);
                        }
                        if (name == "WizardJoined")
                        {
                            WizardJoined = propertyNode.attribute("value").as_bool();
                            PendingChangesCheckAndSetter(EntityType::PLAYER, Component::WIZARDJOINED, 0, WizardJoined, sceneID);
                            player->WizardJoined = WizardJoined;
                        }
                        if (name == "CorneliusJoined")
                        {
                            CorneliusJoined = propertyNode.attribute("value").as_bool();
                            PendingChangesCheckAndSetter(EntityType::PLAYER, Component::CORNELIUSJOINED, 0, CorneliusJoined, sceneID);
                            player->CorneliusJoined = CorneliusJoined;
                        }
                    }
                }

                if (entityType == "NPC")
                {
                    int NPC_ID = 0;
                    const char* texturePath = nullptr;
                    bool active = false;
                    int currentDialogueTree = 0;

                    
                    //get NPC data
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();

                        if (name == "NPC_ID")
                            NPC_ID = propertyNode.attribute("value").as_int();

                        if (name == "active")
                            active = propertyNode.attribute("value").as_bool();

                        if (name == "texturePath")
                            texturePath = (const char*)propertyNode.attribute("value").as_string();

                        if (name == "currentDialogueTree") {
                            currentDialogueTree = propertyNode.attribute("value").as_int();
                            PendingChangesCheckAndSetter(EntityType::BASENPC, Component::DIALOGUETREE, NPC_ID, currentDialogueTree, sceneID); //if currentDialogueTree is suposed to be different (changed because
                            // of a quest and couldn't assign it because the in current map the NPC was not active (not in memory)), this fucntion takes the reference of the value and modifies it accordingly.
                        }
                    }

                    if (active)
                    {
                        std::shared_ptr<BaseNPC> npc;
                        if (Engine::GetInstance().entityManager->GetEntity(EntityType::BASENPC, ID) == nullptr)
                        {
                            npc = std::dynamic_pointer_cast<BaseNPC>(Engine::GetInstance().entityManager->CreateEntity(EntityType::BASENPC));
                        }
                        else
                        {
                            npc = std::dynamic_pointer_cast<BaseNPC>(Engine::GetInstance().entityManager->GetEntity(EntityType::BASENPC, ID));
                        }
                        npc->Init(EntityType::BASENPC, active, pos, texturePath, NPC_ID, currentDialogueTree);
                        npc->entity_ID = ID;
                        LOG("NPC -> NPC_ID: %i, entity_ID: %i, at %f, %f.", NPC_ID, ID, pos.getX(), pos.getY());
                        npc->Start();
                    }
                    else {
                        LOG("NPC inactive");
                    }
                }

                if (entityType == "Enemy") 
                {
                    int Enemy_ID = 0;
                    const char* texturePath = nullptr;
                    bool active = false;
                    int fight_ID = 0;
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();

                        if (name == "Enemy_ID")
                            Enemy_ID = propertyNode.attribute("value").as_int();

                        if (name == "active")
                            active = propertyNode.attribute("value").as_bool();

                        if (name == "texturePath")
                            texturePath = (const char*)propertyNode.attribute("value").as_string();

                        if (name == "fight_ID")
                            fight_ID = propertyNode.attribute("value").as_int();
                    }

                    if (active)
                    {
                        std::shared_ptr<BaseEnemy> enemy;
                        if (Engine::GetInstance().entityManager->GetEntity(EntityType::BASEENEMY, ID) == nullptr)
                        {
                            enemy = std::dynamic_pointer_cast<BaseEnemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::BASEENEMY));
                        }
                        else
                        {
                            enemy = std::dynamic_pointer_cast<BaseEnemy>(Engine::GetInstance().entityManager->GetEntity(EntityType::BASEENEMY, ID));
                        }
                        enemy->Init(EntityType::BASEENEMY, active, pos, texturePath, Enemy_ID, fight_ID);
                        enemy->entity_ID = ID;
                        LOG("Enemy -> Enemy_ID: %i, entity_ID: %i, at %f, %f.", Enemy_ID, ID, pos.getX(), pos.getY());
                        enemy->Start();
                    }
                    else {
                        LOG("Enemy inactive");
                    }
                }

                if (entityType == "Companion")
                {

                    int Companion_ID = 0;
                    const char* texturePath = nullptr;
                    bool active = false;
                    int Dialogue_ID = -1;

                    //get NPC data
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();

                        if (name == "Companion_ID") //player id in combatData is = Companion_ID + 1
                            Companion_ID = propertyNode.attribute("value").as_int();

                        if (name == "active")
                            active = propertyNode.attribute("value").as_bool();

                        if (name == "texturePath")
                            texturePath = (const char*)propertyNode.attribute("value").as_string();

                        if (name == "Dialogue_ID")
                            Dialogue_ID = propertyNode.attribute("value").as_int();
                    }

                    if (active)
                    {
                        std::shared_ptr<BaseCompanion> companion;
                        if (Engine::GetInstance().entityManager->GetEntity(EntityType::BASECOMPANION, ID) == nullptr)
                        {
                            companion = std::dynamic_pointer_cast<BaseCompanion>(Engine::GetInstance().entityManager->CreateEntity(EntityType::BASECOMPANION));
                        }
                        else
                        {
                            companion = std::dynamic_pointer_cast<BaseCompanion>(Engine::GetInstance().entityManager->GetEntity(EntityType::BASECOMPANION, ID));
                        }
                        companion->Init(EntityType::BASECOMPANION, active, pos, texturePath, Companion_ID, Dialogue_ID);
                        companion->entity_ID = ID;
                        LOG("Companion -> Companion_ID: %i, entity_ID: %i, at %f, %f.", Companion_ID, ID, pos.getX(), pos.getY());
                        companion->Start();
                    }
                    else {
                        LOG("Companion inactive");
                    }
                }


                if (entityType == "Event")
                {

                    int Event_ID = 0;
                    const char* texturePath = nullptr;
                    bool active = false;
                    const char* event_Name = nullptr;
                    bool activated = false;
                    //get NPC data
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();

                        if (name == "Event_ID")
                            Event_ID = propertyNode.attribute("value").as_int();

                        if (name == "active")
                            active = propertyNode.attribute("value").as_bool();

                        if (name == "texturePath")
                            texturePath = (const char*)propertyNode.attribute("value").as_string();

                        if (name == "event_Name")
                            event_Name = (const char*)propertyNode.attribute("value").as_string();

                        if (name == "activated")
                            activated = propertyNode.attribute("value").as_bool();

                    }

                    if (active /*&& !(activated && (event_Name == "Button1" || event_Name == "Button2" || event_Name == "Button3" || event_Name == "Button4"))*/)
                    {
                        std::shared_ptr<Event> event;
                        if (Engine::GetInstance().entityManager->GetEntity(EntityType::EVENT, ID) == nullptr)
                        {
                            event = std::dynamic_pointer_cast<Event>(Engine::GetInstance().entityManager->CreateEntity(EntityType::EVENT));
                        }
                        else
                        {
                            event = std::dynamic_pointer_cast<Event>(Engine::GetInstance().entityManager->GetEntity(EntityType::EVENT, ID));
                        }
                        event->Init(EntityType::EVENT, active, pos, texturePath, Event_ID, event_Name, activated);
                        event->entity_ID = ID;
                        LOG("Event -> Event ID: %i, Active: %i, at %f, %f.", Event_ID, active, pos.getX(), pos.getY());
                        event->Start();
                    }
                    else {
                        LOG("Event inactive");
                    }
                }
            }
        }
    }

    isReloading = false;
}

//L15 TODO 4: Define a method to save entities to the map XML
void Map::SaveEntities(std::shared_ptr<Player> player, SceneID sceneID) {

    //Iterate the object groups
    for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

        //Check if the object group is "Entities"
        if (objectGroupNode.attribute("name").as_string() == std::string("Entities")) {

            //Iterate the objects
            for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
                std::string entityType = objectNode.attribute("type").as_string();
                // Modify entity based on type
                int ID = objectNode.attribute("id").as_int();
                if (entityType == "Player") {
                    // Modify the Player entity values
                    Vector2D playerPos = player->GetPosition();
                    objectNode.attribute("x").set_value(playerPos.getX());
                    objectNode.attribute("y").set_value(playerPos.getY());
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();
                        if (name == "HP")
                        {
                            int HP = propertyNode.attribute("value").as_int();
                            if (HP > 0) {
                                propertyNode.attribute("value").set_value(player->HP);
                            }
                        } 
                        if (name == "magicPoints") 
                        { propertyNode.attribute("value").set_value(magicPoints); }
                        if (name == "WizardJoined") 
                        { propertyNode.attribute("value").set_value(player->WizardJoined); }
                        if (name == "CorneliusJoined") 
                        { propertyNode.attribute("value").set_value(player->CorneliusJoined); }
                    }
                }

                if (entityType == "NPC")
                {
                    /*int NPC_ID = objectNode.attribute("id").as_int();*/
                    std::shared_ptr<BaseNPC> npc = std::dynamic_pointer_cast<BaseNPC>(Engine::GetInstance().entityManager->GetEntity_Map(ID, EntityType::BASENPC));
                    if (npc) {
                        const char* texturePath = npc->texturePath;
                        bool active = npc->active;
                        int currentDialogueTree = npc->currentDialogueTree;

                        //get NPC data
                        for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                            propertyNode;
                            propertyNode = propertyNode.next_sibling("property"))
                        {
                            std::string name = propertyNode.attribute("name").as_string();
                            /*if (name == "NPC_ID")
                                propertyNode.attribute("value").set_value(NPC_ID);*/

                            if (name == "active")
                                propertyNode.attribute("value").set_value(active);

                            if (name == "texturePath")
                                propertyNode.attribute("value").set_value(texturePath);

                            if (name == "currentDialogueTree")
                                propertyNode.attribute("value").set_value(currentDialogueTree);
                        }
                    }
                }

                if (entityType == "Enemy")
                {
                    /*int ENEMY_ID = objectNode.attribute("id").as_int();*/
                    std::shared_ptr<BaseEnemy> enemy = std::dynamic_pointer_cast<BaseEnemy>(Engine::GetInstance().entityManager->GetEntity_Map(ID, EntityType::BASEENEMY));
                    if (enemy) { //if enemy exists (the ones that are dead, their data is saved when they are killed in combat (Combat Manager))
                        const char* texturePath = enemy->texturePath;
                        bool active = enemy->active;
                        /*int enemy_ID = -1;*/
                        //get NPC data
                        for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                            propertyNode;
                            propertyNode = propertyNode.next_sibling("property"))
                        {
                            std::string name = propertyNode.attribute("name").as_string();

                            /*if (name == "ENEMY_ID")
                                propertyNode.attribute("value").as_int();*/

                            if (name == "active")
                            {
                                propertyNode.attribute("value").set_value(active);
                                LOG("Enemy with Enemy_ID: %i has Active: %i", enemy->ID, active);
                            }
                            if (name == "texturePath")
                                propertyNode.attribute("value").set_value(texturePath);
                        }
                    }
                }

                if (entityType == "Companion")
                {
                    /*int ENEMY_ID = objectNode.attribute("id").as_int();*/
                    std::shared_ptr<BaseCompanion> companion = std::dynamic_pointer_cast<BaseCompanion>(Engine::GetInstance().entityManager->GetEntity_Map(ID, EntityType::BASECOMPANION));
                    if (companion) {
                        const char* texturePath = companion->texturePath;
                        bool active;
                        if (std::strcmp(companion->name.c_str(), "Wizard") == 0) { active = player->WizardJoined; }
                        if (std::strcmp(companion->name.c_str(), "Cornelius") == 0) { active = player->CorneliusJoined; }
                        /*int companion_ID = -1;*/
                        //get NPC data
                        for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                            propertyNode;
                            propertyNode = propertyNode.next_sibling("property"))
                        {
                            std::string name = propertyNode.attribute("name").as_string();

                            /*if (name == "ENEMY_ID")
                                propertyNode.attribute("value").as_int();*/

                            if (name == "active")
                                propertyNode.attribute("value").set_value(active);

                            if (name == "texturePath")
                                propertyNode.attribute("value").set_value(texturePath);
                        }
                    }
                }


                if (entityType == "Event")
                {
                    /*int ENEMY_ID = objectNode.attribute("id").as_int();*/
                    std::shared_ptr<Event> event = std::dynamic_pointer_cast<Event>(Engine::GetInstance().entityManager->GetEntity_Map(ID, EntityType::EVENT));
                    if (event) {
                        bool active = event->active;
                        bool activated = event->activated;
                        /*int companion_ID = -1;*/
                        //get Event data
                        for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                            propertyNode;
                            propertyNode = propertyNode.next_sibling("property"))
                        {
                            std::string name = propertyNode.attribute("name").as_string();

                            if (name == "active")
                                propertyNode.attribute("value").set_value(active);

                            if (name == "activated")
                                propertyNode.attribute("value").set_value(activated);

                            /*LOG("Map: Event saved. Active: %i, Activated: %i", active, activated);*/
                        }
                    }
                }
            }
        }

        //Important: save the modifications to the XML 
        std::string mapPathName = mapPath + mapFileName;
        mapFileXML.save_file(mapPathName.c_str());

    }
}

MapLayer* Map::GetLayer(const std::string& name) const
{
    for (auto& layer : mapData.layers)
    {
        if (layer->name == name)
            return layer;
    }
    return nullptr;
}

void Map::DrawLayers(bool aboveEntities)
{
    if (!mapLoaded) return;

    // Normalizamos la b�squeda de propiedades: si layer no tiene "AboveEntities", tratamos como false.
    const unsigned int FLIPPED_MASK = 0xE0000000u;

    for (const auto& mapLayer : mapData.layers) {
        // Si la capa no tiene Draw=true, no la dibujamos
        auto drawProp = mapLayer->properties.GetProperty("Draw");
        if (drawProp == nullptr || drawProp->value == false) continue;

        // Propiedad que marca si la capa va por encima de las entidades
        auto aboveProp = mapLayer->properties.GetProperty("AboveEntities");
        bool layerAbove = (aboveProp != nullptr) ? aboveProp->value : false;

        if (layerAbove != aboveEntities) continue; // s�lo dibujamos las que correspondan al pase

        for (int i = 0; i < mapData.width; ++i) {
            for (int j = 0; j < mapData.height; ++j) {
                if (i >= mapLayer->width || j >= mapLayer->height) continue;
                
                unsigned int gidWithFlags = mapLayer->Get(i, j);
                unsigned int gid = gidWithFlags & ~FLIPPED_MASK; // quitar flags de flip
                if (gid == 0) continue;

                TileSet* tileSet = GetTilesetFromTileId((int)gid);
                if (!tileSet) continue;

                int drawGid = (int)gid;

                // animaciones si existen
                auto itAnim = tileSet->animations.find(drawGid);
                if (itAnim != tileSet->animations.end() && !itAnim->second.empty()) {
                    const auto& frames = itAnim->second;
                    int totalMs = 0;
                    for (const auto& f : frames) totalMs += f.second;
                    if (totalMs > 0) {
                        double t = std::fmod(animationTimerMs, static_cast<double>(totalMs));
                        int acc = 0;
                        for (const auto& f : frames) {
                            acc += f.second;
                            if (t < acc) {
                                drawGid = f.first;
                                break;
                            }
                        }
                    }
                }

                SDL_Rect tileRect = tileSet->GetRect(static_cast<unsigned int>(drawGid));
                Vector2D mapCoord = MapToWorld(i, j);
                Engine::GetInstance().render->DrawTexture(tileSet->texture,
                    (int)mapCoord.getX(),
                    (int)mapCoord.getY(),
                    &tileRect);
            }
        }
    }
}

Vector2D Map::GetCameraPositionInTiles() {

    // Gets the camera position in world space. Moving the camera right means drawing the world shifted left. 
    // Multiplying by -1 converts render offset actual world - space camera position
    Vector2D camPos = Vector2D(Engine::GetInstance().render->camera.x * -1, Engine::GetInstance().render->camera.y * -1);
    if (camPos.getX() < 0) camPos.setX(0);
    if (camPos.getY() < 0) camPos.setY(0);

    // Converts the camera position to map tile coordinates
    Vector2D camPosTile = WorldToMap(camPos.getX(), camPos.getY());

    return camPosTile;
}

// L19 TODO 2: Calculate Camera limits in Tiles
Vector2D Map::GetCameraLimitsInTiles(Vector2D camPosTile) {

    // Gets the camera size in world space and converts it to map tile coordinates
    Vector2D camSize = Vector2D(Engine::GetInstance().render->camera.w, Engine::GetInstance().render->camera.h);
    Vector2D camSizeTile = WorldToMap(camSize.getX(), camSize.getY());

    // Computes the tile range to draw
    Vector2D limits = Vector2D(camPosTile.getX() + camSizeTile.getX(), camPosTile.getY() + camSizeTile.getY());
    if (limits.getX() > mapData.width) limits.setX(mapData.width);
    if (limits.getY() > mapData.height) limits.setY(mapData.height);

    return limits;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    // L07 TODO 2: clean up all layer data
    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

	// Clean up collider list
    for (const auto& collider : colliderList) {
		Engine::GetInstance().physics->DeletePhysBody(collider);
    }
	colliderList.clear();

    //Cleanup teleports
    teleportZones.clear();
    autoSaves.clear();
    /*events.clear();*/

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    //L15 TODO 2: make mapFileXML an attribute of the Map class
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {

        // L06: TODO 3: Implement LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Iterate over all the tiles and assign the values in the data array
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }



        // L08 TODO 3: Create colliders
        // L08 TODO 7: Assign collider type
        // Later you can create a function here to load and create the colliders from the map
        //Iterate the layer and create colliders
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset");
            tilesetNode;
            tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            pugi::xml_node tilesetDef = tilesetNode;
            // Soporte .tsx externo
            std::string source = tilesetNode.attribute("source").as_string();
            pugi::xml_document tsxDoc;
            std::string tilesetBasePath;
            if (!source.empty())
            {
                std::string tsxPath = mapPath + source;
                pugi::xml_parse_result r = tsxDoc.load_file(tsxPath.c_str());
                if (r) {
                    tilesetDef = tsxDoc.child("tileset");
                    // tileset image path inside tsx is relative to the tsx file.
                    // compute base path from tsxPath
                    size_t pos = tsxPath.find_last_of("/\\");
                    tilesetBasePath = (pos != std::string::npos) ? tsxPath.substr(0, pos + 1) : mapPath;
                }
                else {
                    LOG("Could not load external TSX: %s (%s)", tsxPath.c_str(), r.description());
                    // fallback: continue using inline definition (tilesetDef remains tilesetNode)
                }
            }
            else {
                tilesetBasePath = mapPath;
            }

            //Iterate the Tileset
            for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
            {
                //Load Tileset attributes
                TileSet* tileSet = new TileSet();
                tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
                tileSet->name = tilesetNode.attribute("name").as_string();
                tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
                tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
                tileSet->spacing = tilesetNode.attribute("spacing").as_int();
                tileSet->margin = tilesetNode.attribute("margin").as_int();
                tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
                tileSet->columns = tilesetNode.attribute("columns").as_int();

                //Load the tileset image
                std::string imgName = tilesetNode.child("image").attribute("source").as_string();
                std::string fullImgPath = tilesetBasePath + imgName;
                tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

                // If columns or tileCount are missing, try to deduce them from the texture size
                if (tileSet->texture != nullptr) {
                    int texW = 0, texH = 0;
                    Engine::GetInstance().textures->GetSize(tileSet->texture, texW, texH);
                    if (tileSet->columns <= 0 && tileSet->tileWidth > 0) {
                        tileSet->columns = texW / tileSet->tileWidth;
                        if (tileSet->columns <= 0) tileSet->columns = 1;
                    }
                    if (tileSet->tileCount <= 0 && tileSet->columns > 0 && tileSet->tileHeight > 0) {
                        int rows = texH / tileSet->tileHeight;
                        tileSet->tileCount = tileSet->columns * (rows > 0 ? rows : 1);
                    }
                }

                // Parse animated tiles inside this tileset definition
                for (pugi::xml_node tileNode = tilesetDef.child("tile"); tileNode; tileNode = tileNode.next_sibling("tile"))
                {
                    pugi::xml_node animNode = tileNode.child("animation");
                    if (!animNode) continue;

                    int localBaseId = tileNode.attribute("id").as_int();
                    int globalBaseGid = tileSet->firstGid + localBaseId;

                    std::vector<std::pair<int, int>> frames; // pair<frameGlobalGid, durationMs>
                    for (pugi::xml_node frame = animNode.child("frame"); frame; frame = frame.next_sibling("frame"))
                    {
                        int frameTileId = frame.attribute("tileid").as_int();
                        int duration = frame.attribute("duration").as_int(100);
                        int frameGlobalGid = tileSet->firstGid + frameTileId;
                        frames.emplace_back(frameGlobalGid, duration);
                    }

                    if (!frames.empty()) {
                        tileSet->animations.emplace(globalBaseGid, std::move(frames));
                    }
                }

                mapData.tilesets.push_back(tileSet);
            }

            //-------------------------------Colliders, Teleport-------------------------------------
            for (pugi::xml_node objectGroup = mapFileXML.child("map").child("objectgroup"); objectGroup;objectGroup = objectGroup.next_sibling("objectgroup"))
            {
                std::string groupName = objectGroup.attribute("name").as_string();

                //--------------------------------------Colliders Start-----------------------------------------------
                if (groupName == "Collision")
                {
                    for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
                    {
                        float x = object.attribute("x").as_float();
                        float y = object.attribute("y").as_float();
                        float width = object.attribute("width").as_float();
                        float height = object.attribute("height").as_float();

                        // Validate dimensions before using them
                        if (std::isfinite(width) && std::isfinite(height) && width > 0.0f && height > 0.0f)
                        {
                            float centerX = x + width / 2.0f;
                            float centerY = y + height / 2.0f;

                            PhysBody* collider = Engine::GetInstance().physics.get()->CreateRectangle(
                                centerX, centerY, width, height, STATIC);

                            collider->ctype = ColliderType::PLATFORM;
                            colliderList.push_back(collider);
                        }
                        else
                        {
                            std::cerr << "Invalid collider dimensions: width=" << width << ", height=" << height << std::endl;
                        }
                    }
                }
                //--------------------------------------------Colliders End--------------------------------------------
                //-------------------------------------------Teleports Start-------------------------------------------
                
                else if (groupName == "Teleport")
                {
                    for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
                    {
                        TeleportZone zone;
                        zone.x = object.attribute("x").as_float();
                        zone.y = object.attribute("y").as_float();
                        zone.width = object.attribute("width").as_float();
                        zone.height = object.attribute("height").as_float();
                        
                        // Leer propiedades custom
                        for (pugi::xml_node prop = object.child("properties").child("property"); prop; prop = prop.next_sibling("property"))
                        {
                            std::string propName = prop.attribute("name").as_string();

                            if (propName == "targetMap")
                                zone.targetMap = prop.attribute("value").as_string();
                        }

                        teleportZones.push_back(zone);
                        LOG("TeleportZone loaded -> map:%s at (%.0f,%.0f)", zone.targetMap.c_str());
                    }
                }
                
                //--------------------------------------------Teleports End----------------------------------------------
                //-------------------------------------------Auto Saves Start--------------------------------------------
                else if (groupName == "Checkpoints")
                {
                    for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
                    {
                        AutoSave zone;
                        zone.x = object.attribute("x").as_float();
                        zone.y = object.attribute("y").as_float();
                        zone.width = object.attribute("width").as_float();
                        zone.height = object.attribute("height").as_float();

                        autoSaves.push_back(zone);
                        LOG("AutoSave loaded");
                    }
                }
                //--------------------------------------------Auto Saves End---------------------------------------------
                ////---------------------------------------------Events Start----------------------------------------------
                //if (groupName == "Events")
                //{
                //    for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
                //    {
                //        Event event;
                //        event.x = object.attribute("x").as_float();
                //        event.y = object.attribute("y").as_float();
                //        event.width = object.attribute("width").as_float();
                //        event.height = object.attribute("height").as_float();
                //        event.name = object.attribute("Name").as_string();

                //        events.push_back(event);
                //    }
                //}
                ////----------------------------------------------Events End-----------------------------------------------
                ret = true;

                // L06: TODO 5: LOG all the data loaded iterate all tilesetsand LOG everything
                if (ret == true)
                {
                    LOG("Successfully parsed map XML file :%s", fileName.c_str());
                    LOG("width : %d height : %d", mapData.width, mapData.height);
                    LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);
                    LOG("Tilesets----");

                    //iterate the tilesets
                    for (const auto& tileset : mapData.tilesets) {
                        LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                        LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                        LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
                    }

                    LOG("Layers----");

                    for (const auto& layer : mapData.layers) {
                        LOG("id : %d name : %s", layer->id, layer->name.c_str());
                        LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
                    }
                }
                else {
                    LOG("Error while parsing map file: %s", mapPathName.c_str());
                }

                //L15 TODO 2: Remove mapFileXML.reset(); we want keep a reference to the XML

            }

            mapLoaded = ret;
            return ret;
        }

        // L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions


        // L09: TODO 6: Load a group of properties from a node and fill a list with it
    }

   /* if(Engine::GetInstance().scene->GetCurrentScene() == SceneID::LEVEL4) { Engine::GetInstance().eventManager->GetEvents(); }*/
}

void Map::UpdateEnemiesData()
{
    bool active = false;
    int ID;
    auto ids = Engine::GetInstance().combatManager->enemies_to_destroy;
    for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

        if (objectGroupNode.attribute("name").as_string() == std::string("Entities")) {

            for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
                bool change = false;
                std::string entityType = objectNode.attribute("type").as_string();
                
                if (entityType == "Enemy")
                {
                    for (pugi::xml_node propertyNode = objectNode.child("properties").child("property");
                        propertyNode;
                        propertyNode = propertyNode.next_sibling("property"))
                    {
                        std::string name = propertyNode.attribute("name").as_string();
                        if (name == "Enemy_ID")
                        {
                            ID = propertyNode.attribute("value").as_int();
                        }
                        if (name == "active")
                        {
                            for (auto i : ids) { if (ID == i) change = true; }
                            if (change) { 
                                propertyNode.attribute("value").set_value(active);  //change active value
                                LOG("Enemy ID: %i marked %i (0 dead, 1 alive)", ID, active);
                                std::string mapPathName = mapPath + mapFileName; //save file
                                mapFileXML.save_file(mapPathName.c_str());
                            };
                        }
                    }
                    }
                }
            }
        }
}

void Map::PendingChangesCheckAndSetter(EntityType type, Component component, int ID, int& current_value, SceneID entity_Scene)
{
    for (auto change : pendingChanges)
    {
        if (change.ID == ID && change.entityType == EntityType::BASENPC && (change.entity_Scene == entity_Scene))
        {
            switch (component)
            {
            case Component::DIALOGUETREE:
                if (change.inc)
                {
                    current_value++;
                }
                else
                { 
                    current_value = change.new_value;
                }
                
                LOG("Map: CurrentDialogueTree of NPC ID: %i was changed (it was pending)", ID);
                pendingChanges.erase(
                    std::remove(pendingChanges.begin(), pendingChanges.end(), change),
                    pendingChanges.end()
                );
                break;
            }
        }

        if (change.entityType == EntityType::PLAYER)
        {
            switch (component)
            {
            case Component::MAGICPOINTS:
                current_value = change.new_value;
                break;

            case Component::WIZARDJOINED:
                current_value = change.new_value;
                break;

            case Component::CORNELIUSJOINED:
                current_value = change.new_value;
                break;
            
            }
        }
    }
}