#pragma once

#include "Module.h"
#include <list>
#include <vector>
#include "Player.h"
#include "Scene.h"
// L09: TODO 5: Add attributes to the property structure
struct Properties
{
    struct Property
    {
        std::string name;
        bool value; //We assume that we are going to work only with bool for the moment
    };

    std::list<Property*> propertyList;

    ~Properties()
    {
        for (const auto& property : propertyList)
        {
            delete property;
        }

        propertyList.clear();
    }

    // L09: DONE 7: Method to ask for the value of a custom property
    Property* GetProperty(const char* name)
    {
        for (const auto& property : propertyList) {
            if (property->name == name) {
                return property;
            }
        }

        return nullptr;
    }

};

struct MapLayer
{
    // L07: TODO 1: Add the info to the MapLayer Struct
    int id;
    std::string name;
    int width;
    int height;
    std::vector<int> tiles;
    Properties properties;

    // L07: TODO 6: Short function to get the gid value of i,j
    unsigned int Get(int i, int j) const
    {
        return tiles[(j * width) + i];
    }
};

// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!

struct TileSet
{
    int firstGid;
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int tileCount;
    int columns;
    SDL_Texture* texture;

    // Animations: map global gid -> list of <frameGlobalGid, durationMs>
    std::unordered_map<int, std::vector<std::pair<int, int>>> animations;

    // L07: TODO 7: Implement the method that receives the gid and returns a Rect
    SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstGid;
        rect.w = tileWidth;
        rect.h = tileHeight;
        rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

        return rect;
    }

	//Implement a method to check if a gid belongs to the tileset
    /*SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0, 0, 0, 0 };

        if (columns <= 0 || tileWidth <= 0 || tileHeight <= 0)
        {
            // invalid data
            return rect;
        }

        int relativeIndex = static_cast<int>(gid) - firstGid;
        if (relativeIndex < 0 || (tileCount > 0 && relativeIndex >= tileCount))
        {
            // gid out of range
            return rect;
        }

        rect.w = tileWidth;
        rect.h = tileHeight;
        rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

        return rect;
    }*/

};

// L06: TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
    std::list<TileSet*> tilesets;

    // L07: TODO 2: Add the info to the MapLayer Struct
    std::list<MapLayer*> layers;
};

struct TeleportZone {
    float x, y, width, height;   // ï¿½rea en pï¿½xeles
    std::string targetMap;
};

struct AutoSave {
    float x, y, width, height;   // área en píxeles
};

enum class Component {
    DIALOGUETREE,
    MAGICPOINTS
};

struct PendingChange {
    EntityType entityType;
    int ID;
    Component type;
    int new_value;
    bool inc;
    SceneID entity_Scene;

    bool operator==(const PendingChange& other) const {
        return (entityType == other.entityType && ID == other.ID && type == other.type);
    }
};

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake();

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(std::string path, std::string mapFileName);

    void UpdateEnemiesData();

    // L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
    Vector2D MapToWorld(int x, int y) const;
    Vector2D WorldToMap(int x, int y);

    // L09: TODO 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L09: TODO 6: Load a group of properties 
    bool LoadProperties(pugi::xml_node& node, Properties& properties);

	// L10: TODO 7: Create a method to get the map size in pixels
	Vector2D GetMapSizeInPixels();
    Vector2D GetMapSizeInTiles();

    MapLayer* GetNavigationLayer();

    int GetTileWidth() {
        return mapData.tileWidth;
    }

    int GetTileHeight() {
        return mapData.tileHeight;
    }

    //L15 TODO 2: Define a method to load entities from the map XML
    void LoadEntities(std::shared_ptr<Player>& player, SceneID sceneID);
	//L15 TODO 4: Define a method to save entities to the map XML
    void SaveEntities(std::shared_ptr<Player> player, SceneID sceneID);

    //Get the layers in order to draw them correctly
    MapLayer* GetLayer(const std::string& name) const;
    void DrawLayers(bool aboveEntities);

    // L19 TODO 2: Calculate Camera limits in Tiles
    Vector2D GetCameraLimitsInTiles(Vector2D camPosTile);
    Vector2D GetCameraPositionInTiles();

    void PendingChangesCheckAndSetter(EntityType type, Component component, int ID, int& current_value, SceneID entity_Scene);

public: 
    std::string mapFileName;
    std::string mapPath;
    std::vector<TeleportZone> teleportZones;
    std::vector<AutoSave> autoSaves;
    /*std::vector<Event> events;*/
    int magicPoints;

    bool isReloading = false;

    std::vector<PendingChange> pendingChanges;

private:
    bool mapLoaded;
    // L06: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
	//L15 TODO 2: make the mapFileXML an attribute of the Map class
    pugi::xml_document mapFileXML;
    //
	std::list<PhysBody*> colliderList;

    //global animation timer in milliseconds
    double animationTimerMs = 0.0;
};