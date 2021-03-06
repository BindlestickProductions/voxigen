
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "voxigen/defines.h"
#include "voxigen/cell.h"
#include "voxigen/regularGrid.h"
#include "voxigen/simpleRenderer.h"
#include "voxigen/equiRectWorldGenerator.h"

#include <boost/filesystem.hpp>

namespace fs=boost::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool key_w=false;
bool key_a=false;
bool key_d=false;
bool key_s=false;
bool key_space=false;
bool key_left_shift=false;
voxigen::SimpleFpsCamera player;

unsigned int playerRegion;
glm::ivec3 playerRegionIndex;
unsigned int playerChunk;

namespace voxigen
{
//force generator instantiation
typedef Chunk<Cell, 64, 64, 16> Chunk_64_64_16;
template GeneratorTemplate<EquiRectWorldGenerator<Chunk_64_64_16>>;
}

int main(int argc, char ** argv)
{
    GLFWwindow* window;

    /* Initialize the library */
    if(!glfwInit())
        return -1;

    size_t width=1920;
    size_t height=1080;

    window=glfwCreateWindow(width, height, "TestApp", NULL, NULL);
    
    if(!window)
    {
        glfwTerminate();
        return -1;

    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    glewInit();
    glViewport(0, 0, width, height);

    typedef voxigen::RegularGrid<voxigen::Cell, 64, 64, 16> World;
    World world;

    fs::path worldsDirectory("worlds");

    if(!fs::exists(worldsDirectory))
        fs::create_directory(worldsDirectory);

    std::vector<fs::directory_entry> worldDirectories;

    for(auto &entry:fs::directory_iterator(worldsDirectory))
        worldDirectories.push_back(entry);

    if(worldDirectories.empty())
    {
        std::string worldDirectory=worldsDirectory.string()+"/TestApWorld";
        fs::path worldPath(worldDirectory);
        
        fs::create_directory(worldPath);
        world.create(worldDirectory, "TestApWorld", glm::ivec3(2048, 2048, 1024), "EquiRectWorldGenerator");
    }
    else
        world.load(worldDirectories[0].path().string());
    
    glm::ivec3 regionCellSize=world.regionCellSize();
    glm::ivec3 worldMiddle=(world.getDescriptors().m_size)/2;

    worldMiddle.z+=5.0f;
    
    player.setYaw(0.0f);
    player.setPitch(0.0f);
    
    voxigen::Key hashes=world.getHashes(worldMiddle);

    playerRegion=hashes.regionHash;
    playerChunk=hashes.chunkHash;
    playerRegionIndex=world.getRegionIndex(playerRegion);
    
    //set player position to local region
    player.setPosition(playerRegion, world.gridPosToRegionPos(playerRegion, worldMiddle));

    voxigen::SimpleRenderer<World> renderer(&world);

    renderer.setCamera(&player);
    renderer.build();
    renderer.setViewRadius(512.0f);
    renderer.updateChunks();

    float movementSpeed=100;

    float lastFrame=glfwGetTime();
    float currentFrame;
    float deltaTime;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window))
    {
        currentFrame=glfwGetTime();
        deltaTime=currentFrame-lastFrame;
        lastFrame=currentFrame;

        glm::vec3 direction(0.0f, 0.0f, 0.0f);
        bool move=false;

        if(key_w)
        {
            direction+=glm::vec3(1.0f, 0.0f, 0.0f);
            move=true;
        }
        if(key_s)
        {
            direction+=glm::vec3(-1.0f, 0.0f, 0.0f);
            move=true;
        }
        if(key_a)
        {
            direction+=glm::vec3(0.0f, -1.0f, 0.0f);
            move=true;
        }
        if(key_d)
        {
            direction+=glm::vec3(0.0f, 1.0f, 0.0f);
            move=true;
        }
        if(key_space)
        {
            direction+=glm::vec3(0.0f, 0.0f, 1.0f);
            move=true;
        }
        if(key_left_shift)
        {
            direction+=glm::vec3(0.0f, 0.0f, -1.0f);
            move=true;
        }

        if(move)
        {
            unsigned int chunkHash;

            player.move(direction*movementSpeed*deltaTime);

            glm::ivec3 worldSize=world.getDescriptors().m_size;
            glm::vec3 playerPos=player.getPosition();

            bool resetPos=false;

//            if(playerPos.x<0.0f)
//            {
//                playerPos.x=0.0f;
//                resetPos=true;
//            }
//            else if(playerPos.x>worldSize.x)
//            {
//                playerPos.x=worldSize.x;
//                resetPos=true;
//            }
//            
//            if(playerPos.y<0.0f)
//            {
//                playerPos.y=0.0f;
//                resetPos=true;
//            }
//            else if(playerPos.y>worldSize.y)
//            {
//                playerPos.y=worldSize.y;
//                resetPos=true;
//            }
//
//            if(playerPos.z<0.0f)
//            {
//                playerPos.z=0.0f;
//                resetPos=true;
//            }
//            else if(playerPos.z>worldSize.z)
//            {
//                playerPos.z=worldSize.z;
//                resetPos=true;
//            }
//
//            if(resetPos)
//                player.setPosition(playerPos);
            unsigned int regionHash=playerRegion;
            bool updateRegion=false;

            if(playerPos.x<0)
            {
                playerRegionIndex.x--;
                playerPos.x+=regionCellSize.x;
                updateRegion=true;
            }
            else if(playerPos.x>regionCellSize.x)
            {
                playerRegionIndex.x++;
                playerPos.x-=regionCellSize.x;
                updateRegion=true; 
            }

            if(playerPos.y<0)
            {
                playerRegionIndex.y--;
                playerPos.y+=regionCellSize.y;
                updateRegion=true;
            }
            else if(playerPos.y>regionCellSize.y)
            {
                playerRegionIndex.y++;
                playerPos.y-=regionCellSize.y;
                updateRegion=true;
            }

            if(playerPos.z<0)
            {
                playerRegionIndex.z--;
                playerPos.z+=regionCellSize.z;
                updateRegion=true;
            }
            else if(playerPos.z>regionCellSize.z)
            {
                playerRegionIndex.z++;
                playerPos.z-=regionCellSize.z;
                updateRegion=true;
            }

            if(updateRegion)
            {
                regionHash=world.getRegionHash(playerRegionIndex);
                player.setPosition(regionHash, playerPos);
                playerRegion=regionHash;

                //region changed, chunk hash will change too.
            }
            chunkHash=world.getChunkHash(regionHash, playerPos);

            if(playerChunk!=chunkHash)
            {
                renderer.updateChunks();
                playerChunk=chunkHash;
            }
        }


        /* Render here */
//        glDepthFunc(GL_LESS);

        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        

//        renderer.setCamera(player);
        renderer.draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

    }

    //bring renderers down before world is terminated;
    renderer.destroy();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    player.setView(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse=true;
    static float lastX;
    static float lastY;
    static float yaw=0.0f;
    static float pitch=0.0f;

    if(firstMouse)
    {
        lastX=xpos;
        lastY=ypos;
        firstMouse=false;
    }

    float xoffset=lastX-xpos;
    float yoffset=lastY-ypos;

    lastX=xpos;
    lastY=ypos;

    float sensitivity=0.1;

    xoffset*=sensitivity;
    yoffset*=sensitivity;

    yaw+=xoffset;
    pitch+=yoffset;

    if(yaw > 360.0f)
        yaw-=360.0f;
    if(yaw < 0)
        yaw+=360.0f;

    if(pitch > 89.0f)
        pitch=89.0f;
    if(pitch < -89.0f)
        pitch=-89.0f;

    player.setYaw(glm::radians(yaw));
    player.setPitch(glm::radians(pitch));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if(key==GLFW_KEY_W)
    {
        if(action==GLFW_PRESS)
            key_w=true;
        else if(action==GLFW_RELEASE)
            key_w=false;
    }
    else if(key==GLFW_KEY_S)
    {
        if(action==GLFW_PRESS)
            key_s=true;
        else if(action==GLFW_RELEASE)
            key_s=false;
    }
    else if(key==GLFW_KEY_A)
    {
        if(action==GLFW_PRESS)
            key_a=true;
        else if(action==GLFW_RELEASE)
            key_a=false;
    }
    else if(key==GLFW_KEY_D)
    {
        if(action==GLFW_PRESS)
            key_d=true;
        else if(action==GLFW_RELEASE)
            key_d=false;
    }
    else if(key==GLFW_KEY_SPACE)
    {
        if(action==GLFW_PRESS)
            key_space=true;
        else if(action==GLFW_RELEASE)
            key_space=false;
    }
    else if(key==GLFW_KEY_LEFT_SHIFT)
    {
        if(action==GLFW_PRESS)
            key_left_shift=true;
        else if(action==GLFW_RELEASE)
            key_left_shift=false;
    }
}